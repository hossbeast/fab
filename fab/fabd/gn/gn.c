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

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <dirent.h>

#include "xlinux.h"
#include "xlinux/mempolicy.h"

#include "global.h"
#include "gn.h"
#include "args.h"
#include "fml.h"
#include "ff.h"
#include "strstack.h"
#include "canon.h"
#include "graph.h"

#include "unitstring.h"
#include "map.h"
#include "cksum.h"
#include "parseint.h"
#include "macros.h"

#define restrict __restrict

// avoid circular dependency on ff
char * ff_idstring(struct ff_file * const);

//
// static
//

#if 0
static void freenode(gn * const gn)
{
	if(gn)
	{
		path_free(gn->path);
		free(gn->idstring);

		hashblock_free(gn->hb);

		free(gn->dscvs);

		int x;
		for(x = 0; x < gn->needs.l; x++)
			free(gn->needs.e[x]);

		free(gn->needs.e);
		map_free(gn->needs.by_B);

		free(gn->feeds.e);
		map_free(gn->feeds.by_A);
	}

	free(gn);
}
#endif

/// lookup
//
// SUMMARY
//  lookup a gn by absolute path, relative path, or NOFILE-name
//
// PARAMETERS
//  base   - directory path for resolving relative paths
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
	__attribute__((nonnull(1, 3)));

static int lookup(const char * const base, strstack * const sstk, const char * const s, int l, gn ** r)
{
	char space[512];									// space for canonicalization
	const char * canp	= s;						// canonical path for lookup
	int canpl		= l ?: strlen(s);			// canp length

	if(canpl > 4 && memcmp(canp, "/../", 4) == 0 && sstk)
	{
		char * sstr = 0;
		fatal(strstack_string, sstk, 0, "/", "/", &sstr);
		fatal(canon, canp + 4, canpl - 4, sstr, 0, space, sizeof(space), 0, CAN_SLASH | CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT);

		canp = space;
		canpl = strlen(canp);
	}
	else
	{
		fatal(canon, canp, canpl, base, 0, space, sizeof(space), 0, CAN_REALPATH);

		canp = space;
		canpl = strlen(canp);
	}

	gn ** R = 0;
	if((R = map_get(graph.by_path, canp, canpl)))
		(*r) = *R;

	finally : coda;
}

///
/// public
///

int gn_lookup(const char * const s, int sl, const char * const base, int basel, gn ** const r)
{
	char can[512];

	if(sl > 0 && s[0] == '@')
	{
		// nofile
		int d = 0;
		d += snprintf(can + d, sizeof(can) - d, "/..");

		const char * p[2] = { [0] = s + 1 };
		while((p[1] = memchr(p[0], '.', sl - (p[0] - s))))
		{
			d += snprintf(can + d, sizeof(can) - d, "/%.*s", (int)(p[1] - p[0]), p[0]);
			p[0] = p[1] + 1;
		}

		d += snprintf(can + d, sizeof(can) - d, "/%.*s", (int)(sl - (p[0] - s)), p[0]);
	}
	else
	{
		// relative to init-fabfile-path, or an absolute path
		fatal(canon, s, sl, base, basel, can, sizeof(can), 0, CAN_REALPATH);
	}

	gn ** R = 0;
	if((R = map_get(graph.by_path, can, strlen(can))))
		*r = *R;

	finally : coda;
}

