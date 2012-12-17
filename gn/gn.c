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
	// skip for non-file-backed nodes
	if(strcmp(n->dir, "/.."))
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
}

static int gn_create(const char * const restrict realwd, char * const restrict A, int Al, gn ** gna, int * const restrict new)
{
	// p will point to a null-terminated string of the full canonical path to the file
	char * p = 0;
	int pl = 0;

	// n will point to a length-limited string of the name of the file
	char * n = 0;
	int nl = 0;

	// d will point to a length-limited string of the name of the file
	char * d = 0;
	int dl = 0;

	char * space = 0;

	if(Al)
	{
		if(A[0] == '/')
		{
			space = alloca(Al + 1);
			sprintf(space, "%.*s", Al, A);
		}
		else
		{
			int realwdl = strlen(realwd);
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
			int realwdl = strlen(realwd);
			space = alloca(realwdl + 1 + Al + 1);
			sprintf(space, "%s/%.*s", realwd, Al, A);

			p = space;
		}
	}

	pl = strlen(p);
	n = p + pl - 1;
	while(n[0] != '/')
		n--;

	n++;
	nl = strlen(n);

	d = p;
	dl = pl - nl - 1;
	while(dl && d[dl] == '/')
		dl--;

	dl++;

	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	if((*gna = gn_lookup_canon(p, pl)) == 0)
	{
		fatal(coll_doubly_add, &gn_nodes.c, 0, gna);

		map_set(gn_nodes.by_path, p, pl, gna, sizeof(*gna));

		// populate gna
		(*gna)->vrs[1]		= GN_VERSION;
		(*gna)->path			= strdup(p);
		(*gna)->pathl			= pl;
		(*gna)->name			= strdup(n);
		(*gna)->namel			= nl;
		(*gna)->dir				= strndup(d, dl);
		(*gna)->dirl			= dl;
		(*gna)->needs.z		= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z		= sizeof((*gna)->feeds.e[0]);
		char * xt = n + nl;
		while(*xt != '.' && xt != n)
			xt--;
		if(*xt == '.')
		{
			xt++;
			(*gna)->ext = strdup(xt);
			(*gna)->extl = strlen(xt);
		}

		gn_stat(*gna);
		if(new)
			(*new)++;
	}

	return 1;
}

static int raise_cycle(gn ** stack, size_t stack_elsize, int ptr)
{
	log_start(L_ERROR, "detected cycle : ");

	int top;
	for(top = 1; top < ptr; top++)
	{
		if(stack[top] == stack[0])
			break;
	}

	if(top != ptr)
		top++;

	int x;
	for(x = 0; x < top; x++)
	{
		if(x)
			log_add(" -> ");

		log_add("%s", gn_idstring(stack[x]));
	}

	if(ptr == stack_elsize)
		log_finish(" -> ...");
	else
		log_finish("");

	return 0;
}

//
// public
//

int gn_lookup(char * s, char * cwd, gn ** r)
{
	char * sp = 0;

	*r = 0;
	if(gn_nodes.by_path)
	{
		// absolute path
		if(s[0] == '/')
			(*r) = gn_lookup_canon(s, 0);

		// relative to cwd
		if(!(*r))
		{
			fatal(xrealloc, &sp, 1, strlen(cwd) + strlen(s) + 2, 0);
			sprintf(sp, "%s/%s", cwd, s);
			(*r) = gn_lookup_canon(sp, 0);
		}

		// NOFILE nodes
		if(!(*r))
		{
			fatal(xrealloc, &sp, 1, strlen(s) + 5, 0);
			sprintf(sp, "/../%s", s);
			(*r) = gn_lookup_canon(sp, 0);
		}
	}

	if((*r) == 0)
		fail("not found : %s", s);

	free(sp);
	return 1;
}

gn * gn_lookup_canon(char* s, int l)
{
	gn ** r = map_get(gn_nodes.by_path, s, l ?: strlen(s));

	if(r)
		return *r;

	return 0;
}

int gn_add(char * const restrict realwd, void * const restrict A, int Al, gn ** r, int * const restrict newa)
{
	gn *	gna = 0;

	fatal(gn_create, realwd, A, Al, &gna, newa);

	if(r)
		*r = gna;

	return 1;
}

