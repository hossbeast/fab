/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 hashtable - dynamically resizing unordered collection of unique entries

REMARKS
 hashtable stores objects - the put operation makes a copy

*/

#include "types.h"

typedef struct hashtable_bucket {
  uint32_t hash;
  uint8_t attr;
  union {
    void *p;
    uint32_t u32;
    uint64_t u64;
  };
} hashtable_bucket;

typedef struct hashtable {
  size_t size;
  size_t table_size;
  uint32_t hash;          // order invariant hash of entries

  size_t dirty_size;
  size_t lm;              // bitmask equal to table_size - 1
  size_t overflow_size;   // size at which to rehash
  hashtable_bucket * tab; // bucket table

  // callbacks
  uint32_t (*hash_fn)(uint32_t h, const void * entry);
  int (*cmp_fn)(const void * A, const void * B);
  void (*delete_fn)(void * entry);
} hashtable;

/*
 * allocate a hashtable
 *
 * ht          - (returns) newly allocated hashtable
 * [capacity]  - initial capacity, the minimum number of entries which can be set without rehashing
 * [delete_fn] - invoked with the entry just before freeing it
 */
void hashtable_createx(
    hashtable ** restrict ht
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * entry)
  , int (*cmp_fn)(const void * A, const void * B)
  , void (*delete_fn)(void * entry)
)
  __attribute__((nonnull(1)));

void hashtable_create(hashtable ** restrict ht)
  __attribute__((nonnull(1)));

void hashtable_free(hashtable * restrict ht);
void hashtable_ifree(hashtable ** restrict ht)
  __attribute__((nonnull));

void hashtable_init(hashtable * restrict ht)
  __attribute__((nonnull(1)));

void hashtable_initx(
    hashtable * restrict ht
  , size_t capacity
  , uint32_t (*hash_fn)(uint32_t h, const void * entry)
  , int (*cmp_fn)(const void * A, const void * B)
  , void (*delete_fn)(void * entry)
)
  __attribute__((nonnull(1)));

void hashtable_destroy(hashtable * restrict ht)
  __attribute__((nonnull));

typedef struct hashtable_search_context {
  void *entry;
  uint32_t hash;
  int probe_result;
  size_t index;
  uint32_t (*hash_fn)(uint32_t h, const void * entry);
  int (*cmp_fn)(const void * A, const void * B);
} hashtable_search_context;

/*
 * search a hashtable for an entry
 *
 * returns a boolean value indicating whether the entry was found
 */
bool hashtable_search(const hashtable * restrict ht, hashtable_search_context * restrict ctx)
  __attribute__((nonnull));

/* put an entry in the hashtable after hashtable_search returns true */
void * hashtable_set(hashtable * restrict ht, hashtable_search_context * restrict ctx)
  __attribute__((nonnull));

/* get an entry in the hashtable after hashtable_search returns true */
void * hashtable_fetch(const hashtable * restrict ht, hashtable_search_context * restrict ctx)
  __attribute__((nonnull));

/* delete an entry from the hashtable after hashtable_search returns true */
void hashtable_remove(hashtable * restrict m, hashtable_search_context * restrict ctx)
  __attribute__((nonnull));

/*
 * create/update an entry in the hashtable
 *
 * REMARKS
 *  destroys an existing entry, if any
 *
 * PARAMETERS
 *  ht    - hashtable
 *  entry - pointer to entry
 */
void * hashtable_put(hashtable * restrict ht, void * restrict entry)
  __attribute__((nonnull(1)));

/*
 * get an entry in the hashtable
 *
 * ht    - hashtable
 * entry - pointer to entry
 *
 * RETURNS
 *  pointer to entry, if any
 */
void * hashtable_get(const hashtable * restrict ht, const void * restrict entry)
  __attribute__((nonnull(1)));

bool hashtable_contains(const hashtable * restrict ht, const void * restrict entry)
  __attribute__((nonnull(1)));

/* get the entry at the specified index, if any */
void * hashtable_table_entry(const hashtable * restrict ht, size_t x)
  __attribute__((nonnull));

/*
 * remove an entry from the hashtable, if any
 *
 * m    - hashtable
 * key  - pointer to key
 *
 * if hashtable_delete returns true, then the entry was both found and removed
 */
bool hashtable_delete(hashtable * restrict m, void * restrict entry)
  __attribute__((nonnull));

/// hashtable_equal
//
// SUMMARY
//  determine whether two hashtables contain the same set of active entries
//
bool hashtable_equal(hashtable * restrict A, hashtable * restrict B)
  __attribute__((nonnull));

/// hashtable_recycle
//
// disassociate all entries - internal structures remain allocated
//
void hashtable_recycle(hashtable * restrict ht)
  __attribute__((nonnull));

/// hashtable_splice
//
// SUMMARY
//  move entries from one hashtable to another
//
void hashtable_splice(hashtable * dst, hashtable * src)
  __attribute__((nonnull));

/// hashtable_replicate
//
// SUMMARY
//  copy entries from one hashtable to another
//
void hashtable_replicate(hashtable * dst, hashtable * src)
  __attribute__((nonnull));

void hashtable_enumerate(hashtable * restrict ht, void * restrict _entries, size_t * restrict entriesl)
  __attribute__((nonnull));
