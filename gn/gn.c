#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "gn.h"

#include "fml.h"
#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "unitstring.h"
#include "cksum.h"

#define restrict __restrict

char * ff_idstring(struct ff_file * const);

//
// data
//

union gn_nodes_t		gn_nodes = { { .size = sizeof(gn) } };

//
// static
//

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
		(*gna)->path					= strdup(p);
		(*gna)->pathl					= pl;
		(*gna)->pathhash			= cksum((*gna)->path, strlen((*gna)->path));
		(*gna)->name					= strdup(n);
		(*gna)->namel					= nl;
		(*gna)->dir						= strndup(d, dl);
		(*gna)->dirl					= dl;
		(*gna)->needs.z				= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z				= sizeof((*gna)->feeds.e[0]);

		char * xt = n;
		while(xt != n + nl && xt[0] != '.')
			xt++;

		if(*xt == '.')
		{
			xt++;
			(*gna)->ext = strdup(xt);
		}
		else
		{
			(*gna)->ext = strdup("(none)");
		}
		(*gna)->extl = strlen(xt);

		if(new)
			(*new)++;
	}

	finally : coda;
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

gn * gn_lookup(char * s, int l, char * cwd)
{
	char * sp = 0;
	l = l ?: strlen(s);

	gn * r = 0;

	if(gn_nodes.by_path)
	{
		// absolute path
		if(s[0] == '/')
			r = gn_lookup_canon(s, l);

		// relative to cwd
		if(!r)
		{
			sp = alloca(strlen(cwd) + l + 2);
			sprintf(sp, "%s/%.*s", cwd, l, s);
			r = gn_lookup_canon(sp, 0);
		}

		// NOFILE nodes
		if(!r)
		{
			sp = alloca(l + 5);
			sprintf(sp, "/../%.*s", l, s);
			r = gn_lookup_canon(sp, 0);
		}
	}

	return r;
}

gn * gn_lookup_canon(char* s, int l)
{
	gn ** r = map_get(gn_nodes.by_path, s, l ?: strlen(s));

	if(r)
		return *r;

	return 0;
}

int gn_add(char * const realwd, void * const A, int Al, gn ** r, int * const newa)
{
	gn *	gna = 0;

	fatal(gn_create, realwd, A, Al, &gna, newa);

	if(r)
		*r = gna;

	finally : coda;
}

int gn_edge_add(
	  char * const realwd
	, void ** const A, int Al, int At
	, void ** const B, int Bl, int Bt
	, struct ff_node * const reg_ffn
	, gn * const dscv_gn
	, int isweak
	, int * const newa
	, int * const newb
	, int * const newr
)
{
	gn *	gna = 0;
	gn *	gnb = 0;

	int _newa = 0;
	int _newb = 0;

	if(At)
		gna = *(gn**)A;
	else
		fatal(gn_create, realwd, *(char**)A, Al, &gna, &_newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_create, realwd, *(char**)B, Bl, &gnb, &_newb);

	// as syntactic sugar, silently ignore dependencies of a node on itself
	if(gna != gnb)
	{
		if(newa)
			(*newa) += _newa;
		if(newb)
			(*newb) += _newb;

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
			rel->weak = isweak;

			if(reg_ffn)
			{
				rel->type = GN_RELATION_REGULAR;
				rel->ffn = reg_ffn;
			}
			if(dscv_gn)
			{
				rel->type = GN_RELATION_CACHED;
				rel->dscv_gn = dscv_gn;
			}

			if(newr)
				(*newr)++;
		}
	}

	*A = gna;
	*B = gnb;

	finally : coda;
}

