// SomeThing yanked from the Unicon "cin2tab.c"

#include <iostream.h>
#include <stdio.h>
#include <stdarg.h> 
#include <string.h>
#include <bsd_mem.h>
#include <String.h>

#include <TypeConstants.h>
#include <File.h>
#include <Directory.h>
#include <Resources.h>


#include <besavager/HashTable.h>
#include <besavager/StringArray.h>


#define MAX_KEY_NUM			16


inline char *skip_space(char *s)
{
	while((*s == ' ' || *s == '\t') && *s) s++;
	return s;
}


inline char *to_space(char *s)
{
	if(!s) return NULL;

	while(*s != ' ' && *s != '\t' && *s) s++;
	return s;
}


inline void del_nl_space(char *s)
{
	if(!s) return;

	char *t;

	int32 len = strlen(s);
	t = s + len - 1;

	if(*t == '\n' || *t == '\r' || *t == ' ' || *t == '\t')
	{
		while((*t == '\n' || *t == '\r' || *t == ' ' || *t == '\t') && s < t) t--;
		*(t+1) = 0;
	}
}


typedef enum s_input_encoding {
	ENCODING_UNKNOWN = 0,
	ENCODING_SIMPLIFIED_CHINESE = 1,
	ENCODING_TRADITIONAL_CHINESE = 2,
	ENCODING_JAPANESE = 3,
	ENCODING_KOREAN = 4,
} s_input_encoding;


class SReadCin {
public:
	SReadCin(const char *fname);
	~SReadCin();

	bool IsValid();

	void EmptyInputTable();

	void PrintToStream();

	bool ConvertToDictionary(const char* fname);

private:
	char* fname;

	int32 lineno;
	FILE *file;

	char *name;
	char *locale_name;

	char *select_keys;
	char *version;
	char *copyright;
	char *help;

	bool words_first;
	bool auto_select;
	bool auto_fix_up;
	bool enter_to_output_original;
	s_input_encoding encoding;

	bool custom_page_up;
	bool custom_page_down;

	char custom_page_up_key;
	char custom_page_down_key;

	bool allow_changed;
	bool adjust_order_by_frequence;

	SStringArray *keyname;
	SStringArray *keymap;

	SStringHash *key_value_hash;
	SStringHash *value_key_hash;
	SStringHash *key_index_hash_array;

	bool get_line(char *buffer, size_t buffer_size);
	bool cmd_arg(char *buffer, size_t buffer_size, const char **cmd, const char **arg);

	bool CommandArgumentsReceived(const char *cmd, const char *arg);

	bool ReadCin();

	static void do_list_dictionary(char *key, char *value);
	static void do_convert_key_value(char *key, char *value, BFile *file);
	static void do_convert_key_index(char *key, char *value, BFile *file);
};


SReadCin::SReadCin(const char *fname)
	: fname(NULL), lineno(0), file(NULL),
	  name(NULL), locale_name(NULL), select_keys(NULL), version(NULL), copyright(NULL), help(NULL),
	  words_first(false), auto_select(false), auto_fix_up(false), enter_to_output_original(false),
	  encoding(ENCODING_UNKNOWN), custom_page_up(false), custom_page_down(false), allow_changed(false), adjust_order_by_frequence(false),
	  keyname(NULL), keymap(NULL),
	  key_value_hash(NULL), value_key_hash(NULL),
	  key_index_hash_array(NULL)
{
	SReadCin::fname = Sstrdup(fname);

	ReadCin();
}


SReadCin::~SReadCin()
{
	EmptyInputTable();
	if(fname) delete[] fname;
}


