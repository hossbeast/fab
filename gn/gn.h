#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "coll.h"
#include "map.h"

#define restrict __restrict

#define GN_VERSION				0x01

/*
** graph node designations
**
** 1. has any dependencies
** 2. can be fabricated by some formula
** 3. has a backing file
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

struct ff_file;
struct ff_node;
struct fmleval;
struct gn;

typedef struct
{
	struct ff_node *	ffn;		// FFN_DEPENDENCY node which gave rise to this relation

	struct gn *				A;			// A needs B
	struct gn *				B;			// B feeds A

	int								weak;		// whether this is a weak relation

	// tracking
	int								mark;
	int								guard;
} relation;

typedef struct gn
{
	uint8_t						flags;

	/* exported to listwise - maintain corresponding length
	** these strings ARE null-terminated, though
	*/
	char*							dir;						// canonical path to directory file is in
	int								dirl;
	char*							name;						// file name
	int								namel;
	char*							path;						// canonical path to file
	int								pathl;
	char*							ext;						// portion of file name following the last '.', or null
	int								extl;
	char*							idstring;				// identifier string, subject to execution parameters
	int								idstringl;

  char*							hashfile_path;  // canonical path to hashfile

	// fields for computing prophash
	struct __attribute__((packed))
	{
		// stat fields
		dev_t							dev;     /* ID of device containing file */
		ino_t							ino;     /* inode number */
		mode_t						mode;    /* protection */
		nlink_t						nlink;   /* number of hard links */
		uid_t							uid;     /* user ID of owner */
		gid_t							gid;     /* group ID of owner */
		off_t							size;    /* total size, in bytes */
		time_t						mtime;   /* time of last modification */
		time_t						ctime;   /* time of last status change */
	};

	// fields for the hashfile
	uint32_t					vrs[2];
	uint32_t					prop_hash[2];	// hash of fs properties
	uint32_t					cmd_hash[2];	// hash of cmd which last fabricated this node

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

	// formula evaluation context which fabricates this node
	struct fmleval *	fabv;

	// formula evaluation context which gives dependencies for this node
	struct fmleval *	dscv;

	// buildplan create tracking
	int								height;		// distance of longest route to a leaf node
	int								stage;		// assigned stage - NEARLY always equal to height

	// traversal tracking
	int								guard;

	// discovery tracking
	int								dscv_mark;

	// buildplan prune tracking
	char							changed;
	char							rebuild;
	char							poison;

	char							fab_success;
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
//  realwd - canonical path to directory for resolving relative paths
//  A      - pointer to 1) filename (relative to realwd), or
//                      2) relative filepath (relative to realwd), or
//                      3) canonical filepath, or
//                      4) gn *
//  Al     - length of A, or 0 for strlen
//  At     - LISTWISE_TYPE_GNLW if A is a gn * and 0 otherwise
//  B      - same possibilities as A
//  Bl     - length of B, 0 for strlen
//	Bt		 - LISTWISE_TYPE_GNLW if B is a gn * and 0 otherwise
//  ffn    - originating dependency node
//  [newa] - incremented if A was created
//  [newb] - incremented if B was created
//  [newr] - incremented if a new edge was created
//
// RETURNS
//  returns 0 on failure (memory, io) otherwise returns 1 and sets *A to gn for a, and *B to gn for b
//
int gn_edge_add(
	  char * const restrict realwd
	, void ** const restrict A, int Al, int At
	, void ** const restrict B, int Bl, int Bt
	, struct ff_node * const restrict ffn
	, int * const restrict newa
	, int * const restrict newb
	, int * const restrict newr
)
	__attribute__((nonnull(1,2,5,8)));

/// gn_dump
//
// log under L_DGRAPH properties of the gn
//
void gn_dump(gn *);

void gn_hashcmd(gn * gn, char * s, int l);

int gn_hashes_read(gn * gn);

int gn_hashes_write(gn * gn);

int gn_hashes_cmp(gn *);

/// gn_traverse_needs
//
// SUMMARY
//  traverse the graph needs-wise, depth first, starting at gn.
//  apply logic at each node along the way
//  detect cycles, and fail if one is found
//
int gn_depth_traversal_nodes_needsward(gn * gn, void (*logic)(struct gn *, int d));

/// gn_traverse_relations_needsward
//
// SUMMARY
//  same as gn_traverse_needs, except execute logic on relations
//  note, this excludes the starting node
//
int gn_depth_traversal_relations_needsward(gn * r, void (*logic)(relation*));

/// gn_idstring
//
// get a string identifying a node, subject to execution arguments
//
char * gn_idstring(gn * gn)
	__attribute__((nonnull));

#endif
