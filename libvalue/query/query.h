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

#ifndef VALUE_QUERY_H
#define VALUE_QUERY_H

#include "xapi.h"

struct value;

#define restrict __restrict

/// value_query
//
// SUMMARY
//  query a value tree
//
// PARAMETERS
//  val   - an aggregate value
//  query -
//
// RETURNS
//  value matching the query, if any
//
struct value * value_query(struct value * restrict val, const char * const restrict query)
  __attribute__((nonnull));

#undef restrict
#endif
