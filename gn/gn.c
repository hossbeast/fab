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
#include "map.h"
#include "cksum.h"

#define restrict __restrict

char * ff_idstring(struct ff_file * const);

//
// data
//

union gn_nodes_t gn_nodes = { { .size = sizeof(gn) } };

//
// static
//

// map from (uint32) hash of detected cycle -> (int) number of times that particular cycle was encountered
static map * o_cycles;

/// raise_cycle
//
// SUMMARY
//  handle the fact that a cycle was detected in the dependency graph
//
// PARAMETERS
//  stack   - stack of nodes representing the cycle
//  stacksz - number of elements the stack can hold
//  num     - number of elements set in stack
//
static int raise_cycle(gn ** stack, size_t stacksz, int num)
{
	void logcycle(int top)
	{
		int x;
		for(x = top; x >= 0; x--)
		{
			if(x != top)
				log_add(" -> ");

			log_add("%s", stack[x]->idstring);
		}

		if(num == stacksz)
			log_finish(" -> ...");
		else
			log_finish("");
	};

	if(g_args.mode_cycl == MODE_CYCL_DEAL)
		return 1;

	// skip the beginning of the traversal, to the start of the cycle
	int top;
	for(top = num - 1; top >= 0; top--)
	{
		if(stack[top] == stack[0])
			break;
	}

	if(g_args.mode_cycl == MODE_CYCL_FAIL)
	{
		log_start(L_ERROR, "detected cycle : ");
		logcycle(top);
		return 0;
	}
	else if(g_args.mode_cycl == MODE_CYCL_WARN)
	{
		if(!o_cycles)
			fatal(map_create, &o_cycles, 0);

		// hash this cycle
		uint32_t cyc = cksum(stack, top * sizeof(*stack));

		// get or set the hash into the map
		int * c = 0;
		if((c = map_get(o_cycles, MM(cyc))) == 0)
		{
			int cc = 0;
			c = map_set(o_cycles, MM(cyc), MM(cc));
		}

		if((*c)++ == 0)
		{
			log_start(L_WARN, "detected cycle : ");
			logcycle(top);
		}

		return 1;
	}

	finally : coda;
}

static void freenode(gn * const gn)
{
	if(gn)
	{
		path_free(gn->path);
		free(gn->idstring);

		hashblock_free(gn->hb_fab);
		hashblock_free(gn->hb_dscv);
		depblock_free(gn->dscv_block);

		free(gn->dscvs);

		int x;
		for(x = 0; x < gn->needs.l; x++)
			free(gn->needs.e[x]);

		free(gn->needs.e);
		map_free(gn->needs.by_B);

		free(gn->feeds.e);
		map_free(gn->feeds.by_A);

		free(gn->closure_ffs);

		free(gn->noforce_dir);
		free(gn->noforce_path);
	}

	free(gn);
}

/// lookup
//
// SUMMARY
//  lookup a gn by absolute path, relative path, or NOFILE-name
//
// PARAMETERS
//  [base] - directory path for resolving relative paths
//  [sstk] - stringstack for resolving nofile paths
//  iscan  - skip canonicalization (path is already known to be canonicalized)
//  s      - string
//  l      - string length, or 0 for strlen
//  r      - node returned here, if found
//
// RETURNS
//  0 on failure (ENOMEN) and 1 otherwise
//
static int lookup(const char * const restrict base, strstack * const restrict sstk, const char * const restrict s, int l, gn ** r)
	__attribute__((nonnull(3)));

static int lookup(const char * const base, strstack * const sstk, const char * const s, int l, gn ** r)
{
	char space[512];									// space for canonicalization
	const char * canp	= s;						// canonical path for lookup
	int canpl		= l ?: strlen(s);			// canp length

	if(canpl > 4 && memcmp(canp, "/../", 4) == 0 && sstk)
	{
		char * sstr = 0;
		fatal(strstack_string, sstk, "/", "/", &sstr);
		fatal(canon, canp + 4, canpl - 4, space, sizeof(space), sstr, CAN_REALPATH);

		canp = space;
		canpl = strlen(canp);
	}
	else if(base)
	{
		fatal(canon, canp, canpl, space, sizeof(space), base, CAN_REALPATH);

		canp = space;
		canpl = strlen(canp);
	}

	gn ** R = 0;
	if((R = map_get(gn_nodes.by_path, canp, canpl)))
		(*r) = *R;

	finally : coda;
}

