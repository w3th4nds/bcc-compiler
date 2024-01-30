#include "include/hashmap.h"

int gethash(char *id)
{
  // index 0 reserved for global scope
  if (id == NULL) return 0;
  int idx = 0, acc = 0;
  while (id[idx] != '\0') acc += id[idx++];
  return (acc % (HASHMAP_SZ-1)) + 1;
}

Bucket_t *create_bucket(char *scope_id)
{
  Bucket_t *bucket = calloc(1, sizeof(Bucket_t));
  bucket->scope_id = scope_id;
  bucket->specs_type = NO_TYPE;
  bucket->params = NULL;
  bucket->symtab = init_symtab();
  return bucket;
}

Bucket_t **create_hashmap(void)
{
  Bucket_t **hashmap = calloc(HASHMAP_SZ, sizeof(Bucket_t *));
  hashmap[0] = create_bucket(NULL);
  return hashmap;
}

// add a symbol table entry to current symtab
void hashmap_add(Bucket_t **hashmap, char *scope_id, SymtabEntry_t *symtab_entry)
{
  int key = gethash(scope_id);
  Bucket_t *bucket = hashmap[key];
  if (key != 0) {
    for (;;) {
      if (bucket == NULL) error_exit("hashmap_add() - bucket = NULL, current scope id not found in hashmap\n");
      if (strcmp(bucket->scope_id, scope_id) == 0) break;
      bucket = bucket->next;
    }
  }
  symtab_entry->offset = bucket->symtab->offset;
  bucket->symtab->offset += symtab_entry->size;
  list_push(bucket->symtab->list, symtab_entry);
}

void hashmap_add_specs(Bucket_t **hashmap, char *scope_id, int type, List_t *params)
{
  int key = gethash(scope_id);
  Bucket_t *bucket = hashmap[key];
  if (key != 0) {
    for (;;) {
      if (bucket == NULL) error_exit("hashmap_add_specs(): scope_id does not exist\n");;
      if (strcmp(bucket->scope_id, scope_id) == 0) break;
      bucket = bucket->next;
    }
  }
  bucket->specs_type = type;
  bucket->params = params;
}

bool hashmap_key_exists(Bucket_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  if (key == 0) return true;
  Bucket_t *bucket = hashmap[key];
  for (;;) {
    if (bucket == NULL) break;
    if (strcmp(bucket->scope_id, scope_id) == 0) return true;
    bucket = bucket->next;
  }
  return false;
}

// create a bucket for a NEW scope
void hashmap_set(Bucket_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  Bucket_t *new_bucket = create_bucket(scope_id);
  if (hashmap[key] == NULL) {
    hashmap[key] = new_bucket;
  }
  else {
    Bucket_t *bucket = hashmap[key];
    while (bucket->next != NULL) bucket = bucket->next;
    bucket->next = new_bucket;
  }
}

Bucket_t *hashmap_getitem(Bucket_t **hashmap, char *scope_id)
{
  int key = gethash(scope_id);
  if (key == 0) return hashmap[0];
  Bucket_t *bucket = hashmap[key];
  for (;;) {
    if (bucket == NULL) break;
    if (strcmp(bucket->scope_id, scope_id) == 0) return bucket;
    bucket = bucket->next;
  }
  error_exit("hashmap_getitem() - scope_id not found\n");
  return NULL;
}

List_t *hashmap_get_all_ids(Bucket_t **hashmap)
{
  List_t *ids = init_list(sizeof(char *));
  for (int i = 1; i < HASHMAP_SZ; ++i) {
    Bucket_t *bucket = hashmap[i];
    while (bucket != NULL) {
      list_push(ids, bucket->scope_id);
      bucket = bucket->next;
    }
  }
  return ids;
}
