/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef _ATTRS16_H
#define _ATTRS16_H

/*

MODULE
 attrs

SUMMARY
 manipulate x-macros-style tables of named attributes, lookup-by-name, lookup-by-value

EXAMPLES
 liblogger/attr
 fab/fabd/ff_node

*/

#include <stdint.h>
#include <stdlib.h>

#include "macros.h"
#include "types.h"

/// named attribute definition
typedef struct attrs16 {
  const char * restrict name;
  uint8_t namel;
  uint16_t value;
} attrs16;

/// attrs_compare_byname
//
// compare two attrs by name, suitable as a sorting callback a la qsort
//
static int attrs16_compare_byname(const void * _A, const void * _B)
{
  const attrs16 * A = _A;
  const attrs16 * B = _B;
  return memncmp(A->name, A->namel, B->name, B->namel);
}

/// attrs_lookup_byname
//
// SUMMARY
//  lookup an attribute value by its name
//
// REMARKS
//  assumes that the attrs table had previously beeen sorted with e.g. qsort/attrs_compare_byname
//
static uint16_t attrs16_lookup_byname(const attrs16 * restrict table, uint16_t table_len, const char * restrict name, uint8_t namel)
{
  attrs16 key = { .name = name, .namel = namel };
  attrs16 * attr = bsearch(&key, table, table_len, sizeof(*table), attrs16_compare_byname);
  if(attr)
    return attr->value;

  return 0;
}

/// attrs_option_name
//
// SUMMARY
//  get the name of a particular option from an attrs bitmask
//
// PARAMETERS
//  table       - table of named attributes
//  table_len   - entries in table
//  option_mask - bitmask for the option 
//  attrs       - attrs value
//
// EXAMPLE
//  liblogger/attr
//
static inline const char * attrs16_option_name(const attrs16 * restrict table, uint16_t table_len, uint16_t option_mask, uint16_t attrs)
{
  attrs &= option_mask;

  uint16_t x;
  for(x = 0; x < table_len; x++)
  {
    if(attrs == table[x].value)
      return table[x].name;
  }

  return 0;
}

#endif