///
/// public
///

int gn_match(const char * const base, const char * const s, gn *** const r, int * const rl, int * const ra)
{
	int add(gn * e)
	{
		if((*rl) == (*ra))
		{
			int ns = (*ra) ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, r, sizeof(**r), ns, *ra);
			*ra = ns;
		}
		(*r)[(*rl)++] = e;

		finally : coda;
	};

	if(!gn_nodes.by_path)
		fatal(map_create, &gn_nodes.by_path, 0);

	char can[512];

	int sl = strlen(s);
	if(s[0] == '/' && s[sl - 1] == '/')
	{
		// regex match (substring for now)
		int x;
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(xstrstr(gn_nodes.e[x]->path->can, 0, s + 1, sl - 2, 0) || xstrstr(gn_nodes.e[x]->path->abs, 0, s + 1, sl - 2, 0) || xstrstr(gn_nodes.e[x]->path->rel, 0, s + 1, sl - 2, 0))
				fatal(add, gn_nodes.e[x]);
		}
	}
	else if(s[0] == '@')
	{
		// nofile
		int d = 0;
		d += snprintf(can + d, sizeof(can) - d, "/..");

		const char * p[2] = { [0] = s + 1 };
		while((p[1] = strstr(p[0], ".")))
		{
			d += snprintf(can + d, sizeof(can) - d, "/%.*s", (int)(p[1] - p[0]), p[0]);
			p[0] = p[1] + 1;
		}

		d += snprintf(can + d, sizeof(can) - d, "/%s", p[0]);

		gn ** R = 0;
		if((R = map_get(gn_nodes.by_path, can, strlen(can))))
			fatal(add, *R);
	}
	else
	{
		// relative to base, or canonical path
		if(canon(s, 0, can, sizeof(can), base, CAN_REALPATH) == 0)
			return 0;

		gn ** R = 0;
		if((R = map_get(gn_nodes.by_path, can, strlen(can))))
			fatal(add, *R);
	}

	finally : coda;
}