int gn_edge_add(
	  char * const restrict realwd
	, void ** const restrict A, int Al, int At
	, void ** const restrict B, int Bl, int Bt
	, struct ff_node * const restrict ffn
	, int * const restrict newa
	, int * const restrict newb
	, int * const restrict newr
)
{
	gn *	gna = 0;
	gn *	gnb = 0;

	if(At)
		gna = *(gn**)A;
	else
		fatal(gn_create, realwd, *(char**)A, Al, &gna, newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_create, realwd, *(char**)B, Bl, &gnb, newb);

	// as syntactic sugar, silently ignore dependencies of a node on itself
	if(gna != gnb)
	{
		// error check
		if(strcmp(gnb->dir, "/..") == 0)
		{
			if(strcmp(gna->dir, "/.."))
			{
				fail("file-backed node may not depend on non-file-backed node");
			}
		}

		relation * rel = 0;
		if(!gna->needs.by_B)
			fatal(map_create, &gna->needs.by_B, 0);
		
		if(!gnb->feeds.by_A)
			fatal(map_create, &gnb->feeds.by_A, 0);

		if((rel = map_get(gna->needs.by_B, &gnb, sizeof(gnb))) == 0)
		{
			fatal(xmalloc, &rel, sizeof(*rel));

			fatal(coll_doubly_add, &gna->needs.c, &rel, 0);
			fatal(coll_doubly_add, &gnb->feeds.c, &rel, 0);

			fatal(map_set, gna->needs.by_B, &gnb, sizeof(gnb), &rel, sizeof(rel));
			fatal(map_set, gnb->feeds.by_A, &gna, sizeof(gna), &rel, sizeof(rel));

			rel->A = gna;
			rel->B = gnb;
			rel->ffn = ffn;
			rel->weak = ffn->flags & FFN_WEAK;

			if(newr)
				(*newr)++;
		}
	}

	*A = gna;
	*B = gnb;

	return 1;
}

