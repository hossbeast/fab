#ifndef _DEPBLOCK_H
#define _DEPBLOCK_H

#include <sys/types.h>

#define restrict __restrict

typedef struct
{
	int		needsl;
	char	needs[1][256];

	int		feedsl;
	char	feeds[128][256];
} dep_relations_set;

typedef struct depblock
{
	char *						blockdir;			// directory containing the block
	char *						block_path;		// path to the block file

	// mmap fields
	void *						addr;
	size_t						size;
	int								fd;

	// dependency block
	struct
	{
		dep_relations_set weak;
		dep_relations_set strong;
	}								* block;
} depblock;

int depblock_create(depblock ** const restrict block, const char * const restrict dirfmt, ...)
	__attribute__((nonnull));

int depblock_allocate(depblock * const restrict block)
	__attribute__((nonnull));

void depblock_free(depblock * const restrict block)
	__attribute__((nonnull));

void depblock_xfree(depblock ** const restrict block)
	__attribute__((nonnull));

int depblock_read(depblock * const restrict block)
	__attribute__((nonnull));

int depblock_close(depblock * const restrict block)
	__attribute__((nonnull));

int depblock_write(const depblock * const restrict block)
	__attribute__((nonnull));

#undef restrict
#endif
