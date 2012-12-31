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
#define _GNRT(a, b, c) a = c,
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
		//
		// GN_RELATION_REGULAR
		//
		struct ff_node *	ffn;			// FFN_DEPENDENCY node which gave rise to this relation

		//
		// GN_RELATION_CACHED
		//
		struct gn *				dscv_gn;	// node whose discovery contained this relation
	};

	struct gn *				A;			// A needs B
	struct gn *				B;			// B feeds A
	int								weak;		// whether this is a weak relation
} relation;

typedef struct gn
{
	uint8_t						flags;
	uint8_t						designation;

	/* exported to listwise - maintain corresponding length
	** these strings ARE null-terminated, though
	*/
	char*							dir;						// canonical path to directory file is in
	int								dirl;
	char*							name;						// file name
	int								namel;
	char*							path;						// canonical path to file
	int								pathl;
	uint32_t					pathhash;				// hash of path
	char*							ext;						// portion of file name following the last '.', or null
	int								extl;
	char*							idstring;				// identifier string, subject to execution parameters
	int								idstringl;

	//
	// PRIMARY
	//

	// change-tracking for the backing file
	hashblock *				hb;
	int								hb_loaded;

	// formula eval context for dependency discovery
	struct fmleval *		dscv;

	// depblock for dependency discovery
	struct depblock *		dscv_block;

	//
	// SECONDARY
	//
	int								exists;

	//
	// SECONDARY, GENERATED, TASK
	//

	// formula evaluation context which fabricates this node
	struct fmleval *		fabv;

	// this node depends on the nodes in this list
	union {
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

	// discovery tracking
	int									dscv_mark;

	// buildplan prune tracking
	char								changed;
	char								rebuild;
	char								poison;

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

/// gn_lookup_canon
//
// lookup a gn by absolute path
//
// returns the node, or 0 if it was not found
//
gn * gn_lookup_canon(char* s, int l);

/// gn_lookup
//
// SUMMARY
//  lookup a gn by absolute path, relative path, or NOFILE-name
//
// PARAMETERS
//  s   - string
//  l   - string length, or 0 for strlen
//  cwd - directory path for resolving relative paths
//  r   - node returned here
//
// RETURNS
//  pointer to the node, or 0 if not found
//
gn * gn_lookup(char * const restrict s, int l, char * const restrict cwd)
	__attribute__((nonnull));

/// gn_add
//
// SUMMARY
//  adds a graph node
//
// PARAMETERS
//  realwd - canonical path to directory for resolving relative paths
//  A      - 1) filename (relative to realwd), or
//           2) relative filepath (relative to realwd), or
//           3) canonical filepath
//  Al     - length of A, 0 for strlen
//  [r]    - gn for A returned here
//  [newa] - incremented by 1 if a new node was created
// 
// returns 0 on failure (memory, io) and 1 otherwise
//
int gn_add(char * const restrict realwd, void * const restrict A, int Al, gn ** r, int * const restrict newa);

/// gn_edge_add
//
// SUMMARY
//  1) possibly create graph nodes for A, and B
//  2) add the directed edge : A -> B (A depends on B)
//  3) return gn for A, and B
//
// PARAMETERS
//  [realwd]   - canonical path to directory for resolving relative paths
//  A          - pointer to 1) filename (relative to realwd), or
//                        2) relative filepath (relative to realwd), or
//                        3) canonical filepath, or
//                        4) gn *
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
	  char * const restrict realwd
	, void ** const restrict A, int Al, int At
	, void ** const restrict B, int Bl, int Bt
	, struct ff_node * const restrict ffn
	, gn * const restrict dscv_gn
	, int isweak
	, int * const restrict newa
	, int * const restrict newb
	, int * const restrict newr
)
	__attribute__((nonnull(2,5)));

/// gn_dump
//
// log under L_DGRAPH properties of the gn
//
void gn_dump(gn *);

/// gn_secondary_exists
//
// for a SECONDARY file - check whether the file exists, populate {exists}
//
int gn_secondary_exists(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_reload_dscv
//
// for a PRIMARY file - call gn_primary_reload, load dscv block
//
int gn_primary_reload_dscv(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_reload
//
// for a PRIMARY file - load the previous hashblock, stat the file
//
int gn_primary_reload(gn * const restrict)
	__attribute__((nonnull));

/// gn_primary_rewrite
//
// for a PRIMARY file - write the current hashblock
//
int gn_primary_rewrite(gn * const restrict)
	__attribute__((nonnull));

/// gn_traverse_needs
//
// SUMMARY
//  traverse the graph needs-wise, depth first, starting at gn, with guards to detect a cycle
//  apply logic at each node along the way, which returns 0 to stop the traversal with failure
//
// RETURNS
//  0 if a cycle was detected, or if logic ever returned 0, and 1 otherwise
//
int gn_depth_traversal_nodes_needsward(gn * gn, int (*logic)(struct gn *, int d));

/// gn_idstring
//
// get a string identifying a node, subject to execution arguments
//
char * gn_idstring(gn * const restrict gn)
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
void gn_invalidations();

/// gn_teardown
//
// free the dependency graph
//
void gn_teardown();

#undef restrict
#endif