void
SReadCin::EmptyInputTable()
{
	if(name) delete[] name;
	if(locale_name) delete[] locale_name;
	if(select_keys) delete[] select_keys;
	if(version) delete[] version;
	if(copyright) delete[] copyright;
	if(help) delete[] help;

	if(keyname) delete keyname;
	if(keymap) delete keymap;

	if(key_value_hash) delete key_value_hash;
	if(value_key_hash) delete value_key_hash;
	if(key_index_hash_array) delete[] key_index_hash_array;

	name = NULL;
	locale_name = NULL;
	select_keys = NULL;
	version = NULL;
	copyright = NULL;
	help = NULL;
	keyname = NULL;
	keymap = NULL;
	key_value_hash = NULL;
	value_key_hash = NULL;
	key_index_hash_array = NULL;

	words_first = false;
	auto_select = false;
	auto_fix_up = false;
	enter_to_output_original = false;
	adjust_order_by_frequence = false;
	encoding = ENCODING_UNKNOWN;
}


void
SReadCin::do_list_dictionary(char *key, char *value)
{
	fprintf(stdout, "%s\t%s\n", key, value);
}


void
SReadCin::do_convert_key_value(char *key, char *value, BFile *file)
{
	if(file && key && value)
	{
		int32 len_key = strlen(key);
		int32 len_value = strlen(value);
		file->Write(&len_key, sizeof(int32));
		file->Write(&len_value, sizeof(int32));
		file->Write(key, sizeof(char) * len_key);
		file->Write(value, sizeof(char) * len_value);
	}
}


void
SReadCin::do_convert_key_index(char *key, char *value, BFile *file)
{
	if(file && key && value)
	{
		BString str(value);
		str.Remove(0, 1);
		str.Remove(str.Length() - 1, 1);
		const char* index = str.String();

		int32 len_key = strlen(key);
		int32 len_value = strlen(index);
		file->Write(&len_key, sizeof(int32));
		file->Write(&len_value, sizeof(int32));
		file->Write(key, sizeof(char) * len_key);
		file->Write(index, sizeof(char) * len_value);
	}
}


struct dic_header {
	int32 dictionary_version;

	int32 name;
	int32 locale_name;

	int32 version;
	int32 copyright;
	int32 help;

	int32 select_keys;
	int32 keyname;
	bool words_first;
	bool auto_select;
	bool auto_fix_up;
	bool enter_to_output_original;
	int32 encoding;

	bool custom_page_up;
	bool custom_page_down;

	char custom_page_up_key;
	char custom_page_down_key;

	bool allow_changed;

	bool adjust_order_by_frequence;

	char reserves[1024];
};


static const char dic_magic[] = "BeCJKInputMethodDictionary";

bool
SReadCin::ConvertToDictionary(const char* fname)
{
	if(!fname) return false;
	if(!IsValid()) return false;

	BFile bfile(fname, B_WRITE_ONLY);
	BDirectory bdir;
	if(bdir.CreateFile(fname, &bfile, false) != B_OK) return false;

	bfile.Write(dic_magic, strlen(dic_magic) * sizeof(char));

	dic_header header;
	header.dictionary_version = 1;
	header.name = (name != NULL ? strlen(name) : 0);
	header.locale_name = (locale_name != NULL ? strlen(locale_name) : 0);
	header.version = (version != NULL ? strlen(version) : 0);
	header.copyright = (copyright != NULL ? strlen(copyright) : 0);
	header.help = (help != NULL ? strlen(help) : 0);
	header.select_keys = strlen(select_keys);
	header.keyname = keyname->CountItems();
	header.words_first = words_first;
	header.auto_select = auto_select;
	header.auto_fix_up = auto_fix_up;
	header.enter_to_output_original = enter_to_output_original;
	header.encoding = (int32)encoding;
	header.custom_page_up = custom_page_up;
	header.custom_page_down = custom_page_down;
	header.custom_page_up_key = custom_page_up_key;
	header.custom_page_down_key = custom_page_down_key;
	header.allow_changed = allow_changed;
	header.adjust_order_by_frequence = adjust_order_by_frequence;

	bfile.Write(&header, sizeof(header));
	if(name) bfile.Write(name, header.name * sizeof(char));
	if(locale_name) bfile.Write(locale_name, header.locale_name * sizeof(char));
	if(version) bfile.Write(version, header.version * sizeof(char));
	if(copyright) bfile.Write(copyright, header.copyright * sizeof(char));
	if(help) bfile.Write(help, header.help * sizeof(char));
	bfile.Write(select_keys, header.select_keys * sizeof(char));
	for(int32 i = 0; i < keyname->CountItems(); i++)
		bfile.Write(keyname->ItemAt(i)->String(), sizeof(char));
	for(int32 i = 0; i < keymap->CountItems(); i++)
	{
		const SString *str = keymap->ItemAt(i);
		int32 len = str->Length();
		bfile.Write(&len, sizeof(int32));
		bfile.Write(str->String(), sizeof(char) * len);
	}

	int32 nitem = key_value_hash->CountNodes();
	bfile.Write(&nitem, sizeof(int32));

	cout << "Items(KEY):" << nitem << "\n";
	key_value_hash->DoForeach((SHashTableForeachFunction)do_convert_key_value, &bfile);

	for(int32 i = 0; i < MAX_KEY_NUM; i++)
	{
		nitem = key_index_hash_array[i].CountNodes();
		bfile.Write(&nitem, sizeof(int32));

		cout << "Items(INDEX):" << nitem << "\n";
		if(nitem > 0)
			key_index_hash_array[i].DoForeach((SHashTableForeachFunction)do_convert_key_index, &bfile);
	}

	return true;
}