int gn_add(const char * const restrict base, strstack * const restrict sstk, char * const restrict A, int Al, gn ** gna, int * const restrict new)
{
  int mpc = 0;

	fatal(lookup, base, sstk, A, Al, gna);

	if((*gna) == 0)
	{
    // redirect graph allocations
    fatal(mempolicy_push, graph_mempolicy, &mpc);
		fatal(coll_doubly_add, &graph.c, 0, gna);
		fatal(hashblock_create, &(*gna)->hb);
    fatal(mempolicy_pop, &mpc);

		Al = Al ?: strlen(A);

		if(Al > 4 && memcmp(A, "/../", 4) == 0 && sstk)
		{
			char * sstr;
			fatal(strstack_string, sstk, 0, "/", "/", &sstr);
      fatal(mempolicy_push, graph_mempolicy, &mpc);
			fatal(path_create, &(*gna)->path, "/..", "%s/%.*s", sstr, Al - 4, A + 4);
      fatal(mempolicy_pop, &mpc);
		}
		else
		{
      fatal(mempolicy_push, graph_mempolicy, &mpc);
			fatal(path_create, &(*gna)->path, base, "%.*s", Al, A);
      fatal(mempolicy_pop, &mpc);
		}

		(*gna)->needs.z		= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z		= sizeof((*gna)->feeds.e[0]);

		// idstring
		if(Al > 4 && memcmp(A, "/../", 4) == 0 && sstk && g_args->mode_gnid != MODE_CANONICAL)
		{
			char * sstr;
			fatal(strstack_string, sstk, 1, "", ".", &sstr);
      fatal(mempolicy_push, graph_mempolicy, &mpc);
			fatal(ixsprintf, &(*gna)->idstring, "@%s", sstr);

			int x;
			for(x = 4; x < Al; x++)
			{
				int y;
				for(y = x; y <= Al; y++)
				{
					if(A[y] == '/' || y == Al)
					{
						if(strlen((*gna)->idstring) > 1)
							fatal(ixstrcatf, &(*gna)->idstring, ".%.*s", y - x, &A[x]);
						else
							fatal(ixstrcatf, &(*gna)->idstring, "%.*s", y - x, &A[x]);

						x = y;
						break;
					}
				}
			}
      fatal(mempolicy_pop, &mpc);
		}
    else
    {
      fatal(mempolicy_push, graph_mempolicy, &mpc);
      if(g_args->mode_gnid == MODE_CANONICAL)
      {
        fatal(ixstrdup, &(*gna)->idstring, (*gna)->path->can);
      }
      else if(g_args->mode_gnid == MODE_ABSOLUTE)
      {
        fatal(ixstrdup, &(*gna)->idstring, (*gna)->path->abs);
      }
      else if(g_args->mode_gnid == MODE_RELATIVE_CWD)
      {
        fatal(ixstrdup, &(*gna)->idstring, (*gna)->path->rel_cwd);
      }
      else if(g_args->mode_gnid == MODE_RELATIVE_FABFILE_DIR)
      {
        fatal(ixstrdup, &(*gna)->idstring, (*gna)->path->rel_fab);
      }
      fatal(mempolicy_pop, &mpc);
    }
		(*gna)->idstringl = strlen((*gna)->idstring);

    fatal(mempolicy_push, graph_mempolicy, &mpc);
		fatal(map_set, graph.by_path, (*gna)->path->can, (*gna)->path->canl, gna, sizeof(*gna), 0);
		fatal(map_set, graph.by_pathhash, MM((*gna)->path->can_hash), gna, sizeof(*gna), 0);
    fatal(mempolicy_pop, &mpc);

		fatal(gn_finalize, *gna);

		if(new)
			(*new)++;
	}

finally:
  mempolicy_unwind(&mpc);
coda;
}

