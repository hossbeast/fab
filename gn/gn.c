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
#include "canon.h"
#include "xstring.h"
#include "dirutil.h"

#define restrict __restrict

char * ff_idstring(struct ff_file * const);

//
// data
//

union gn_nodes_t		gn_nodes = { { .size = sizeof(gn) } };

//
// static
//

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

		log_add("%s", stack[x]->idstring);
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

int gn_lookup_match(const char * const s, gn *** const r, int * const rl, int * const ra)
{
	int x;
	for(x = 0; x < gn_nodes.l; x++)
	{
		if(strstr(gn_nodes.e[x]->path->can, s) || strstr(gn_nodes.e[x]->path->abs, s) || strstr(gn_nodes.e[x]->path->rel, s))
		{
			if((*rl) == (*ra))
			{
				int ns = (*ra) ?: 10;
				ns = ns * 2 + ns / 2;
				fatal(xrealloc, r, sizeof(**r), ns, *ra);
				*ra = ns;
			}
			(*r)[(*rl)++] = gn_nodes.e[x];
		}
	}

	finally : coda;
}

int gn_lookup_nofile(const char * const s, int l, const char * const base, gn ** r)
{
	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	// canonicalize for lookup
	char can[512];
	if(canon(s, l, can, sizeof(can), base, CAN_REALPATH) == 0)
		return 0;

	gn ** R = 0;
	if((R = map_get(gn_nodes.by_path, can, strlen(can))))
		*r = *R;
	else
	{
		if(canon(s, l, can, sizeof(can), "/..", CAN_REALPATH) == 0)
			return 0;

		if((R = map_get(gn_nodes.by_path, can, strlen(can))))
			*r = *R;
	}

	finally : coda;
}

int gn_lookup(const char * const s, int l, const char * const base, gn ** r)
{
	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	// canonicalize for lookup
	char can[512];
	if(canon(s, l, can, sizeof(can), base, CAN_REALPATH) == 0)
		return 0;

	gn ** R = 0;
	if((R = map_get(gn_nodes.by_path, can, strlen(can))))
		*r = *R;

	finally : coda;
}

int gn_lookup_canon(const char * const s, int l, gn ** r)
{
	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	gn ** R = 0;
	if((R = map_get(gn_nodes.by_path, s, l)))
		*r = *R;

	finally : coda;
}

int gn_add(const char * const restrict base, char * const restrict A, int Al, gn ** gna, int * const restrict new)
{
	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	if(base)
		fatal(gn_lookup, A, Al, base, gna);
	else
		fatal(gn_lookup_canon, A, Al, gna);

	if((*gna) == 0)
	{
		fatal(coll_doubly_add, &gn_nodes.c, 0, gna);

		// populate gna
		if(base)
			fatal(path_create, &(*gna)->path, base, "%.*s", Al ?: strlen(A), A);
		else
			fatal(path_create_canon, &(*gna)->path, "%.*s", Al ?: strlen(A), A);

		(*gna)->needs.z		= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z		= sizeof((*gna)->feeds.e[0]);

		// idstring
		if(g_args.mode_gnid == MODE_GNID_RELATIVE)
		{
			(*gna)->idstring = strdup((*gna)->path->rel);
		}
		else if(g_args.mode_gnid == MODE_GNID_CANON)
		{
			(*gna)->idstring = strdup((*gna)->path->can);
		}
		(*gna)->idstringl = strlen((*gna)->idstring);

		map_set(gn_nodes.by_path, (*gna)->path->can, (*gna)->path->canl, gna, sizeof(*gna));

		if(new)
			(*new)++;
	}

	finally : coda;
}

