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

#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "hashblock.h"
#include "depblock.h"

#include "coll.h"
#include "map.h"
#include "strstack.h"
#include "path.h"

#define restrict __restrict

/*
** graph node designations
**
** 1. HASNEED - has any dependencies
** 2. CANFAB  - can be fabricated by some formula
** 3. !NOFILE - has a backing file
**
**              1  2  3
** PRIMARY   - [ ][ ][x]
** SECONDARY - [x][x][x]
** GENERATED - [ ][x][x]
** TASK      - [x][x][ ]
** NOFILE    - [x][ ][ ]
**
*/

//
// gn designation table
//
#define GN_DESIGNATION_TABLE(x)																											\
	_GN_DESIGNATION(GN_DESIGNATION_TASK								, 0x01	, "TASK"				, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_SECONDARY					, 0x02	, "SECONDARY"		, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_GENERATED					, 0x03	, "GENERATED"		, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_GROUP							, 0x04	, "GROUP"				, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_PRIMARY						, 0x05	, "PRIMARY"			, x)

enum {
#define _GN_DESIGNATION(a, b, c, d) a = b,
GN_DESIGNATION_TABLE(0)
#undef _GN_DESIGNATION
};

#define _GN_DESIGNATION(a, b, c, d) (d) == b ? c :
#define GN_DESIGNATION_STR(gn) GN_DESIGNATION_TABLE(gn) "unknown"

#define GN_FLAGS_HASNEED			0x01
#define GN_FLAGS_CANFAB				0x02
#define GN_FLAGS_NOFILE				0x04

//
// gn relation type table
//
#define GNR_TYPE_TABLE(x)																																						\
	_GNRT(GN_RELATION_REGULAR			, 0x01	, x)		/* relation having arisen from parsing a fabfile */	\
	_GNRT(GN_RELATION_CACHED			, 0x02	, x)		/* relation recovered from cached ddisc results  */	\

enum {
#define _GNRT(a, b, c) a = b,
GNR_TYPE_TABLE(0)
#undef _GNRT
};

#define _GNRT(a, b, c) (c) == b ? #a : 
#define GNRT_STR(x) GNR_TYPE_TABLE(x) "unknown"

#define GN_IS_INVALID(x) (																																											\
	   (x)->designate == GN_DESIGNATION_TASK				/* always invalid */																					\
	|| (x)->designate == GN_DESIGNATION_GENERATED		/* always invalid */																					\
	|| (x)->force_invalid														/* has been invalidated */																		\
	|| (x)->force_ff																/* has been invalidated due to associated FF invalidation	*/	\
	|| (x)->force_needs															/* has been invalidated due to dependency gn invalidation	*/	\
	|| (x)->force_noexists													/* is invalid because backing file is missing */							\
	|| (x)->force_changed														/* is invalid because backing file has changed */							\
)

struct ff_file;
struct ff_node;
struct fmleval;
struct gn;

typedef struct
{
	uint32_t					type;		// one of GN_RELATION_*

	union
	{
		struct {									// GN_RELATION_REGULAR
			struct ff_node *	ffn;			// FFN_DEPENDENCY node which gave rise to this relation
		};

		struct {									// GN_RELATION_CACHED
			struct gn *				dscv_gn;	// node whose discovery contained this relation
		};
	};

	struct gn *				A;			// A needs B
	struct gn *				B;			// B feeds A
	int								weak;		// whether this is a weak relation
	int								bridge;	// whether this is a bridge relation (unreachable via fabfile traversals)
} relation;

