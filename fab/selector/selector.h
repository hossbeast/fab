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

#define SELECTOR_TABLE(x)																												\
	_SELECTOR(SELECTOR_FABRICATE		, 0x01	, x)	/* fabricate */									\
	_SELECTOR(SELECTOR_FABRICATEX		, 0x02	, x)	/* fabricate-exact */						\
	_SELECTOR(SELECTOR_FABRICATEN		, 0x04	, x)	/* fabricate-nofile */					\
	_SELECTOR(SELECTOR_INVALIDATE		, 0x08	, x)	/* invalidate */								\
	_SELECTOR(SELECTOR_DISCOVERY		, 0x10	, x)	/* discovery */									\
	_SELECTOR(SELECTOR_INSPECT			, 0x20	, x)	/* inspect */										\
	_SELECTOR(SELECTOR_QUERY				, 0x40	, x)	/* query */											\

enum {
#define _SELECTOR(a, b, c) a = b,
SELECTOR_TABLE(0)
#undef _SELECTOR
};

#define _SELECTOR(a, b, c) (c) == b ? #a :
#define SELECTOR_STR(x) SELECTOR_TABLE(x) "unknown"

#define SELECTOR_BASE_TABLE(x)																																		\
	_SELECTOR_BASE(SELECTOR_BASE_FABFILE_DIR	, 0x01	, x)	/* resolve against init-fabfile-dir */	\
	_SELECTOR_BASE(SELECTOR_BASE_CWD					, 0x02	, x)	/* resolve against cwd */								\

enum {
#define _SELECTOR_BASE(a, b, c) a = b,
SELECTOR_BASE_TABLE(0)
#undef _SELECTOR_BASE
};

#define _SELECTOR_BASE(a, b, c) (c) == b ? #a :
#define SELECTOR_BASE_STR(x) SELECTOR_BASE_TABLE(x) "unknown"

// node selectors
typedef struct selector {
	char * 			s;					// selector text
	char				mode;				// '-' : remove, '+' : append
	uint8_t			base;				// base for path resolution
	uint32_t		lists;			// list(s) affected
} selector;

/// selector_string
//
// SUMMARY
//
char * selector_string(const selector * const restrict s, char * const restrict dst, const size_t z);

/// selector_process
//
// SUMMARY
//
int selector_process(selector * const restrict s, const int id, const ff_parser * const restrict ffp, map * const restrict tmap, lstack *** restrict stax, int * restrict staxa, int staxp);

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

#undef restrict
#endif
