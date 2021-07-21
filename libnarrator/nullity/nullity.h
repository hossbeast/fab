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

#ifndef _NARRATOR_NULLITY_H
#define _NARRATOR_NULLITY_H

/*

MODULE
 narrator/nullity

SUMMARY
 narrator that discards writes

*/

#include "narrator.h"

typedef struct narrator_nullity {
  narrator base;
} narrator_nullity;


/// narrator_nullity_create
//
// SUMMARY
//  allocate a nullity narrator
//
// PARAMETERS
//  n - (returns) narrator
//
void narrator_nullity_create(narrator_nullity ** const restrict n)
  __attribute__((nonnull));

void narrator_nullity_free(narrator_nullity * restrict n);

#endif
