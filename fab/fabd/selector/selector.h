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

#ifndef _SELECTOR_H
#define _SELECTOR_H

/* selector execution */

#include <sys/types.h>
#include <stdint.h>

// selector definition and persistence
#include "selector.def.h"

struct map;								    // map.h
struct fabd_handler_context;	// fabd/handler.h
struct ff_parser;             // ff.h
struct lwx;                   // listwise.h
struct selector;              // args.def.h

#define restrict __restrict

/// selector_process
//
// SUMMARY
//  execute the selector to populate the node lists
//
int selector_process(
	  selector * const restrict s
	, const int id
	, const struct ff_parser * const restrict ffp
	, struct map * const restrict tmap
	, struct lwx *** restrict stax
	, size_t * restrict staxa
	, size_t staxp
);

/// selector_finalize
//
// SUMMARY
//  finalize selectors, obtain result lists
//
int selector_finalize(struct fabd_handler_context * ctx)
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

#undef restrict
#endif
