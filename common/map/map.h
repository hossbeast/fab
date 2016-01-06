/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _MAP_H
#define _MAP_H

#include <sys/types.h>
#include <stdint.h>
#include "xapi.h"

#define restrict __restrict

// opaque map type declaration
struct map;
typedef struct map map;

// keys / values are dereferenced before being returned (map_get, map_keys, map_values)
#define MAP_DEREF   1

/// map_create
//
// create an empty map
//
// parameters
//  map					- created map goes here
//  destructor  - invoked with key/value just before freeing their associated memory
//
// returns nonzero on success
//
xapi map_create(map** const restrict m, void (*destructor)(void*, void*))
	__attribute__((nonnull(1)));

/// map_free
//
// free a map with free semantics
//
void map_free(map* const restrict map);

/// map_xfree
//
// free a map with xfree semantics
//
void map_xfree(map** const restrict map)
	__attribute__((nonnull));

/// map_set
//
// set key/value pair on the map
//
// parameters
//  map				- map instance
//  key				- pointer to key
//  key_len		- key length
//  [value]		- pointer to value, or 0
//  value_len	- value length
//  [rv]      - internally cast to void**, returns pointer to the stored value
//
// returns zero on success
//
xapi map_set(map* const restrict map, const void* const restrict key, size_t key_len, const void* const restrict value, size_t value_len, void * restrict rv)
	__attribute__((nonnull(1, 2)));

/// map_get
//
// SUMMARY
//  given a key, returns pointer to associated value, or 0 if not found
//
// PARAMETERS
//  map			- map instance
//	key			- pointer to key
//	key_len	- length of key
//  [opts]  - bitwise combination of MAP_*
//
// EXAMPLE
//	int k = 15;
//	int* v = 0;
//	if((v = map_get(map, MM(k))) == 0)
//		/* key is not set */
//
// SUMMARY
//  returns pointer to the stored value, or 0 if not found
//
void * map_getx(const map * const restrict m, const void * const restrict key, size_t keyl, uint32_t opts)
	__attribute__((nonnull));

void * map_get (const map * const restrict m, const void * const restrict key, size_t keyl)
	__attribute__((nonnull));

/// map_clear
//
// disassociate all keys - internal structures in the map remain allocated
//
// returns nonzero on success
//
void map_clear(map* const restrict map)
	__attribute__((nonnull));

/// map_delete
//
// disassociates the specified key - if not found, does nothing
//
// returns 1 if the key was found, 0 otherwise
//
int map_delete(map* const restrict map, const void* const restrict key, size_t key_len)
	__attribute__((nonnull));
//
/// map_values
//
// SUMMARY
//  returns a list of values in the map
//
// PARAMETERS
//  m      - map instance
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//  [opts] - bitwise combination of MAP_*
//
// RETURNS
//  xapi semantics
//
xapi map_keysx(const map * const restrict m, void * const restrict list, size_t * const restrict listl, uint32_t opts)
	__attribute__((nonnull));

xapi map_keys (const map * const restrict m, void * const restrict list, size_t * const restrict listl)
	__attribute__((nonnull));

/// map_values
//
// SUMMARY
//  returns a list of values in the map
//
// PARAMETERS
//  m      - map instance
//  list   - (returns) list, to be freed by the caller
//  listl  - (returns) size of list
//  [opts] - bitwise combination of MAP_*
//
// RETURNS
//  xapi semantics
//
xapi map_valuesx(const map * const restrict m, void * const restrict list, size_t * const restrict listl, uint32_t opts)
	__attribute__((nonnull));

xapi map_values (const map * const restrict m, void * const restrict list, size_t * const restrict listl)
	__attribute__((nonnull));

/// map_clone
//
// copy all keys and values from src to dst
//
xapi map_clone(map* const restrict dst, const map * const restrict src)
	__attribute__((nonnull));

xapi map_copyto(map* const restrict dst, const map * const restrict src)
	__attribute__((nonnull));

size_t map_size(const map * const restrict m)
  __attribute__((nonnull));

size_t map_slots(const map * const restrict m)
  __attribute__((nonnull));

void * map_keyat(const map * const restrict m, int x)
  __attribute__((nonnull));

void * map_keyatx(const map * const restrict m, int x, uint32_t o)
  __attribute__((nonnull));

void * map_valueat(const map * const restrict m, int x)
  __attribute__((nonnull));

void * map_valueatx(const map * const restrict m, int x, uint32_t o)
  __attribute__((nonnull));

#undef restrict
#endif
