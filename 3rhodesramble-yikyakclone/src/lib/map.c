#include "map.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnv64.h"

// Entry struct.
typedef struct entry_t {
  struct entry_t *next; // next entry in linked list
  struct entry_t *prev; // previous entry
  char *key;            // string key
  uint64_t hkey;        // 64-bit hash of key
  void *value;          // value stored for the key
} entry_t;

// Map struct.
struct map_t {
  entry_t **entries; // array of pointers to entries
  uint32_t n;        // size of entries array
};

// creates a map with the given initial size and returns a pointer to said map
// if created. Else it returns NULL.
map_t *map_create(uint32_t init_size) {
  // allocates memory for the map
  map_t *mptr = (map_t *)malloc(sizeof(map_t));
  // if failed create return null
  if (!mptr) {
    return NULL;
  }
  // set values for the struct
  mptr->n = init_size;
  // creates pointers to the entries in the map.
  mptr->entries = (entry_t **)calloc(init_size, sizeof(entry_t *));
  // if can't create pointers to the entries return NULL.
  if (!mptr->entries) {
    free(mptr);
    return NULL;
  }
  return mptr;
}

map_metrics_t *map_metrics(map_t *map) {
  // if map is not yet created return NULL.
  if (!map) {
    return NULL;
  }
  // allocate memory for the metrics. if fails return NULL
  map_metrics_t *metrics = malloc(sizeof(map_metrics_t));
  if (!metrics) {
    return NULL;
  }
  // initialize values
  metrics->num_entries = 0;
  metrics->max_depth = 0;
  metrics->curr_size = map->n;
  // go through each entry finding the num of entries and max depth
  for (uint32_t i = 0; i < map->n; i++) {
    uint32_t max = 0;
    entry_t *curr = map->entries[i];
    while (curr) {
      max++;
      metrics->num_entries++;
      curr = curr->next;
    }
    if (max > metrics->max_depth) {
      metrics->max_depth = max;
    }
  }
  return metrics;
}

// frees the memory used my map and sets it to NULL
void map_free(map_t **map) {
  // if map or its pointer is NULL return
  if (!map || !(*map)) {
    return;
  }
  // goes through all of entries freeing any memory allocated (keys, entries)
  for (uint32_t i = 0; i < (*map)->n; i++) {
    entry_t *curr = (*map)->entries[i];
    while (curr) {
      entry_t *next = curr->next;
      free(curr->key);
      curr->key = NULL;
      free(curr);
      curr = NULL;
      curr = next;
    }
  }
  // frees the array of entries and map
  free((*map)->entries);
  (*map)->entries = NULL;
  free(*map);
  *map = NULL;
}

// puts an entry into the map. if the key of the entry already exists, update
// the value and return 0. if it is a new key allocate memory for the entry and
// set parameters for it. create a copy of the key and make this new node the
// head of the list at the index.
int map_put(map_t *map, const char *key, void *new_value) {
  // provides a hashed key for the map
  uint64_t hashkey = fnv64(key);
  // calculates the index by doing a modulo operation on the hashed key with
  // size of entries array
  uint32_t i = hashkey % map->n;
  // go to the calculated index in map
  entry_t *curr = map->entries[i];
  // search through entries to see if we have the same hashkey already
  // if so update the value to new_value and return 0
  while (curr) {
    if (curr->hkey == hashkey) {
      curr->value = new_value;
      return 0;
    }
    curr = curr->next;
  }
  // allocates mem for a new entry, if not allocated return -1 for failure of
  // function.
  entry_t *entry = (entry_t *)malloc(sizeof(entry_t));
  if (!entry) {
    return -1;
  }
  // sets parameters of entry and creates copy of key dynamically. If this copy
  // fails return -1 and free the memory. if there is already a head, this
  // entry's next parameter is set to the head and will replace it as the head
  // (this replacement is done further below)
  entry->next = map->entries[i];
  entry->prev = NULL;
  entry->key = strdup(key);
  if (!entry->key) {
    free(entry);
    return -1;
  }
  entry->hkey = hashkey;
  entry->value = new_value;
  // if values in this index already exist set the previous head's prev value
  // to the new entry
  if (map->entries[i]) {
    map->entries[i]->prev = entry;
  }
  // set the new entry as the head of the linked list on this index of the map.
  map->entries[i] = entry;
  return 1;
}

