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

#include "xapi.h"

struct value; // value.h

/// value_merge
//
// SUMMARY
//  combine two value trees
//
// PARAMETERS
//  dst - destination tree
//  src - source tree
//
// THROWS
//  DIFFTYPE - src and dst are not the same type
//  BADTYPE  - src and dst are scalars
//
xapi value_merge(struct value * const restrict dst, const struct value * const restrict src)
  __attribute__((nonnull));

#endif
