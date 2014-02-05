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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "depblock.h"
#include "identity.h"

#include "global.h"

#include "dirutil.h"
#include "xlinux.h"
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

	// open the file readonly - gxopen only fails when errno != ENOENT
	fatal(gxopen, block->blockpath, O_RDONLY, &block->fd);

	if(block->fd != -1)
	{
		// get the size of the file
		fatal(xlseek, block->fd, 0, SEEK_END, (void*)&block->size);

		// seek back to the start
		fatal(xlseek, block->fd, 0, SEEK_SET, 0);
		
		if((block->addr = mmap(0, block->size, PROT_READ, MAP_PRIVATE, block->fd, 0)) == MAP_FAILED)
			sysfatality("mmap");

		// block is ready to process
		block->block = block->addr;
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
		fatal(xopen_mode, block->blockpath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);

		// set the filesize
		sysfatalize(ftruncate, fd, size);

		// map the entire file writable
		fatal(xmmap, 0, size, PROT_WRITE, MAP_SHARED, fd, 0, &addr);

		// copy the data in
		memcpy(addr, block->block, size);

		// close and unmap
		fatal(xmunmap, addr, size);

		fatal(xclose, fd);

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
			fatal(xmunmap, block->addr, block->size);

			block->addr = MAP_FAILED;
			block->block = 0;
		}

		if(block->fd != -1)
		{
			fatal(xclose, block->fd);

			block->fd = -1;
		}

		free(block->block);
		block->block = 0;
	}

	finally : coda;
}
