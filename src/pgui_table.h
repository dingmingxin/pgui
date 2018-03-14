 
#ifndef __PGUI_TABLE_H 
#define __PGUI_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

enum ptable_indextypes
{
	PGUI_TABLE_POINTERINDEXED = 10, 
	PGUI_TABLE_STRINGINDEXED        
};

enum ptable_types
{
	PGUI_TABLE_POINTER,     
	PGUI_TABLE_STRING,      
	PGUI_TABLE_FUNCPOINTER  
};


typedef void (*pfunc)(void);

struct ptable;

struct ptable *pgui_table_create(enum ptable_indextypes indextype);
struct ptable *pgui_table_create_sized(enum ptable_indextypes indextype, unsigned int initialsizeindex);
void pgui_table_destroy(struct ptable *it);
void pgui_table_clear(struct ptable *it);
int pgui_table_count(struct ptable *it);
void pgui_table_set(struct ptable *it, const char *key, void *value, enum ptable_types itemtype);
void pgui_table_set_func(struct ptable *it, const char *key, pfunc func);
void *pgui_table_get(struct ptable *it, const char *key);
pfunc pgui_table_get_func(struct ptable *it, const char *key, void **value);
void *pgui_table_get_typed(struct ptable *it, const char *key, enum ptable_types *itemtype);
void pgui_table_remove(struct ptable *it, const char *key);
char *pgui_table_first(struct ptable *it);
char *pgui_table_next(struct ptable *it);
void *pgui_table_get_curr(struct ptable *it);
int pgui_table_get_currtype(struct ptable *it);
void pgui_table_set_curr(struct ptable *it, void* value, enum ptable_types itemtype);
char *pgui_table_remove_curr(struct ptable *it);


#ifdef __cplusplus
}
#endif

#endif

