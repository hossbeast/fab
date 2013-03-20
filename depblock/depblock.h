#ifndef _DEPBLOCK_H
#define _DEPBLOCK_H

#include <stdint.h>
#include <sys/types.h>

#include "path.h"

#define restrict __restrict

/*
** for now a single depblock corresponds to a single PRIMARY file, and may only contain
** relations A -> B from a single node A
**
**
**
*/

typedef struct
{
	uint8_t setsl;

	struct
	{
		uint8_t	weak;

		char		needs[256];
		char		nbase[256];

		uint8_t	feedsl;
		char		feeds[128][256];
		char		fbase[256];
	} sets[4];
} dep_relations;

typedef struct depblock
{
	char *						blockdir;			// directory containing the block
	char *						block_path;		// path to the block file

	// mmap fields
	void *						addr;
	size_t						size;
	int								fd;

	// dependency block
	dep_relations *		block;
} depblock;

int depblock_create(depblock ** const restrict block, const char * const restrict dirfmt, ...)
	__attribute__((nonnull));

int depblock_allocate(depblock * const restrict block)
	__attribute__((nonnull));

void depblock_free(depblock * const restrict block);

void depblock_xfree(depblock ** const restrict block)
	__attribute__((nonnull));

int depblock_read(depblock * const restrict block)
	__attribute__((nonnull));

int depblock_close(depblock * const restrict block)
	__attribute__((nonnull));

int depblock_write(const depblock * const restrict block)
	__attribute__((nonnull));

int depblock_addrelation(depblock * const restrict block, const path * const restrict A, const path * const restrict B, int isweak)
	__attribute__((nonnull));

#undef restrict
#endif
