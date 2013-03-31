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

#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "hashblock.h"

#include "log.h"
#include "control.h"
#include "identity.h"
#include "args.h"

#include "args.h"
#include "xmem.h"
#include "unitstring.h"
#include "cksum.h"
#include "xstring.h"
#include "dirutil.h"

#define restrict __restrict

//
// public
//

int hashblock_create(hashblock ** const hb, const char * const dirfmt, ...)
{
	fatal(xmalloc, hb, sizeof(**hb));

	va_list va;
	va_start(va, dirfmt);
	int req = vsnprintf(0, 0, dirfmt, va);
	va_end(va);

	fatal(xmalloc, &(*hb)->hashdir, req + 1);
	va_start(va, dirfmt);
	vsprintf((*hb)->hashdir, dirfmt, va);
	va_end(va);

	fatal(xsprintf, &(*hb)->stathash_path, "%s/stat", (*hb)->hashdir);
	fatal(xsprintf, &(*hb)->contenthash_path, "%s/content", (*hb)->hashdir);
	fatal(xsprintf, &(*hb)->vrshash_path, "%s/vrs", (*hb)->hashdir);

	// attach version id
	(*hb)->vrshash[1] = FAB_VERSION;

	finally : coda;
}

int hashblock_stat(const char * const path, hashblock * const hb0, hashblock * const hb1, hashblock * const hb2)
{
	struct stat stb;

	hashblock * hbs[3];
	int					hbsl = 0;

	if(hb0)
		hbs[hbsl++] = hb0;
	if(hb1)
		hbs[hbsl++] = hb1;
	if(hb2)
		hbs[hbsl++] = hb2;

	fatal(identity_assume_fabsys);

	int x;

	// STAT for A
	if(stat(path, &stb) == 0)
	{
		uint32_t cks = 0;

		for(x = 0; x < hbsl; x++)
		{
			hbs[x]->dev			= stb.st_dev;
			hbs[x]->ino			= stb.st_ino;
			hbs[x]->mode		= stb.st_mode;
			hbs[x]->nlink		= stb.st_nlink;
			hbs[x]->uid			= stb.st_uid;
			hbs[x]->gid			= stb.st_gid;
			hbs[x]->size		= stb.st_size;
			hbs[x]->mtime		= stb.st_mtime;
			hbs[x]->ctime		= stb.st_ctime;

			if(x == 0)
			{
				cks = cksum(
						(char*)&hbs[0]->dev
					, (char*)&hbs[0]->ctime - (char*)&hbs[0]->dev
				);
			}

			hbs[x]->stathash[1] = cks;
		}
	}
	else if(errno != ENOENT)
	{
		fail("stat(%s)=[%d][%s]", path, errno, strerror(errno));
	}
	else
	{
		for(x = 0; x < hbsl; x++)
			hbs[x]->stathash[1] = 0;
	}

	fatal(identity_assume_user);

	finally : coda;
}

int hashblock_read(hashblock * const hb)
{
	int fd = 0;

	fatal(identity_assume_fabsys);

	if((fd = open(hb->stathash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->stathash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		if(read(fd, &hb->stathash[0], sizeof(hb->stathash[0])) == -1)
			fail("read failed [%d][%s]", errno, strerror(errno));
		close(fd);
		fd = 0;
	}

	if((fd = open(hb->contenthash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->contenthash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		if(read(fd, &hb->contenthash[0], sizeof(hb->contenthash[0])) == -1)
			fail("read failed [%d][%s]", errno, strerror(errno));
		close(fd);
		fd = 0;
	}

	if((fd = open(hb->vrshash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->vrshash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		if(read(fd, &hb->vrshash[0], sizeof(hb->vrshash[0])) == -1)
			fail("read failed [%d][%s]", errno, strerror(errno));
		close(fd);
		fd = 0;
	}

	fatal(identity_assume_user);

finally:
	if(fd)
		close(fd);
coda;
}

int hashblock_write(const hashblock * const hb)
{
	int fd = 0;

	fatal(identity_assume_fabsys);

	// ensure directory exists for this hb
	fatal(mkdirp, hb->hashdir, S_IRWXU | S_IRWXG | S_IRWXO);

	if(hb->stathash[1])
	{
		if((fd = open(hb->stathash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			fail("open(%s) failed [%d][%s", hb->stathash_path, errno, strerror(errno));
		if(write(fd, &hb->stathash[1], sizeof(hb->stathash[0])) == -1)
			fail("write failed [%d][%s]", errno, strerror(errno));
		close(fd);
	}

	if(hb->contenthash[1])
	{
		if((fd = open(hb->contenthash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			fail("open(%s) failed [%d][%s", hb->contenthash_path, errno, strerror(errno));
		if(write(fd, &hb->contenthash[1], sizeof(hb->contenthash[0])) == -1)
			fail("write failed [%d][%s]", errno, strerror(errno));
		close(fd);
	}

	if((fd = open(hb->vrshash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		fail("open(%s) failed [%d][%s", hb->vrshash_path, errno, strerror(errno));
	if(write(fd, &hb->vrshash[1], sizeof(hb->vrshash[0])) == -1)
		fail("write failed [%d][%s]", errno, strerror(errno));
	close(fd);

	fatal(identity_assume_user);

	finally : coda;
}

int hashblock_cmp(const hashblock * const hb)
{
	return	   hb->vrshash[1] - hb->vrshash[0]					? HB_VERSION
					 : hb->stathash[1] - hb->stathash[0]				? HB_STAT
					 : hb->contenthash[1] - hb->contenthash[0]	? HB_CONTENT
																											: HB_SAME;
}

void hashblock_free(hashblock * const hb)
{
	if(hb)
	{
		free(hb->hashdir);
		free(hb->stathash_path);
		free(hb->contenthash_path);
		free(hb->vrshash_path);
	}
	free(hb);
}

void hashblock_xfree(hashblock ** const hb)
{
	hashblock_free(*hb);
	*hb = 0;
}

