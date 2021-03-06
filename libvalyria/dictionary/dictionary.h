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

#ifndef _VALYRIA_DICTIONARY_H
#define _VALYRIA_DICTIONARY_H

/*

MODULE
 dictionary

SUMMARY
 dynamically resizing unordered collection of key/fixed-size value pairs

REMARKS
 dictionary stores the values

*/

#include "xapi.h"
#include "types.h"

typedef struct dictionary {
  size_t size;
  size_t table_size;
} dictionary;

/// dictionary_create
//
// SUMMARY
//  create an empty dictionary
//
// PARAMETERS
//  dictionary       - (returns) newly allocated dictionary
//  vsz              - value size > 0
//  [destroy_value]  - invoked with the value just before freeing it (keys may not be managed in this way)
//  [xdestroy_value] - invoked with the value just before freeing it (keys may not be managed in this way)
//  [capacity]       - initial capacity, the minimum number of entries which can be set without rehashing
//
xapi dictionary_create(dictionary ** restrict m, size_t vsz)
  __attribute__((nonnull));

xapi dictionary_createx(
    dictionary ** restrict m
  , size_t vsz
  , size_t capacity
  , void (*destroy_value)(void * entry)
  , xapi (*xdestroy_value)(void * entry)
)
  __attribute__((nonnull(1)));

/// dictionary_xfree
//
// SUMMARY
//  free a dictionary
//
xapi dictionary_xfree(dictionary * restrict dictionary);

/// dictionary_ixfree
//
// SUMMARY
//  free a dictionary, zero its reference
//
xapi dictionary_ixfree(dictionary ** restrict dictionary)
  __attribute__((nonnull));

/// dictionary_put
//
// SUMMARY
//  set a key/value mapping on a dictionary
//
// REMARKS
//  destroys an existing value, if any
//
// PARAMETERS
//  m       - dictionary
//  key     - pointer to key
//  keyl    - key length
//  [value] - pointer to value to store
//
xapi dictionary_put(dictionary * restrict m, const void * restrict key, uint16_t keyl, void * restrict value)
  __attribute__((nonnull(1, 2)));

/// dictionary_store
//
// SUMMARY
//  (idempotent) set a key in a dictionary, get or create the value
//
// REMARKS
//  if the key is already mapped, returns the existing value
//
// PARAMETERS
//  m     - dictionary
//  key   - pointer to key
//  keyl  - key length
//  value - (returns) pointer to value
//
xapi dictionary_store(dictionary * restrict m, const void * restrict key, uint16_t keyl, void * restrict value)
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
//  pointer to value or 0
//
void * dictionary_get(dictionary * restrict m, const void * restrict key, uint16_t keyl)
  __attribute__((nonnull));

/// dictionary_recycle
//
// SUMMARY
//  disassociate all keys, retain internal allocations
//
xapi dictionary_recycle(dictionary * restrict dictionary)
  __attribute__((nonnull));

/// dictionary_splice
//
// SUMMARY
//  move entries from one dictionary to another
//
xapi dictionary_splice(dictionary * restrict dst, dictionary * restrict src)
  __attribute__((nonnull));

/// dictionary_replicate
//
// SUMMARY
//  copy entries from one dictionary to another
//
xapi dictionary_replicate(dictionary * restrict dst, dictionary * restrict src)
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
xapi dictionary_delete(dictionary * restrict m, const void * restrict key, uint16_t keyl)
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
xapi dictionary_keys(const dictionary * restrict m, void * restrict keys, uint16_t * restrict keysl)
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
xapi dictionary_values(const dictionary * restrict m, void * restrict values, size_t * restrict valuesl)
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
const void * dictionary_table_key(const dictionary * restrict m, size_t x)
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
void * dictionary_table_value(const dictionary * restrict m, size_t x)
  __attribute__((nonnull));

#endif
