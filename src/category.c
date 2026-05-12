#include "../include/category.h"
#include "lilc/eq.h"
#include "lilc/hash.h"
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <lilc/hashmap.h>

void category_init(Category *category, const char *category_name) {
  category->category_name = category_name;
  category->entries = hashmap_new(char *, u64, &HEAP_ALLOCATOR, str_ptrv_hash, str_ptrv_eq, NULL);
}

void category_add(Category *category, const char *name, u64 elem_id) {
  CategoryEntry *entry = hashmap_value(&category->entries, &name);
  if (entry == NULL) {
    CategoryEntry new_entry = {.elems = array_new(u64, &HEAP_ALLOCATOR) };
    hashmap_insert(&category->entries, &name, &new_entry);
    entry = &new_entry;
  }

  array_add(entry->elems, elem_id);
}

bool category_contains(Category *category, const char *name, u64 elem_id) {
  u64 *elem_ids = category_elem_ids(category, name);
  if (elem_ids == NULL) {
    return false;
  }

  u64 *id;
  array_foreach(elem_ids, id) {
    if (*id == elem_id) {
      return true;
    }
  }
  return false;
}

u64 *category_elem_ids(Category *category, const char *name) {
  CategoryEntry *entry = hashmap_value(&category->entries, &name);
  if (entry == NULL) {
    return NULL;
  }

  return entry->elems;
}

char **categories_by_elem_id(Category *category, u64 id, Allocator *allocator) {
  char **names = array_new(char *, allocator);

  hashmap_foreach(&category->entries, char **key, CategoryEntry *val, {
    u64 *elem;
    array_foreach(val->elems, elem) {
      if (*elem == id) {
        array_add(names, *key);
        continue;
      }
    }
  });

  return names;
}

bool is_category_of_elem_id(Category *category, u64 id, const char *name) {
  hashmap_foreach(&category->entries, char **key, CategoryEntry *val, {
    u64 *elem;
    array_foreach(val->elems, elem) {
      if (*elem == id) {
        return true;
      }
    }
  });

  return false;
}
