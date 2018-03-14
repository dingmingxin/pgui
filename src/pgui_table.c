
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "pgui_table.h"
#include "pgui_str.h"

static const unsigned int itable_max_table_size_index = 8;
static const unsigned int itable_hash_table_size[] = { 31, 101, 401, 1601, 4001, 8009, 16001, 32003, 64007 };
static const unsigned int itable_resize_limit = 2;
static const unsigned int itable_item_grow = 5;

struct ptable_context {
	unsigned int entry_index;  
	unsigned int item_index;   
};

struct ptable_key {
	unsigned long key_index;  
	const char * key_str;
};


struct ptable_item {
	enum ptable_types item_type;
	struct ptable_key key;
	void * value;
};

struct ptable_entry {
	unsigned int next_item_index;
	unsigned int item_size;
	struct ptable_item * items;
};

struct ptable {
	unsigned int entries_size;
	unsigned int number_of_entries;
	unsigned int table_size_index;  
	enum ptable_indextypes index_type;  
	struct ptable_entry * entries;
	struct ptable_context context;
};


static void ptable_free_item_array(enum ptable_indextypes index_type, unsigned int next_free_index, struct ptable_item *items);
static unsigned int ptable_get_entry_index(struct ptable *it, const char *key, unsigned long *key_index);
static unsigned int ptable_find_item(struct ptable *it, const char *key, struct ptable_entry **entry, unsigned int *item_index, unsigned long *key_index);
static unsigned int ptable_resize(struct ptable *it);
static void ptable_add(struct ptable *it, struct ptable_key *key, void *value, enum ptable_types item_type);
static void ptable_update_array_size(struct ptable_entry *entry);   

#ifdef DEBUGTABLE
static void ptable_show_statistics(struct ptable *it);
static void ptable_check_duplicated(
		struct ptable_item *item,
		unsigned int next_item_index, 
		const char *key, 
		unsigned long key_index);
#endif


struct ptable *
pgui_table_create(enum ptable_indextypes index_type) {
	return pgui_table_create_sized(index_type, 1);  
}


struct ptable *
pgui_table_create_sized(enum ptable_indextypes index_type, unsigned int initial_size_index) {
	struct ptable *it = (struct ptable *)malloc(sizeof(struct ptable));
//	PGUI_ASSERT(it!=NULL);
	if (!it)
		return 0;

	if (initial_size_index > itable_max_table_size_index)
		initial_size_index = itable_max_table_size_index;

	it->entries_size    = itable_hash_table_size[initial_size_index];
	it->table_size_index  = initial_size_index;
	it->number_of_entries = 0;
	it->index_type       = index_type;

	it->entries = (struct ptable_entry *)malloc(it->entries_size * sizeof(struct ptable_entry));
//	PGUI_ASSERT(it->entries!=NULL);
	if (!it->entries) {
		free(it);
		return 0;
	}

	memset(it->entries, 0, it->entries_size * sizeof(struct ptable_entry));

	it->context.entry_index = (unsigned int)-1;
	it->context.item_index = (unsigned int)-1;

	return it;
}

void 
pgui_table_clear(struct ptable *it) {
	unsigned int i;

	if (!it)
		return;

	for (i = 0; i < it->entries_size; i++)
	{
		struct ptable_entry *entry = &(it->entries[i]);
		if (entry->items) {
			ptable_free_item_array(it->index_type, entry->next_item_index, entry->items);
			entry->items = NULL;
		}
	}

	it->number_of_entries = 0;

	memset(it->entries, 0, it->entries_size * sizeof(struct ptable_entry));

	it->context.entry_index = (unsigned int)-1;
	it->context.item_index = (unsigned int)-1;
}

void 
pgui_table_destroy(struct ptable *it) {
	if (!it)
		return;

#ifdef DEBUGTABLE
	ptable_show_statistics(it);
#endif

	pgui_table_clear(it);

	if (it->entries) {
		free(it->entries);
		it->entries = NULL;
	}

	free(it);
}

