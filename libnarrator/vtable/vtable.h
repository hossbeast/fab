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

#ifndef _VTABLE_H
#define _VTABLE_H

#include "types.h"

struct narrator;

typedef struct narrator_vtable {
  xapi (*sayvf)(struct narrator * const restrict n, const char * const restrict fmt, va_list va)
    __attribute__((nonnull));
  xapi (*sayw)(struct narrator * const restrict n, const void * const restrict b, size_t l)
    __attribute__((nonnull));
  xapi (*seek)(struct narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
    __attribute__((nonnull(1)));
  xapi (*read)(struct narrator * restrict n, void * dst, size_t count, size_t * restrict r)
    __attribute__((nonnull(1, 2)));
  xapi (*flush)(struct narrator * restrict n)
    __attribute__((nonnull));
} narrator_vtable;

#define NARRATOR_VTABLE(T) {    \
  sayvf : T ## _sayvf,          \
  sayw : T ## _sayw,            \
  seek : T ## _seek,            \
  read : T ## _read,            \
  flush : T ## _flush,          \
}

#endif