int gn_edge_add(
	  char * const base
	, strstack * const sstk
	, void ** const A, int Al, int At
	, void ** const B, int Bl, int Bt
	, struct ff_node * const reg_ffn
	, gn * const dscv_gn
	, int isweak
	, int isbridge
	, int * const newa
	, int * const newb
	, int * const newr
)
{
	gn *	gna = 0;
	gn *	gnb = 0;

	int _newa = 0;
	int _newb = 0;
	int _newr = 0;

  int mpc = 0;

	if(At)
		gna = *(gn**)A;
	else
		fatal(gn_add, base, sstk, *(char**)A, Al, &gna, &_newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_add, base, sstk, *(char**)B, Bl, &gnb, &_newb);

  // redirect graph allocations
  fatal(mempolicy_push, graph_mempolicy, &mpc);

	if(gna == gnb)
	{
		// silently ignore dependencies of a node upon itself
	}
	else
	{
		if(newa)
			(*newa) += _newa;
		if(newb)
			(*newb) += _newb;

		// error check
		if(gnb->path->is_nofile && !gna->path->is_nofile)
			failf(FAB_BADPLAN, "file-backed node %s may not depend on non-file-backed node %s", gna->path->abs, gnb->path->abs);

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

			fatal(map_set, gna->needs.by_B, &gnb, sizeof(gnb), &rel, sizeof(rel), 0);
			fatal(map_set, gnb->feeds.by_A, &gna, sizeof(gna), &rel, sizeof(rel), 0);

			rel->A = gna;
			rel->B = gnb;
			rel->weak = isweak;
			rel->bridge = isbridge;

			if(reg_ffn)
			{
				rel->type = GN_RELATION_REGULAR;
				rel->ffn = reg_ffn;
			}
			else if(dscv_gn)
			{
				rel->type = GN_RELATION_DISCOVERED;
				rel->dscvgn = dscv_gn;
			}

			_newr++;
		}
	}

	*A = gna;
	*B = gnb;

	if(_newr)
	{
		fatal(gn_finalize, *A);
		fatal(gn_finalize, *B);
	}

	if(newr)
		(*newr) += _newr;

finally:
  mempolicy_unwind(&mpc);
coda;
}

size_t gn_invalid_reasons_write(gn * const gn, char * dst, const size_t sz)
{
	size_t z = 0;

	if(gn->invalid)
	{
		if(gn->invalid & GN_INVALIDATION_NXFILE)
			z += znloadf(dst + z, sz - z, "file-not-found");
		if(gn->invalid & GN_INVALIDATION_CHANGED)
			z += znloadf(dst + z, sz - z, "%sfile-changed", z ? ", " : "");
		if(gn->invalid & GN_INVALIDATION_SOURCES)
			z += znloadf(dst + z, sz - z, "%ssources-invalid", z ? ", " : "");
		if(gn->invalid & GN_INVALIDATION_USER)
			z += znloadf(dst + z, sz - z, "%suser-invalidated", z ? ", " : "");
		if(gn->invalid & GN_INVALIDATION_DISCOVERY)
			z += znloadf(dst + z, sz - z, "%sdiscovery", z ? ", " : "");
		if(gn->invalid & GN_INVALIDATION_FABRICATE)
			z += znloadf(dst + z, sz - z, "%sfabricate", z ? ", " : "");
	}
	else
	{
		z += znloadf(dst + z, sz - z, "up-to-date");
	}

	return z;
}

