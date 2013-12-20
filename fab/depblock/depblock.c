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
#include "global.h"
#include "xmem.h"
#include "xstring.h"

///
/// public
///

int depblock_create(depblock ** const block, const char * const fmt, ...)
{
	fatal(xmalloc, block, sizeof(**block));

	va_list va;
	va_start(va, fmt);
	int req = vsnprintf(0, 0, fmt, va);
	va_end(va);

	fatal(xmalloc, &(*block)->blockpath, req + 1);
	va_start(va, fmt);
	vsprintf((*block)->blockpath, fmt, va);
	va_end(va);

	(*block)->fd = -1;
	(*block)->addr = MAP_FAILED;

	finally : coda;
}

void depblock_free(depblock * const block)
{
	if(block)
	{
		free(block->blockpath);
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
	fatal(identity_assume_fabsys);

	// open the file readonly
	if((block->fd = open(block->blockpath, O_RDONLY)) > 0)
	{
		// get the size of the file
		if((block->size = lseek(block->fd, 0, SEEK_END)) == (off_t)-1)
			fail("lseek(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

		// seek back to the start
		if(lseek(block->fd, 0, SEEK_SET) == (off_t)-1)
			fail("lseek(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

		if((block->addr = mmap(0, block->size, PROT_READ, MAP_PRIVATE, block->fd, 0)) == MAP_FAILED)
			fail("mmap(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

		// block is ready to process
		block->block = block->addr;
	}
	else if(errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));
	}

	fatal(identity_assume_user);

	finally : coda;
}

int depblock_allocate(depblock * const block)
{
	fatal(xmalloc, &block->block, sizeof(*block->block));

	finally : coda;
}

int depblock_addrelation(depblock * const db, const path * const A, const path * const B, int isweak, int isbridge)
{
	if(A->in_basel >= (sizeof(db->block->sets[0].nbase) / sizeof(db->block->sets[0].nbase[0])))
		return 1; // nbase too long

	if(A->in_pathl >= (sizeof(db->block->sets[0].needs) / sizeof(db->block->sets[0].needs[0])))
		return 1;	// in too long

	if(B->in_basel >- (sizeof(db->block->sets[0].fbase) / sizeof(db->block->sets[0].fbase[0])))
		return 1; // fbase too long

	if(B->in_pathl >= (sizeof(db->block->sets[0].feeds) / sizeof((db->block->sets[0].feeds[0]))))
		return 1;	// in too long

	// locate a suitable set, which has matching bases, needs, and weak
	int x;
	for(x = 0; x < db->block->setsl; x++)
	{
		if(db->block->sets[x].weak == isweak)
		{
			if(db->block->sets[x].bridge == isbridge)
			{
				if(strcmp(db->block->sets[x].nbase, A->in_base ?: "") == 0 && strcmp(db->block->sets[x].needs, A->in_path) == 0)
				{
					if(strcmp(db->block->sets[x].fbase, B->in_base ?: "") == 0)
					{
						break;
					}
				}
			}
		}
	}

	if(x == db->block->setsl)
	{
		// no suitable set was found
		if(x == (sizeof(db->block->sets) / sizeof(db->block->sets[0])))
			return 1;	// nospace

		if(A->in_base)
			memcpy(db->block->sets[x].nbase, A->in_base, A->in_basel);

		if(B->in_base)
			memcpy(db->block->sets[x].fbase, B->in_base, B->in_basel);

		memcpy(db->block->sets[x].needs, A->in_path, A->in_pathl);

		db->block->sets[x].weak = isweak;
		db->block->sets[x].bridge = isbridge;
		db->block->setsl++;
	}

	if(db->block->sets[x].feedsl == (sizeof(db->block->sets[0].feeds) / sizeof(db->block->sets[0].feeds[0])))
		return 1;	// too many feeds

	memcpy(db->block->sets[x].feeds[db->block->sets[x].feedsl++], B->in_path, B->in_pathl);
	return 0;
}

int depblock_write(const depblock * const block)
{
	if(block->block)
	{
		int fd = 0;
		void * addr = 0;
		size_t size = sizeof(*block->block);

		fatal(identity_assume_fabsys);

		// open the file for writing
		if((fd = open(block->blockpath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			fail("open(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

		// set the filesize
		if(ftruncate(fd, size) == -1)
			fail("ftruncate(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

		// map the entire file writable
		if((addr = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
			fail("mmap(%s)=[%d][%s]", block->blockpath, errno, strerror(errno));

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

int depblock_close(depblock * const block)
{
	if(block)
	{
		if(block->addr != MAP_FAILED)
		{
			if(munmap(block->addr, block->size) == -1)
				fail("munmap()=[%d][%s]", errno, strerror(errno));

			block->addr = MAP_FAILED;
			block->block = 0;
		}

		if(block->fd != -1)
		{
			if(close(block->fd) == -1)
				fail("close(%d)=[%d][%s]", block->fd, errno, strerror(errno));

			block->fd = -1;
		}

		free(block->block);
		block->block = 0;
	}

	finally : coda;
}
