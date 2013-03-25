#ifndef _HASHBLOCK_H
#define _HASHBLOCK_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define restrict __restrict

#define HB_SAME			0		/* unchanged */
#define HB_VERSION	1		/* differ in version number */
#define HB_STAT			2		/* differ in stat properties */
#define HB_CONTENT	3		/* differ in file contents */

typedef struct
{
	struct __attribute__((packed))
	{
		// stathash fields
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

	char *						hashdir;				// directory where the other *hash_path's reside

	uint32_t					stathash[2];		// hash of fs properties
	char *						stathash_path;

	uint32_t					contenthash[2];	// hash of file contents
	char *						contenthash_path;

	uint32_t					vrshash[2];			// version identifier
	char *						vrshash_path;
} hashblock;

/// hashblock_create
//
// SUMMARY
//  create a hashblock 
//
int hashblock_create(hashblock ** const restrict hb, const char * const restrict dirfmt, ...)
	__attribute__((nonnull));

/// hashblock_free
//
// SUMMARY
//  free a hashblock with free semantics
//
void hashblock_free(hashblock * const restrict hb);


/// hashblock_xfree
//
// SUMMARY
//  free a hashblock with xfree semantics
//
void hashblock_xfree(hashblock ** const restrict hb)
	__attribute__((nonnull));

/// hashblock_stat
//
// SUMMARY
//  stat the specified file and populate stathash[1] on all specified hashblocks
//
int hashblock_stat(const char * const restrict path, hashblock * const restrict hb0, hashblock * const restrict hb1, hashblock * const restrict hb2);

/// hashblock_read
//
// SUMMARY
//  read the hashes for the hashblock, populate hb->*hash[0]
//
int hashblock_read(hashblock * const restrict)
	__attribute__((nonnull));

/// hashblock_write
//
//
//
int hashblock_write(const hashblock * const restrict)
	__attribute__((nonnull));

/// hashblock_cmp
//
// SUMMARY
//  compare the two states contained in the hashblock
//
// RETURNS
//  one of HBLOCK_*
//
int hashblock_cmp(const hashblock * const restrict)
	__attribute__((nonnull));

#undef restrict
#endif
