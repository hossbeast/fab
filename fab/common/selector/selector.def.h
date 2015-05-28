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

#ifndef _SELECTOR_DEF_H
#define _SELECTOR_DEF_H

/* selector persistence */

#include <sys/types.h>
#include <stdint.h>

#include "args.def.h"     // selector

struct memblk;            // memblk.def.h

#define restrict __restrict

/// selector_string
//
// SUMMARY
//  write description of the selector to the specified buffer
//
char * selector_string(const struct selector * const restrict s, char * const restrict dst, const size_t z);

/// selector_freeze
//
// SUMMARY
//  freeze selector that was allocated into the specified memblk (see memblk.def.h)
//
void selector_freeze(struct memblk * const restrict mb, struct selector * restrict s)
	__attribute__((nonnull));

/// selector_thaw
//
// SUMMARY
//  recover selector from the consolidated memblk (see memblk.def.h)
//
void selector_thaw(char * const restrict p, struct selector * restrict s)
	__attribute__((nonnull));

/// selector_parse
//
// SUMMARY
//  
//
int selector_parse(char * const s, uint32_t * const lists, uint8_t * const base, int * const mode)
  __attribute__((nonnull));

#undef restrict
#endif
