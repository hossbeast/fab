/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "xlinux.h"

#include "hashblock.h"

#include "logs.h"
#include "global.h"
#include "identity.h"
#include "args.h"

#include "args.h"
#include "xlinux.h"
#include "unitstring.h"
#include "cksum.h"
#include "dirutil.h"

#define restrict __restrict

//
// public
//

int hashblock_create(hashblock ** const hb)
{
	fatal(xmalloc, hb, sizeof(**hb));

	finally : coda;
}

int hashblock_stat(const char * const path, hashblock * const hb)
{
	struct stat stb;

	// cycle
	hb->stathash[0] = hb->stathash[1];
	hb->contenthash[0] = hb->contenthash[1];

	// STAT for A
	int r;
	fatal(uxstat, path, &stb, &r);
	if(r == 0)
	{
		uint32_t cks = 0;

		hb->dev			= stb.st_dev;
		hb->ino			= stb.st_ino;
		hb->mode		= stb.st_mode;
		hb->nlink		= stb.st_nlink;
		hb->uid			= stb.st_uid;
		hb->gid			= stb.st_gid;
		hb->size		= stb.st_size;
		hb->mtime		= stb.st_mtime;
		hb->ctime		= stb.st_ctime;

		cks = cksum(
				(char*)&hb->dev
			, (char*)&hb->ctime - (char*)&hb->dev
		);

		hb->stathash[1] = cks;
	}
	else
	{
		hb->stathash[1] = 0;
	}

	finally : coda;
}

int hashblock_cmp(const hashblock * const hb)
{
	return		 hb->stathash[1] - hb->stathash[0]				? HB_STAT
					 : hb->contenthash[1] - hb->contenthash[0]	? HB_CONTENT
																											: HB_SAME;
}

void hashblock_free(hashblock * const hb)
{
	free(hb);
}

void hashblock_xfree(hashblock ** const hb)
{
	hashblock_free(*hb);
	*hb = 0;
}

void hashblock_reset(hashblock * const restrict hb)
{
	hb->stathash[0] = hb->stathash[1] = 0;
	hb->contenthash[0] = hb->contenthash[1] = 0;
}
