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

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

/*

MODULE
 dictionary

SUMMARY
 dynamically resizing unordered collection of key/value pairs

REMARKS
 meant to be used as primary storage for the elements in the collection

*/

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

struct map;
typedef struct map dictionary;

#ifndef DICTIONARY_VALUE_TYPE
# define DICTIONARY_VALUE_TYPE void
#endif

#define restrict __restrict

/// dictionary_create
//
// SUMMARY
//  create an empty dictionary
//
// PARAMETERS
//  dictionary   - (returns) newly allocated dictionary
//  vsz          - value size > 0
//  [destructor] - invoked with key/value just before freeing their associated memory
//  [capacity]   - initial capacity, the minimum number of entries which can be set without rehashing
//
xapi dictionary_create(dictionary ** const restrict m, size_t vsz)
  __attribute__((nonnull(1)));

xapi dictionary_createx(dictionary ** const restrict m, size_t vsz, void (*destructor)(const char *, DICTIONARY_VALUE_TYPE *), size_t capacity)
  __attribute__((nonnull(1)));

/// dictionary_free
//
// SUMMARY
//  free a dictionary with free semantics
//
void dictionary_free(dictionary * const restrict dictionary);

/// dictionary_xfree
//
// SUMMARY
//  free a dictionary with xfree semantics
//
void dictionary_xfree(dictionary ** const restrict dictionary)
  __attribute__((nonnull));

/// dictionary_set
//
// SUMMARY
//  set a key/value pair on a MAP_PRIMARY dictionary
//
// PARAMETERS
//  m     - dictionary
//  key   - pointer to key
//  keyl  - key length
//  value - (returns) pointer to value
//
xapi dictionary_set(dictionary * const restrict m, const char * const restrict key, size_t keyl, DICTIONARY_VALUE_TYPE * const * const restrict value)
  __attribute__((nonnull));

/// dictionary_get
//
// SUMMARY
//  given a key, returns pointer to associated value, or 0 if not found
//
// PARAMETERS
//  m    - dictionary
//  key  - pointer to key
//  keyl - key size
//
// RETURNS
//  pointer to element or 0
//
DICTIONARY_VALUE_TYPE * dictionary_get(const dictionary * const restrict m, const char * const restrict key, size_t keyl)
  __attribute__((nonnull));

/// dictionary_clear
//
// SUMMARY
//  disassociate all keys, retain internal allocations
//
void dictionary_clear(dictionary * const restrict dictionary)
  __attribute__((nonnull));

/// dictionary_delete
//
// SUMMARY
//  remove an entry from the dictionary, if any
//
// PARAMETERS
//  m    - dictionary
//  key  - pointer to key
//  keyl - length of key
//
// RETURNS
//  boolean value indicating whether an entry was found and removed
//
int dictionary_delete(dictionary * const restrict m, const char * const restrict key, size_t keyl)
  __attribute__((nonnull));

/// dictionary_size
//
// SUMMARY
//  get the number of entries in the dictionary
//
size_t dictionary_size(const dictionary * const restrict m)
  __attribute__((nonnull));

/// dictionary_keys
//
// SUMMARY
//  get a list of all keys in the dictionary
//
// PARAMETERS
//  m      - dictionary
//  keys   - (returns) list of keys, to be freed by the caller
//  keysl  - (returns) size of list
//
xapi dictionary_keys(const dictionary * const restrict m, const char *** const restrict keys, size_t * const restrict keysl)
  __attribute__((nonnull));

/// dictionary_values
//
// SUMMARY
//  returns a list of values in the dictionary
//
// PARAMETERS
//  m       - dictionary
//  values  - (returns) list, to be freed by the caller
//  valuesl - (returns) size of list
//
xapi dictionary_values(const dictionary * const restrict m, DICTIONARY_VALUE_TYPE *** restrict values, size_t * const restrict valuesl)
  __attribute__((nonnull));

/// dictionary_table_size
//
// SUMMARY
//  get the size of the dictionary tables for use with dictionary_keyat / dictionary_valueat
//
// PARAMETERS
//  m - dictionary 
//
size_t dictionary_table_size(const dictionary * const restrict m)
  __attribute__((nonnull));

/// dictionary_table_key
//
// SUMMARY
//  get the key at the specified slot, if any
//
// PARAMETERS
//  m - dictionary
//  x - slot index, 0 <= x < dictionary_table_size
//
const char * dictionary_table_key(const dictionary * const restrict m, size_t x)
  __attribute__((nonnull));

/// dictionary_table_value
//
// SUMMARY
//  get the value at the specified slot, if any
//
// PARAMETERS
//  m - dictionary
//  x - slot index, 0 <= x < dictionary_table_size
//
DICTIONARY_VALUE_TYPE * dictionary_table_value(const dictionary * const restrict m, size_t x)
  __attribute__((nonnull));

#undef restrict
#endif
