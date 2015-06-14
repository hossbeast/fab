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

#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "coll.h"

struct strstack;
struct map;
struct path;
struct hashblock;

#define restrict __restrict

/*
** graph node designations
**
** 1. HASNEED - has any dependencies
** 2. CANFAB  - can be fabricated by some formula
** 3. HASFILE - has a backing file
**
**              1  2  3
** PRIMARY   - [ ][ ][x]
** SECONDARY - [x][x][x]
** GENERATED - [ ][x][x]
** TASK      - [x][x][ ]
** GROUP     - [x][ ][ ]
**
*/

//
// gn type table
//
#define GN_TYPE_HASNEED	0x01
#define GN_TYPE_CANFAB	0x02
#define GN_TYPE_HASFILE	0x04

#define GN_TYPE_PRIMARY_BIT		(0x01 << 8)
#define GN_TYPE_SECONDARY_BIT	(0x02 << 8)
#define GN_TYPE_GENERATED_BIT	(0x04 << 8)
#define GN_TYPE_TASK_BIT			(0x08 << 8)
#define GN_TYPE_GROUP_BIT			(0x10 << 8)
#define GN_TYPE_BITSPACE			(0xFF << 8)

#define GN_TYPE_TABLE(x)																																																					\
	_GN_TYPE(PRIMARY						, GN_TYPE_PRIMARY_BIT		|                                    GN_TYPE_HASFILE, "PRIMARY(p)"		, x)	\
	_GN_TYPE(SECONDARY					, GN_TYPE_SECONDARY_BIT	| GN_TYPE_HASNEED | GN_TYPE_CANFAB | GN_TYPE_HASFILE, "SECONDARY(s)"	, x)	\
	_GN_TYPE(GENERATED					, GN_TYPE_GENERATED_BIT	|                   GN_TYPE_CANFAB | GN_TYPE_HASFILE, "GENERATED(g)"	, x)	\
	_GN_TYPE(TASK								, GN_TYPE_TASK_BIT			| GN_TYPE_HASNEED | GN_TYPE_CANFAB                  , "TASK(t)"				, x)	\
	_GN_TYPE(GROUP							, GN_TYPE_GROUP_BIT			| GN_TYPE_HASNEED                                   , "GROUP(n)"			, x)

enum {
#define _GN_TYPE(a, b, c, d) GN_TYPE_ ## a = (b),
GN_TYPE_TABLE(0)
#undef _GN_TYPE
};

#define _GN_TYPE(a, b, c, d) (d) == (b) ? c :
#define GN_TYPE_STR(gn) GN_TYPE_TABLE(gn) "unknown"

//
// gn relation type table
//
#define GNR_TYPE_TABLE(x)																																												\
	_GNRT(GN_RELATION_REGULAR			, 0x01	, x)		/* relation from an FFN_DEPENDENCY node in a regular fabfile */	\
	_GNRT(GN_RELATION_DISCOVERED	, 0x02	, x)		/* relation from an FFN_DEPENDENCY node in a dscv fabfile */

enum {
#define _GNRT(a, b, c) a = b,
GNR_TYPE_TABLE(0)
#undef _GNRT
};

#define _GNRT(a, b, c) (c) == b ? #a : 
#define GNRT_STR(x) GNR_TYPE_TABLE(x) "unknown"

//
// gn invalidation table (in descending order)
//
#define GN_INVALIDATION_TABLE(x)																																	\
	_GN_INVALIDATION(NXFILE				, 0x01	, x)			/* no such file */															\
	_GN_INVALIDATION(CHANGED			, 0x02	, x)			/* backing file has changed */									\
	_GN_INVALIDATION(SOURCES			, 0x04	, x)			/* sources invalidated (non-primary) */					\
	_GN_INVALIDATION(USER					, 0x08	, x)			/* invalidated by the user */										\
	_GN_INVALIDATION(DISCOVERY		, 0x10	, x)			/* dependency discovery needed (primary) */			\
	_GN_INVALIDATION(FABRICATE		, 0x20	, x)			/* fabrication needed (primary) */							\


enum {
#define _GN_INVALIDATION(a, b, c) GN_INVALIDATION_ ## a = b,
GN_INVALIDATION_TABLE(0)
#undef _GN_INVALIDATION
};

#define _GN_INVALIDATION(a, b, c) (c) & b ? #a :
#define GN_INVALIDATION_STR(x) GN_INVALIDATION_TABLE(x) "UNKNWN"

// always use this instead of inspecting the invalid member to check node status
#define GN_IS_INVALID(x) (((x)->type & GN_TYPE_HASFILE) ? (x)->invalid : 1)

struct ff_file;
struct ff_node;
struct fmleval;
struct gn;

typedef struct
{
	uint32_t					type;		// one of GN_RELATION_*

	struct ff_node *	ffn;		// FFN_DEPENDENCY node which gave rise to this relation
	struct gn *				dscvgn;	// node whose discovery results contained this relation (if any)

	struct gn *				A;			// A needs B
	struct gn *				B;			// B feeds A
	int								weak;		// whether this is a weak relation
	int								bridge;	// whether this is a bridge relation (unreachable via fabfile traversals)
} relation;

