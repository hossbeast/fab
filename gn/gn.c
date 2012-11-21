#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "gn.h"

#include "log.h"
#include "control.h"
#include "fml.h"

#include "args.h"
#include "xmem.h"
#include "unitstring.h"

//
// data
//

union gn_nodes_t		gn_nodes = { { .size = sizeof(gn) } };

//
// static
//

static uint32_t hash(const char* s, size_t l)
{
	uint32_t h = 0;

	int x;
	for(x = 0; x < l; x++)
	{
		h += s[x];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h;
}

static int hashfile_mkpath(gn * gn)
{
	if(!gn->hashfile_path)
	{
		uint32_t pathhash = hash(gn->path, strlen(gn->path));

		int l = snprintf(0, 0, "%s/%u", g_args.hashdir, pathhash);
		gn->hashfile_path = malloc(l + 1);
		snprintf(gn->hashfile_path, l + 1, "%s/%u", g_args.hashdir, pathhash);
		snprintf(gn->hashfile_path, l + 1, "%s/%u", g_args.hashdir, pathhash);
	}

	return 1;
}

static void gn_stat(gn * n)
{
	struct stat stb;

	// STAT for A
	if(stat(n->path, &stb) == 0)
	{
		n->dev			= stb.st_dev;
		n->ino			= stb.st_ino;
		n->mode			= stb.st_mode;
		n->nlink		= stb.st_nlink;
		n->uid			= stb.st_uid;
		n->gid			= stb.st_gid;
		n->size			= stb.st_size;
		n->mtime		= stb.st_mtime;
		n->ctime		= stb.st_ctime;
	}

	// compute hashes
	n->prop_hash[1] = hash(
			(char*)&n->dev
		, (char*)&n->ctime - (char*)&n->dev
	);
}

static int gn_create(char * realwd, char * A, int Al, gn ** gna, int * new)
{
	char* space = 0;

	// p will point to a null-terminated string of the full canonical path to the file
	// A will point to a null-terminated string of the name of the file
	char * p = 0;
	int pl = 0;
	int realwdl = strlen(realwd);

	if(Al)
	{
		if(A[0] == '/')
		{
			space = alloca(Al + 1);
			sprintf(space, "%.*s", Al, A);
		}
		else
		{
			space = alloca(realwdl + 1 + Al + 1);
			sprintf(space, "%s/%.*s", realwd, Al, A);
		}

		p = space;
	}
	else
	{
		if((Al = strlen(A)) == 0)
			fail("zero-length name");
		
		if(A[0] == '/')
		{
			p = A;
		}
		else
		{
			space = alloca(realwdl + 1 + Al + 1);
			sprintf(space, "%s/%.*s", realwd, Al, A);

			p = space;
		}
	}

	pl = strlen(p);
	A = p + pl - 1;
	while(A != p && A[0] != '/')
		A--;

	A++;
	Al = strlen(A);

	if(!gn_nodes.by_path || (*gna = idx_lookup_val(gn_nodes.by_path, (char*[]) { p }, 0)) == 0)
	{
		// allocate new gnode in collection for A
		fatal(coll_doubly_add, &gn_nodes.c, 0, gna);

		// populate gna
		(*gna)->vrs[1]		= GN_VERSION;
		(*gna)->path			= strdup(p);
		(*gna)->pathl			= pl;
		(*gna)->name			= strdup(A);
		(*gna)->namel			= Al;
		(*gna)->dir				= strdup(realwd);
		(*gna)->dirl			= realwdl;
		(*gna)->needs.z		= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z		= sizeof((*gna)->feeds.e[0]);
		char * xt = A + Al;
		while(*xt != '.' && xt != A)
			xt--;
		if(*xt == '.')
		{
			xt++;
			(*gna)->ext = strdup(xt);
			(*gna)->extl = strlen(xt);
		}

		gn_stat(*gna);
		(*new)++;
	}

	return 1;
}

//
// public
//

int gn_add(char * const restrict realwd, void * const restrict A, int Al, gn ** r)
{
	gn *	gna = 0;
	int		new = 0;

	fatal(gn_create, realwd, A, Al, &gna, &new);

	if(new)
	{
		fatal(idx_mkindex
			, gn_nodes.e
			, gn_nodes.l
			, gn_nodes.z
			, offsetof(typeof(gn_nodes.e[0][0]), path)
			, 0
			, INDEX_UNIQUE | INDEX_STRING | INDEX_DEREF
			, &gn_nodes.by_path
		);

		fatal(idx_mkindex
			, gn_nodes.e
			, gn_nodes.l
			, gn_nodes.z
			, offsetof(typeof(gn_nodes.e[0][0]), dir)
			, 0
			, INDEX_MULTI | INDEX_STRING | INDEX_DEREF
			, &gn_nodes.by_dir
		);
	}

	if(r)
		*r = gna;

	return 1;
}

int gn_edge_add(char * const restrict realwd, void ** const restrict A, int Al, int At, void ** const restrict B, int Bl, int Bt, struct ff_node * ffn)
{
	gn *	gna = 0;
	gn *	gnb = 0;
	int		new = 0;

	if(At)
		gna = *(gn**)A;
	else
		fatal(gn_create, realwd, *(char**)A, Al, &gna, &new);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_create, realwd, *(char**)B, Bl, &gnb, &new);

	// reindex the collections
	if(new)
	{
		fatal(idx_mkindex
			, gn_nodes.e
			, gn_nodes.l
			, gn_nodes.z
			, offsetof(typeof(gn_nodes.e[0][0]), path)
			, 0
			, INDEX_UNIQUE | INDEX_STRING | INDEX_DEREF
			, &gn_nodes.by_path
		);

		fatal(idx_mkindex
			, gn_nodes.e
			, gn_nodes.l
			, gn_nodes.z
			, offsetof(typeof(gn_nodes.e[0][0]), dir)
			, 0
			, INDEX_MULTI | INDEX_STRING | INDEX_DEREF
			, &gn_nodes.by_dir
		);
	}

	// add dependency
	relation * ra = 0;
	relation * rb = 0;
	fatal(coll_singly_add, &gna->needs.c, 0, &ra);
	fatal(coll_singly_add, &gnb->feeds.c, 0, &rb);

	ra->gn = gnb;
	ra->ffn = ffn;
	rb->gn = gna;
	rb->ffn = ffn;

/*
	coll_singly_add(&gna->needs.c, &gnb, 0);
	coll_singly_add(&gnb->feeds.c, &gna, 0);
*/

	*A = gna;
	*B = gnb;

	return 1;
}