void gn_dump(gn * gn)
{
	int x;
	char space[128];

	if(log_would(L_NODE))
	{
		// path properties
		logf(L_NODE, "%18s : %s", "idstring"				, gn->idstring);
		logf(L_NODE, "%18s : %s", "type"						, GN_TYPE_STR(gn->type));
		logf(L_NODE, "%18s : %s", "can-path"				, gn->path->can);
		logf(L_NODE, "%18s : %s", "in-path"				, gn->path->in_path);
		logf(L_NODE, "%18s : %s", "in-base"				, gn->path->in_base);
		logf(L_NODE, "%18s : %s", "abs-path"				, gn->path->abs);
		logf(L_NODE, "%18s : %s", "rel-fab-path"		, gn->path->rel_fab);
		logf(L_NODE, "%18s : %s", "rel-cwd-path"		, gn->path->rel_cwd);
		logf(L_NODE, "%18s : %s", "rel-nofile-path", gn->path->rel_nofile);
		logf(L_NODE, "%18s : %u", "canhash"				, gn->path->can_hash);
		logf(L_NODE, "%18s : %s", "name"						, gn->path->name);
		logf(L_NODE, "%18s : %s", "ext"						, gn->path->ext);
		logf(L_NODE, "%18s : %s", "ext_last"				, gn->path->ext_last);

		if(gn->type == GN_TYPE_PRIMARY || gn->type == GN_TYPE_SECONDARY)
		{
			gn_invalid_reasons_write(gn, space, sizeof(space));
			logf(L_NODE, "%18s : %s", "state", space);
		}

		if(gn->type == GN_TYPE_PRIMARY)
		{
			if(gn->dscvsl)
			{
				for(x = 0; x < gn->dscvsl; x++)
				{
					int y = gn->dscvs[x]->ctx->locsl - 1;

					logf(L_NODE, "%18s : [%3d,%3d - %3d,%3d] @ %s", "dsc formula"
						, gn->dscvs[x]->ctx->locs[y]->f_lin + 1
						, gn->dscvs[x]->ctx->locs[y]->f_col + 1
						, gn->dscvs[x]->ctx->locs[y]->l_lin + 1
						, gn->dscvs[x]->ctx->locs[y]->l_col + 1
						, ff_idstring(gn->dscvs[x]->ctx->locs[y]->ff)
					);

					for(y--; y >= 0; y--)
					{
						logf(L_NODE, "%18s   [%3d,%3d - %3d,%3d] @ %s", ""
							, gn->dscvs[x]->ctx->locs[y]->f_lin + 1
							, gn->dscvs[x]->ctx->locs[y]->f_col + 1
							, gn->dscvs[x]->ctx->locs[y]->l_lin + 1
							, gn->dscvs[x]->ctx->locs[y]->l_col + 1
							, ff_idstring(gn->dscvs[x]->ctx->locs[y]->ff)
						);
					}
				}
			}
			else
			{
				logf(L_NODE, "%18s : none", "dsc formula");
			}

			logf(L_NODE, "%18s : %d", "size", (int)gn->hb->size);
			if(gn->hb->mtime)
			{
				struct tm ltm;
				localtime_r(&gn->hb->mtime, &ltm);
				strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

				logf(L_NODE, "%18s : %s", "mtime-abs", space);
				logf(L_NODE, "%18s : %s", "mtime-del", durationstring(time(0) - gn->hb->mtime));
			}
			else
			{
				logf(L_NODE, "%18s : %s", "mtime", "");
			}
		}

		if(gn->type & GN_TYPE_CANFAB)
		{
			if(gn->fabv)
			{
				int y = gn->fabv->ctx->locsl - 1;

				logf(L_NODE, "%18s : [%3d,%3d - %3d,%3d] @ %s", "fab formula"
					, gn->fabv->ctx->locs[y]->f_lin + 1
					, gn->fabv->ctx->locs[y]->f_col + 1
					, gn->fabv->ctx->locs[y]->l_lin + 1
					, gn->fabv->ctx->locs[y]->l_col + 1
					, ff_idstring(gn->fabv->ctx->locs[y]->ff)
				);

				for(y--; y >= 0; y--)
				{
					logf(L_NODE, "%18s   [%3d,%3d - %3d,%3d] @ %s", ""
						, gn->fabv->ctx->locs[y]->f_lin + 1
						, gn->fabv->ctx->locs[y]->f_col + 1
						, gn->fabv->ctx->locs[y]->l_lin + 1
						, gn->fabv->ctx->locs[y]->l_col + 1
						, ff_idstring(gn->fabv->ctx->locs[y]->ff)
					);
				}

				if(gn->fabv->productsl > 1)
				{
					int x;
					for(x = 0; x < gn->fabv->productsl; x++)
						logf(L_NODE, "%18s --> %s", "", gn->fabv->products[x]->idstring);
				}
			}
			else
			{
				logf(L_WARN | L_NODE, "%18s : %s", "fab formula", "(no formula)");
			}
		}

		logf(L_NODE, "%18s : %d needs", "in-degree", gn->needs.l);
		for(x = 0; x < gn->needs.l; x++)
		{
			if(gn->needs.e[x]->type == GN_RELATION_REGULAR)
			{
				logf(L_NODE, "%16s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
					, ""
					, gn->needs.e[x]->weak ? "*" : gn->needs.e[x]->bridge ? "^" : " "
					, gn->needs.e[x]->B->idstring
					, ff_idstring(gn->needs.e[x]->ffn->loc.ff)
					, gn->needs.e[x]->ffn->loc.f_lin + 1
					, gn->needs.e[x]->ffn->loc.f_col + 1
					, gn->needs.e[x]->ffn->loc.l_lin + 1
					, gn->needs.e[x]->ffn->loc.l_col + 1
				);
			}
			else if(gn->needs.e[x]->type == GN_RELATION_DISCOVERED)
			{
				logf(L_NODE, "%16s %s --> %-40s"
					, ""
					, gn->needs.e[x]->weak ? "*" : gn->needs.e[x]->bridge ? "^" : " "
					, gn->needs.e[x]->B->idstring
					, gn->needs.e[x]->dscvgn->idstring
				);
			}
		}

		logf(L_NODE, "%18s : %d feeds", "out-degree", gn->feeds.l);
		for(x = 0; x < gn->feeds.l; x++)
		{
			if(gn->feeds.e[x]->type == GN_RELATION_REGULAR)
			{
				logf(L_NODE, "%16s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
					, ""
					, gn->feeds.e[x]->weak ? "*" : gn->feeds.e[x]->bridge ? "^" : " "
					, gn->feeds.e[x]->A->idstring
					, ff_idstring(gn->feeds.e[x]->ffn->loc.ff)
					, gn->feeds.e[x]->ffn->loc.f_lin + 1
					, gn->feeds.e[x]->ffn->loc.f_col + 1
					, gn->feeds.e[x]->ffn->loc.l_lin + 1
					, gn->feeds.e[x]->ffn->loc.l_col + 1
				);
			}
			else if(gn->feeds.e[x]->type == GN_RELATION_DISCOVERED)
			{
				logf(L_NODE, "%16s %s --> %-40s"
					, ""
					, gn->feeds.e[x]->weak ? "*" : gn->feeds.e[x]->bridge ? "^" : " "
					, gn->feeds.e[x]->A->idstring
					, gn->feeds.e[x]->dscvgn->idstring
				);
			}
		}

		logs(L_NODE, "");
	}
}

