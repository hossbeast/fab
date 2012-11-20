#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "coll.h"
#include "idx.h"

#define restrict __restrict

#define GN_VERSION				0x01

struct fmleval;
typedef struct gn
{
	/* exported to listwise - maintain corresponding length */
	char*							dir;						// canonical path to directory file is in
	int								dirl;
	char*							name;						// file name
	int								namel;
	char*							path;						// canonical path to file
	int								pathl;
	char*							ext;						// portion of file name following the last '.', or null
	int								extl;

	char*							hashfile_path;	// canonical path to hashfile

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

	// dependencies
	union {
		coll_singly c;

		struct
		{
			int						l;
			int						a;
			int						z;
			struct gn **	e;
		};
	} needs;

	// prerequisites
	union {
		coll_singly c;

		struct
		{
			int						l;
			int						a;
			int						z;
			struct gn **	e;
		};
	} feeds;

	// formula evaluation context which fabricates this node
	struct fmleval *	fmlv;

	// tracking fields
	char							mark;

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
		int		l;				// length
		int		a;				// allocated
		int		z;				// element size

		gn ** e;				// elements

		idx*	by_path;	// indexed by canonical path
		idx*	by_dir;		// indexed by canonical dir
	};
} gn_nodes;

extern gn * gn_root;

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
// 
// returns 0 on failure (memory, io) and 1 otherwise
//
int gn_add(char * const restrict realwd, void * const restrict A, int Al, gn ** r);

/// gn_edge_add
//
// SUMMARY
//  1) possibly create graph nodes for A, and B
//  2) add the directed edge : A -> B (A depends on B)
//  3) return gn for A
//
// PARAMETERS
//  realwd - canonical path to directory for resolving relative paths
//  A      - pointer to 1) filename (relative to realwd), or
//                      2) relative filepath (relative to realwd), or
//                      3) canonical filepath, or
//                      4) gn *
//  Al     - length of A, or 0 for strlen
//  At     - LISTWISE_TYPE_GNLW if A is a gn *, and 0 otherwise
//  B      - same possibilities as A
//  Bl     - length of B, 0 for strlen
//
// RETURNS
//  returns 0 on failure (memory, io) otherwise returns 1 and sets *A to gn for a, and *B to gn for b
//
int gn_edge_add(char * const restrict realwd, void ** const restrict A, int Al, int At, void ** const restrict B, int Bl, int Bt)
	__attribute__((nonnull));

/// gn_dump
//
// log under L_DGRAPH properties of the gn
//
void gn_dump(gn *);

void gn_hashcmd(gn * gn, char * s, int l);

int gn_hashes_read(gn * gn);

int gn_hashes_write(gn * gn);

int gn_hashes_cmp(gn *);

#endif