void
SReadCin::PrintToStream()
{
	if(!IsValid())
	{
		fprintf(stdout, "The table is not valid!\n");
		return;
	}

	fprintf(stdout, "Prompt:%s\n", (name == NULL ? "(NULL)" : name));
	fprintf(stdout, "Version:%s\n", (version == NULL ? "(NULL)" : version));
	fprintf(stdout, "Copyright:%s\n", (copyright == NULL ? "(NULL)" : copyright));
	fprintf(stdout, "Select Keys:%s\n", (select_keys == NULL ? "(NULL)" : select_keys));

	int32 key_num = (keyname ? keyname->CountItems() : 0);

	fprintf(stdout, "KeyName\tKeyMap\n");
	for(int32 i = 0; i < key_num; i++)
		fprintf(stdout, "%s\t%s\n", keyname->ItemAt(i)->String(), keymap->ItemAt(i)->String());
	
	fprintf(stdout, "KEY\tVALUE\n");
	key_value_hash->DoForeach((SHashTableForeachFunction)do_list_dictionary);

	fprintf(stdout, "VALUE\tKEY\n");
	value_key_hash->DoForeach((SHashTableForeachFunction)do_list_dictionary);

	fprintf(stdout, "INDEX\tKEYS\n");
	for(int32 i = 0; i < MAX_KEY_NUM; i++)
		key_index_hash_array[i].DoForeach((SHashTableForeachFunction)do_list_dictionary);
}


bool
SReadCin::IsValid()
{
	if(!fname) return false;

	if(!name) return false;
	if(!select_keys) return false;

	if(!keyname) return false;
	if(!keymap) return false;

	if(keyname->CountItems() < 1) return false;
	if(keymap->CountItems() < 1) return false;
	if(keyname->CountItems() != keymap->CountItems()) return false;

	if(!key_value_hash) return false;
	if(key_value_hash->CountNodes() < 1) return false;

	if(!value_key_hash) return false;
	if(value_key_hash->CountNodes() < 1) return false;

	if(!key_index_hash_array) return false;

	return true;
}


bool
SReadCin::get_line(char *buffer, size_t buffer_size)
{
	if(!buffer || !file) return false;

	while(!feof(file)) 
	{
		if(!fgets(buffer, buffer_size, file)) continue;
		
		lineno++;
		if(buffer[0] == '#') continue;
		else return true;
	}
	
	return false;
}


