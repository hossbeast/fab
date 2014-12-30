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

#include <stdlib.h>
#include <sys/mman.h>
#include <alloca.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "listwise/object.h"
#include "listwise/lstack.h"

#include "dep.h"

#include "params.h"
#include "args.h"
#include "gnlw.h"
#include "fml.h"
#include "bp.h"
#include "logs.h"
#include "map.h"
#include "var.h"
#include "list.h"
#include "lwutil.h"

#include "global.h"
#include "xlinux.h"

#define restrict __restrict

///
/// static
///

static int dep_add_single(
	  ff_node * ffn
	, strstack * const restrict sstk
	, map * vmap
	, transform_parser * const gp
	, lwx *** stax
	, int * staxa
	, int staxp
	, int pl
	, gn ** first
	, int * newnp
	, int * newrp
)
{
	int i;
	int j;

	int pr = staxp;

	// resolve the right-hand side
	fatal(list_resolveflat, ffn->feeds, vmap, gp, stax, staxa, &staxp, 0, 0);

	// add edges, which are the cartesian product needs x feeds
	LSTACK_ITERATE((*stax)[pl], i, goa);
	if(goa)
	{
		char * As;
		void * Ap;
		int Al;
		uint8_t At;
		fatal(lstack_readrow, (*stax)[pl], 0, i, &As, &Al, &At, 0, 0, 0, 0);

		if(At)
			Ap = *(void**)As;
		else
			Ap = As;

		LSTACK_ITERATE((*stax)[pr], j, gob);
		if(gob)
		{
			char * Bs = 0;
			void * Bp = 0;
			int Bl = 0;
			uint8_t Bt = 0;

			fatal(lstack_readrow, (*stax)[pr], 0, j, &Bs, &Bl, &Bt, 0, 0, 0, 0);

			if(Bt)
				Bp = *(void**)Bs;
			else
				Bp = Bs;

			int newa = 0;
			int newb = 0;
			int newr = 0;

			if(ffn->loc.ff->type == FFT_DDISC)
			{
				// dependencies arising from an FFN_DEPENDENCY node in a DDISC fabfile yield paths which
				// are specified relative to base path of the DDISC node itself
				fatal(gn_edge_add
					, g_args->init_fabfile_path->abs_dir
					, sstk
					, &Ap
					, Al
					, At
					, &Bp
					, Bl
					, Bt
					, ffn
					, 0
					, ffn->flags & FFN_WEAK
					, ffn->flags & FFN_BRIDGE
					, &newa
					, &newb
					, &newr
				);
			}
			else
			{
				// dependencies arising from an FFN_DEPENDENCY node in a regular fabfile yield paths which
				// are specified relative to the absolute directory of the fabfile
				fatal(gn_edge_add
					, g_args->init_fabfile_path->abs_dir
					, sstk
					, &Ap
					, Al
					, At
					, &Bp
					, Bl
					, Bt
					, ffn
					, 0
					, ffn->flags & FFN_WEAK
					, ffn->flags & FFN_BRIDGE
					, &newa
					, &newb
					, &newr
				);
			}

			// if A was a string, gn_edge_add has just made it a gn*
			At = 1;

			if(newnp)
			{
				(*newnp) += newa;
				(*newnp) += newb;
			}
			if(newrp)
			{
				(*newrp) += newr;
			}
			if(first)
			{
				*first = Ap;
				first = 0;
			}

			uint64_t tag = L_DG;
			if(ffn->loc.ff->type == FFT_DDISC)
			{
				if((newnp && (newa || newb)) || (newrp && newr))
					tag |= L_DSCNEW;
			}

			if(newr)
			{
				logf(tag, "[%1s][%1s][%1s][%1s](%s)[%3d,%3d - %3d,%3d] %s -> %s"
					, "S"
					, newa ? "x" : ""
					, newb ? "x" : ""
					, newr ? "x" : ""
					, ff_idstring(ffn->loc.ff)
					, ffn->loc.f_lin + 1
					, ffn->loc.f_col + 1
					, ffn->loc.l_lin + 1
					, ffn->loc.l_col + 1
					, ((gn*)Ap)->idstring
					, ((gn*)Bp)->idstring
				);
			}
		}
		LSTACK_ITEREND;
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int dep_add_multi(
	  ff_node * ffn
	, strstack * const restrict sstk
	, map * vmap
	, transform_parser * const gp
	, lwx *** stax
	, int * staxa
	, int staxp
	, int pl
	, gn ** first
	, int * newnp
	, int * newrp
)
{
	// newa tracks whether the left-hand-side of a dependency references a newly-created node
	int * newa = 0;
	size_t newaa = 0;
	size_t newal = 0;

	int pli = staxp;			// left hand side instance
	int pr = staxp + 1;		// right hand side

	int x;
	for(x = 0; x < lwx_lists((*stax)[pl]); x++)
	{
		// prepare space for new variable definition list
		fatal(lw_reset, stax, staxa, pli);

		// populate the "<" variable (left-hand side)
//		newal = (*stax)[pl]->s[x].l;
		newal = lwx_rows((*stax)[pl], x);
		if(newaa < newal)
		{
			newaa = newal;
			newa = alloca(sizeof(*newa) * newaa);
		}
		memset(newa, 0, sizeof(*newa) * newal);
		
		int i;
		int j;
		for(i = 0; i < lwx_rows((*stax)[pl], x); i++)
		{
			char * rv;
			int rl;
			uint8_t rt;
			fatal(lstack_readrow, (*stax)[pl], x, i, &rv, &rl, &rt, 0, 0, 0, 0);

			gn * r = 0;
			if(rt)
			{
				r = *(void**)rv;
			}
			else
			{
				fatal(gn_add
					, g_args->init_fabfile_path->abs_dir
					, sstk
					, rv
					, rl
					, &r
					, &newa[i]
				);
			}

			fatal(lstack_obj_add, (*stax)[pli], r, LISTWISE_TYPE_GNLW);
		}

		// resolve the right-hand side in the context of $<
		fatal(var_set, vmap, "<", (*stax)[pli], 0, 1, 0);
		int pn = pr;
		fatal(list_resolveflat, ffn->feeds, vmap, gp, stax, staxa, &pn, 0, 0);

		for(i = 0; i < lwx_rows((*stax)[pl], x); i++)
		{
			char * As = 0;
			void * Ap = 0;
			int Al = 0;
			uint8_t At = 0;
			fatal(lstack_readrow, (*stax)[pl], x, i, &As, &Al, &At, 0, 0, 0, 0);

			if(At)
				Ap = *(void**)As;
			else
				Ap = As;

			LSTACK_ITERATE((*stax)[pr], j, gob);
			if(gob)
			{
				char * Bs = 0;
				void * Bp = 0;
				int Bl = 0;
				uint8_t Bt = 0;
				fatal(lstack_readrow, (*stax)[pr], 0, j, &Bs, &Bl, &Bt, 0, 0, 0, 0);

				if(Bt)
					Bp = *(void**)Bs;
				else
					Bp = Bs;

				int newb = 0;
				int newr = 0;

				if(ffn->loc.ff->type == FFT_DDISC)
				{
					fatal(gn_edge_add
						, g_args->init_fabfile_path->abs_dir
						, sstk
						, &Ap
						, Al
						, At
						, &Bp
						, Bl
						, Bt
						, ffn
						, 0
						, ffn->flags & FFN_WEAK
						, ffn->flags & FFN_BRIDGE
						, 0
						, &newb
						, &newr
					);
				}
				else
				{
					fatal(gn_edge_add
						, g_args->init_fabfile_path->abs_dir
						, sstk
						, &Ap
						, Al
						, At
						, &Bp
						, Bl
						, Bt
						, ffn
						, 0
						, ffn->flags & FFN_WEAK
						, ffn->flags & FFN_BRIDGE
						, 0
						, &newb
						, &newr
					);
				}

				// if A was a string, gn_edge_add has just made it a gn*
				At = 1;

				if(newnp)
				{
					(*newnp) += newa[i];
					(*newnp) += newb;
				}
				if(newrp)
				{
					(*newrp) += newr;
				}
				if(first)
				{
					*first = Ap;
					first = 0;
				}

				uint64_t tag = L_DG;
				if(ffn->loc.ff->type == FFT_DDISC)
				{
					if((newnp && (newa[i] || newb)) || (newrp && newr))
						tag |= L_DSCNEW;
				}

				if(newr)
				{
					logf(tag, "[%1s][%1s][%1s][%1s](%s)[%3d,%3d - %3d,%3d] %s -> %s"
						, "M"
						, newa[i] ? "x" : ""
						, newb ? "x" : ""
						, newr ? "x" : ""
						, ff_idstring(ffn->loc.ff)
						, ffn->loc.f_lin + 1
						, ffn->loc.f_col + 1
						, ffn->loc.l_lin + 1
						, ffn->loc.l_col + 1
						, ((gn*)Ap)->idstring
						, ((gn*)Bp)->idstring
					);
				}
			}
			LSTACK_ITEREND;
		}
	}

	finally : coda;
}

///
/// public
///

int dep_process(
	  ff_node * const ffn
	, strstack * const sstk
	, map * const vmap
	, transform_parser * const gp
	, lwx *** const stax
	, int * const staxa
	, int staxp
	, gn ** const first
	, int * const newn
	, int * const newr
)
{
	// resolve the left-hand side
	int pn = staxp;
	fatal(list_resolveflat, ffn->needs, vmap, gp, stax, staxa, &staxp, 0, 0);

	if(ffn->flags & FFN_SINGLE)
	{
		fatal(dep_add_single, ffn, sstk, vmap, gp, stax, staxa, pn + 1, pn, first, newn, newr);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(dep_add_multi, ffn, sstk, vmap, gp, stax, staxa, pn + 1, pn, first, newn, newr);
	}

	finally : coda;
}
