/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _SELECTOR_H
#define _SELECTOR_H

#include <sys/types.h>
#include <stdint.h>

#include "listwise.h"

#include "ff.h"
#include "map.h"
struct gn;

#define restrict __restrict

/// selector_process
//
// SUMMARY
//
int selector_process(selector * const restrict s, const int id, const ff_parser * const restrict ffp, map * const restrict tmap, lwx *** restrict stax, int * restrict staxa, int staxp);

/// selector_finalize
//
// SUMMARY
//  finalize selectors, obtain result lists
//
int selector_finalize(
	  struct gn **** restrict fabrications, int * restrict fabricationsl
	, struct gn **** restrict fabricationxs, int * restrict fabricationxsl
	, struct gn **** restrict fabricationns, int * restrict fabricationnsl
	, struct gn **** restrict invalidations, int * restrict invalidationsl
	, struct gn **** restrict discoveries, int * restrict discoveriesl
	, struct gn **** restrict inspections, int * restrict inspectionsl
)
	__attribute__((nonnull));

/// selector_init
//
// SUMMARY
//  initialize selector module
//
int selector_init();

/// selector_teardown
//
// SUMMARY
//  teardown selector module
//
void selector_teardown();

void selector_freeze(char * const restrict p, selector * restrict s)
	__attribute__((nonnull));

void selector_thaw(char * const restrict p, selector * restrict s)
	__attribute__((nonnull));

#undef restrict
#endif
