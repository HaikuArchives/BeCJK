#include <stdio.h>
#include <iostream.h>

#include <besavager/Settings.h>


void show_usage(const char *prog)
{
	fprintf(stdout, "Usage: %s --read[--write] filename\n", prog);
}


void do_read(const char *fname)
{
	SSettings settings(fname);

	bool bl;
	if(settings.ReadBool("Bool test 1", &bl))
		cout << "Bool test 1[" << (bl ? "true" : "false") << "]\n";
	if(settings.ReadBool("Bool test 2", &bl))
		cout << "Bool test 2[" << (bl ? "true" : "false") << "]\n";

	int32 n;
	if(settings.ReadInt32("Int32 test 1", &n))
		cout << "Int32 test 1[" << n << "]\n";
	if(settings.ReadInt32("Int32 test 2", &n))
		cout << "Int32 test 2[" << n << "]\n";

	char *string = NULL;
	if(settings.ReadString("String test 1", &string))
	{
		cout << "String test 1[" << string << "]\n";
		delete[] string;
		string = NULL;
	}

	if(settings.ReadString("String test 2", &string))
	{
		cout << "String test 2[" << string << "]\n";
		delete[] string;
		string = NULL;
	}
}


void do_write(const char *fname)
{
	SSettings settings(fname);

	settings.WriteBool("Bool test 1", true);
	settings.WriteBool("Bool test 2", false);

	settings.WriteInt32("Int32 test 1", 3);
	settings.WriteInt32("Int32 test 2", 10);

	settings.WritePoint("Point test 1", BPoint(111.1, 222.2));
	settings.WritePoint("Point test 2", BPoint(222.2, 333.3));

	settings.WriteRect("Rect test 1", BRect(111.1, 222.2, 333.3, 444.4));
	settings.WriteRect("Rect test 2", BRect(222.2, 333.3, 444.4, 555.5));

	settings.WriteString("String test 1", "TEST 1.....");
	settings.WriteString("String test 2", "******** 2 Test **********");
}


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		show_usage(argv[0]);
		return -1;
	}

	if(strcmp(argv[1], "--read") == 0)
		do_read(argv[2]);
	else if(strcmp(argv[1], "--write") == 0)
		do_write(argv[2]);
	else
		show_usage(argv[0]);

	return 0;
}
