// BeCJK - Input Server Add-On for CJK Input
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// General Input Module

#include <stdio.h>
#include <string.h>
#include <bsd_mem.h>
#include <syslog.h>
#include <ctype.h>

#include <storage/Path.h>
#include <storage/FindDirectory.h>
#include <File.h>
#include <Directory.h>
#include <List.h>
#include <InterfaceDefs.h>
#include <NodeInfo.h>
#include <Roster.h>

#include <Menu.h>
#include <MenuItem.h>
#include <Alert.h>

#include "config.h"
#include "BeCJKInputModule.h"
#include "GeneralInputModule.h"
#include "Locale.h"
#include "AllMessages.h"

#define SDIC_MAX_KEY_NUM	16
#define SDIC_MAX_BUFFER		64
#define SDIC_MAX_PAGES		10
#define SDIC_MAX_ORIGINAL	128

typedef struct s_dictionary_header {
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
} s_dictionary_header;

static const char s_dictionary_magic[] = "BeCJKInputMethodDictionary";


SGeneralInputModuleFrequence::SGeneralInputModuleFrequence()
{
	hash = new SStringInt32Hash();
	array = new SStringArray();
}


SGeneralInputModuleFrequence::~SGeneralInputModuleFrequence()
{
	if(hash) delete hash;
	if(array) delete array;
}


void
SGeneralInputModuleFrequence::Reset()
{
	if(array) array->MakeEmpty();
	if(hash) hash->MakeEmpty(true);
}


int32
SGeneralInputModuleFrequence::Lookup(const char *string)
{
	if(!hash || !array || !string) return -1;
	if(strlen(string) < 1) return -1;

	int32 *freq = (int32*)hash->Lookup(string);
	if(!freq) return -1;

	return *freq;
}


void
SGeneralInputModuleFrequence::ToFront(const char *string)
{
	if(!hash || !array || !string) return;
	if(strlen(string) < 1) return;

	int32 *orig_freq = (int32*)hash->Lookup(string);

	if(!orig_freq)
	{
		if(array->AppendItem(string))
		{
			hash->InsertInt32(string, array->CountItems() - 1);
			if((int32)hash->CountNodes() != array->CountItems()) array->RemoveItem(array->CountItems() - 1);
		}
	}
	else if(*orig_freq > 0)
	{
		const SString *prev = array->ItemAt(*orig_freq - 1);

		if(prev)
		{
			int32 *prev_freq = (int32*)hash->Lookup(prev->String());
			if(prev_freq)
			{
				if(array->SwapItems(*orig_freq, *prev_freq))
				{
					int32 tmp = *orig_freq;
					*orig_freq = *prev_freq;
					*prev_freq = tmp;
				}
			}
		}
	}
}


void
SGeneralInputModuleFrequence::ToBack(const char *string)
{
	if(!hash || !array || !string) return;
	if(strlen(string) < 1) return;

	int32 *orig_freq = (int32*)hash->Lookup(string);

	if(!orig_freq)
	{
		if(array->AppendItem(string))
		{
			hash->InsertInt32(string, array->CountItems() - 1);
			if((int32)hash->CountNodes() != array->CountItems()) array->RemoveItem(array->CountItems() - 1);
		}
	}
	else if(*orig_freq >= 0 && *orig_freq < array->CountItems() - 1)
	{
		const SString *next = array->ItemAt(*orig_freq + 1);

		if(next)
		{
			int32 *next_freq = (int32*)hash->Lookup(next->String());
			if(next_freq)
			{
				if(array->SwapItems(*orig_freq, *next_freq))
				{
					int32 tmp = *orig_freq;
					*orig_freq = *next_freq;
					*next_freq = tmp;
				}
			}
		}
	}
}


SGeneralInputModule::SGeneralInputModule(const char *file_name)
	: SInputModule(NULL),
	  name(NULL), locale_name(NULL), select_keys(NULL), version(NULL), copyright(NULL), help(NULL),
	  custom_page_up_key(B_PAGE_UP), custom_page_down_key(B_PAGE_DOWN),
	  keyname(NULL), keymap(NULL),
	  key_value_hash(NULL), value_key_hash(NULL), key_index_hash_array(NULL),
	  select_items(NULL), select_items_key(NULL),
	  words(NULL), words_key(NULL), freq(NULL),
	  words_first(false), auto_select(false), auto_fix_up(false), enter_to_output_original(false), adjust_order_by_frequence(false),
	  nchange(-1), nselect(1), npage(1), confirmed(CONFIRMED_NONE), table_fname(NULL), force_select_key(false),
	  menu(NULL), activated(false)
{
	if(file_name) Load(file_name);

	menu = GetMenu();
}


SGeneralInputModule::~SGeneralInputModule()
{
	SetMenu(NULL);
	if(menu) delete menu;

	EmptyTable();
	if(freq) delete freq;

	if(table_fname) delete[] table_fname;
}


void
SGeneralInputModule::EmptyTable()
{
	Reset();

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

	if(freq) delete freq;

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
	freq = NULL;

	words_first = false;
	auto_select = false;
	auto_fix_up = false;
	enter_to_output_original = false;
	encoding = ENCODING_UNKNOWN;

	custom_page_up_key = B_PAGE_UP;
	custom_page_down_key = B_PAGE_DOWN;

	allow_changed = false;
	adjust_order_by_frequence = false;
}


