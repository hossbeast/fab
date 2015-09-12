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

#ifndef _XAPI_INFO_H
#define _XAPI_INFO_H

#include <sys/types.h>

struct memblk;

typedef struct info
{
  char *	ks;		// key
  size_t	kl;
  size_t	ka;

  char *	vs;		// value
  size_t  vl;
  size_t  va;
} info;

#define restrict __restrict 

void info_freeze(struct memblk * const restrict mb, info * restrict i)
  __attribute__((nonnull));

void info_unfreeze(struct memblk * const restrict mb, info * restrict i)
  __attribute__((nonnull));

void info_thaw(char * const restrict mb, info * restrict i)
  __attribute__((nonnull));

#undef restrict
#endif