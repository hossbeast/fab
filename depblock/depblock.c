#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "depblock.h"
#include "identity.h"
#include "dirutil.h"

#include "control.h"
#include "xmem.h"
#include "xstring.h"

///
/// public
///

int depblock_create(depblock ** const block, const char * const dirfmt, ...)
{
	fatal(xmalloc, block, sizeof(**block));

	va_list va;
	va_start(va, dirfmt);
	int req = vsnprintf(0, 0, dirfmt, va);
	va_end(va);

	fatal(xmalloc, &(*block)->blockdir, req + 1);
	va_start(va, dirfmt);
	vsprintf((*block)->blockdir, dirfmt, va);
	va_end(va);

	fatal(xsprintf, &(*block)->block_path, "%s/dscv", (*block)->blockdir);

	finally : coda;
}

void depblock_free(depblock * const block)
{
	if(block)
	{
		free(block->blockdir);
		free(block->block_path);
	}

	free(block);
}

void depblock_xfree(depblock ** const block)
{
	depblock_free(*block);
	*block = 0;
}

int depblock_read(depblock * const block)
{
	// open the file readonly
	if((block->fd = open(block->block_path, O_RDONLY)) > 0)
	{
		// get the size of the file
		if((block->size = lseek(block->fd, 0, SEEK_END)) == (off_t)-1)
			fail("lseek(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		// seek back to the start
		if(lseek(block->fd, 0, SEEK_SET) == (off_t)-1)
			fail("lseek(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		if((block->addr = mmap(0, block->size, PROT_READ, MAP_PRIVATE, block->fd, 0)) == MAP_FAILED)
			fail("mmap(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		// block ready to process
		block->block = block->addr;
	}
	else if(errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", block->block_path, errno, strerror(errno));
	}

	finally : coda;
}

int depblock_allocate(depblock * const block)
{
	fatal(xmalloc, &block->block, sizeof(*block->block));

	finally : coda;
}

int depblock_close(depblock * const block)
{
	if(block->block)
	{
		if(munmap(block->addr, block->size) == -1)
			fail("munmap()=[%d][%s]", errno, strerror(errno));

		if(close(block->fd) == -1)
			fail("close()=[%d][%s]", errno, strerror(errno));

		block->fd = 0;
		block->addr = 0;
		block->size = 0;
		block->block = 0;
	}

	finally : coda;
}

int depblock_write(const depblock * const block)
{
	if(block->block)
	{
		int fd = 0;
		void * addr = 0;
		size_t size = sizeof(*block->block);

		fatal(identity_assume_fabsys);

		// ensure directory exists
		fatal(mkdirp, block->blockdir, S_IRWXU | S_IRWXG | S_IRWXO);

		// open the file for writing
		if((fd = open(block->block_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			fail("open(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		// set the filesize
		if(ftruncate(fd, size) == -1)
			fail("ftruncate(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		// map the entire file writable
		if((addr = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
			fail("mmap(%s)=[%d][%s]", block->block_path, errno, strerror(errno));

		// copy the data in
		memcpy(addr, block->block, size);

		// close and unmap
		if(munmap(addr, size) == -1)
			fail("munmap()=[%d][%s]", errno, strerror(errno));

		if(close(fd) == -1)
			fail("close()=[%d][%s]", errno, strerror(errno));

		fatal(identity_assume_user);
	}

	finally : coda;
}