int gn_finalize(gn * const restrict gn)
{
	// recompute type
	if(gn->fabv)
	{
		if(gn->path->is_nofile)
			gn->type = GN_TYPE_TASK;
		else if(gn->needs.l)
			gn->type = GN_TYPE_SECONDARY;
		else
			gn->type = GN_TYPE_GENERATED;
	}
	else if(gn->path->is_nofile)
	{
		gn->type = GN_TYPE_GROUP;
	}
	else if(gn->needs.l)
	{
		gn->type = GN_TYPE_SECONDARY;
	}
	else
	{
		gn->type = GN_TYPE_PRIMARY;
	}

	finally : coda;
}

int gn_process_invalidations(gn ** const invalidations, int invalidationsl, int * const restrict repeat_discovery)
{
	int x;

	int c = invalidationsl;
	if(g_args->invalidationsz)
		c = graph.l;

	for(x = 0; x < c; x++)
	{
		gn * gn = 0;
		if(g_args->invalidationsz)
			gn = graph.e[x];
		else
			gn = invalidations[x];
		
		if(!gn->invalid)
			logf(L_INVALID, "%10s %s", GN_INVALIDATION_STR(GN_INVALIDATION_USER), gn->idstring);

		gn->invalid |= GN_INVALIDATION_USER;

		if(GN_IS_INVALID(gn))
		{
			if(gn->dscvsl)
			{
				gn->invalid |= GN_INVALIDATION_DISCOVERY;
				*repeat_discovery = 1;
			}
			else if(gn->type != GN_TYPE_PRIMARY)
			{
				gn->invalid |= GN_INVALIDATION_FABRICATE;
			}
		}
	}

	finally : coda;
}

int gn_init()
{
  finally : coda;
}

void gn_teardown()
{

}