int 
pgui_table_count(struct ptable *it) {
//	PGUI_ASSERT(it!=NULL);
	if (!it)
		return 0;
	return it->number_of_entries;
}

void 
pgui_table_set_func(struct ptable *it, const char *key, pfunc func) {
	pgui_table_set(it, key, (void*)func, PGUI_TABLE_FUNCPOINTER); /* type cast from function pointer to void* */
}

void 
pgui_table_set(struct ptable *it, const char *key, void *value, enum ptable_types item_type) {
	unsigned int  item_index,
				  item_found;
	unsigned long key_index;
	struct ptable_entry  *entry;
	struct ptable_item   *item;
	void         *v;

//	PGUI_ASSERT(it!=NULL);
//	PGUI_ASSERT(key!=NULL);
	if (!it || !key || !value)
		return;

	item_found = ptable_find_item(it, key, &entry, &item_index, &key_index);

#ifdef DEBUGTABLE
	if (it->index_type == PGUI_TABLE_STRINGINDEXED)
		ptable_check_duplicated(&(entry->items[0]), entry->next_item_index, key, key_index);
#endif

	if (!item_found) {
		if (ptable_resize(it)) {
			ptable_find_item(it, key, &entry, &item_index, &key_index);
		}

		ptable_update_array_size(entry);

		if (item_type == PGUI_TABLE_STRING)
			v = pgui_str_dup(value);
		else
			v = value;

		item = &(entry->items[entry->next_item_index]);

		item->item_type     = item_type;
		item->key.key_index = key_index;
		item->key.key_str   = it->index_type == PGUI_TABLE_STRINGINDEXED? pgui_str_dup(key) : key;
		item->value        = v;

		entry->next_item_index++;
		it->number_of_entries++;
	}
	else {
		void *v;
		item = &(entry->items[item_index]);

		if (item_type == PGUI_TABLE_STRING && item->item_type == PGUI_TABLE_STRING) {
			if (pgui_str_equal((char*)item->value, (char*)value))
				return;
		}

		if (item_type == PGUI_TABLE_STRING)
			v = pgui_str_dup(value);
		else
			v = value;

		if (item->item_type == PGUI_TABLE_STRING)
			free(item->value);

		item->value    = v;
		item->item_type = item_type;
	}
}

static void 
ptable_remove_item(struct ptable *it, struct ptable_entry *entry, unsigned int item_index) {
	struct ptable_item   *item;
	unsigned int  i;

	item = &(entry->items[item_index]);

	if (it->index_type == PGUI_TABLE_STRINGINDEXED) {
		free((void *)item->key.key_str);
		item->key.key_str = NULL;
	}

	if (item->item_type == PGUI_TABLE_STRING) {
		free(item->value);
		item->value = NULL;
	}

	/* re-order the remaining items */
	for (i = item_index; i < entry->next_item_index-1; i++)
		entry->items[i] = entry->items[i+1];

	/* clear the released item */
	memset(entry->items + entry->next_item_index-1, 0, sizeof (struct ptable_item));

	entry->next_item_index--;
	it->number_of_entries--;
}

void 
pgui_table_remove(struct ptable *it, const char *key) {
	unsigned int item_found,
				 item_index;
	unsigned long key_index;
	struct ptable_entry  *entry;

//	PGUI_ASSERT(it!=NULL);
//	PGUI_ASSERT(key!=NULL);
	if (!it || !key)
		return;

	item_found = ptable_find_item(it, key, &entry, &item_index, &key_index);
	if (item_found)
		ptable_remove_item(it, entry, item_index);
}

void *
pgui_table_get(struct ptable *it, const char *key) {
	unsigned int  item_found,
				  item_index;
	unsigned long key_index;
	struct ptable_entry  *entry;
	void         *value = 0;

//	PGUI_ASSERT(it!=NULL);
//	PGUI_ASSERT(key!=NULL);
	if (!it || !key)
		return 0;

	item_found = ptable_find_item(it, key, &entry, &item_index, &key_index);
	if (item_found)
		value = entry->items[item_index].value;

	return value;
}

