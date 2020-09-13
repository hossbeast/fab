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

#ifndef _NARRATOR_FIXED_H
#define _NARRATOR_FIXED_H

/*

MODULE
 narrator/fixed

SUMMARY
 narrator that writes to a fixed-size buffer, discarding overflow

REMARKS
 a fixed narrator requires no dynamic allocation, and passing a fixed narrator to the narrator free
 apis is a no-op

*/

#include "types.h"
#include "narrator.h"

typedef struct narrator_fixed {
  narrator base;
  char *  s;      // buffer
  size_t  l;      // position
  size_t  a;      // buffer size
  size_t  m;      // maximum position
} narrator_fixed;

/// narrator_fixed_init
//
// SUMMARY
//  initialize a fixed narrator
//
// PARAMETERS
//  fixed - storage for the fixed narrator
//
// RETURNS
//  fixed size narrator suitable for passing to other narrator apis
//
narrator * narrator_fixed_init(narrator_fixed * restrict n, char * s, size_t a)
  __attribute__((nonnull));

xapi narrator_fixed_destroy(narrator_fixed * restrict n)
  __attribute__((nonnull));

#endif
