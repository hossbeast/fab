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

#include <listwise/lstack.h>
#include <listwise/object.h>

#include "dep.h"

#include "args.h"
#include "gnlw.h"
#include "fml.h"
#include "bp.h"
#include "log.h"
#include "map.h"
#include "var.h"
#include "list.h"
#include "lwutil.h"

#include "control.h"
#include "xmem.h"

#define restrict __restrict

///
/// static
///

static int dep_add_single(
	  ff_node * ffn
	, strstack * const restrict sstk
	, map * vmap
	, generator_parser * const gp
	, lstack *** stax
	, int * staxa
	, int staxp
	, int pl
	, gn ** first
	, int * newnp
	, int * newrp
	, depblock * const block
)
{
	int i;
	int j;

	int pr = staxp;

	// resolve the right-hand side
	fatal(list_resolveflat, ffn->feeds, vmap, gp, stax, staxa, &staxp, 0);

	// add edges, which are the cartesian product needs x feeds
	LSTACK_ITERATE((*stax)[pl], i, goa);
	if(goa)
	{
		void * A = 0;
		int Al = 0;
		int At = 0;
		if((*stax)[pl]->s[0].s[i].type)
		{
			A = *(void**)(*stax)[pl]->s[0].s[i].s;
			At = LISTWISE_TYPE_GNLW;
		}
		else
		{
			A = (*stax)[pl]->s[0].s[i].s;
			Al = (*stax)[pl]->s[0].s[i].l;
		}

		LSTACK_ITERATE((*stax)[pr], j, gob);
		if(gob)
		{
			void * B = 0;
			int Bl = 0;
			int Bt = 0;
			if((*stax)[pr]->s[0].s[j].type)
			{
				B = *(void**)(*stax)[pr]->s[0].s[j].s;
				Bt = LISTWISE_TYPE_GNLW;
			}
			else
			{
				B = (*stax)[pr]->s[0].s[j].s;
				Bl = (*stax)[pr]->s[0].s[j].l;
			}

			int newa = 0;
			int newb = 0;
			int newr = 0;

			if(ffn->loc.ff->type == FFT_DDISC)
			{
				// dependencies arising from an FFN_DEPENDENCY node in a DDISC fabfile yield paths which
				// are specified relative to base path of the DDISC node itself
				fatal(gn_edge_add
					, g_args.init_fabfile_path->abs_dir
					, sstk
					, &A
					, Al
					, At
					, &B
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
					, g_args.init_fabfile_path->abs_dir
					, sstk
					, &A
					, Al
					, At
					, &B
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

				// update affected lists
				fatal(ff_regular_enclose_gn, ffn->loc.ff, A);
				fatal(ff_regular_enclose_gn, ffn->loc.ff, B);
			}

			// if A was a string, gn_edge_add has just made it a gn*
			At = 1;

			if(block && block->block)
			{
				// attempt to add the relation to the block
				if(depblock_addrelation(block, ((gn*)A)->path, ((gn*)B)->path, ffn->flags & FFN_WEAK, ffn->flags & FFN_BRIDGE) == 0)
				{
					log(L_WARN, "unable to cache discovery %s", ff_idstring(ffn->loc.ff));
					xfree(&block->block);
				}
			}

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
				*first = A;
				first = 0;
			}

			uint64_t tag = L_DG | L_DGDEPS;
			if(ffn->loc.ff->type == FFT_DDISC)
			{
				if((newnp && (newa || newb)) || (newrp && newr))
					tag |= L_DSCNEW;
			}

			if(newr)
			{
				log(tag, "[%1s][%1s][%1s][%1s](%s)[%3d,%3d - %3d,%3d] %s -> %s"
					, "S"
					, newa ? "x" : ""
					, newb ? "x" : ""
					, newr ? "x" : ""
					, ff_idstring(ffn->loc.ff)
					, ffn->loc.f_lin + 1
					, ffn->loc.f_col + 1
					, ffn->loc.l_lin + 1
					, ffn->loc.l_col + 1
					, ((gn*)A)->idstring
					, ((gn*)B)->idstring
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
	, generator_parser * const gp
	, lstack *** stax
	, int * staxa
	, int staxp
	, int pl
	, gn ** first
	, int * newnp
	, int * newrp
	, depblock * const block
)
{
	// newa tracks whether the left-hand-side of a dependency references a newly-created node
	int * newa = 0;
	size_t newaa = 0;
	size_t newal = 0;

	int pli = staxp;			// left hand side instance
	int pr = staxp + 1;		// right hand side

	int x;
	for(x = 0; x < (*stax)[pl]->l; x++)
	{
		// prepare space for new variable definition list
		fatal(lw_reset, stax, staxa, pli);

		// populate the "<" variable (left-hand side)
		newal = (*stax)[pl]->s[x].l;
		if(newaa < newal)
		{
			newaa = newal;
			newa = alloca(sizeof(*newa) * newaa);
		}
		memset(newa, 0, sizeof(*newa) * newal);
		
		int i;
		int j;
		for(i = 0; i < (*stax)[pl]->s[x].l; i++)
		{
			gn * r = 0;
			if((*stax)[pl]->s[x].s[i].type)
			{
				r = *(void**)(*stax)[pl]->s[x].s[i].s;
			}
			else
			{
				fatal(gn_add
					, g_args.init_fabfile_path->abs_dir
					, sstk
					, (*stax)[pl]->s[x].s[i].s
					, (*stax)[pl]->s[x].s[i].l
					, &r
					, &newa[i]
				);
			}

			fatal(lstack_obj_add, (*stax)[pli], r, LISTWISE_TYPE_GNLW);
		}

		// resolve the right-hand side in the context of $<
		fatal(var_set, vmap, "<", (*stax)[pli], 0, 1, 0);
		int pn = pr;
		fatal(list_resolveflat, ffn->feeds, vmap, gp, stax, staxa, &pn, 0);

		for(i = 0; i < (*stax)[pl]->s[x].l; i++)
		{
			void * A = 0;
			int Al = 0;
			int At = 0;
			if((*stax)[pl]->s[x].s[i].type)
			{
				A = *(void**)(*stax)[pl]->s[x].s[i].s;
				At = LISTWISE_TYPE_GNLW;
			}
			else
			{
				A = (*stax)[pl]->s[x].s[i].s;
				Al = (*stax)[pl]->s[x].s[i].l;
			}

			LSTACK_ITERATE((*stax)[pr], j, gob);
			if(gob)
			{
				void * B = 0;
				int Bl = 0;
				int Bt = 0;
				if((*stax)[pr]->s[0].s[j].type)
				{
					B = *(void**)(*stax)[pr]->s[0].s[j].s;
					Bt = LISTWISE_TYPE_GNLW;
				}
				else
				{
					B = (*stax)[pr]->s[0].s[j].s;
					Bl = (*stax)[pr]->s[0].s[j].l;
				}

				int newb = 0;
				int newr = 0;

				if(ffn->loc.ff->type == FFT_DDISC)
				{
					fatal(gn_edge_add
						, g_args.init_fabfile_path->abs_dir
						, sstk
						, &A
						, Al
						, At
						, &B
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
						, g_args.init_fabfile_path->abs_dir
						, sstk
						, &A
						, Al
						, At
						, &B
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

					// update affected lists
					fatal(ff_regular_enclose_gn, ffn->loc.ff, A);
					fatal(ff_regular_enclose_gn, ffn->loc.ff, B);
				}

				// if A was a string, gn_edge_add has just made it a gn*
				At = 1;

				if(block && block->block)
				{
					// attempt to add the relation to the block
					if(depblock_addrelation(block, ((gn*)A)->path, ((gn*)B)->path, ffn->flags & FFN_WEAK, ffn->flags & FFN_BRIDGE) == 0)
					{
						log(L_WARN, "unable to cache discovery %s", ff_idstring(ffn->loc.ff));
						xfree(&block->block);
					}
				}

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
					*first = A;
					first = 0;
				}

				uint64_t tag = L_DG | L_DGDEPS;
				if(ffn->loc.ff->type == FFT_DDISC)
				{
					if((newnp && (newa[i] || newb)) || (newrp && newr))
						tag |= L_DSCNEW;
				}

				if(newr)
				{
					log(tag, "[%1s][%1s][%1s][%1s](%s)[%3d,%3d - %3d,%3d] %s -> %s"
						, "M"
						, newa[i] ? "x" : ""
						, newb ? "x" : ""
						, newr ? "x" : ""
						, ff_idstring(ffn->loc.ff)
						, ffn->loc.f_lin + 1
						, ffn->loc.f_col + 1
						, ffn->loc.l_lin + 1
						, ffn->loc.l_col + 1
						, ((gn*)A)->idstring
						, ((gn*)B)->idstring
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
	, generator_parser * const gp
	, lstack *** const stax
	, int * const staxa
	, int staxp
	, gn ** const first
	, int * const newn
	, int * const newr
	, depblock * const block
)
{
	// resolve the left-hand side
	int pn = staxp;
	fatal(list_resolveflat, ffn->needs, vmap, gp, stax, staxa, &staxp, 0);

	if(ffn->flags & FFN_SINGLE)
	{
		fatal(dep_add_single, ffn, sstk, vmap, gp, stax, staxa, pn + 1, pn, first, newn, newr, block);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(dep_add_multi, ffn, sstk, vmap, gp, stax, staxa, pn + 1, pn, first, newn, newr, block);
	}

	finally : coda;
}