bool
SGeneralInputModule::IsValid()
{
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
SGeneralInputModule::Load(const char *fname)
{
	SetName(NULL);
	SetIcon((const BBitmap*)NULL);

	if(!fname) return false;

	BFile bfile;
	if(bfile.SetTo(fname, B_READ_ONLY) != B_OK)
	{
		syslog(LOG_WARNING, "%s:Can't open %s\n", APP_NAME, fname);
		return false;
	}

	if(table_fname) delete[] table_fname;

	EmptyTable();

	SetName(NULL);
	SetIcon((const BBitmap*)NULL);

	char buffer[2048];
	bzero(buffer, sizeof(buffer));
	if(bfile.Read(buffer, strlen(s_dictionary_magic) * sizeof(char)) <= 0)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		return false;
	}
	if(strcmp(buffer, s_dictionary_magic) != 0)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		return false;
	}

	s_dictionary_header header;
	if(bfile.Read(&header, sizeof(header)) <= 0)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		return false;
	}

	if(header.dictionary_version != 1 || header.name < 1 || header.select_keys < 1 || header.keyname < 1)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		return false;
	}

	words_first = header.words_first;
	auto_select = header.auto_select;
	auto_fix_up = header.auto_fix_up;
	enter_to_output_original = header.enter_to_output_original;
	adjust_order_by_frequence = header.adjust_order_by_frequence;
	encoding = (s_input_encoding)header.encoding;

	if(header.custom_page_up) custom_page_up_key = header.custom_page_up_key;
	if(header.custom_page_down) custom_page_down_key = header.custom_page_down_key;

	allow_changed = header.allow_changed;

	if(adjust_order_by_frequence) freq = new SGeneralInputModuleFrequence();

	int32 header_len = MAX(MAX(MAX(MAX(MAX(MAX(header.name, header.locale_name), header.select_keys), header.keyname), header.version), header.copyright), header.help);
	char *header_buf = new char[header_len + 1];

	if(header.name > 0)
	{
		bzero(header_buf, sizeof(char) * (header_len + 1));
		if(bfile.Read(header_buf, header.name * sizeof(char)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] header_buf;
			return false;
		}
		name = Sstrdup(header_buf);
	}

	if(header.locale_name > 0)
	{
		bzero(header_buf, sizeof(char) * (header_len + 1));
		if(bfile.Read(header_buf, header.locale_name * sizeof(char)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] header_buf;
			return false;
		}
		locale_name = Sstrdup(header_buf);
	}

	if(header.version > 0)
	{
		bzero(header_buf, sizeof(char) * (header_len + 1));
		if(bfile.Read(header_buf, header.version * sizeof(char)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] header_buf;
			return false;
		}
		version = Sstrdup(header_buf);
	}

	if(header.copyright > 0)
	{
		bzero(header_buf, sizeof(char) * (header_len + 1));
		if(bfile.Read(header_buf, header.copyright * sizeof(char)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] header_buf;
			return false;
		}
		copyright = Sstrdup(header_buf);
	}

	if(header.help > 0)
	{
		bzero(header_buf, sizeof(char) * (header_len + 1));
		if(bfile.Read(header_buf, header.help * sizeof(char)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] header_buf;
			return false;
		}
		help = Sstrdup(header_buf);
	}

	bzero(header_buf, sizeof(char) * (header_len + 1));
	if(bfile.Read(header_buf, header.select_keys * sizeof(char)) <= 0)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		delete[] header_buf;
		return false;
	}
	select_keys = Sstrdup(header_buf);

	bzero(header_buf, sizeof(char) * (header_len + 1));
	if(bfile.Read(header_buf, header.keyname * sizeof(char)) <= 0)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		delete[] header_buf;
		return false;
	}
	keyname = new SStringArray();
	for(int32 i = 0; i < (int32)strlen(header_buf); i++)
	{
		char tmp[2];
		tmp[0] = header_buf[i]; tmp[1] = 0;
		keyname->AppendItem(tmp);
	}
	delete[] header_buf;


	keymap = new SStringArray();
	for(int32 i = 0; i < keyname->CountItems(); i++)
	{
		int32 len = 0;
		if(bfile.Read(&len, sizeof(int32)) <= 0 || len < 1)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			return false;
		}
		char *buf = new char[len + 1];
		bzero(buf, sizeof(char) * (len + 1));
		if(bfile.Read(buf, sizeof(char) * len) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] buf;
			return false;
		}
		keymap->AppendItem(buf);
		delete[] buf;
	}


	int32 nitem = 0;
	if(bfile.Read(&nitem, sizeof(int32)) <= 0 || nitem < 1)
	{
		syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
		return false;
	}
	for(int32 i = 0; i < nitem; i++)
	{
		int32 len_key = 0, len_value = 0;
		if(bfile.Read(&len_key, sizeof(int32)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			return false;
		}
		if(bfile.Read(&len_value, sizeof(int32)) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			return false;
		}
		if(len_key < 1 || len_value < 1)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			return false;
		}

		char *key = new char[len_key + 1];
		char *value = new char[len_value + 1];

		bzero(key, sizeof(char) * (len_key + 1));
		bzero(value, sizeof(char) * (len_value + 1));

		if(bfile.Read(key, sizeof(char) * len_key) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] key;
			delete[] value;
			return false;
		}
		if(bfile.Read(value, sizeof(char) * len_value) <= 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			delete[] key;
			delete[] value;
			return false;
		}

		// the length of keys must little by or equal to SDIC_MAX_KEY_NUM
		if(strlen(key) > SDIC_MAX_KEY_NUM)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			EmptyTable();
			delete[] key;
			delete[] value;
			return false;
		}

		// key_value_hash
		if(!key_value_hash) key_value_hash = new SStringHash();
		key_value_hash->InsertString(key, value);

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

		delete[] key;
		delete[] value;
	}


	for(int32 k = 0; k < SDIC_MAX_KEY_NUM; k++)
	{
		nitem = -1;
		if(bfile.Read(&nitem, sizeof(int32)) <= 0 || nitem < 0)
		{
			syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
			return false;
		}
		if(nitem == 0) continue;

		for(int32 i = 0; i < nitem; i++)
		{
			int32 len_key = 0, len_value = 0;
			if(bfile.Read(&len_key, sizeof(int32)) <= 0)
			{
				syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
				return false;
			}
			if(bfile.Read(&len_value, sizeof(int32)) <= 0)
			{
				syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
				return false;
			}
			if(len_key < 1 || len_value < 1 || len_key > SDIC_MAX_KEY_NUM)
			{
				syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
				return false;
			}

			char *key = new char[len_key + 1];
			char *value = new char[len_value + 1];

			bzero(key, sizeof(char) * (len_key + 1));
			bzero(value, sizeof(char) * (len_value + 1));

			if(bfile.Read(key, sizeof(char) * len_key) <= 0)
			{
				syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
				delete[] key;
				delete[] value;
				return false;
			}
			if(bfile.Read(value, sizeof(char) * len_value) <= 0)
			{
				syslog(LOG_WARNING, "%s:(%s)(%s)(%s)Bad Dictionary\n", APP_NAME, __FILE__, __LINE__, fname);
				delete[] key;
				delete[] value;
				return false;
			}

			// key_index_hash_array
			if(!key_index_hash_array) key_index_hash_array = new SStringHash[SDIC_MAX_KEY_NUM];
			key_index_hash_array[k].InsertString(key, value);

			delete[] key;
			delete[] value;
		}
	}

	bool ___is_valid = IsValid();

	if(___is_valid)
	{
		table_fname = Sstrdup(fname);
		SetName(name);

		BBitmap the_bitmap(BRect(0, 0, 15, 15), B_CMAP8);
		BNodeInfo info(&bfile);
		if(info.GetIcon(&the_bitmap, B_MINI_ICON) == B_OK) SetIcon(&the_bitmap);
	}

	return ___is_valid;
}


