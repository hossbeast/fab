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

	fatal(ixsprintf, &(*hb)->stathash_path, "%s/stat", (*hb)->hashdir);
	fatal(ixsprintf, &(*hb)->contenthash_path, "%s/content", (*hb)->hashdir);
	fatal(ixsprintf, &(*hb)->vrshash_path, "%s/vrs", (*hb)->hashdir);

	// attach version id
	(*hb)->vrshash[1] = FABVERSIONN;

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

	int x;

	// STAT for A
	fatal(uxstat, path, &stb, 0);
	if(stb.st_dev)
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
	else
	{
		for(x = 0; x < hbsl; x++)
			hbs[x]->stathash[1] = 0;
	}

	finally : coda;
}

int hashblock_read(hashblock * const hb)
{
	int fd = -1;

	fatal(identity_assume_fabsys);

	fatal(gxopen, hb->stathash_path, O_RDONLY, &fd);

	if(fd != -1)
	{
		fatal(xread, fd, &hb->stathash[0], sizeof(hb->stathash[0]), 0);
	}

	fatal(ixclose, &fd);
	fatal(gxopen, hb->contenthash_path, O_RDONLY, &fd);

	if(fd != -1)
	{
		fatal(xread, fd, &hb->contenthash[0], sizeof(hb->contenthash[0]), 0);
	}

	fatal(ixclose, &fd);
	fatal(gxopen, hb->vrshash_path, O_RDONLY, &fd);

	if(fd != -1)
	{
		fatal(xread, fd, &hb->vrshash[0], sizeof(hb->vrshash[0]), 0);
	}

	fatal(identity_assume_user);

finally:
	fatal(ixclose, &fd);
coda;
}

int hashblock_write(const hashblock * const hb)
{
	int fd = -1;

	fatal(identity_assume_fabsys);

	// ensure directory exists for this hb
	fatal(mkdirp, hb->hashdir, S_IRWXU | S_IRWXG | S_IRWXO);

	if(hb->stathash[1])
	{
		fatal(xopen_mode, hb->stathash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);
		fatal(xwrite, fd, &hb->stathash[1], sizeof(hb->stathash[0]), 0);
	}

	if(hb->contenthash[1])
	{
		fatal(ixclose, &fd);
		fatal(xopen_mode, hb->contenthash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);
		fatal(xwrite, fd, &hb->contenthash[1], sizeof(hb->contenthash[0]), 0);
	}

	fatal(ixclose, &fd);
	fatal(xopen_mode, hb->vrshash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &fd);
	fatal(xwrite, fd, &hb->vrshash[1], sizeof(hb->vrshash[0]), 0);

	fatal(identity_assume_user);

finally:
	fatal(ixclose, &fd);
coda;
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