void gn_dump(gn * gn)
{
	int x;
	char space[128];

	gn->flags = 0;
	if(strcmp(gn->dir, "/..") == 0)
		gn->flags |= GN_FLAGS_NOFILE;
	if(gn->needs.l)
		gn->flags |= GN_FLAGS_HASNEED;
	if(gn->fabv)
		gn->flags |= GN_FLAGS_CANFAB;

	if(log_would(L_DG | L_DGRAPH))
	{
		log(L_DG | L_DGRAPH, "%8s : %s", "path", gn->path);
		log(L_DG | L_DGRAPH, "%8s : %s", "name", gn->name);
		log(L_DG | L_DGRAPH, "%8s : %s", "dir", gn->dir);
		log(L_DG | L_DGRAPH, "%8s : %s", "ext", gn->ext);

		if(gn->fabv)
		{
			if(strcmp(gn->dir, "/..") == 0)
				log(L_DG | L_DGRAPH, "%12s : %s", "designation", "TASK");
			else if(gn->needs.l)
				log(L_DG | L_DGRAPH, "%12s : %s", "designation", "SECONDARY");
			else
				log(L_DG | L_DGRAPH, "%12s : %s", "designation", "GENERATED");
		}
		else if(strcmp(gn->dir, "/..") == 0)
		{
			log(L_DG | L_DGRAPH, "%12s : %s", "designation", "NOFILE");
		}
		else if(gn->needs.l)
		{
			log(L_WARN | L_DG | L_DGRAPH, "%12s : %s", "designation", "SECONDARY (no formula)");
		}
		else
		{
			log(L_DG | L_DGRAPH, "%12s : %s", "designation", "PRIMARY");
		}

		if(!(gn->flags & GN_FLAGS_NOFILE))
		{
			log(L_DG | L_DGRAPH, "%12s : %d", "size", (int)gn->size);
			if(gn->mtime)
			{
				struct tm ltm;
				localtime_r(&gn->mtime, &ltm);
				strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-abs", space);
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-del", durationstring(time(0) - gn->mtime));
			}
			else
			{
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime", "");
			}
		}

		if(gn->flags & GN_FLAGS_CANFAB)
		{
			if(gn->fabv)
			{
				log(L_DG | L_DGRAPH, "%12s : [%3d,%3d - %3d,%3d]", "formula"
					, gn->fabv->fml->ffn->loc.f_lin + 1
					, gn->fabv->fml->ffn->loc.f_col + 1
					, gn->fabv->fml->ffn->loc.l_lin + 1
					, gn->fabv->fml->ffn->loc.l_col + 1
				);

				if(gn->fabv->products_l > 1)
				{
					int x;
					for(x = 0; x < gn->fabv->products_l; x++)
						log(L_DG | L_DGRAPH, "%12s --> %s", "", gn->fabv->products[x]->path);
				}
			}
			else
			{
				log(L_WARN | L_DG | L_DGRAPH, "%12s : %s", "formula", "(no formula)");
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "depth", gn->depth);
		log(L_DG | L_DGRAPH, "%12s : %d", "height", gn->height);
		log(L_DG | L_DGRAPH, "%12s : %d", "stage", gn->stage);

		if(gn->flags & GN_FLAGS_HASNEED)
		{
			log(L_DG | L_DGRAPH, "%12s : %d", "needs", gn->needs.l);
			for(x = 0; x < gn->needs.l; x++)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-25s @ (%s)[%3d,%3d - %3d,%3d]"
					, ""
					, gn->needs.e[x]->weak ? "*" : " "
					, gn_idstring(gn->needs.e[x]->B)
					, ff_idstring(gn->needs.e[x]->ffn->loc.ff)
					, gn->needs.e[x]->ffn->loc.f_lin + 1
					, gn->needs.e[x]->ffn->loc.f_col + 1
					, gn->needs.e[x]->ffn->loc.l_lin + 1
					, gn->needs.e[x]->ffn->loc.l_col + 1
				);
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "feeds", gn->feeds.l);
		for(x = 0; x < gn->feeds.l; x++)
		{
			log(L_DG | L_DGRAPH, "%10s %s --> %-25s @ (%s)[%3d,%3d - %3d,%3d]"
				, ""
				, gn->feeds.e[x]->weak ? "*" : " "
				, gn_idstring(gn->feeds.e[x]->A)
				, ff_idstring(gn->feeds.e[x]->ffn->loc.ff)
				, gn->feeds.e[x]->ffn->loc.f_lin + 1
				, gn->feeds.e[x]->ffn->loc.f_col + 1
				, gn->feeds.e[x]->ffn->loc.l_lin + 1
				, gn->feeds.e[x]->ffn->loc.l_col + 1
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

int gn_traverse_needsward(gn * r, void (*logic)(gn*))
{
	gn * stack[64] = {};
	int ptr = 0;

	int enter(gn * n)
	{
		if(n->guard)
		{
			if(ptr < sizeof(stack) / sizeof(stack[0]))
				stack[ptr++] = n;

			return 1;
		}
		n->guard = 1;

		// descend
		int x;
		for(x = 0; x < n->needs.l; x++)
		{
			if(enter(n->needs.e[x]->B))
			{
				if(ptr < sizeof(stack) / sizeof(stack[0]))
					stack[ptr++] = n;

				return 1;
			}
		}

		// logic on this node
		logic(n);

		n->guard = 0;

		return 0;
	};

	if(enter(r))
		return raise_cycle(stack, sizeof(stack) / sizeof(stack[0]), ptr);

	return 1;
}

int gn_traverse_relations_needsward(gn * r, void (*logic)(relation*))
{
	gn * stack[64] = {};
	int ptr = 0;

	int enter(void * N, int root)
	{
		gn * n = 0;
		if(root)
			n = (gn*)N;
		else
			n = ((relation*)N)->B;

		if(n->guard)
		{
			if(ptr < sizeof(stack) / sizeof(stack[0]))
				stack[ptr++] = n;

			return 1;
		}
		n->guard = 1;

		// descend
		int x;
		for(x = 0; x < n->needs.l; x++)
		{
			if(enter(n->needs.e[x], 0))
			{
				if(ptr < sizeof(stack) / sizeof(stack[0]))
					stack[ptr++] = n;

				return 1;
			}
		}

		// logic on this node
		logic(N);

		n->guard = 0;

		return 0;
	};

	if(enter(r, 1))
		return raise_cycle(stack, sizeof(stack) / sizeof(stack[0]), ptr);

	return 1;
}

char * gn_idstring(gn * gn)
{
	if(g_args.mode_gnid == MODE_GNID_CANON)
	{
		return gn->path;
	}
	if(g_args.mode_gnid == MODE_GNID_RELATIVE)
	{
		if(gn->idstring == 0)
		{
			int x;
			for(x = 0; x < strlen(g_args.fabfile_canon) && x < gn->pathl; x++)
			{
				if(g_args.fabfile_canon[x] != gn->path[x])
					break;
			}

			gn->idstring = strdup(&gn->path[x]);
		}

		return gn->idstring;
	}

	return 0;
}
