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

#ifndef _NARRATOR_MULTI_H
#define _NARRATOR_MULTI_H

/*

MODULE
 narrator/multi

SUMMARY
 narrator that propagates writes to each of a set of underlying narrators

*/

#include "xapi.h"
#include "types.h"

/// narrator_multi_create
//
// SUMMARY
//  allocate a multi narrator
//
// PARAMETERS
//  n - (returns) narrator
//
xapi narrator_multi_create(narrator ** const restrict n)
  __attribute__((nonnull));

/// narrator_multi_add
//
// SUMMARY
//  add a narrator to the list of narrators underlying a multi narrator
//
// PARAMETERS
//  n  - multi narrator
//  np - narrator to add to the multi narrator
//
API xapi narrator_multi_add(narrator * const restrict n, narrator * const restrict np)
  __attribute__((nonnull));

#endif
