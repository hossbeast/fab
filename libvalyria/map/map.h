/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 map

SUMMARY
 dynamically resizing unordered collection of key/value pairs

REMARKS
 map stores references to values - the put operation stores a reference only
 map stores copies of keys

*/

#include "types.h"

typedef struct map_entry {
  void * key;        // key
  uint16_t key_len;  // length
  uint16_t key_a;    // allocated size

  void * val;        // pointer to value
  size_t val_len;    // value length
  uint8_t attr;
  uint32_t hash;
} map_entry;

typedef struct map {
  size_t size;
  size_t table_size;
  uint32_t hash;

  size_t dirty_size;
  size_t lm;              // bitmask equal to table_size - 1
  size_t overflow_size;   // size at which to rehash
  map_entry * tab;        // bucket table

  void (*delete_fn)(void * entry);
} map;

typedef struct map_search_context {
  const void *key;
  uint16_t key_len;
  uint32_t hash;
  int probe_result;
  size_t index;
} map_search_context;

/// map_create
//
// SUMMARY
//  create an empty map
//
// PARAMETERS
//  map         - (returns) newly allocated map
//  [capacity]  - initial capacity, the minimum number of entries which can be set without rehashing
//  [delete_fn] - invoked with the value just before freeing it (keys may not be managed in this way)
//
void map_createx(map ** restrict m, size_t capacity, void (*delete_fn)(void * value))
  __attribute__((nonnull(1)));

void map_create(map ** restrict m)
  __attribute__((nonnull));

/// map_free
//
// SUMMARY
//  free a map
//
void map_free(map * restrict map);

/// map_ifree
//
// SUMMARY
//  free a map, zero its reference
//
void map_ifree(map ** restrict map)
  __attribute__((nonnull));

void map_initx(map * restrict m, size_t capacity, void (*delete_fn)(void * value))
  __attribute__((nonnull(1)));

void map_init(map * restrict m)
  __attribute__((nonnull));

void map_destroy(map * restrict m)
  __attribute__((nonnull));

bool map_search(const map * restrict m, map_search_context * restrict ctx)
  __attribute__((nonnull));

void map_store(map * restrict m, map_search_context * restrict ctx, void * val, size_t val_len)
  __attribute__((nonnull));

/// map_put
//
// SUMMARY
//  create or update a mapping
//
// PARAMETERS
//  map         - map
//  key         - pointer to key
//  keyl        - key length
//  [value]     - pointer to value
//  [value_len] - value length
//
void map_put(map * restrict m, const void * restrict key, uint16_t keyl, void * value, size_t value_len)
  __attribute__((nonnull(1, 2)));

/// map_get
//
// SUMMARY
//  given a key, get a pointer to the associated value, or 0 if not found
//
// PARAMETERS
//  map  - map
//  key  - pointer to key
//  keyl - length of key
//
// SUMMARY
//  returns pointer to the stored value, or 0 if not found
//
void * map_get(const map * restrict m, const void * restrict key, uint16_t keyl)
  __attribute__((nonnull));

/// map_get_mapping
//
// SUMMARY
//  given a key, returns
//
// PARAMETERS
//  m           - map
//  key         - pointer to key
//  keyl        - length of key
//  [value]     - (returns) pointer to the value
//  [value_len] - (returns) value length
//
// RETURNS
//  true if a mapping exists with the specified key
//
bool map_get_mapping(const map * restrict m, const void * restrict key, uint16_t keyl, void * value, size_t * restrict value_len)
  __attribute__((nonnull(1, 2)));

bool map_equal(map * const restrict A, map * const restrict B)
  __attribute__((nonnull));

/// map_recycle
//
// disassociate all keys - internal structures in the map remain allocated
//
void map_recycle(map * restrict map)
  __attribute__((nonnull));

/// map_delete
//
// SUMMARY
//  remove an entry from the map, if any
//
// PARAMETERS
//  m    - map
//  key  - pointer to key
//  keyl - length of key
//
void map_delete(map * restrict m, const void * restrict key, uint16_t keyl)
  __attribute__((nonnull));

/// map_keys
//
// SUMMARY
//  returns a list of keys in the map
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
void map_keys(const map * restrict m, void * restrict keys, uint16_t * restrict keysl)
  __attribute__((nonnull));

/// map_values
//
// SUMMARY
//  returns a list of values in the map
//
// PARAMETERS
//  m      - map
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//
void map_values(const map * restrict m, void * restrict values, size_t * restrict valuesl)
  __attribute__((nonnull));

/// map_table_key
//
// SUMMARY
//  get the key at the specified slot, if any
//
// PARAMETERS
//  m - map
//  x - slot index
//
void * map_table_key(const map * restrict m, size_t x)
  __attribute__((nonnull));

/// map_table_value
//
// SUMMARY
//  get the value at the specified slot, if any
//
// PARAMETERS
//  m - map
//  x - slot index
//
void * map_table_value(const map * restrict m, size_t x)
  __attribute__((nonnull));

/// map_table_mapping
//
// SUMMARY
//  get the mapping at the specified slot, if any
//
// PARAMETERS
//  m    - map
//  x    - slot index
//  [k]  - (returns) key
//  [kl] - (returns) key length
//  [v]  - (returns) value
//  [vl] - (returns) value length
//
bool map_table_mapping(const map * restrict m, size_t x, void ** restrict k, uint16_t * restrict kl, void * v, size_t * restrict vl)
  __attribute__((nonnull(1)));

/// map_splice
//
// SUMMARY
//  copy elements from one map to another
//
void map_splice(map * restrict dst, map * restrict src)
  __attribute__((nonnull));

/// map_replicate
//
// SUMMARY
//  copy elements from one map to another
//
void map_replicate(map * restrict dst, map * restrict src)
  __attribute__((nonnull));
