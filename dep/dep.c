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
	fatal(list_resolve, ffn->feeds, vmap, stax, staxa, pr, 0);

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
//					, ffn->loc.ff->dscv_gn->path->base
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
//					, ffn->loc.ff->path->abs_dir
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
				if(depblock_addrelation(block, ((gn*)A)->path, ((gn*)B)->path, ffn->flags & FFN_WEAK) == 0)
				{
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

	int x;
	for(x = 0; x < (*stax)[pl]->l; x++)
	{
		// prepare space for new variable definition list
		fatal(list_ensure, stax, staxa, staxp);

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
//					, ffn->loc.ff->path->abs_dir
					, g_args.init_fabfile_path->abs_dir
					, sstk
					, (*stax)[pl]->s[x].s[i].s
					, (*stax)[pl]->s[x].s[i].l
					, &r
					, &newa[i]
				);
			}

			fatal(lstack_obj_add, (*stax)[staxp], r, LISTWISE_TYPE_GNLW);
		}

		// resolve the right-hand side in the context of $<
		fatal(var_push_list, vmap, "<", 0, (*stax)[staxp], 0);
		fatal(list_resolve, ffn->feeds, vmap, stax, staxa, staxp + 1, 0);
		fatal(var_pop, vmap, "<", 0);

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

			LSTACK_ITERATE((*stax)[staxp + 1], j, gob);
			if(gob)
			{
				void * B = 0;
				int Bl = 0;
				int Bt = 0;
				if((*stax)[staxp + 1]->s[0].s[j].type)
				{
					B = *(void**)(*stax)[staxp + 1]->s[0].s[j].s;
					Bt = LISTWISE_TYPE_GNLW;
				}
				else
				{
					B = (*stax)[staxp + 1]->s[0].s[j].s;
					Bl = (*stax)[staxp + 1]->s[0].s[j].l;
				}

				int newb = 0;
				int newr = 0;

				if(ffn->loc.ff->type == FFT_DDISC)
				{
					fatal(gn_edge_add
						, ffn->loc.ff->dscv_gn->path->abs_dir
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
						, 0
						, &newb
						, &newr
					);
				}
				else
				{
					fatal(gn_edge_add
//						, ffn->loc.ff->path->abs_dir
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
					if(depblock_addrelation(block, ((gn*)A)->path, ((gn*)B)->path, ffn->flags & FFN_WEAK) == 0)
					{
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
	fatal(list_resolve, ffn->needs, vmap, stax, staxa, staxp, 0);

	if(ffn->flags & FFN_SINGLE)
	{
		fatal(dep_add_single, ffn, sstk, vmap, stax, staxa, staxp + 1, staxp, first, newn, newr, block);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(dep_add_multi, ffn, sstk, vmap, stax, staxa, staxp + 1, staxp, first, newn, newr, block);
	}
	else
	{
		fail("bad flags : %hhu", ffn->flags);
	}

	finally : coda;
}
