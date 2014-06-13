#include <string.h>
#include <stdio.h>

#include <besavager/HashTable.h>

static void foreach(void *key, void *value, void *user_data)
{
	fprintf(stdout, "KEY: %s\t\tVALUE: %s\n", (char*)key, (char*)value);
}


int main(int argc, char** argv)
{
	SStringHash *hash = new SStringHash();

	hash->InsertString("First Test", "Value 1");
	hash->InsertString("Second Test", "Value 2");

#if 0
	/* the action below will put the application to the debugger because of error in the delete function */
	hash->Insert((void*)"yes", (void*)"no");
#endif

	void *orig_key = NULL;
	void *orig_value = NULL;
	
	if(hash->Lookup("First Test", &orig_key, &orig_value))
		fprintf(stdout, "KEY: %s\tVALUE: %s\n", (char*)orig_key, (char*)orig_value);
	else
		fprintf(stdout, "Nothing found\n");

	hash->DoForeach((SHashTableForeachFunction)foreach, NULL);

	delete hash;

	return 0;
}