int gn_add(const char * const restrict base, strstack * const restrict sstk, char * const restrict A, int Al, gn ** gna, int * const restrict new)
{
	if(gn_nodes.by_path)
		fatal(lookup, base, sstk, A, Al, gna);
	else
		fatal(map_create, &gn_nodes.by_path, 0);

	if((*gna) == 0)
	{
		fatal(coll_doubly_add, &gn_nodes.c, 0, gna);

		Al = Al ?: strlen(A);

		// populate gna
		if(Al > 4 && memcmp(A, "/../", 4) == 0 && sstk)
		{
			char * sstr;
			fatal(strstack_string, sstk, "/", "/", &sstr);
			fatal(path_create_canon, &(*gna)->path, "%s/%.*s", sstr, Al - 4, A + 4);
		}
		else if(base)
		{
			fatal(path_create, &(*gna)->path, base, "%.*s", Al, A);
		}
		else
		{
			fatal(path_create_canon, &(*gna)->path, "%.*s", Al, A);
		}

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
	, strstack * const sstk
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
		fatal(gn_add, base, sstk, *(char**)A, Al, &gna, &_newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_add, base, sstk, *(char**)B, Bl, &gnb, &_newb);

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
			else if(dscv_gn)
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

int gn_depth_traversal_nodes_needsward_useweak(gn * gn, int (*logic)(struct gn *, int d));
{
	gn * stack[64] = {};
	int num = 0;

	// RETURNS
	//  0 - success
	//  1 - cycle detected
	// -1 - callback failure
	//
	int enter(gn * n, int d)
	{
		if(n->guard)
		{
			if(num < sizeof(stack) / sizeof(stack[0]))
				stack[num++] = n;

			n->guard = 0;
			return 1;
		}

		// descend
		n->guard = 1;

		int x;
		for(x = 0; x < n->needs.l; x++)
		{
			int e = enter(n->needs.e[x]->B, d + 1);
			if(e == 1)
			{
				if(num < sizeof(stack) / sizeof(stack[0]))
					stack[num++] = n;

				n->guard = 0;
				return 1;
			}
			if(e == -1)
			{
				n->guard = 0;
				return -1;
			}
		}

		// logic on this node
		if(logic(n, d) == 0)
		{
			n->guard = 0;
			return -1;
		}

		n->guard = 0;
		return 0;
	};

	int e = enter(r, 0);
	if(e == 1)
		return raise_cycle(stack, sizeof(stack) / sizeof(stack[0]), num);
	else if(e == -1)
		return 0;

	return 1;
}

int gn_depth_traversal_nodes_feedsward_useweak(gn * r, int (*logic)(gn*, int))
{
	gn * stack[64] = {};
	int num = 0;

	// RETURNS
	//  0 - success
	//  1 - cycle detected
	// -1 - callback failure
	//
	int enter(gn * n, int d)
	{
		if(n->guard)
		{
			if(num < sizeof(stack) / sizeof(stack[0]))
				stack[num++] = n;

			n->guard = 0;
			return 1;
		}

		// descend
		n->guard = 1;

		int x;
		for(x = 0; x < n->feeds.l; x++)
		{
			int e = enter(n->feeds.e[x]->B, d + 1);
			if(e == 1)
			{
				if(num < sizeof(stack) / sizeof(stack[0]))
					stack[num++] = n;

				n->guard = 0;
				return 1;
			}
			else if(e == -1)
			{
				n->guard = 0;
				return -1;
			}
		}

		// logic on this node
		if(logic(n, d) == 0)
		{
			n->guard = 0;
			return -1;
		}

		n->guard = 0;
		return 0;
	};

	int e = enter(r, 0);
	if(e == 1)
		return raise_cycle(stack, sizeof(stack) / sizeof(stack[0]), num);
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
			// primary node with associated discovery fml eval(s)
			if(gn->dscvsl)
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

			if(gn_nodes.e[x]->designation == GN_DESIGNATION_SECONDARY)
				gn_nodes.e[x]->invalid = 1;
		}
	}
	else
	{
		for(x = 0; x < g_args.invalidationsl; x++)
		{
			gn * gn = 0;
			fatal(lookup, g_args.init_fabfile_path->abs_dir, 0, g_args.invalidations[x], 0, &gn);

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

void gn_teardown()
{
	int x;
	for(x = 0; x < gn_nodes.l; x++)
		freenode(gn_nodes.e[x]);

	free(gn_nodes.e);
	map_free(gn_nodes.by_path);

	map_free(o_cycles);
}

// necessary for a module to call which cannot include the struct gn definition
char* gn_idstring(gn * const gn)
{
	return gn->idstring;
}

int gn_enclose_ff(gn * const gn, struct ff_file * const ff, int * const newa)
{
	int x;
	for(x = 0; x < gn->closure_ffsl; x++)
	{
		if(gn->closure_ffs[x] == ff)
			break;
	}

	if(x >= gn->closure_ffsl)
	{
		if(gn->closure_ffsl == gn->closure_ffsa)
		{
			int ns = gn->closure_ffsa ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &gn->closure_ffs, sizeof(*gn->closure_ffs), ns, gn->closure_ffsa);
			gn->closure_ffsa = ns;
		}

		gn->closure_ffs[gn->closure_ffsl++] = ff;
		(*newa)++;
	}

	finally : coda;
}

void gn_dump(gn * gn)
{
	int x;
	char space[128];

	if(log_would(L_DG | L_DGRAPH))
	{
		// path properties
		log(L_DG | L_DGRAPH, "%12s : %s", "can-path"	, gn->path->can);
		log(L_DG | L_DGRAPH, "%12s : %s", "abs-path"	, gn->path->abs);
		log(L_DG | L_DGRAPH, "%12s : %s", "rel-path"	, gn->path->rel);
		log(L_DG | L_DGRAPH, "%12s : %s", "path-in"	, gn->path->in);
		log(L_DG | L_DGRAPH, "%12s : %s", "path-base", gn->path->base);
		log(L_DG | L_DGRAPH, "%12s : %u", "canhash"	, gn->path->can_hash);
		log(L_DG | L_DGRAPH, "%12s : %s", "name"			, gn->path->name);
		log(L_DG | L_DGRAPH, "%12s : %s", "ext"			, gn->path->ext);
		log(L_DG | L_DGRAPH, "%12s : %s", "ext_last"	, gn->path->ext_last);

		log(L_DG | L_DGRAPH, "%12s : %s", "designation", gn_designate(gn));

		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			if(gn->dscvsl)
			{
				for(x = 0; x < gn->dscvsl; x++)
				{
					log(L_DG | L_DGRAPH, "%12s : (%s)[%3d,%3d - %3d,%3d]", "dsc formula"
						, ff_idstring(gn->dscvs[x]->fml->ffn->loc.ff)
						, gn->dscvs[x]->fml->ffn->loc.f_lin + 1
						, gn->dscvs[x]->fml->ffn->loc.f_col + 1
						, gn->dscvs[x]->fml->ffn->loc.l_lin + 1
						, gn->dscvs[x]->fml->ffn->loc.l_col + 1
					);
				}
			}
			else
			{
				log(L_DG | L_DGRAPH, "%12s : none", "dsc formula");
			}

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
				log(L_DG | L_DGRAPH, "%12s : (%s)[%3d,%3d - %3d,%3d]", "fab formula"
					, ff_idstring(gn->fabv->fml->ffn->loc.ff)
					, gn->fabv->fml->ffn->loc.f_lin + 1
					, gn->fabv->fml->ffn->loc.f_col + 1
					, gn->fabv->fml->ffn->loc.l_lin + 1
					, gn->fabv->fml->ffn->loc.l_col + 1
				);

				if(gn->fabv->productsl > 1)
				{
					int x;
					for(x = 0; x < gn->fabv->productsl; x++)
						log(L_DG | L_DGRAPH, "%12s --> %s", "", gn->fabv->products[x]->path);
				}
			}
			else
			{
				log(L_WARN | L_DG | L_DGRAPH, "%12s : %s", "fab formula", "(no formula)");
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "height", gn->height);
		log(L_DG | L_DGRAPH, "%12s : %d", "stage", gn->stage);

		log(L_DG | L_DGRAPH, "%12s : %d", "needs", gn->needs.l);
		for(x = 0; x < gn->needs.l; x++)
		{
			if(gn->needs.e[x]->type == GN_RELATION_REGULAR)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
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
			else if(gn->needs.e[x]->type == GN_RELATION_CACHED)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (DSC:%s)[%6s%s%6s]"
					, ""
					, gn->needs.e[x]->weak ? "*" : " "
					, gn->needs.e[x]->B->idstring
					, gn->needs.e[x]->dscv_gn->idstring
					, "", "cache", ""
				);
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "feeds", gn->feeds.l);
		for(x = 0; x < gn->feeds.l; x++)
		{
			if(gn->feeds.e[x]->type == GN_RELATION_REGULAR)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
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
			else if(gn->feeds.e[x]->type == GN_RELATION_CACHED)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (DSC:%s)[%6s%s%6s]"
					, ""
					, gn->feeds.e[x]->weak ? "*" : " "
					, gn->feeds.e[x]->A->idstring
					, gn->feeds.e[x]->dscv_gn->idstring
					, "", "cache", ""
				);
			}
		}

		log(L_DG | L_DGRAPH, "");
	}
}
