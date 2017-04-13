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

#ifndef _VALUE_MERGE_H
#define _VALUE_MERGE_H

/*

MODULE
 value/merge

SUMMARY
 functionality to combine two value trees.

REMARKS
 only the merge attributes in the src tree are relevant, and merge attributes are not
 propagated to the destination by the merge operation

*/

#include "xapi.h"

struct value; // value.h

#define MERGE_OPT   UINT16_C(0x000F)

// get the name of a modifier or option from its value
#define VALUE_ATTR_DEF(a, b, x, m) ((x) & (m)) == (b) ? "VALUE_" #a :
#define MERGE_VALUE(x) VALUE_ATTR_TABLE(x, MERGE_OPT) "NONE"

/// value_merge
//
// SUMMARY
//  combine two value trees from the same value_store
//
// PARAMETERS
//  dst - destination tree
//  src - source tree
//
// THROWS
//  DIFFTYPE - src and dst are not the same type
//
xapi value_merge(struct value * const restrict dst, const struct value * const restrict src)
  __attribute__((nonnull));

#endif