void
SGeneralInputModule::Reset()
{
	EmptySelectItems();

	nchange = -1;
	original = "";
	buffer = "";
	display = "";
	confirmed = CONFIRMED_NONE;

	if(words)
	{
		delete words;
		words = NULL;
	}
	
	if(words_key)
	{
		delete words_key;
		words_key = NULL;
	}
}


bool
SGeneralInputModule::IsEmpty()
{
	return(!(buffer.Length() > 0 || display.Length() > 0 || (original.Length() > 0 && enter_to_output_original)));
}


void
SGeneralInputModule::EmptySelectItems()
{
	if(select_items)
	{
		delete select_items;
		select_items = NULL;
	}

	if(select_items_key)
	{
		char *item = NULL;
		for(int32 i = 0; (item = (char*)select_items_key->ItemAt(i)) != NULL; i++)
		{
			if(item) delete[] item;
		}
		delete select_items_key;
		
		select_items_key = NULL;
	}

	nselect = 1;
	npage = 1;
}


inline void insert_items(SStringArray *dest_items, BList *dest_items_key,
						 SStringArray *src_items, BList *src_items_key)
{
	if(!dest_items || !dest_items_key || !src_items || !src_items_key) return;
	if(src_items->IsEmpty()) return;
	if(src_items->CountItems() != src_items_key->CountItems()) return;

	for(int32 i = 0; i < src_items->CountItems(); i++)
	{
		const SString *item = src_items->ItemAt(i);
		if(!item) continue;

		SStringArray *array = item->Split('\t');
		if(array) item = array->ItemAt(0);

		void *item_key = src_items_key->ItemAt(i);

		if(dest_items->FindString(item->String()) < 0 && item->Length() > 0 && item_key)
		{
			dest_items->AppendItem(src_items->ItemAt(i)->String());
			dest_items_key->AddItem(item_key);
		}
		else
		{
			if(item_key)
			{
				char *data = (char*)item_key;
				src_items_key->ReplaceItem(i, NULL);
				delete[] data;
			}
		}

		if(array) delete array;
	}
}


inline void sort_itmes(SStringArray *items, BList *items_key, SGeneralInputModuleFrequence *freq)
{
	if(!items || !items_key || !freq) return;
	if(items->CountItems() != items_key->CountItems()) return;
	if(items->CountItems() < 2) return;

	// frequence sort
	for(int32 i = 0; i < items->CountItems() - 1; i++)
	{
		for(int32 j = 0; j < items->CountItems() - 1 - i; j++)
		{
			const SString* tmp1 = items->ItemAt(j);
			const SString* tmp2 = items->ItemAt(j + 1);

			SStringArray *array1 = tmp1->Split('\t');
			SStringArray *array2 = tmp2->Split('\t');

			if(array1) tmp1 = array1->ItemAt(0);
			if(array2) tmp2 = array2->ItemAt(0);

			int32 nfreq1 = freq->Lookup(tmp1->String());
			int32 nfreq2 = freq->Lookup(tmp2->String());
					
			if(nfreq1 < 0 || nfreq2 < 0)
			{
				if(nfreq2 >= 0)
				{
					items->SwapItems(j, j + 1);
					items_key->SwapItems(j, j + 1);
				}
			}
			else
			{
				if(nfreq2 > nfreq1)
				{
					items->SwapItems(j, j + 1);
					items_key->SwapItems(j, j + 1);
				}
			}

			if(array1) delete array1;
			if(array2) delete array2;
		}
	}
}