// removes a entry from the map
int map_remove(map_t *map, const char *key) {
  // if any of input is NULL return
  if (!map || !key) {
    return 0;
  }
  uint64_t hashkey = fnv64(key);
  uint32_t i = hashkey % map->n;
  entry_t *curr = map->entries[i];
  entry_t *prev = NULL;
  // search entries for the hashkey. if found start series of checks. if there
  // is a previous node, set it's next value to curr's next. otherwise, set
  // curr's next as the head of the nodes. if curr has a node after it, set it's
  // prev node to curr's prev node. now free curr's key and then curr.
  while (curr) {
    if (curr->hkey == hashkey) {
      if (prev) {
        prev->next = curr->next;
      } else {
        map->entries[i] = curr->next;
      }
      if (curr->next) {
        curr->next->prev = prev;
      }
      free(curr->key);
      free(curr);
      return 1;
    }
    prev = curr;
    curr = curr->next;
  }
  return 0;
}

// searchs the map for a entry with the same hashkey as entered. if found sets
// value_ptr as a pointer to its value and return 1. if not, set it to a NULL
// pointer and return 0.
int map_get(map_t *map, const char *key, void **value_ptr) {
  // calculates hashkey and index
  uint64_t hashkey = fnv64(key);
  uint32_t i = hashkey % map->n;
  entry_t *curr = map->entries[i];
  // goes through all entries at index i of map and if exists updates value_ptr
  // to point to curr's value and returns 1
  while (curr) {
    if (curr->hkey == hashkey) {
      // if value_ptr is not NULL update its value
      if (value_ptr) {
        *value_ptr = curr->value;
      }
      return 1;
    }
    curr = curr->next;
  }
  // if not found and value_ptr is not NULL sets value_ptr to a NULL pointer and
  // return 0
  if (value_ptr) {
    *value_ptr = NULL;
  }
  return 0;
}

// resizes the map to the given size
void map_resize(map_t *map, uint32_t new_size) {
  // create a new array of pointers to represent new entries
  entry_t **new_map = (entry_t **)calloc(new_size, sizeof(entry_t *));
  // if failed to create new entries array return
  if (!new_map) {
    free(new_map);
    return;
  }
  // go through each entry in the current map and add it to the new entries
  for (uint32_t i = 0; i < map->n; i++) {
    entry_t *curr = map->entries[i];
    while (curr) {
      entry_t *next = curr->next;
      uint32_t i = curr->hkey % new_size;
      curr->next = new_map[i];
      curr->prev = NULL;
      if (new_map[i]) {
        new_map[i]->prev = curr;
      }
      new_map[i] = curr;
      curr = next;
    }
  }
  // free the space taken by the old entries and update the maps entries and
  // size to the new ones.
  free(map->entries);
  map->entries = new_map;
  map->n = new_size;
}

// prints out size of the map and the entries in the map
void map_debug(map_t *map) {
  for (uint32_t i = 0; i < map->n; i++) {
    printf("%u ", i);
    entry_t *curr = map->entries[i];
    while (curr) {
      printf("%p %s -> ", curr->value, curr->key);
      curr = curr->next;
    }
    printf("X\n");
  }
}

void map_apply(map_t *map, void *apply_fn(const char *key, void *value)) {
  for (int i = 0; i < map->n; i++) {
    // for each entry
    entry_t *cur = map->entries[i];
    while (cur) {
      // traverse linked list of entries, applying along the way
      cur->value = apply_fn(cur->key, cur->value);
      cur = cur->next;
    }
  }
}

uint32_t map_size(map_t *map) { return map->n; }