pfunc 
pgui_table_get_func(struct ptable *it, const char *key, void **value) {
	enum ptable_types item_type = PGUI_TABLE_POINTER;
	*value = pgui_table_get_typed(it, key, &item_type);
	if (item_type == PGUI_TABLE_FUNCPOINTER)
		return (pfunc)(*value); /* type cast from void* to function pointer */
	else
		return (pfunc)0;
}

void *
pgui_table_get_typed(struct ptable *it, const char *key, enum ptable_types *item_type) {
	unsigned int  item_found,
				  item_index;
	unsigned long key_index;
	struct ptable_entry  *entry;
	void         *value = 0;

//	PGUI_ASSERT(it!=NULL);
//	PGUI_ASSERT(key!=NULL);
	if (!it || !key)
		return 0;

	item_found = ptable_find_item(it, key, &entry, &item_index, &key_index);
	if (item_found) {
		value = entry->items[item_index].value;
		if (item_type) 
			*item_type = entry->items[item_index].item_type;
	}

	return value;
}

void 
pgui_table_set_curr(struct ptable *it, void* value, enum ptable_types item_type) {
	void* v;
	struct ptable_item *item;

//	PGUI_ASSERT(it!=NULL);
	if (!it || it->context.entry_index == (unsigned int)-1
			|| it->context.item_index == (unsigned int)-1)
		return;

	item = &(it->entries[it->context.entry_index].items[it->context.item_index]);

	if (item_type == PGUI_TABLE_STRING && item->item_type == PGUI_TABLE_STRING) {
		/* this will avoid to free + alloc of a new pointer */
		if (pgui_str_equal((char*)item->value, (char*)value))
			return;
	}

	if (item_type == PGUI_TABLE_STRING)
		v = pgui_str_dup(value);
	else
		v = value;

	if (item->item_type == PGUI_TABLE_STRING)
		free(item->value);

	item->value    = v;
	item->item_type = item_type;
}

void* 
pgui_table_get_curr(struct ptable *it) {
//	PGUI_ASSERT(it!=NULL);
	if (!it || it->context.entry_index == (unsigned int)-1
			|| it->context.item_index == (unsigned int)-1)
		return 0;

	return it->entries[it->context.entry_index].items[it->context.item_index].value;
}

int 
pgui_table_get_curr_type(struct ptable *it) {
//	PGUI_ASSERT(it!=NULL);
	if (!it || it->context.entry_index == (unsigned int)-1
			|| it->context.item_index == (unsigned int)-1)
		return -1;

	return it->entries[it->context.entry_index].items[it->context.item_index].item_type;
}

char *
pgui_table_first(struct ptable *it) {
	unsigned int entry_index;

//	PGUI_ASSERT(it!=NULL);
	if (!it)
		return 0;

	it->context.entry_index = (unsigned int)-1;
	it->context.item_index = (unsigned int)-1;

	/* find the first used entry */
	for (entry_index = 0; entry_index < it->entries_size; entry_index++) {
		if (it->entries[entry_index].next_item_index > 0) {
			it->context.entry_index = entry_index;
			it->context.item_index = 0;
			return (char*)it->entries[entry_index].items[0].key.key_str;
		}
	}

	return 0;
}


char *
pgui_table_next(struct ptable *it) {
	unsigned int entry_index;

//	PGUI_ASSERT(it!=NULL);
	if (!it || it->context.entry_index == (unsigned int)-1
			|| it->context.item_index == (unsigned int)-1)
		return 0;

	if (it->context.item_index + 1 < it->entries[it->context.entry_index].next_item_index) {
		/* key in the current entry */
		it->context.item_index++;
		return (char*)it->entries[it->context.entry_index].items[it->context.item_index].key.key_str;
	}
	else {
		/* find the next used entry */
		for (entry_index = it->context.entry_index+1; entry_index < it->entries_size; entry_index++) {
			if (it->entries[entry_index].next_item_index > 0) {
				it->context.entry_index = entry_index;
				it->context.item_index = 0;
				return (char*)it->entries[entry_index].items[0].key.key_str;
			}
		}
	}

	return 0;
}