bool
SReadCin::cmd_arg(char *buffer, size_t buffer_size, const char **cmd, const char **arg)
{
	if(!buffer || !cmd || !arg) return false;

	bzero(buffer, buffer_size);

	*cmd = NULL;
	*arg = NULL;

	if(!get_line(buffer, buffer_size)) return false;

	char *tmp;

	char *buf = skip_space(buffer);
	if(!buf || strlen(buf) == 0) return false;
	*cmd = buf;

	int32 len = (int32)strlen(buf);

	tmp = to_space(buf);
	if(!tmp) return false;

	if(tmp - buf < len - 1)
	{
		*tmp = 0;
		tmp++;

		tmp = skip_space(tmp);
		if(!tmp) return false;

		del_nl_space(tmp);

		if(strlen(tmp) > 0) *arg = tmp;
	}

	return(*arg != NULL);
}


bool
SReadCin::CommandArgumentsReceived(const char *cmd, const char *arg)
{
	if(!cmd || !arg) return false;
	if(cmd[0] != '%') return false;
	if(strlen(cmd) < 2) return false;


	if(strcmp(cmd, "%name") == 0)
		name = Sstrdup(arg);
	else if(strcmp(cmd, "%locale_name") == 0)
		locale_name = Sstrdup(arg);
	else if(strcmp(cmd, "%selkey") == 0)
		select_keys = Sstrdup(arg);
	else if(strcmp(cmd, "%version") == 0)
		version = Sstrdup(arg);
	else if(strcmp(cmd, "%copyright") == 0)
		copyright = Sstrdup(arg);
	else if(strcmp(cmd, "%help") == 0)
		help = Sstrdup(arg);
	else if(strcmp(cmd, "%words_first") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) words_first = true;
		else words_first = false;
	}
	else if(strcmp(cmd, "%auto_select") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) auto_select = true;
		else auto_select = false;
	}
	else if(strcmp(cmd, "%auto_fix_up") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) auto_fix_up = true;
		else auto_fix_up = false;
	}
	else if(strcmp(cmd, "%enter_to_output_original") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) enter_to_output_original = true;
		else enter_to_output_original = false;
	}
	else if(strcmp(cmd, "%encoding") == 0)
	{
		if(strcasecmp(arg, "SC") == 0) encoding = ENCODING_SIMPLIFIED_CHINESE;
		else if(strcasecmp(arg, "TC") == 0) encoding = ENCODING_TRADITIONAL_CHINESE;
		else if(strcasecmp(arg, "JP") == 0) encoding = ENCODING_JAPANESE;
		else if(strcasecmp(arg, "KO") == 0) encoding = ENCODING_KOREAN;
		else encoding = ENCODING_UNKNOWN;
	}
	else if(strcmp(cmd, "%allow_changed") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) allow_changed = true;
		else allow_changed = false;
	}
	else if(strcmp(cmd, "%custom_page_up_key") == 0)
	{
		custom_page_up = true;
		custom_page_up_key = arg[0];
	}
	else if(strcmp(cmd, "%custom_page_down_key") == 0)
	{
		custom_page_down = true;
		custom_page_down_key = arg[0];
	}
	else if(strcmp(cmd, "%adjust_order_by_frequence") == 0)
	{
		if(strcasecmp(arg, "true") == 0 || (strlen(arg) == 1 && arg[0] == '1')) adjust_order_by_frequence = true;
		else adjust_order_by_frequence = false;
	}

	return true;
}


