#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "gn.h"

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

#define GN_FLAGS_HASNEED			0x01
#define GN_FLAGS_CANFAB				0x02
#define GN_FLAGS_NOFILE				0x04

//
// gn designation table
//
#define GN_DESIGNATION_TABLE(x)																											\
	_GN_DESIGNATION(GN_DESIGNATION_TASK								, 0x01	, "TASK"				, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_SECONDARY					, 0x02	, "SECONDARY"		, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_SECONDARY_NOFORM		, 0x03	, "SECONDARY"		, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_GENERATED					, 0x04	, "GENERATED"		, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_NOFILE							, 0x05	, "NOFILE"			, x)		\
	_GN_DESIGNATION(GN_DESIGNATION_PRIMARY						, 0x06	, "PRIMARY"			, x)		\

enum {
#define _GN_DESIGNATION(a, b, c, d) a = b,
GN_DESIGNATION_TABLE(0)
#undef _GN_DESIGNATION
};

#define _GN_DESIGNATION(a, b, c, d) (d) == b ? c :
#define GN_DESIGNATION_STR(x) GN_DESIGNATION_TABLE(x) "unknown"

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
} relation;

typedef struct gn
{
	uint8_t						flags;
	uint8_t						designation;

	path *						path;
	char*							idstring;				// identifier string, subject to execution parameters
	int								idstringl;

	/*
	** the ff closure of an gn is all of the ff_files which might affect this node if they change
	** (FFT_REGULAR only)
	*/
	struct ff_file **	closure_ffs;
	int								closure_ffsl;
	int								closure_ffsa;

	//
	// PRIMARY
	//
	struct
	{
		// change-tracking for the backing file
		hashblock *				hb_fab;				// rewritten following successful fabrication of all feeds
		hashblock *				hb_dscv;			// rewritten following successful dependency discovery of this node
		int								hb_loaded;

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
	struct
	{
		char *						noforce_dir;
		char *						noforce_path;	// canonical path to the noforce file

		int								exists;				// whether the file exists
		int								fab_noforce;	// whether fabrication of the file is not forced
	};

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

	// buildplan prune tracking
	char								changed;
	char								rebuild;
	char								poison;
	char								invalid;

	char								fab_success;
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
	};
} gn_nodes;

/// gn_match
//
// SUMMARY
//  get graph nodes whose path(s) match certain criteria
//
// PARAMETERS
//  base - directory path for resolving relative paths
//  s    - one of
//          1) /s/ - regex to match on can/abs/rel paths
//          2) .s  - nofile specifier
//          3) /s  - canonical path
//          4)  s  - path relative to base
//  r    - results appended to this list
//  rl   - length of r
//  ra   - allocated size of r
//
// RETURNS
//  returns 0 on failure (memory, io) and 1 otherwise
//
int gn_match(const char * const base, const char * const restrict s, gn *** const restrict r, int * const restrict rl, int * const ra)
	__attribute__((nonnull));

/// gn_add
//
// SUMMARY
//  lookup or create a graph node
//
// PARAMETERS
//  [base] - directory path for resolving relative paths
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
	__attribute__((nonnull(3)));

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

/// gn_secondary_reload
//
// for a SECONDARY file - check whether the file exists, reload 
//
int gn_secondary_reload(gn * const restrict)
	__attribute__((nonnull));

/// gn_secondary_rewrite_fab
//
// for a SECONDARY file - rewrite the fab/noforce file
//
int gn_secondary_rewrite_fab(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_reload
//
// for a PRIMARY file - load the previous hashblocks, stat the file
//
int gn_primary_reload(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_rewrite_fab
//
// for a PRIMARY file - write the current fab hashblock
//
int gn_primary_rewrite_fab(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_reload_dscv
//
// for a PRIMARY file - call gn_primary_reload, load dscv block
//
int gn_primary_reload_dscv(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_rewrite_dscv
//
// for a PRIMARY file - write the current dscv hashblock, write the dscv block, if any
//
int gn_primary_rewrite_dscv(gn * const restrict)
	__attribute__((nonnull));

/// gn_designate
//
// populate gn->flags and gn->designation
//
// return GN_DESIGNATION_STR(gn->designation)
//
char * gn_designate(gn * gn)
	__attribute__((nonnull));

/// gn_invalidations
//
// SUMMARY
//  process user-specified invalidations - also calls gn_designate on all nodes
//
//  for PRIMARY   - may set gn->changed
//  for SECONDARY - may set gn->rebuild
//
// RETURNS
//  0 on ENOMEM, 1 otherwise
//
int gn_invalidations();

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

#undef restrict
#endif