void gn_dump(gn * gn)
{
	int x;
	char space[128];

	if(log_would(L_DG | L_DGRAPH))
	{
		log(L_DG | L_DGRAPH, "%8s : %s", "path", gn->path);
		log(L_DG | L_DGRAPH, "%8s : %u", "pathhash", gn->pathhash);
		log(L_DG | L_DGRAPH, "%8s : %s", "dir", gn->dir);
		log(L_DG | L_DGRAPH, "%8s : %s", "name", gn->name);
		log(L_DG | L_DGRAPH, "%8s : %s", "ext", gn->ext);
		log(L_DG | L_DGRAPH, "%12s : %s", "designation", gn_designate(gn));

		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			log(L_DG | L_DGRAPH, "%12s : %d", "size", (int)gn->hb->size);
			if(gn->hb->mtime)
			{
				struct tm ltm;
				localtime_r(&gn->hb->mtime, &ltm);
				strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-abs", space);
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-del", durationstring(time(0) - gn->hb->mtime));
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
				log(L_DG | L_DGRAPH, "%12s : (%s)[%3d,%3d - %3d,%3d]", "formula"
					, ff_idstring(gn->fabv->fml->ffn->loc.ff)
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

		log(L_DG | L_DGRAPH, "%12s : %d", "height", gn->height);
		log(L_DG | L_DGRAPH, "%12s : %d", "stage", gn->stage);

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

int gn_depth_traversal_nodes_needsward(gn * r, int (*logic)(gn*, int))
{
	gn * stack[64] = {};
	int ptr = 0;

	// RETURNS
	//  0 - success
	//  1 - cycle detected
	// -1 - callback failure
	//
	int enter(gn * n, int d)
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
			int e = enter(n->needs.e[x]->B, d + 1);
			if(e == 1)
			{
				if(ptr < sizeof(stack) / sizeof(stack[0]))
					stack[ptr++] = n;

				return 1;
			}
			if(e == -1)
				return -1;
		}

		// logic on this node
		if(logic(n, d) == 0)
			return -1;

		n->guard = 0;

		return 0;
	};

	int e = enter(r, 0);
	if(e == 1)
		return raise_cycle(stack, sizeof(stack) / sizeof(stack[0]), ptr);
	if(e == -1)
		return 0;

	return 1;
}

char * gn_idstring(gn * const gn)
{
	if(g_args.mode_gnid == MODE_GNID_RELATIVE)
	{
		if(gn->idstring == 0)
		{
			if(strcmp(gn->dir, "/..") == 0)
			{
				gn->idstring = strdup(gn->path);
			}
			else
			{
				int x;
				for(x = 0; x < strlen(g_args.fabfile_canon) && x < gn->pathl; x++)
				{
					if(g_args.fabfile_canon[x] != gn->path[x])
						break;
				}

				gn->idstring = strdup(&gn->path[x]);
			}
		}

		return gn->idstring;
	}
	if(g_args.mode_gnid == MODE_GNID_CANON)
	{
		return gn->path;
	}

	return 0;
}

char* gn_designate(gn * gn)
{
	gn->flags = 0;
	if(strcmp(gn->dir, "/..") == 0)
		gn->flags |= GN_FLAGS_NOFILE;
	if(gn->needs.l)
		gn->flags |= GN_FLAGS_HASNEED;
	if(gn->fabv)
		gn->flags |= GN_FLAGS_CANFAB;
	
	if(gn->flags & GN_FLAGS_CANFAB)
	{
		if(gn->flags & GN_FLAGS_NOFILE)
			gn->designation = GN_DESIGNATION_TASK;
		else if(gn->flags & GN_FLAGS_HASNEED)
			gn->designation = GN_DESIGNATION_SECONDARY;
		else
			gn->designation = GN_DESIGNATION_GENERATED;
	}
	else if(gn->flags & GN_FLAGS_NOFILE)
		gn->designation = GN_DESIGNATION_NOFILE;
	else if(gn->flags & GN_FLAGS_HASNEED)
	{
		// but there's no fmlv, which is a warning
		gn->designation = GN_DESIGNATION_SECONDARY;
	}
	else
		gn->designation = GN_DESIGNATION_PRIMARY;

	return GN_DESIGNATION_STR(gn->designation);
}

int gn_secondary_exists(gn * const gn)
{
	if(euidaccess(gn->path, F_OK) == 0)
	{
		gn->exists = 1;
	}
	else if(errno != ENOENT)
	{
		fail("access(%s)=[%d][%s]", gn->path, errno, strerror(errno));
	}

	finally : coda;
}

int gn_primary_reload_dscv(gn * const gn)
{
	// ensure we've reloaded hashes for the backing file
	fatal(gn_primary_reload, gn);

	// create ddisc block
	fatal(depblock_create, &gn->dscv_block, "%s/PRIMARY/%u", GN_DIR_BASE, gn->pathhash);

	// backing file has not changed
	if(hashblock_cmp(gn->hb) == 0)
	{
		// node has not been invalidated
		if(gn->changed == 0)
		{
			// primary node with associated discovery fml eval
			if(gn->dscv)
			{
				// fabfile containing the fml eval has not changed
				if(hashblock_cmp(gn->dscv->fml->ffn->loc.ff->hb) == 0)
				{
					// actually load the depblock from cache
					fatal(depblock_read, gn->dscv_block);
				}
			}
		}
	}

	finally : coda;
}

int gn_primary_reload(gn * const gn)
{
	if(gn->hb_loaded == 0)
	{
		// create hashblock
		fatal(hashblock_create, &gn->hb, "%s/PRIMARY/%u", GN_DIR_BASE, gn->pathhash);

		// load the previous hashblock
		fatal(hashblock_read, gn->hb);

		log(L_HASHBLK
			, "%s <== 0x%08x%08x%08x"
			, gn_idstring(gn)
			, gn->hb->stathash[0]
			, gn->hb->contenthash[0]
			, gn->hb->vrshash[0]
		);

		// stat the file, compute new stathash
		fatal(hashblock_stat, gn->hb, gn->path);

		gn->hb->vrshash[1] = FAB_VERSION;
		gn->hb_loaded = 1;
	}

	finally : coda;
}

int gn_primary_rewrite(gn * const gn)
{
	// rewrite dependency discovery block
	if(gn->dscv_block)
		fatal(depblock_write, gn->dscv_block);

	// rewrite the hashblock
	fatal(hashblock_write, gn->hb);

	log(L_HASHBLK
		, "%s ==> 0x%08x%08x%08x"
		, gn_idstring(gn)
		, gn->hb->stathash[1]
		, gn->hb->contenthash[1]
		, gn->hb->vrshash[1]
	);

	finally : coda;
}

void gn_invalidations()
{
	int x;

	// update all node designations
	for(x = 0; x < gn_nodes.l; x++)
		gn_designate(gn_nodes.e[x]);

	if(g_args.invalidate_all)
	{
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(gn_nodes.e[x]->designation == GN_DESIGNATION_PRIMARY)
				gn_nodes.e[x]->changed = 1;

			if(gn_nodes.e[x]->designation == GN_DESIGNATION_SECONDARY)
				gn_nodes.e[x]->rebuild = 1;
		}
	}
	else
	{
		for(x = 0; x < g_args.invalidate_len; x++)
		{
			gn * gn = 0;
			if((gn = gn_lookup(g_args.invalidate[x], 0, g_args.cwd)))
			{
				if(gn->designation == GN_DESIGNATION_PRIMARY)
					gn->changed = 1;

				if(gn->designation == GN_DESIGNATION_SECONDARY)
					gn->rebuild = 1;
			}
		}
	}
}

static void gn_freenode(gn * const gn)
{
	int x;

	if(gn)
	{
		free(gn->dir);
		free(gn->name);
		free(gn->path);
		free(gn->ext);
		free(gn->idstring);

		hashblock_free(gn->hb);
		depblock_free(gn->dscv_block);

		for(x = 0; x < gn->needs.l; x++)
			free(gn->needs.e[x]);
		free(gn->needs.e);
		map_free(gn->needs.by_B);

		free(gn->feeds.e);
		map_free(gn->feeds.by_A);
	}

	free(gn);
}

void gn_teardown()
{
	int x;
	for(x = 0; x < gn_nodes.l; x++)
		gn_freenode(gn_nodes.e[x]);

	free(gn_nodes.e);
	map_free(gn_nodes.by_path);
}