bool
SReadCin::ReadCin()
{
	if(!fname) return false;

	if((file = fopen(fname, "r")) == NULL) return false;

	EmptyInputTable();

	const char *cmd = NULL, *arg = NULL;
	char tmp[2048];

	while(cmd_arg(tmp, sizeof(tmp), &cmd, &arg))
	{
		if(strcmp(cmd, "%keyname") == 0 && strcmp(arg, "begin") == 0)
		{
			// reading keyname & keymap
			while(cmd_arg(tmp, sizeof(tmp), &cmd, &arg))
			{
				if(strcmp(cmd, "%keyname") == 0 && strcmp(arg, "end") == 0)
				{
					if(keyname->CountItems() > 1 && keymap->CountItems() > 1 &&
					   keyname->CountItems() == keymap->CountItems())
					{
						break;
					}
					else
					{
						fclose(file);
						return false;
					}

					break;
				}

				// the length of keyname must be 1
				if(strlen(cmd) != 1)
				{
					fclose(file);
					return false;
				}

				if(!keyname) keyname = new SStringArray();
				if(!keymap) keymap = new SStringArray();

				*(keyname) += cmd;
				*(keymap) += arg;
			}
		}
		else
		{
			if(!CommandArgumentsReceived(cmd, arg))
			{
				// reading dictionary
				do
				{
					const char* key = cmd;
					const char* value = arg;

					// the length of keys must little by or equal to MAX_KEY_NUM
					if(strlen(key) > MAX_KEY_NUM)
					{
						fclose(file);
						return false;
					}

					// key_value_hash
					if(!key_value_hash) key_value_hash = new SStringHash();
					char *string = (char*)key_value_hash->Lookup(key);
					if(!string)
					{
						// insert new string to the dictionary hashtable
						key_value_hash->InsertString(key, value);
					}
					else
					{
						// the string already existed
						BString bstr;
						bstr << string << " " << value;
						key_value_hash->ReplaceString(key, bstr);
					}

					SString value_str(value);
					SStringArray *array = value_str.Split(' ');
					if(array)
					{
						for(int32 i = 0; i < array->CountItems(); i++)
						{
							const SString *ivalue = array->ItemAt(i);
							if(!ivalue) break;

							// value_key_hash
							if(!value_key_hash) value_key_hash = new SStringHash();
							char *cstring = (char*)value_key_hash->Lookup(ivalue->String());
							if(!cstring)
							{
								// insert new cstring to the dictionary hashtable
								value_key_hash->InsertString(ivalue->String(), key);
							}
							else
							{
								// the cstring already existed
								BString bstr;
								bstr << cstring << " " << key;
								value_key_hash->ReplaceString(ivalue->String(), bstr);
							}
						}

						delete array;
					}


					// key_index_hash_array
					if(!key_index_hash_array) key_index_hash_array = new SStringHash[MAX_KEY_NUM];
					for(uint32 i = 0; i < strlen(key); i++)
					{
						BString bstr;
						bstr.SetTo(key, i + 1);

						char *istring = (char*)key_index_hash_array[i].Lookup(bstr.String());

						if(!istring)
						{
							// insert new istring to the dictionary hashtable
							BString bstr1;
							bstr1 << " " << key << " ";
							key_index_hash_array[i].InsertString(bstr, bstr1);
						}
						else
						{
							// the istring already existed
							BString query;
							query << " " << key << " ";

							BString src(istring);
							if(src.FindFirst(query) == B_ERROR)
							{
								BString bstr1;
								bstr1 << istring << key << " ";
								key_index_hash_array[i].ReplaceString(bstr, bstr1);
							}
						}
					}

				}while(cmd_arg(tmp, sizeof(tmp), &cmd, &arg));
			}
		}
	}

	fclose(file);

	return true;
}



#include <OS.h>

int main(int argc, char **argv)
{
	char fname_in[1024], fname_out[1024];
	
	bzero(fname_in, sizeof(fname_in));
	bzero(fname_out, sizeof(fname_out));

	if(argc < 2)
	{
		cout << "Enter table file name: ";
		cin >> fname_in;
	}
	else
	{
		strcpy(fname_in, argv[1]);
	}


	cout << "Reading...\n";

	bigtime_t start_time = system_time();
	SReadCin cin(fname_in);
	bigtime_t end_time = system_time();

	cout << "Reading Cost: " << (float)(end_time - start_time) / (float)1000000. << " Seconds\n";

	if(argc < 3)
	{
		cin.PrintToStream();
	}
	else
	{
		strcpy(fname_out, argv[2]);

		cout << "Generating the dictionary...(perhaps it took a long time and wait...)\n";
		start_time = system_time();
		if(!cin.ConvertToDictionary(fname_out))
		{
			cout << "Failed!\n";
		}
		else
		{
			end_time = system_time();
			cout << "Writing Cost: " << (float)(end_time - start_time) / (float)1000000. << " Seconds\n";
			cout << "Success\n";
		}
	}

	return 0;
}
