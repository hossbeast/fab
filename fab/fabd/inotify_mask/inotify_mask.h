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

#ifndef _INOTIFY_MASK
#define _INOTIFY_MASK

#include <stdint.h>
#include "xapi.h"

struct narrator;

xapi inotify_mask_say(uint32_t mask, struct narrator * const restrict N)
  __attribute__((nonnull));

size_t inotify_mask_znload(char * restrict dst, size_t sz, uint32_t mask)
  __attribute__((nonnull));

#endif
