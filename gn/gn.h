#ifndef _GN_H
#define _GN_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "coll.h"
#include "idx.h"

#define GN_VERSION				0x01

typedef struct gn
{
	char*							dir;						// canonical path to directory file is in
	char*							name;						// file name
	char*							path;						// canonical path to file
	char*							ext;						// portion of file name following the last '.', or null
	char *						hashfile_path;	// canonical path to hashfile

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

	// formula for fabricating this node
	struct fml *			fml;

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
// add the directed edge : A -> B (A depends on B)
//
int gn_add(char* cwd, char* A, char* B, gn ** r);

/// gn_lookup
//
//
//
gn* gn_lookup(char* path);

/// gn_dump
//
//
//
void gn_dump(gn *);

void gn_cmdhash(gn * gn, char *, int);

int gn_hashes_read(gn * gn);

int gn_hashes_write(gn * gn);

int gn_hashes_cmp(gn *);

#endif
