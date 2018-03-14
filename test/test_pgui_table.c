#include "pgui_table.h"

int main(int argc, const char *argv[])
{
	printf("hello world\n");
	struct ptable * table = pgui_table_create(PGUI_TABLE_STRINGINDEXED);
	pgui_table_set(table, "name", (void *)"dmx", PGUI_TABLE_POINTER);
	char * name = (char *)pgui_table_get(table, "name");
	printf("name is :%s\n", name);
	return 0;
}
