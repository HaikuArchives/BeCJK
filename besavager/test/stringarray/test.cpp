#include <stdio.h>
#include <iostream.h>
#include <besavager/StringArray.h>


int main(int argc, char **argv)
{
	printf("IsNumber(\"+0.12\") = %s\n", SString("+0.12").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"-0.12\") = %s\n", SString("-0.12").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"+.12\") = %s\n", SString("+.12").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"-.12\") = %s\n", SString("-.12").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\".12\") = %s\n", SString(".12").IsNumber() ? "TRUE" : "FALSE");

	printf("IsNumber(\"+1\") = %s\n", SString("+1").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"-1\") = %s\n", SString("-1").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"1\") = %s\n", SString("1").IsNumber() ? "TRUE" : "FALSE");
	printf("IsNumber(\"-0.abcd12\") = %s\n", SString("-0.abcd12").IsNumber() ? "TRUE" : "FALSE");

	printf("IsInteger(\"+1.000\") = %s\n", SString("+1.000").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"-1.000\") = %s\n", SString("-1.000").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"+1\") = %s\n", SString("+1").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"-1\") = %s\n", SString("-1").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"+.000\") = %s\n", SString("+.000").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"-.000\") = %s\n", SString("-.000").IsInteger() ? "TRUE" : "FALSE");
	printf("IsInteger(\"1.3200\") = %s\n", SString("1.3200").IsInteger() ? "TRUE" : "FALSE");

	double test_value = 0;

	if(!SString("-.0004235").GetDouble(&test_value))
	{
		cerr << "SString(\"-.0004235\").GetDouble() ERROR\n";
		exit(-1);
	}
	printf("SString(\"-.0004235\").GetDouble = %g\n", test_value);

	if(!SString("+400.23567444").GetDouble(&test_value))
	{
		cerr << "SString(\"+400.23567444\").GetDouble() ERROR\n";
		exit(-1);
	}
	printf("SString(\"+400.23567444\").GetDouble = %g\n", test_value);

	printf("\nStarting to split...\n");
	printf("\"Hello.\\nIt is a example to split SString to the strings array.\\n\\nSometimes, the array is empty!\\nIt is end, thanks.\"\n");

	SString astring("Hello.\nIt is a example to split SString to the strings array.\n\nSometimes, the array is empty!\nIt is end, thanks.");
	SStringArray *array = astring.Split("\n");

	if(array)
	{
		const SString *tmp;
		int32 i = 0;
		while((tmp = array->ItemAt(i)) != NULL)
		{
			printf("Line: %li  Content: %s\n", i, tmp->String() ? tmp->String() : "[EMPTY]");
			i++;
		}

		delete array;
	}

	return 0;
}
