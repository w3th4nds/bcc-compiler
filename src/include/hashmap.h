#ifndef _HASHMAP_H_
#define _HASHMAP_H_

typedef struct Bucket_s Bucket_t;

#include "debug.h"
#include "list.h"
#include "types.h"
#include "scope.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define HASHMAP_SZ  10

typedef struct Bucket_s {
  char *scope_id;
  int specs_type;
  List_t *params;
  Symtab_t *symtab;
  // linked list for hash collisions
  struct Bucket_s *next;
} Bucket_t;

int gethash(char *id);
Bucket_t *create_bucket(char *scope_id);
Bucket_t **create_hashmap(void);
void hashmap_add(Bucket_t **hashmap, char *scope_id, SymtabEntry_t *symtab_entry);
void hashmap_add_specs(Bucket_t **hashmap, char *scope_id, int type, List_t *params);
bool hashmap_key_exists(Bucket_t **hashmap, char *scope_id);
void hashmap_set(Bucket_t **hashmap, char *scope_id);
SymtabEntry_t *hashmap_getsymtabentry(Bucket_t **hashmap, char *scope_id, char *entry_id);
Bucket_t *hashmap_getbucket(Bucket_t **hashmap, char *scope_id);
List_t *hashmap_get_all_ids(Bucket_t **hashmap);

#endif