char *
pgui_table_remove_curr(struct ptable *it) {
	char* key;
	unsigned int entry_index;
	struct ptable_entry  *entry;
	unsigned int item_index;

//	PGUI_ASSERT(it!=NULL);
	if (!it || it->context.entry_index == (unsigned int)-1
			|| it->context.item_index == (unsigned int)-1)
		return 0;

	entry = &it->entries[it->context.entry_index];
	item_index = it->context.item_index;

	if (it->context.item_index + 1 < it->entries[it->context.entry_index].next_item_index) {
		/* key in the current entry */
		it->context.item_index++;
		key = (char*)it->entries[it->context.entry_index].items[it->context.item_index].key.key_str;

		ptable_remove_item(it, entry, item_index);
		return key;
	}
	else {
		/* find the next used entry */
		for (entry_index = it->context.entry_index+1; entry_index < it->entries_size; entry_index++) {
			if (it->entries[entry_index].next_item_index > 0) {
				it->context.entry_index = entry_index;
				it->context.item_index = 0;
				key = (char*)it->entries[entry_index].items[0].key.key_str;

				ptable_remove_item(it, entry, item_index);
				return key;
			}
		}
	}

	return 0;
}


/********************************************/
/*           Private functions              */
/********************************************/


static void 
ptable_free_item_array(enum ptable_indextypes index_type, unsigned int next_free_index, struct ptable_item *items) {
	unsigned int i;


//	PGUI_ASSERT(items!=NULL);
	if (!items)
		return;

	if (index_type == PGUI_TABLE_STRINGINDEXED) {
		for (i = 0; i < next_free_index; i++) {
			free((void *)(items[i].key.key_str));
			items[i].key.key_str = NULL;
		}
	}

	for (i = 0; i < next_free_index; i++) {
		if (items[i].item_type == PGUI_TABLE_STRING) {
			free(items[i].value);
			items[i].value = NULL;
		}
	}

	free(items);
}


static unsigned int 
ptable_get_entry_index(struct ptable *it, const char *key, unsigned long *key_index) {
	if (it->index_type == PGUI_TABLE_STRINGINDEXED) {
		register unsigned int checksum = 0;

		while (*key) {
			checksum *= 31;
			checksum += *key;
			key++;
		}

		*key_index = checksum;            
	}
	else {
		*key_index = (unsigned long)key;   
	}

	return (unsigned int)((*key_index) % it->entries_size);
}

#ifdef DEBUGTABLE
static void 
ptable_check_duplicated(struct ptable_item *item, unsigned int next_item_index, const char *key, 
		unsigned long key_index) {
	unsigned int i;
	for (i = 0; i < next_item_index; i++, item++) {
		if (!pgui_str_equal((char*)item->key.key_str, (char*)key) && 
				item->key.key_index == key_index) {
			fprintf(stderr, "#ERROR# Duplicated key index (%ld): %s %s \n", key_index, 
					(char*)item->key.key_str, 
					(char*)key);
		}
	}
}
#endif

static unsigned int 
ptable_find_item(struct ptable *it, const char *key, struct ptable_entry **entry, unsigned int *item_index, unsigned long *key_index) {
	unsigned int entry_index,
				 item_found,
				 i;
	struct ptable_item  *item;

	entry_index = ptable_get_entry_index(it, key, key_index);

	*entry = &(it->entries[entry_index]);

	item = &((*entry)->items[0]);
	for (i = 0; i < (*entry)->next_item_index; i++, item++) {
		if (it->index_type == PGUI_TABLE_STRINGINDEXED)
			item_found = item->key.key_index == *key_index; 
		else
			item_found = item->key.key_str == key;

		if (item_found) {
			*item_index = i;
			return 1;
		}
	}

	*item_index = i;
	return 0;
}

