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

	finally : coda;
}

int hashblock_stat(hashblock * const hb, const char * const path)
{
	struct stat stb;

	// STAT for A
	if(stat(path, &stb) == 0)
	{
		hb->dev			= stb.st_dev;
		hb->ino			= stb.st_ino;
		hb->mode		= stb.st_mode;
		hb->nlink		= stb.st_nlink;
		hb->uid			= stb.st_uid;
		hb->gid			= stb.st_gid;
		hb->size		= stb.st_size;
		hb->mtime		= stb.st_mtime;
		hb->ctime		= stb.st_ctime;

		hb->stathash[1] = cksum(
				(char*)&hb->dev
			, (char*)&hb->ctime - (char*)&hb->dev
		);
	}
	else if(errno != ENOENT)
	{
		fail("stat(%s)=[%d][%s]", path, errno, strerror(errno));
	}
	else
	{
		hb->stathash[1] = 0;
	}

	finally : coda;
}

int hashblock_read(hashblock * const hb)
{
	int fd;
	if((fd = open(hb->stathash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->stathash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		read(fd, &hb->stathash[0], sizeof(hb->stathash[0]));
		close(fd);
	}

	if((fd = open(hb->contenthash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->contenthash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		read(fd, &hb->contenthash[0], sizeof(hb->contenthash[0]));
		close(fd);
	}

	if((fd = open(hb->vrshash_path, O_RDONLY)) == -1 && errno != ENOENT)
	{
		fail("open(%s)=[%d][%s]", hb->vrshash_path, errno, strerror(errno));
	}
	else if(fd > 0)
	{
		read(fd, &hb->vrshash[0], sizeof(hb->vrshash[0]));
		close(fd);
	}

	finally : coda;
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
		write(fd, &hb->stathash[1], sizeof(hb->stathash[0]));
		close(fd);
	}

	if(hb->contenthash[1])
	{
		if((fd = open(hb->contenthash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			fail("open(%s) failed [%d][%s", hb->contenthash_path, errno, strerror(errno));
		write(fd, &hb->contenthash[1], sizeof(hb->contenthash[0]));
		close(fd);
	}

	if((fd = open(hb->vrshash_path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		fail("open(%s) failed [%d][%s", hb->vrshash_path, errno, strerror(errno));
	write(fd, &hb->vrshash[1], sizeof(hb->vrshash[0]));
	close(fd);

	fatal(identity_assume_user);

	finally : coda;
}

int hashblock_cmp(const hashblock * const hb)
{
	return     hb->vrshash[1] - hb->vrshash[0]
					?: hb->stathash[1] - hb->stathash[0]
					?: hb->contenthash[1] - hb->contenthash[0]
					?: 0;
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