void gn_dump(gn * n)
{
	char space[128];

	if(log_would(L_DG | L_DGRAPH))
	{
		log(L_DG | L_DGRAPH, "%8s : %s", "path", n->path);
		if(n->fmlv)
		{
			if(n->needs.l)
				log(L_DG | L_DGRAPH, "%12s : %s", "designation", "SECONDARY");
			else
				log(L_DG | L_DGRAPH, "%12s : %s", "designation", "GENERATED");
		}
		else if(n->needs.l)
		{
			log(L_WARN | L_DG | L_DGRAPH, "%12s : %s", "designation", "SECONDARY (no formula)");
		}
		else
		{
			log(L_DG | L_DGRAPH, "%12s : %s", "designation", "PRIMARY");
		}
		log(L_DG | L_DGRAPH, "%12s : %d", "size", (int)n->size);
		if(n->mtime)
		{
			struct tm ltm;
			localtime_r(&n->mtime, &ltm);
			strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

			log(L_DG | L_DGRAPH, "%12s : %s", "mtime-abs", space);
			log(L_DG | L_DGRAPH, "%12s : %s", "mtime-del", durationstring(time(0) - n->mtime));
		}
		else
		{
			log(L_DG | L_DGRAPH, "%12s : %s", "mtime", "");
		}

		if(n->fmlv)
		{
			log(L_DG | L_DGRAPH, "%12s : [%3d,%3d - %3d,%3d]", "formula"
				, n->fmlv->fml->ffn->loc.f_lin + 1
				, n->fmlv->fml->ffn->loc.f_col + 1
				, n->fmlv->fml->ffn->loc.l_lin + 1
				, n->fmlv->fml->ffn->loc.l_col + 1
			);

			if(n->fmlv->products_l > 1)
			{
				int x;
				for(x = 0; x < n->fmlv->products_l; x++)
					log(L_DG | L_DGRAPH, "%12s --> %s", "", n->fmlv->products[x]->path);
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "needs", n->needs.l);
		int x;
		for(x = 0; x < n->needs.l; x++)
		{
			log(L_DG | L_DGRAPH, "%12s --> %s @ [%3d,%3d - %3d,%3d]", ""
				, n->needs.e[x].gn->path
				, n->needs.e[x].ffn->loc.f_lin + 1
				, n->needs.e[x].ffn->loc.f_col + 1
				, n->needs.e[x].ffn->loc.l_lin + 1
				, n->needs.e[x].ffn->loc.l_col + 1
			);
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "feeds", n->feeds.l);
		for(x = 0; x < n->feeds.l; x++)
		{
			log(L_DG | L_DGRAPH, "%12s --> %s @ [%3d,%3d - %3d,%3d]", ""
				, n->feeds.e[x].gn->path
				, n->feeds.e[x].ffn->loc.f_lin + 1
				, n->feeds.e[x].ffn->loc.f_col + 1
				, n->feeds.e[x].ffn->loc.l_lin + 1
				, n->feeds.e[x].ffn->loc.l_col + 1
			);
		}

		log(L_DG | L_DGRAPH, "");
	}
}

void gn_hashcmd(gn * gn, char * s, int l)
{
	gn->cmd_hash[1] = hash(s, l);
}

int gn_hashes_read(gn * gn)
{
	fatal(hashfile_mkpath, gn);

	int fd;
	if((fd = open(gn->hashfile_path, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO)) != -1)
	{
		read(fd, &gn->vrs[0]				, sizeof(gn->vrs[0]));
		read(fd, &gn->prop_hash[0]	, sizeof(gn->prop_hash[0]));
		read(fd, &gn->cmd_hash[0]		, sizeof(gn->cmd_hash[0]));

		log(L_DGHASH, "%s <- %s = 0x%08x%08x%08x", gn->path, gn->hashfile_path, gn->vrs[0], gn->prop_hash[0], gn->cmd_hash[0]);

		close(fd);
	}
	else
	{
		log(L_DGHASH, "%s <- %s = 0x%08x%08x%08x", gn->path, "(ENOENT)", gn->vrs[0], gn->prop_hash[0], gn->cmd_hash[0]);
	}

	return 1;
}

int gn_hashes_write(gn * gn)
{
	fatal(hashfile_mkpath, gn);

	unlink(gn->hashfile_path);

	int fd;
	if((fd = open(gn->hashfile_path, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
		fail("open(%s) failed [%d][%s", gn->hashfile_path, errno, strerror(errno));

	log(L_DGHASH, "%s -> %s = 0x%08x%08x%08x", gn->path, gn->hashfile_path, gn->vrs[1], gn->prop_hash[1], gn->cmd_hash[1]);

	write(fd, &gn->vrs[1]					, sizeof(gn->vrs[0]));
	write(fd, &gn->prop_hash[1]		, sizeof(gn->prop_hash[0]));
	write(fd, &gn->cmd_hash[1]		, sizeof(gn->cmd_hash[0]));

	close(fd);
	return 1;
}

int gn_hashes_cmp(gn * gn)
{
	return     gn->vrs[1] - gn->vrs[0]
					?: gn->prop_hash[1] - gn->prop_hash[0]
					?: gn->cmd_hash[1] - gn->cmd_hash[0]
					?: 0;
}