void
SGeneralInputModule::LookForSelectItems(const char* first_item, const char* first_item_key, bool all_match, bool allow_auto_select)
{
	EmptySelectItems();

	if(buffer.Length() < 1) return;

	int32 len = MIN(buffer.Length(), SDIC_MAX_KEY_NUM);
	int32 nitem = (select_items != NULL ? select_items->CountItems() : 0);

	SString sbuf;
	sbuf.SetTo(buffer, len);

	char *cstr = NULL;

	SStringArray *all_single_word_items = new SStringArray();
	BList *all_single_word_items_key = new BList();
	SStringArray *all_words_items = new SStringArray();
	BList *all_words_items_key = new BList();

	SStringArray *single_word_items = new SStringArray();
	BList *single_word_items_key = new BList();
	SStringArray *words_items = new SStringArray();
	BList *words_items_key = new BList();

	SStringArray *auto_fix_up_single_word_items = new SStringArray();
	BList *auto_fix_up_single_word_items_key = new BList();
	SStringArray *auto_fix_up_words_items = new SStringArray();
	BList *auto_fix_up_words_items_key = new BList();


	for(int32 k = 0; k < len; k++)
	{
		if(all_match && k != 0) break;
		if(nitem >= SDIC_MAX_PAGES * (int32)strlen(select_keys)) break;

		SString found;
		found.SetTo(sbuf, (k == 0 ? len : k));

		if((cstr = (char*)key_value_hash->Lookup(found.String())) != NULL)
		{
			SString tmp(cstr);
			SStringArray *array = tmp.Split(' ');
			if(array)
			{
				int32 i = 0;
				const SString *item = NULL;
				while((item = array->ItemAt(i)) != NULL)
				{
					if(nitem >= SDIC_MAX_PAGES * (int32)strlen(select_keys) && k != 0) break;

					SStringArray *__single_word_items = (k == 0 ? all_single_word_items : single_word_items);
					BList *__single_word_items_key = (k == 0 ? all_single_word_items_key : single_word_items_key);
					SStringArray *__words_items = (k == 0 ? all_words_items : words_items);
					BList *__words_items_key = (k == 0 ? all_words_items_key : words_items_key);

					SStringArray *sel_items = (__utf8_strlen(item->String()) > 2 ? __words_items : __single_word_items);
					BList *sel_items_key = (__utf8_strlen(item->String()) > 2 ? __words_items_key : __single_word_items_key);

					if(sel_items->FindString(item->String()) < 0)
					{
						sel_items->AppendItem(item->String());
						sel_items_key->AddItem(Sstrdup(found.String()));
						nitem++;
					}

					i++;
				}

				delete array;
			}
		}
	}


	if(nitem < SDIC_MAX_PAGES * (int32)strlen(select_keys) &&
	   buffer.Length() > 0 && buffer.Length() <= SDIC_MAX_KEY_NUM &&
	   nchange < 0 &&
	   auto_fix_up && !all_match)
	{
		SString found(buffer);

		char *istring = (char*)key_index_hash_array[found.Length() - 1].Lookup(found.String());
		if(istring)
		{
			SString ttmp(istring);
			SStringArray *tmp_array = ttmp.Split(' ');
			if(tmp_array)
			{
				int32 j = 0;
				const SString *tmp_sbuf = NULL;

				while((tmp_sbuf = tmp_array->ItemAt(j)) != NULL)
				{
					if(nitem >= SDIC_MAX_PAGES * (int32)strlen(select_keys)) break;
					if(*tmp_sbuf == found) {j++; continue;}

					char *tmp_cstr = (char*)key_value_hash->Lookup(tmp_sbuf->String());
					if(tmp_cstr)
					{
						SString tmp(tmp_cstr);
						SStringArray *array = tmp.Split(' ');
						if(array)
						{
							int32 i = 0;
							const SString *item = NULL;
							while((item = array->ItemAt(i)) != NULL)
							{
								if(nitem >= SDIC_MAX_PAGES * (int32)strlen(select_keys)) break;

								SStringArray *sel_items = (__utf8_strlen(item->String()) > 2 ? auto_fix_up_words_items : auto_fix_up_single_word_items);
								BList *sel_items_key = (__utf8_strlen(item->String()) > 2 ? auto_fix_up_words_items_key : auto_fix_up_single_word_items_key);

								if(sel_items->FindString(item->String()) < 0)
								{
									BString titem;
									BString tran_sbuf(*tmp_sbuf);
									tran_sbuf.Remove(0, found.Length());

									titem << item->String() << "\t" << tran_sbuf.String();
									sel_items->AppendItem(titem.String());
									sel_items_key->AddItem(Sstrdup(found.String()));
									nitem++;
								}

								i++;
							}

							delete array;
						}
					}
					
					j++;
				}

				delete tmp_array;
			}
		}
	}

	sort_itmes(all_single_word_items, all_single_word_items_key, freq);
	sort_itmes(all_words_items, all_words_items_key, freq);

	sort_itmes(single_word_items, single_word_items_key, freq);
	sort_itmes(words_items, words_items_key, freq);

	sort_itmes(auto_fix_up_single_word_items, auto_fix_up_single_word_items_key, freq);
	sort_itmes(auto_fix_up_words_items, auto_fix_up_words_items_key, freq);

	if(!select_items) select_items = new SStringArray();
	if(!select_items_key) select_items_key = new BList();

	if(!words_first) // single word first
	{
		insert_items(select_items, select_items_key,
					 all_single_word_items, all_single_word_items_key);
		insert_items(select_items, select_items_key,
					 single_word_items, single_word_items_key);

		insert_items(select_items, select_items_key,
					 all_words_items, all_words_items_key);
		insert_items(select_items, select_items_key,
					 words_items, words_items_key);

		insert_items(select_items, select_items_key,
					 auto_fix_up_single_word_items, auto_fix_up_single_word_items_key);
		insert_items(select_items, select_items_key,
					 auto_fix_up_words_items, auto_fix_up_words_items_key);
	}
	else
	{
		insert_items(select_items, select_items_key,
					 all_words_items, all_words_items_key);
		insert_items(select_items, select_items_key,
					 words_items, words_items_key);

		insert_items(select_items, select_items_key,
					 all_single_word_items, all_single_word_items_key);
		insert_items(select_items, select_items_key,
					 single_word_items, single_word_items_key);

		insert_items(select_items, select_items_key,
					 auto_fix_up_words_items, auto_fix_up_words_items_key);
		insert_items(select_items, select_items_key,
					 auto_fix_up_single_word_items, auto_fix_up_single_word_items_key);
	}

	delete all_single_word_items;
	delete all_single_word_items_key;
	delete all_words_items;
	delete all_words_items_key;

	delete single_word_items;
	delete single_word_items_key;
	delete words_items;
	delete words_items_key;

	delete auto_fix_up_single_word_items;
	delete auto_fix_up_single_word_items_key;
	delete auto_fix_up_words_items;
	delete auto_fix_up_words_items_key;

	if(select_items->IsEmpty()) EmptySelectItems();

	if(first_item && first_item_key)
	{
		if(!select_items) select_items = new SStringArray();
		if(!select_items_key) select_items_key = new BList();

		int32 found = select_items->FindString(first_item);
		if(found >= 0)
		{
			select_items->SwapItems(found, 0);
			select_items_key->SwapItems(found, 0);
		}
		else
		{
			select_items->AppendItem(first_item);
			select_items_key->AddItem(Sstrdup(first_item_key));

			int32 count = select_items->CountItems();
			select_items->SwapItems(count - 1, 0);
			select_items_key->SwapItems(count - 1, 0);
		}
	}

	// auto select
	if(select_items && allow_auto_select)
	{
		while(select_items->CountItems() > 0 && auto_select)
		{
			char *items_key = (char*)select_items_key->ItemAt(0);

			if(items_key && buffer == items_key)
			{
				if(select_items->CountItems() > 1)
				{
					char *items_key_tmp = (char*)select_items_key->ItemAt(1);
					if(items_key_tmp && buffer == items_key_tmp) break;
				}

				bool auto_do_it = true;

				char *keys = (char*)key_index_hash_array[strlen(items_key) - 1].Lookup(items_key);

				if(keys)
				{
					SString tmp(keys);
					SStringArray *array_tmp = tmp.Split(' ');
					if(array_tmp)
					{
						for(int32 i = 0; i < array_tmp->CountItems(); i++)
						{
							const SString *str_tmp = array_tmp->ItemAt(i);
							if(!str_tmp || str_tmp->Length() <= (int32)strlen(items_key)) continue;
							
							if(str_tmp->Length() > (int32)strlen(items_key))
							{
								auto_do_it = false;
								break;
							}
						}

						delete array_tmp;
					}
				}

				if(auto_do_it) ItemSelected(0);
			}
			
			break;
		}
	}
}