typedef struct gn
{
	uint16_t						type;						// GN_TYPE_*

	struct path *				path;
	struct filesystem *	fs;							// filesystem the canonical path resides on
	char *							idstring;				// identifier string, subject to execution parameters
	int									idstringl;

	// PRIMARY, SECONDARY, GENERATED
	struct hashblock *	hb;
	uint16_t						invalid;				// whether (and how) this node is invalidated : GN_INVALIDATION_*

	//
	// PRIMARY
	//
	struct
	{
		// formula eval contexts for dependency discovery
		//  ** a PRIMARY node has 0-n fmleval's for discovery
		struct fmleval **	dscvs;
		int								dscvsl;
		int								dscvsa;
	};

	// formula evaluation context which fabricates this node
	//  ** a node has 0 or 1 fmleval's for fabrication
	struct fmleval *		fabv;

	// this node depends on the nodes in this list
	union relations {
		coll_doubly c;

		struct
		{
			int						l;
			int						a;
			int						z;
			relation **		e;

			struct map *					by_B;	// lookup by canonical path of the node at the other end of the relation
		};
	} needs;

	// the nodes in this list depend on this node
	union {
		coll_doubly c;

		struct
		{
			int						l;
			int						a;
			int						z;
			relation **		e;

			struct map *					by_A;	// indexed by canonical path of the node at the other end of the relation
		};
	} feeds;

	// buildplan create tracking
	int									height;		// distance of longest route to a leaf node
	int									stage;		// assigned stage - NEARLY always equal to height

	// traversal tracking
	int									guard;
	int									travel0;
	int									travel1;
	int									mark;

	// buildplan eval tracking
	char								rebuild;
	char								poison;
} gn;

/// gn_lookup
//
// SUMMARY
//  get graph nodes whose path(s) match certain criteria
//
// PARAMETERS
//  s     - one of
//           2) @s  - nofile specifier
//           3) /s  - canonical path
//           4)  s  - path relative to base
//  [sl]  - length of s (0 for strlen)
//  base  - to resolve relative paths
//  basel - length of base (0 for strlen)
//  r     - matching node, if any, returned here
//
// RETURNS
//  returns 0 on failure (memory, io) and 1 otherwise
//
int gn_lookup(const char * const restrict s, int sl, const char * const restrict base, int basel, gn ** const restrict r)
	__attribute__((nonnull));

/// gn_add
//
// SUMMARY
//  lookup or create a graph node
//
// PARAMETERS
//  base   - directory path for resolving relative paths
//  [sstk] - stringstack for resolving nofile paths
//  A        1) filepath (relative to base), or
//           2) nofile path (interpreted via sstk), or
//           2) canonical filepath
//  Al     - length of A, 0 for strlen
//  [r]    - gn for A returned here
//  [newa] - incremented by 1 if a new node was created
// 
// RETURNS
//  returns 0 on failure (memory, io) and 1 otherwise
//
int gn_add(const char * const restrict base, struct strstack * const restrict sstk, char * const restrict A, int Al, gn ** r, int * const restrict newa)
	__attribute__((nonnull(1, 3)));

/// gn_edge_add
//
// SUMMARY
//  1) lookup or create graph nodes for A, and B
//  2) add the directed edge : A -> B (A depends on B)
//  3) return gn for A, and B
//
// PARAMETERS
//  [base]     - directory path for resolving relative paths
//  [sstk]     - stringstack for resolving nofile paths
//  A          - pointer to 1) filename (relative to base), or
//                          2) relative filepath (relative to base), or
//                          3) canonical filepath, or
//                          4) gn *
//  Al         - length of A, or 0 for strlen
//  At         - LISTWISE_TYPE_GNLW if A is a gn * and 0 otherwise
//  B          - same possibilities as A
//  Bl         - length of B, 0 for strlen
//	Bt		     - LISTWISE_TYPE_GNLW if B is a gn * and 0 otherwise
//  [ffn]      - for a regular relation, originating dependency node
//  [dscv_gn]  - for a cached relation, node whose discovery cache contained this relation
//  isweak     - whether to create a weak relation
//  isweak     - whether to create a bridge relation
//  [newa]     - incremented if A was created
//  [newb]     - incremented if B was created
//  [newr]     - incremented if a new edge was created
//
// RETURNS
//  returns 0 on failure (memory, io) otherwise returns 1 and sets *A to gn for a, and *B to gn for b
//
int gn_edge_add(
	  char * const restrict base
	, struct strstack * const restrict sstk
	, void ** const restrict A, int Al, int At
	, void ** const restrict B, int Bl, int Bt
	, struct ff_node * const restrict ffn
	, gn * const restrict dscv_gn
	, int isweak
	, int isbridge
	, int * const restrict newa
	, int * const restrict newb
	, int * const restrict newr
)
	__attribute__((nonnull(3,6)));

/// gn_dump
//
// log under L_NODE properties of the gn
//
void gn_dump(gn *);

/// gn_process_invalidations
//
// SUMMARY
//  apply invalidations
//
int gn_process_invalidations(gn ** const restrict invalidations, int invalidationsl, int * const restrict primary_invalidated)
	__attribute__((nonnull));

/// gn_init
//
// SUMMARY
//  initialize the gn module
//
int gn_init();

/// gn_finalize
//
// recompute node type
//
int gn_finalize(gn * const restrict gn)
	__attribute__((nonnull));

/// gn_teardown
//
// free the dependency graph
//
void gn_teardown();

/// gn_invalid_reasons_write
//
// SUMMARY
//  writes a string describing the validity of the node
//
// PARAMETERS
//  gn  - node
//  dst - dest
//  sz  - size of dst
//
// RETURNS
//  number of bytes written, not including the terminating null byte
//
size_t gn_invalid_reasons_write(gn * const restrict gn, char * restrict dst, const size_t sz)
	__attribute__((nonnull));

#undef restrict
#endif