static void 
ptable_update_array_size(struct ptable_entry *entry) {
	if (entry->next_item_index >= entry->item_size) {
		unsigned int new_size;

		new_size = entry->item_size + itable_item_grow;

		entry->items = (struct ptable_item *)realloc(entry->items, new_size * sizeof(struct ptable_item));
//		PGUI_ASSERT(entry->items!=NULL);
		if (!entry->items)
			return;

		memset(entry->items + entry->item_size, 0, itable_item_grow * sizeof(struct ptable_item));

		entry->item_size = new_size;
	}
}

static void 
ptable_add(struct ptable *it, struct ptable_key *key, void *value, enum ptable_types item_type) {
	unsigned int entry_index;
	unsigned long key_index;
	struct ptable_entry *entry;
	struct ptable_item* item;

	entry_index = ptable_get_entry_index(it, key->key_str, &key_index);

	entry = &(it->entries[entry_index]);
	ptable_update_array_size(entry);

	item = &(entry->items[entry->next_item_index]);
	item->item_type     = item_type;
	item->key.key_index = key_index;
	item->key.key_str   = key->key_str;
	item->value        = value;

	entry->next_item_index++;
	it->number_of_entries++;
}

static unsigned int 
ptable_resize(struct ptable *it) {
	unsigned int   new_size_index,
				   entry_index,
				   i;
	struct ptable        *new_table;
	struct ptable_entry   *entry;
	struct ptable_item    *item;

	if (it->number_of_entries == 0 ||
			it->table_size_index >= itable_max_table_size_index ||
			it->entries_size / it->number_of_entries >= itable_resize_limit)
		return 0;

	new_size_index = it->table_size_index + 1;
	new_table = pgui_table_create_sized(it->index_type, new_size_index);

	for (entry_index = 0; entry_index < it->entries_size; entry_index++) {
		entry = &(it->entries[entry_index]);

		if (entry->items) {
			item = &(entry->items[0]);

			for (i = 0; i < entry->next_item_index; i++, item++) {
				ptable_add(new_table, &(item->key), item->value, item->item_type);
			}     

			free(entry->items);
			entry->items = NULL;
		}
	}

	free(it->entries);

	it->entries_size    = new_table->entries_size;
	it->table_size_index  = new_table->table_size_index;
	it->number_of_entries = new_table->number_of_entries;
	it->entries         = new_table->entries;

	free(new_table);

	return 1;
}

#ifdef DEBUGTABLE
static void 
ptable_show_statistics(struct ptable *it) {
	unsigned int nof_slots        = 0;
	unsigned int nof_keys         = 0;
	double optimal_nof_keys_per_slot = 0.0;
	unsigned int nof_slots_with_more_keys = 0;
	unsigned int nof_slots_with_less_keys = 0;

	unsigned int entry_index;
	fprintf(stderr, "\n--- HASH TABLE STATISTICS ---\n");
	if (!it) {
		fprintf(stderr, "no hash table...\n");
		return;
	}

	nof_slots = it->entries_size;
	nof_keys  = it->number_of_entries;
	optimal_nof_keys_per_slot = (double)nof_keys / (double)nof_slots;

	for (entry_index = 0; entry_index < it->entries_size; entry_index++) {
		struct ptable_entry *entry = &(it->entries[entry_index]);

		if (entry->next_item_index > optimal_nof_keys_per_slot + 3)
			nof_slots_with_more_keys++;
		else if (entry->next_item_index < optimal_nof_keys_per_slot - 3)
			nof_slots_with_less_keys++;
	}

	fprintf(stderr, "Number of slots: %d\n", nof_slots);
	fprintf(stderr, "Number of keys: %d\n", nof_keys);
	fprintf(stderr, "Optimal number of keys per slot: %f\n", optimal_nof_keys_per_slot);
	fprintf(stderr, "Number of slots with much more keys: %d\n", nof_slots_with_more_keys);
	fprintf(stderr, "Number of slots with far less keys: %d\n", nof_slots_with_less_keys);
	fprintf(stderr, "\n");
}
#endif