void
SGeneralInputModule::ItemSelected(int32 index)
{
	if(select_items)
	{
		const SString *sstr = select_items->ItemAt(index + (npage - 1) * strlen(select_keys));
		SStringArray *array = (sstr != NULL ? sstr->Split('\t') : NULL);
		if(array) sstr = array->ItemAt(0);

		if(sstr && select_items_key)
		{
			const char *cstr = (const char*)select_items_key->ItemAt(index + (npage - 1) * strlen(select_keys));

			if(cstr)
			{
				SString real_key(cstr);

				if(buffer.Length() > real_key.Length())
				{
					char *tmp = (char*)value_key_hash->Lookup(sstr->String());
					if(tmp)
					{
						SString stmp(tmp);
						SStringArray *array_tmp = stmp.Split(' ');
						if(array_tmp)
						{
							for(int32 i = 0; i < array_tmp->CountItems(); i++)
							{
								const SString *str_tmp = array_tmp->ItemAt(i);
								if(!str_tmp || str_tmp->Length() <= real_key.Length()) continue;

								if(buffer.Compare(str_tmp->String(), str_tmp->Length()) == 0)
									real_key = str_tmp->String();
							}

							delete array_tmp;
						}
					}
				}

				buffer.Remove(0, real_key.Length());

				if(!words) words = new SStringArray();
				if(!words_key) words_key = new SStringArray();

				words->AppendItem(*sstr);
				words_key->AppendItem(real_key);

				RefreshDisplay();

				if(buffer.Length() > 0)
					LookForSelectItems();
				else
					EmptySelectItems();
				
				// frequence
				if(freq) freq->ToFront(sstr->String());
			}
		}

		if(array) delete array;
	}
}


void
SGeneralInputModule::RefreshDisplay()
{
	display.SetTo("");

	if(words)
	{
		for(int32 i = 0; i < words->CountItems(); i++)
		{
			const SString *str = words->ItemAt(i);
			if(str) display << str->String();
		}
	}
	
	if(buffer.Length() > 0)
	{
		for(int32 i = 0; i < buffer.Length(); i++)
		{
			char tmp[2];
			tmp[0] = buffer[i];
			tmp[1] = 0;
			
			int32 found = -1;

			if((found = keyname->FindString(tmp)) >= 0)
			{
				display << keymap->ItemAt(found)->String();
			}
			else
			{
				display << tmp;
			}
		}
	}
}


bool
SGeneralInputModule::PageUp()
{
	return KeyFilter(B_PAGE_UP, false);
}


bool
SGeneralInputModule::PageDown()
{
	return KeyFilter(B_PAGE_DOWN, false);
}


