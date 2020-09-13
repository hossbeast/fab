/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef BACON_NARRATOR_H
#define BACON_NARRATOR_H

/*

MODULE
 narrator/record

SUMMARY
 narrator that accumulates writes and propagates records to an underlying narrator

*/

#include "xapi.h"
#include "types.h"
#include "narrator.h"

typedef struct bacon_narrator {
  narrator base;
  char *  s;    // temp buffer
  size_t  a;    // allocated size
  narrator * n; // underlying narrator, (not owned)
} bacon_narrator;

xapi bacon_narrator_init(bacon_narrator * const restrict rv, narrator * const restrict np)
  __attribute__((nonnull));

xapi bacon_narrator_create(bacon_narrator ** const restrict rv, narrator * const restrict np)
  __attribute__((nonnull));

xapi bacon_narrator_free(bacon_narrator * const restrict n);

xapi bacon_narrator_destroy(bacon_narrator * const restrict n)
  __attribute__((nonnull));

#endif
