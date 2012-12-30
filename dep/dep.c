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

///
/// static
///

static int addrelation(dep_relations_set * set, gn * A, gn * B)
{
	if(set->needsl == 0)
	{
		if(snprintf(set->needs[0], sizeof(set->needs[0]), "%s", A->path) >= sizeof(set->needs[0]))
		{
			return 0; // path too long
		}
		else
		{
			set->needsl++;
		}
	}
	else if(strcmp(set->needs[0], A->path))
	{
		return 0; // needs[0] is some other file
	}

	if(set->feedsl < sizeof(set->feeds) / sizeof(set->feeds[0]))
	{
		if(snprintf(set->feeds[set->feedsl], sizeof(set->feeds[0]), "%s", B->path) >= sizeof(set->feeds[0]))
		{
			return 0;	// path too long
		}
		else
		{
			set->feedsl++;
		}
	}
	else
	{
		return 0;	// too many feeds
	}

	return 1;
}

static int dep_add_single(
	  ff_node * ffn
	, map * vmap
	, lstack *** stax
	, int * stax_l
	, int * stax_a
	, int pl
	, int p
	, gn ** first
	, int * newnp
	, int * newrp
	, depblock * const block
)
{
	int i;
	int j;

	int pr = p;

	// resolve the right-hand side
	fatal(list_resolve, ffn->feeds, vmap, stax, stax_l, stax_a, pr);

	// add edges, which are the cartesian product needs x feeds
	LSTACK_ITERATE((*stax)[pl], i, goa);
	if(goa)
	{
		LSTACK_ITERATE((*stax)[pr], j, gob);
		if(gob)
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
				fatal(gn_edge_add, g_args.fabfile_canon_dir, &A, Al, At, &B, Bl, Bt, ffn, 0, ffn->flags & FFN_WEAK, &newa, &newb, &newr);
			else
				fatal(gn_edge_add, ffn->loc.ff->dir        , &A, Al, At, &B, Bl, Bt, ffn, 0, ffn->flags & FFN_WEAK, &newa, &newb, &newr);

			if(block && block->block)
			{
				// attempt to add the relation to the block
				if(addrelation(ffn->flags & FFN_WEAK ? &block->block->weak : &block->block->strong, ((gn*)A), ((gn*)B)) == 0)
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
				, gn_idstring((gn*)A)
				, gn_idstring((gn*)B)
			);
		}
		LSTACK_ITEREND;
	}
	LSTACK_ITEREND;

	return 1;
}

static int dep_add_multi(
	  ff_node * ffn
	, map * vmap
	, lstack *** stax
	, int * stax_l
	, int * stax_a
	, int pl
	, int p
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
		int pr = p;

		// prepare space for new variable definition list
		if((*stax_a) <= pr)
		{
			fatal(xrealloc, &(*stax), sizeof(*(*stax)), pr + 1, (*stax_a));
			(*stax_a) = pr + 1;
		}
		if(!(*stax)[pr])
			fatal(xmalloc, &(*stax)[pr], sizeof(*(*stax)[pr]));
		lstack_reset((*stax)[pr]);

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
				fatal(gn_add, ffn->loc.ff->dir, (*stax)[pl]->s[x].s[i].s, (*stax)[pl]->s[x].s[i].l, &r, &newa[i]);
			}

			fatal(lstack_obj_add, (*stax)[pr], r, LISTWISE_TYPE_GNLW);
		}
		fatal(var_set, vmap, "<", (*stax)[pr++]);

		// resolve the right-hand side
		fatal(list_resolve, ffn->feeds, vmap, &(*stax), stax_l, stax_a, pr);

		for(i = 0; i < (*stax)[pl]->s[x].l; i++)
		{
			LSTACK_ITERATE((*stax)[pr], j, gob);
			if(gob)
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
					fatal(gn_edge_add, g_args.fabfile_canon_dir, &A, Al, At, &B, Bl, Bt, ffn, 0, ffn->flags & FFN_WEAK, 0, &newb, &newr);
				else
					fatal(gn_edge_add, ffn->loc.ff->dir        , &A, Al, At, &B, Bl, Bt, ffn, 0, ffn->flags & FFN_WEAK, 0, &newb, &newr);

				if(block && block->block)
				{
					// attempt to add the relation to the block
					if(addrelation(ffn->flags & FFN_WEAK ? &block->block->weak : &block->block->strong, ((gn*)A), ((gn*)B)) == 0)
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
					, gn_idstring((gn*)A)
					, gn_idstring((gn*)B)
				);
			}
			LSTACK_ITEREND;
		}
	}

	return 1;
}

///
/// public
///

int dep_process(
	  ff_node * const ffn
	, map * const vmap
	, lstack *** const stax
	, int * const staxl
	, int * const staxa
	, int p
	, gn ** const first
	, int * const newn
	, int * const newr
	, depblock * const block
)
{
	// resolve the left-hand side
	fatal(list_resolve, ffn->needs, vmap, stax, staxl, staxa, p);

	if(ffn->flags & FFN_SINGLE)
	{
		fatal(dep_add_single, ffn, vmap, stax, staxl, staxa, p, p + 1, first, newn, newr, block);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(dep_add_multi, ffn, vmap, stax, staxl, staxa, p, p + 1, first, newn, newr, block);
	}
	else
	{
		fail("bad flags : %hhu", ffn->flags);
	}

	return 1;
}

int depblock_process(gn * const dscvgn, const depblock * const block, int * const newnp, int * const newrp)
{
	int process(char * _A, char * _B, int isweak)
	{
		void * A = _A;
		int Al = strlen(A);
		int At = 0;

		void * B = _B;
		int Bl = strlen(B);
		int Bt = 0;

		int newa = 0;
		int newb = 0;
		int newr = 0;

		fatal(gn_edge_add, 0, &A, Al, At, &B, Bl, Bt, 0, dscvgn, isweak, &newa, &newb, &newr);

		if(newnp)
		{
			(*newnp) += newa;
			(*newnp) += newb;
		}
		if(newrp)
		{
			(*newrp) += newr;
		}

		log(L_DG | L_DGDEPS | L_DSCNEW, "[%1s][%1s][%1s][%1s](DSC:%s)[%3s,%3s - %3s,%3s] %s -> %s"
			, "S"
			, newa ? "x" : ""
			, newb ? "x" : ""
			, newr ? "x" : ""
			, gn_idstring(dscvgn)
			, "-", "-", "-", "-"
			, gn_idstring((gn*)A)
			, gn_idstring((gn*)B)
		);

		return 1;
	};

	int x;
	int y;
	for(x = 0; x < block->block->weak.needsl; x++)
	{
		for(y = 0; y < block->block->weak.feedsl; y++)
		{
			fatal(process, block->block->weak.needs[x], block->block->weak.feeds[y], 1);
		}
	}

	for(x = 0; x < block->block->strong.needsl; x++)
	{
		for(y = 0; y < block->block->strong.feedsl; y++)
		{
			fatal(process, block->block->strong.needs[x], block->block->strong.feeds[y], 0);
		}
	}

	return 1;
}