bool
SGeneralInputModule::KeyFilter(char key, bool shift_pressed)
{
	if(!IsValid()) return false;

	confirmed = CONFIRMED_NONE;

	if(key == B_LEFT_ARROW || key == B_RIGHT_ARROW)
	{
		if(buffer.Length() == 0 && display.Length() > 0 && words)
		{
			if(key == B_LEFT_ARROW)
			{
				if(nchange < 0)
					nchange = words->CountItems() - 1;
				else if(nchange > 0)
					nchange--;
				else
					nchange = -1;
			}
			else
			{
				if(nchange < 0)
					nchange = 0;
				else if(nchange == words->CountItems() - 1)
					nchange = -1;
				else
					nchange++;
			}

			if(nchange == -1 || nchange == words->CountItems()) nchange = -1;

			if(select_items && nchange >= 0)
			{
				const SString *str = words_key->ItemAt(nchange);
				buffer.SetTo(str->String());
				LookForSelectItems(words->ItemAt(nchange)->String(), str->String(), true);
				buffer.SetTo("");
			}
			else
			{
				EmptySelectItems();
			}

			RefreshDisplay();
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_DELETE && nchange >= 0)
	{
		if(words)
		{
			if(words->CountItems() < 2 ||
			   !words_key || words_key->CountItems() < 1 ||
			   words->CountItems() != words_key->CountItems())
			{
				Reset();
				return true;
			}

			words_key->RemoveItem(nchange);
			words->RemoveItem(nchange);
			if(nchange > words->CountItems() - 1) nchange = -1;

			if(words->CountItems() < 1)
			{
				delete words;		words = NULL;
				delete words_key;	words_key = NULL;
			}

			EmptySelectItems();
			RefreshDisplay();

			return true;
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_BACKSPACE)
	{
		bool original_removed = false;
		if(original.Length() > 0)
		{
			original.Remove(original.Length() - 1, 1);
			original_removed = true;
		}

		if(buffer.Length() > 0 && nchange < 0)
		{
			buffer.Remove(buffer.Length() - 1, 1);
			LookForSelectItems();
			RefreshDisplay();

			return true;
		}
		else if(words)
		{
			if(words->CountItems() < 1 ||
			   !words_key || words_key->CountItems() < 1 ||
			   words->CountItems() != words_key->CountItems())
			{
				Reset();
				return true;
			}

			if(shift_pressed && nchange < 0)
			{
				buffer.SetTo(words_key->LastItem()->String());
				LookForSelectItems();
			}

			int32 pos = (nchange < 0 ? words->CountItems() - 1 : nchange - 1);
			if(pos >= 0)
			{
				words_key->RemoveItem(pos);
				words->RemoveItem(pos);
				if(nchange > 0) nchange--;
			}

			if(words->CountItems() < 1)
			{
				delete words;		words = NULL;
				delete words_key;	words_key = NULL;
			}

			if(!shift_pressed) EmptySelectItems();
			RefreshDisplay();

			return true;
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original) || original_removed)
			return true;
		else
			return false;
	}
	else if(key == B_ESCAPE)
	{
		if(display.Length() > 0 || buffer.Length() > 0)
		{
			if(nchange < 0)
			{
				Reset();
			}
			else
			{
				if(!select_items) nchange = -1;
				EmptySelectItems();
				RefreshDisplay();
			}
			return true;
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_SPACE && shift_pressed)
	{
		bool original_added = false;
		if(original.Length() < SDIC_MAX_ORIGINAL)
		{
			original += ' ';
			original_added = true;
		}

		if(buffer.Length() == 0)
		{
			char *corner_punct = GetCornerOrPunctuation(' ');
			if(corner_punct)
			{
				if(original_added) original.Remove(original.Length() - 1, 1);
				InsertWord(corner_punct, " ");
				delete[] corner_punct;
			}
		}

		if(display.Length() == 0 && buffer.Length() == 0 && original.Length() == 1) original = "";

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if((key == B_SPACE || key == B_ENTER) && !shift_pressed)
	{
		if(key == B_SPACE && original.Length() < SDIC_MAX_ORIGINAL) original += ' ';

		if((!select_items && display.Length() > 0 &&
				buffer.Length() == 0 &&
				(key == B_SPACE || (key == B_ENTER && !enter_to_output_original))) ||
		   (key == B_ENTER && enter_to_output_original))
		{
			EmptySelectItems();
			confirmed = (key == B_SPACE ? CONFIRMED_DISPLAY :
										  (enter_to_output_original ?
												CONFIRMED_ORIGINAL : CONFIRMED_DISPLAY));
			return true;
		}

		if(select_items && nchange < 0)
		{
			ItemSelected(nselect - 1);
			return true;
		}

		if(select_items && nchange >= 0 && words)
sel:	{
			const SString *sstr = select_items->ItemAt((nselect - 1) + (npage - 1) * strlen(select_keys));
			SStringArray *array = (sstr != NULL ? sstr->Split('\t') : NULL);
			if(array) sstr = array->ItemAt(0);

			if(sstr)
			{
				words->ReplaceItem(nchange, sstr->String());

				if(nchange >= 0 && nchange < words->CountItems() - 1) nchange++;
				else if(nchange == words->CountItems() - 1) nchange = -1;

				EmptySelectItems();
				RefreshDisplay();
			}

			if(array) delete array;
			return true;
		}

		if(key == B_SPACE && display.Length() == 0 && buffer.Length() == 0 && original.Length() == 1) original = "";

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_PAGE_UP || key == custom_page_up_key)
	{
		if(select_items)
		{
			if(npage > 1)
			{
				npage--;
				nselect = 1;
			}
			return true;
		}

		if(key != custom_page_up_key)
		{
			if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
				return true;
			else
				return false;
		}
	}
	else if(key == B_PAGE_DOWN || key == custom_page_down_key)
	{
		if(select_items)
		{
			int32 npages = select_items->CountItems() / strlen(select_keys) +
						   (select_items->CountItems() % strlen(select_keys) > 0 ? 1 : 0);
			if(npage < npages)
			{
				npage++;
				nselect = 1;
			}
			return true;
		}

		if(key != custom_page_down_key)
		{
			if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
				return true;
			else
				return false;
		}
	}
	else if(key == B_UP_ARROW)
	{
		if(select_items)
		{
			if(nselect > 1)
				nselect--;
			else
			{
				if(npage > 1)
				{
					npage--;

					int32 npages = select_items->CountItems() / strlen(select_keys) +
								   (select_items->CountItems() % strlen(select_keys) > 0 ? 1 : 0);

					if(npage == npages)
						nselect = (select_items->CountItems() % strlen(select_keys) > 0 ?
										select_items->CountItems() % strlen(select_keys) : strlen(select_keys));
					else
						nselect = strlen(select_keys);
				}
			}

			return true;
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_DOWN_ARROW)
	{
		if(nchange >= 0 && !select_items)
		{
			const SString *str = words_key->ItemAt(nchange);

			buffer.SetTo(str->String());
			LookForSelectItems(words->ItemAt(nchange)->String(), str->String(), true);
			buffer.SetTo("");

			RefreshDisplay();
			
			return true;
		}
		else if(select_items)
		{
			int32 npages = select_items->CountItems() / strlen(select_keys) +
						   (select_items->CountItems() % strlen(select_keys) > 0 ? 1 : 0);
			int32 nselect_max;

			if(npage == npages)
				nselect_max = (select_items->CountItems() % strlen(select_keys) > 0 ?
									select_items->CountItems() % strlen(select_keys) : strlen(select_keys));
			else
				nselect_max = strlen(select_keys);


			if(nselect < nselect_max)
				nselect++;
			else
			{
				if(npage < npages)
				{
					npage++;
					nselect = 1;
				}
			}

			return true;
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_HOME)
	{
		if(buffer.Length() == 0 && display.Length() > 0 && words)
		{
			if(nchange != 0)
			{
				nchange = 0;
				EmptySelectItems();
				RefreshDisplay();
			}
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(key == B_END)
	{
		if(buffer.Length() == 0 && display.Length() > 0 && words)
		{
			if(nchange != -1)
			{
				nchange = -1;
				EmptySelectItems();
				RefreshDisplay();
			}
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}

	SString sstr_select_keys(select_keys);
	SString sstr_keys;
	for(int32 i = 0; i < keyname->CountItems(); i++) sstr_keys << keyname->ItemAt(i)->String();

	char tmp[2];
	tmp[0] = key;
	tmp[1] = 0;

	int32 select_keys_index = sstr_select_keys.FindFirst(tmp);
	int32 keys_index = sstr_keys.FindFirst(tmp);

	if(select_keys_index == B_ERROR && keys_index == B_ERROR)
	{
		bool original_added = false;
		if((isalnum(key) || ispunct(key)) && original.Length() < SDIC_MAX_ORIGINAL)
		{
			original += key;
			original_added = true;
		}

		if(buffer.Length() == 0 && (isalnum(key) || ispunct(key)))
		{
			char *corner_punct = GetCornerOrPunctuation(key);
			if(corner_punct)
			{
				if(original_added) original.Remove(original.Length() - 1, 1);
				InsertWord(corner_punct, tmp);
				delete[] corner_punct;
			}
		}

		if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
			return true;
		else
			return false;
	}
	else if(keys_index == B_ERROR || (select_keys_index != B_ERROR && force_select_key)) // is select keys
	{
		bool original_added = false;
		if((isalnum(key) || ispunct(key)) && original.Length() < SDIC_MAX_ORIGINAL && !force_select_key)
		{
			original += key;
			original_added = true;
		}

		if(select_items && nchange >= 0 && words)
		{
			nselect = select_keys_index + 1;
			goto sel;
		}
		else if(select_items)
		{
			ItemSelected(select_keys_index);
		}
		else if(buffer.Length() == 0 && (isalnum(key) || ispunct(key)))
		{
			char *corner_punct = GetCornerOrPunctuation(key);
			if(corner_punct)
			{
				if(original_added) original.Remove(original.Length() - 1, 1);
				InsertWord(corner_punct, tmp);
				delete[] corner_punct;
			}
		}
	}
	else if(select_keys_index == B_ERROR) // is normal keys
	{
		bool original_added = false;
		if((isalnum(key) || ispunct(key)) && original.Length() < SDIC_MAX_ORIGINAL)
		{
			original += key;
			original_added = true;
		}

		if(nchange >= 0) return true;
		if(buffer.Length() >= SDIC_MAX_BUFFER) return true;

		if(buffer.Length() == 0 && (((isalnum(key) && IsAllCorner()) || (ispunct(key) && IsCJKPunctuation())) || shift_pressed))
		{
			char *corner_punct = GetCornerOrPunctuation(key);
			if(corner_punct)
			{
				if(original_added) original.Remove(original.Length() - 1, 1);
				InsertWord(corner_punct, tmp);
				delete[] corner_punct;
			}
			else
			{
				buffer += key;
				LookForSelectItems(NULL, NULL, false, true);
			}
		}
		else
		{
			buffer += key;
			LookForSelectItems(NULL, NULL, false, true);
		}

		RefreshDisplay();
	}
	else // both
	{
		bool original_added = false;
		if((isalnum(key) || ispunct(key)) && original.Length() < SDIC_MAX_ORIGINAL && !force_select_key)
		{
			original += key;
			original_added = true;
		}

		if(select_items && nchange >= 0 && words)
		{
			nselect = select_keys_index + 1;
			goto sel;
		}

		nchange = -1;

		if(buffer.Length() >= SDIC_MAX_BUFFER)
		{
			ItemSelected(select_keys_index);
		}
		else
		{
			bool is_select = true;

			buffer += key;
			LookForSelectItems(NULL, NULL, true);

			if(select_items)
			{
				const SString *sstr = select_items->ItemAt(0);
				if(sstr && select_items_key)
				{
					char *cstr = (char*)select_items_key->ItemAt(0);
					if(buffer == cstr) is_select = false;
				}
			}

			if(is_select)
			{
				buffer.Remove(buffer.Length() - 1, 1);
				LookForSelectItems();
				ItemSelected(select_keys_index);
			}
			else
			{
				if(buffer.Length() == 1 && (((isalnum(key) && IsAllCorner()) || (ispunct(key) && IsCJKPunctuation())) || shift_pressed))
				{
					char *corner_punct = GetCornerOrPunctuation(key);
					if(corner_punct)
					{
						buffer = "";
						if(original_added) original.Remove(original.Length() - 1, 1);
						InsertWord(corner_punct, tmp);
						delete[] corner_punct;
					}
					else
						LookForSelectItems(NULL, NULL, false, true);
				}
				else
					LookForSelectItems(NULL, NULL, false, true);

				RefreshDisplay();
			}
		}
	}

	if(display.Length() > 0 || buffer.Length() > 0 || (original.Length() > 0 && enter_to_output_original))
		return true;
	else
		return false;
}


const SStringArray*
SGeneralInputModule::SelectItems(int32 *start_item_index, int32 *select_item_index) const
{
	if(start_item_index) *start_item_index = (npage - 1) * strlen(select_keys);
	if(select_item_index) *select_item_index = nselect - 1;

	return select_items;
}


bool
SGeneralInputModule::Selection(int32 *selection_start, int32 *selection_end) const
{
	if((selection_start || selection_end) && words && nchange >= 0)
	{
		int32 offset = 0;
		const SString *str;
		for(int32 i = 0; (str = words->ItemAt(i)) != NULL; i++)
		{
			if(i == nchange)
			{
				if(selection_start) *selection_start = offset;
				if(selection_end) *selection_end = offset + str->Length();
				break;
			}
			
			offset += str->Length();
		}
	}

	return(words && nchange >= 0);
}


const char*
SGeneralInputModule::Original() const
{
	if(enter_to_output_original)
		return (original.Length() > 0  ? original.String() : NULL);
	else
		return NULL;
}


const char*
SGeneralInputModule::Display() const
{
	return (display.Length() > 0 ? display.String() : NULL);
}


const char*
SGeneralInputModule::SelectKeys() const
{
	return select_keys;
}


s_input_confirmed_style
SGeneralInputModule::Confirmed() const
{
	return confirmed;
}


void
SGeneralInputModule::InsertWord(const char *word, const char *key)
{
	if(!word) return;

	if(key)
	{
		int32 going_to_add = MIN(SDIC_MAX_ORIGINAL - original.Length(), (int32)strlen(key));
		if(going_to_add > 0) original.Append(key, going_to_add);
	}

	if(buffer.Length() != 0 || nchange >= 0) return;

	SString tmp;
	if(key) tmp << key;
	tmp << " "; // in order to find nothing by LookForSelectItems()

	if(!words) words = new SStringArray();
	if(!words_key) words_key = new SStringArray();

	words->AppendItem(word);
	words_key->AppendItem(tmp);

	RefreshDisplay();
}


bool
SGeneralInputModule::InitCheck()
{
	return IsValid();
}


bool
SGeneralInputModule::AutoHideSelector()
{
	return(!enter_to_output_original);
}


bool
SGeneralInputModule::ItemChangedFromSelector(int32 index)
{
	if(!select_items) return false;
	if(index < 0) return false;

	int32 npages = select_items->CountItems() / strlen(select_keys) +
				   (select_items->CountItems() % strlen(select_keys) > 0 ? 1 : 0);
	int32 nselect_max;

	if(npage == npages)
		nselect_max = (select_items->CountItems() % strlen(select_keys) > 0 ?
						select_items->CountItems() % strlen(select_keys) : strlen(select_keys));
	else
		nselect_max = strlen(select_keys);

	if(index >= nselect_max) return false;
	if(nselect == index + 1) return false;

	nselect = index + 1;

	return true;
}


bool
SGeneralInputModule::ItemSelectedFromSelector(int32 index)
{
	if(!select_items) return false;
	if(index < 0) return false;

	int32 npages = select_items->CountItems() / strlen(select_keys) +
				   (select_items->CountItems() % strlen(select_keys) > 0 ? 1 : 0);
	int32 nselect_max;

	if(npage == npages)
		nselect_max = (select_items->CountItems() % strlen(select_keys) > 0 ?
						select_items->CountItems() % strlen(select_keys) : strlen(select_keys));
	else
		nselect_max = strlen(select_keys);

	if(index > nselect_max - 1) return false;

	force_select_key = true;
	bool ret = KeyFilter(select_keys[index]);
	force_select_key = false;

	return ret;
}


void
SGeneralInputModule::LocaleChanged(s_input_locale locale)
{
	if(encoding != ENCODING_UNKNOWN && locale == (s_input_locale)encoding)
		SetName(locale_name);
	else
		SetName(name);

	SetMenu(NULL);
	if(menu) delete menu;
	menu = GetMenu();

	if(activated) SetMenu(menu);

	SInputModule::LocaleChanged(locale);
}


void
SGeneralInputModule::ModuleActivated(bool activated)
{
	SetMenu(activated ? menu : NULL);
	SGeneralInputModule::activated = activated;
}


void
SGeneralInputModule::MessageReceivedFromMenu(BMessage *message)
{
	switch(message->what)
	{
		case S_GENERAL_INPUT_MODULE_HELP_MSG:
			{
				if(be_roster->IsRunning("application/x-vnd.Be-TRAK"))
				{
					const char *string = (help ? help : _("file:///boot/beos/documentation/BeCJK/General/index.html"));
					be_roster->Launch("text/html", 1, (char**)&string);
				}
			}
			break;

		case S_GENERAL_INPUT_MODULE_ABOUT_MSG:
			{
				if(!version && !copyright) break;

				BString str;
				str << _("Name:") << " " << (Name() ? Name() : _("Unknown")) << "\n\n";
				if(version) str << _("Version:") << " " << version << "\n\n";
				if(copyright)
				{
					SString sstr(copyright);
					SStringArray *array = sstr.Split("[ENTER]");
					if(array)
					{
						str << _("Copyright:") << "\n";
						for(int32 i = 0; i < array->CountItems(); i++)
							str << array->ItemAt(i)->String() << "\n";
						delete array;
					}
				}
				
				BAlert *alert = new BAlert(_("About..."), str.String(), _("OK"));
				alert->Go(NULL);
			}
			break;
		
		case S_GENERAL_INPUT_MODULE_WORDS_FIRST_MSG:
			{
				words_first = !words_first;
				SetMenu(NULL);
				if(menu) delete menu;
				menu = GetMenu();
				SetMenu(menu);
				WriteHeaderToTable();
			}
			break;

		case S_GENERAL_INPUT_MODULE_AUTO_SELECT_MSG:
			{
				auto_select = !auto_select;
				SetMenu(NULL);
				if(menu) delete menu;
				menu = GetMenu();
				SetMenu(menu);
				WriteHeaderToTable();
			}
			break;

		case S_GENERAL_INPUT_MODULE_AUTO_FIX_UP_MSG:
			{
				auto_fix_up = !auto_fix_up;
				SetMenu(NULL);
				if(menu) delete menu;
				menu = GetMenu();
				SetMenu(menu);
				WriteHeaderToTable();
			}
			break;

		case S_GENERAL_INPUT_MODULE_OUTPUT_ORIGINAL_MSG:
			{
				enter_to_output_original = !enter_to_output_original;
				SetMenu(NULL);
				if(menu) delete menu;
				menu = GetMenu();
				SetMenu(menu);
				WriteHeaderToTable();
			}
			break;
		
		case S_GENERAL_INPUT_MODULE_FREQ_MSG:
			{
				adjust_order_by_frequence = !adjust_order_by_frequence;
				if(adjust_order_by_frequence)
				{
					if(!freq) freq = new SGeneralInputModuleFrequence();
				}
				else
				{
					if(freq) delete freq;
					freq = NULL;
				}

				SetMenu(NULL);
				if(menu) delete menu;
				menu = GetMenu();
				SetMenu(menu);
				WriteHeaderToTable();
			}
			break;
	}
}


BMenu*
SGeneralInputModule::GetMenu()
{
	BMenu *the_menu = new BMenu("");
	if(the_menu)
	{
		the_menu->SetTriggersEnabled(false);
		if(allow_changed)
		{
			BMessage *msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_WORDS_FIRST_MSG);
			BMenuItem *menu_item = new BMenuItem(_("Words always display in the front"), msg);
			if(words_first) menu_item->SetMarked(true);
			the_menu->AddItem(menu_item);

			msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_AUTO_SELECT_MSG);
			menu_item = new BMenuItem(_("Select the word automatically"), msg);
			if(auto_select) menu_item->SetMarked(true);
			the_menu->AddItem(menu_item);

			msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_AUTO_FIX_UP_MSG);
			menu_item = new BMenuItem(_("Fix up typing"), msg);
			if(auto_fix_up) menu_item->SetMarked(true);
			the_menu->AddItem(menu_item);

			msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_OUTPUT_ORIGINAL_MSG);
			menu_item = new BMenuItem(_("Output original when ENTER pressed"), msg);
			if(enter_to_output_original) menu_item->SetMarked(true);
			the_menu->AddItem(menu_item);

			msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_FREQ_MSG);
			menu_item = new BMenuItem(_("Adjust the order by frequence of typing"), msg);
			if(adjust_order_by_frequence) menu_item->SetMarked(true);
			the_menu->AddItem(menu_item);
		}

		if(allow_changed) the_menu->AddSeparatorItem();

		BMessage *msg = new BMessage(S_MODULE_FROM_MENU_MSG);
		msg->AddPointer("module", this);
		msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_HELP_MSG);
		BMenuItem *menu_item = new BMenuItem(_("Help..."), msg);
		the_menu->AddItem(menu_item);

		if(version || copyright)
		{
			msg = new BMessage(S_MODULE_FROM_MENU_MSG);
			msg->AddPointer("module", this);
			msg->AddInt32("module_what", S_GENERAL_INPUT_MODULE_ABOUT_MSG);
			menu_item = new BMenuItem(_("About..."), msg);
			the_menu->AddItem(menu_item);
		}
	}
	
	return the_menu;
}


void
SGeneralInputModule::WriteHeaderToTable()
{
	if(be_roster->IsRunning("application/x-vnd.Be-TRAK") && table_fname && IsValid())
	{
		BFile bfile;
		if(bfile.SetTo(table_fname, B_READ_WRITE) != B_OK) return;

		if(bfile.Seek(strlen(s_dictionary_magic) * sizeof(char), SEEK_SET) == B_ERROR) return;

		s_dictionary_header header;
		if(bfile.Read(&header, sizeof(header)) <= 0) return;
		if(header.dictionary_version != 1) return;

		header.words_first = words_first;
		header.auto_select = auto_select;
		header.auto_fix_up = auto_fix_up;
		header.enter_to_output_original = enter_to_output_original;
		header.adjust_order_by_frequence = adjust_order_by_frequence;

		if(bfile.Seek(strlen(s_dictionary_magic) * sizeof(char), SEEK_SET) == B_ERROR) return;
		bfile.Write(&header, sizeof(header));
	}
}