int gn_edge_add(
	  char * const base
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
		fatal(gn_add, base, *(char**)A, Al, &gna, &_newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_add, base, *(char**)B, Bl, &gnb, &_newb);

	if(gna == gnb)
	{
		// as syntactic sugar, silently ignore dependencies of a node on itself
	}
	else
	{
		if(newa)
			(*newa) += _newa;
		if(newb)
			(*newb) += _newb;

		// error check
		if(gnb->path->is_nofile && !gna->path->is_nofile)
			fail("file-backed node %s may not depend on non-file-backed node %s", gna->path->can, gnb->path->can);

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
		// path properties
		log(L_DG | L_DGRAPH, "%8s : %s", "can-path"	, gn->path->can);
		log(L_DG | L_DGRAPH, "%8s : %s", "abs-path"	, gn->path->abs);
		log(L_DG | L_DGRAPH, "%8s : %s", "rel-path"	, gn->path->rel);
		log(L_DG | L_DGRAPH, "%8s : %s", "path-in"	, gn->path->in);
		log(L_DG | L_DGRAPH, "%8s : %s", "path-base", gn->path->base);
		log(L_DG | L_DGRAPH, "%8s : %u", "canhash"	, gn->path->can_hash);
		log(L_DG | L_DGRAPH, "%8s : %s", "name"			, gn->path->name);
		log(L_DG | L_DGRAPH, "%8s : %s", "ext"			, gn->path->ext);
		log(L_DG | L_DGRAPH, "%8s : %s", "ext_last"	, gn->path->ext_last);

		log(L_DG | L_DGRAPH, "%12s : %s", "designation", gn_designate(gn));

		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			log(L_DG | L_DGRAPH, "%12s : %d", "size", (int)gn->hb_fab->size);
			if(gn->hb_fab->mtime)
			{
				struct tm ltm;
				localtime_r(&gn->hb_fab->mtime, &ltm);
				strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-abs", space);
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-del", durationstring(time(0) - gn->hb_fab->mtime));
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
				, gn->needs.e[x]->B->idstring
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
				, gn->feeds.e[x]->A->idstring
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

char* gn_designate(gn * gn)
{
	gn->flags = 0;

	if(gn->path->is_nofile)
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

int gn_secondary_reload(gn * const gn)
{
	if(euidaccess(gn->path->can, F_OK) == 0)
	{
		gn->exists = 1;
	}
	else if(errno != ENOENT)
	{
		fail("access(%s)=[%d][%s]", gn->path, errno, strerror(errno));
	}

	fatal(xsprintf, &gn->noforce_dir, "%s/SECONDARY/%u/fab", GN_DIR_BASE, gn->path->can_hash);
	fatal(xsprintf, &gn->noforce_path, "%s/SECONDARY/%u/fab/noforce", GN_DIR_BASE, gn->path->can_hash);

	if(euidaccess(gn->noforce_path, F_OK) == 0)
	{
		gn->fab_noforce = 1;
	}
	else if(errno != ENOENT)
	{
		fail("access(%s)=[%d][%s]", gn->noforce_path, errno, strerror(errno));
	}

	finally : coda;
}

int gn_secondary_rewrite_fab(gn * const gn)
{
	fatal(mkdirp, gn->noforce_dir, S_IRWXU | S_IRWXG | S_IRWXO);

	int fd;
	if((fd = open(gn->noforce_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		fail("open failed : [%d][%s]", errno, strerror(errno));

	struct timespec times[2] = { { .tv_nsec = UTIME_NOW } , { .tv_nsec = UTIME_NOW } };

	fatal_os(futimens, fd, times);

	finally : coda;
}

int gn_primary_reload_dscv(gn * const gn)
{
	// ensure we've reloaded hashes for the backing file
	fatal(gn_primary_reload, gn);

	// create ddisc block
	fatal(depblock_create, &gn->dscv_block, "%s/PRIMARY/%u/dscv", GN_DIR_BASE, gn->path->can_hash);

	// backing file has not changed
	if(hashblock_cmp(gn->hb_dscv) == 0)
	{
		// node has not been invalidated (see gn_invalidations)
		if(gn->invalid == 0)
		{
			// primary node with associated discovery fml eval
			if(gn->dscv)
			{
				// actually load the depblock from cache
				fatal(depblock_read, gn->dscv_block);
			}
		}
	}

	finally : coda;
}

int gn_primary_reload(gn * const gn)
{
	if(gn->hb_loaded == 0)
	{
		// create hashblocks
		fatal(hashblock_create, &gn->hb_fab, "%s/PRIMARY/%u/fab", GN_DIR_BASE, gn->path->can_hash);
		fatal(hashblock_create, &gn->hb_dscv, "%s/PRIMARY/%u/dscv", GN_DIR_BASE, gn->path->can_hash);

		// load the previous hashblocks
		fatal(hashblock_read, gn->hb_fab);
		fatal(hashblock_read, gn->hb_dscv);

		log(L_HASHBLK
			, "%s (fab) <== 0x%08x%08x%08x %s"
			, gn->idstring
			, gn->hb_fab->stathash[0]
			, gn->hb_fab->contenthash[0]
			, gn->hb_fab->vrshash[0]
			, gn->hb_fab->hashdir
		);

		log(L_HASHBLK
			, "%s (dscv) <== 0x%08x%08x%08x"
			, gn->idstring
			, gn->hb_dscv->stathash[0]
			, gn->hb_dscv->contenthash[0]
			, gn->hb_dscv->vrshash[0]
		);

		// stat the file, compute new stathash
		fatal(hashblock_stat, gn->path->can, gn->hb_fab, gn->hb_dscv, 0);

		gn->hb_fab->vrshash[1] = FAB_VERSION;
		gn->hb_dscv->vrshash[1] = FAB_VERSION;
		gn->hb_loaded = 1;
	}

	finally : coda;
}

int gn_primary_rewrite_fab(gn * const gn)
{
	// rewrite the fab hashblock
	fatal(hashblock_write, gn->hb_fab);

	log(L_HASHBLK
		, "%s (fab) ==> 0x%08x%08x%08x %s"
		, gn->idstring
		, gn->hb_fab->stathash[1]
		, gn->hb_fab->contenthash[1]
		, gn->hb_fab->vrshash[1]
		, gn->hb_fab->hashdir
	);

	finally : coda;
}

int gn_primary_rewrite_dscv(gn * const gn)
{
	// rewrite dependency discovery block
	if(gn->dscv_block)
		fatal(depblock_write, gn->dscv_block);

	// rewrite the discovery hashblock
	fatal(hashblock_write, gn->hb_dscv);

	log(L_HASHBLK
		, "%s (dscv) ==> 0x%08x%08x%08x"
		, gn->idstring
		, gn->hb_dscv->stathash[1]
		, gn->hb_dscv->contenthash[1]
		, gn->hb_dscv->vrshash[1]
	);

	finally : coda;
}

int gn_invalidations()
{
	int x;

	// update all node designations
	for(x = 0; x < gn_nodes.l; x++)
		gn_designate(gn_nodes.e[x]);

	if(g_args.invalidationsz)
	{
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(gn_nodes.e[x]->designation == GN_DESIGNATION_PRIMARY)
				gn_nodes.e[x]->invalid = 1;
//				gn_nodes.e[x]->changed = 1;

			if(gn_nodes.e[x]->designation == GN_DESIGNATION_SECONDARY)
				gn_nodes.e[x]->invalid = 1;
//				gn_nodes.e[x]->rebuild = 1;
		}
	}
	else
	{
		for(x = 0; x < g_args.invalidationsl; x++)
		{
			gn * gn = 0;
			fatal(gn_lookup, g_args.invalidations[x], 0, g_args.init_fabfile_path->abs_dir, &gn);

			if(gn)
			{
				if(gn->designation == GN_DESIGNATION_PRIMARY)
					gn->invalid = 1;

				if(gn->designation == GN_DESIGNATION_SECONDARY)
					gn->invalid = 1;
			}
			else
			{
				log(L_WARN, "invalidation : %s not found", g_args.invalidations[x]);
			}
		}
	}

	finally : coda;
}

static void gn_freenode(gn * const gn)
{
	int x;

	if(gn)
	{
		path_free(gn->path);
		free(gn->idstring);

		hashblock_free(gn->hb_fab);
		hashblock_free(gn->hb_dscv);
		depblock_free(gn->dscv_block);

		for(x = 0; x < gn->needs.l; x++)
			free(gn->needs.e[x]);
		free(gn->needs.e);
		map_free(gn->needs.by_B);

		free(gn->feeds.e);
		map_free(gn->feeds.by_A);

		free(gn->affecting_ff_file);

		free(gn->noforce_dir);
		free(gn->noforce_path);
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

char* gn_idstring(gn * const gn)
{
	return gn->idstring;
}

int gn_affected_ff_reg(gn * const gn, struct ff_file * const ff, int * const newa)
{
	int x;
	for(x = 0; x < gn->affecting_ff_filel; x++)
	{
		if(gn->affecting_ff_file[x] == ff)
			break;
	}

	if(x >= gn->affecting_ff_filel)
	{
		if(gn->affecting_ff_filel == gn->affecting_ff_filea)
		{
			int ns = gn->affecting_ff_filea ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &gn->affecting_ff_file, sizeof(*gn->affecting_ff_file), ns, gn->affecting_ff_filea);
			gn->affecting_ff_filea = ns;
		}

		gn->affecting_ff_file[gn->affecting_ff_filel++] = ff;
		(*newa)++;
	}

	finally : coda;
}