typedef struct gn
{
	uint8_t						flags;
	uint8_t						designate;

	path *						path;
	char *						idstring;				// identifier string, subject to execution parameters
	int								idstringl;

	char *						designation;
	int								designationl;

	/*
	** the ff closure of an gn is all of the ff_files which might affect this node if they change
	** (FFT_REGULAR only)
	*/
	struct ff_file **	closure_ffs;
	int								closure_ffsl;
	int								closure_ffsa;

	int								reloaded;												// whether this node has been reloaded

	char *						cache_dir;											// canonical path to the cachedir for this node
	char *						ineed_skipweak_dir;
	char *						ifeed_skipweak_dir;

	char *						noforce_invalid_path;						// canonical path to the noforce_invalid file
	char *						noforce_needs_path;							// canonical path to the noforce_needs file
	char *						noforce_ff_path;								// canonical path to the noforce_ff file

	int								force_invalid;									// whether action is forced because this node has been invalidated
	int								force_ff;												// whether action is forced because an associated FF_REG was invalidated
	int								force_needs;										// whether action is forced because an antecedent node was invalidated
	int								force_noexists;									// whether action is forced because the backing file does not exist
	int								force_changed;									// whether action is forced because the backing file has changed

	//
	// PRIMARY
	//
	struct
	{
		// change-tracking for the backing file
		hashblock *				primary_hb;

		// formula eval contexts for dependency discovery
		//  ** a PRIMARY node has 0-n fmleval's for discovery
		struct fmleval **	dscvs;
		int								dscvsl;
		int								dscvsa;

		// depblock for dependency discovery
		struct depblock *	dscv_block;
	};

	//
	// SECONDARY
	//

	//
	// SECONDARY, GENERATED, TASK
	//

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

			map *					by_B;	// lookup by canonical path of the node at the other end of the relation
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

			map *					by_A;	// indexed by canonical path of the node at the other end of the relation
		};
	} feeds;

	// buildplan create tracking
	int									height;		// distance of longest route to a leaf node
	int									stage;		// assigned stage - NEARLY always equal to height

	// traversal tracking
	int									guard;
	int									travel;

	// buildplan eval tracking
//	char								changed;
	char								rebuild;
	char								poison;
//	char								invalid;
//	char								weak_invalid;
} gn;

extern union gn_nodes_t
{
	coll_doubly c;

	struct
	{
		int		l;						// length
		int		a;						// allocated
		int		z;						// element size

		gn ** e;						// elements

		map *	by_path;			// indexed by canonical path
		map *	by_pathhash;	// indexed by canonical path hash
	};
} gn_nodes;

/// gn_lookup
//
// SUMMARY
//  get graph nodes whose path(s) match certain criteria
//
// PARAMETERS
//  s    - one of
//          2) @s  - nofile specifier
//          3) /s  - canonical path
//          4)  s  - path relative to base
//  base - to resolve relative paths
//  r    - matching node, if any, returned here
//
// RETURNS
//  returns 0 on failure (memory, io) and 1 otherwise
//
int gn_lookup(const char * const restrict s, const char * const restrict base, gn ** const restrict r)
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
int gn_add(const char * const restrict base, strstack * const restrict sstk, char * const restrict A, int Al, gn ** r, int * const restrict newa)
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
	, strstack * const restrict sstk
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
// log under L_DGRAPH properties of the gn
//
void gn_dump(gn *);

/// gn_reconcile_dsc
//
// SUMMARY
//  reconcile noforce files with the fact that discovery was successfully completed for a node
//
// PARAMETERS
//  gn - node for which discovery was successfully completed
//
int gn_reconcile_dsc(gn * const restrict)
	__attribute__((nonnull));

/// gn_reconcile_fab
//
// SUMMARY
//  reconcile noforce files with the fact that fabrication was successfully completed for a node
//
// PARAMETERS
//  gn - node for which fabrication was successfully completed
//  ws - workspace (required)
//
int gn_reconcile_fab(gn * const gn, map * const ws)
	__attribute__((nonnull));

/// gn_designation
//
// only available after gn_finalize
//
// return GN_DESIGNATION_STR(gn->designation)
//
char * gn_designation(gn * gn)
	__attribute__((nonnull));

/// gn_process_invalidations
//
// SUMMARY
//  apply invalidations
//
int gn_process_invalidations(gn *** const restrict invalidations, int invalidationsl)
	__attribute__((nonnull));

/// gn_init
//
// SUMMARY
//  initialize gn module
//
int gn_init();

/// gn_finalize
//
// gn fully loaded; populate flags and designation
//
int gn_finalize();

/// gn_teardown
//
// free the dependency graph
//
void gn_teardown();

/// gn_enclose_ff
//
// SUMMARY
//  mark an gn as being affected by an ff_file by appending ff to its closure_ffs list
//
// PARAMETERS
//  newa - *newa incremented by 1 if ff_file was added to gn->affected_ff_file
//
int gn_enclose_ff(gn * const restrict gn, struct ff_file * const restrict ff, int * const restrict newa)
	__attribute__((nonnull));

/// gn_invalid_reason
//
// SUMMARY
//  writes a string describing the validity of the node
//
// PARAMETERS
//  s  - dest
//  sz - size of dest
//  gn - node
//
// RETURNS
//  returns s
//
char * gn_invalid_reason(char * restrict s, const size_t sz, gn * const restrict gn)
	__attribute__((nonnull));

#undef restrict
#endif
