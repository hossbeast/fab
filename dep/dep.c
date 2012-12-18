#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <listwise/lstack.h>
#include <listwise/object.h>

#include "args.h"
#include "ff.h"
#include "gn.h"
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

static int dep_add_single(ff_node * ffn, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int pl, int p, gn ** first, int * newn, int * newr)
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

			fatal(gn_edge_add, ffn->loc.ff->dir, &A, Al, At, &B, Bl, Bt, ffn, &newa, &newb, newr);

			if(newn)
			{
				(*newn) += newa;
				(*newn) += newb;
			}

			if(first)
			{
				*first = A;
				first = 0;
			}

			log(L_DG | L_DGDEPS, "S(%s)[%3d,%3d - %3d,%3d] %s -> %s"
				, ffn->loc.ff->name
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

static int dep_add_multi(ff_node * ffn, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int pl, int p, gn ** first, int * newn, int * newr)
{
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
				fatal(gn_add, ffn->loc.ff->dir, (*stax)[pl]->s[x].s[i].s, (*stax)[pl]->s[x].s[i].l, &r, newn);
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

				int newa = 0;
				int newb = 0;

				fatal(gn_edge_add, ffn->loc.ff->dir, &A, Al, At, &B, Bl, Bt, ffn, &newa, &newb, newr);

				if(newn)
				{
					(*newn) += newa;
					(*newn) += newb;
				}

				if(first)
				{
					*first = A;
					first = 0;
				}

				log(L_DG | L_DGDEPS, "M(%s)[%3d,%3d - %3d,%3d] %s -> %s"
					, ffn->loc.ff->name
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
	  ff_node * const restrict ffn
	, const gn * const restrict defgn
	, map * const restrict vmap
	, lstack *** const restrict stax
	, int * const restrict staxl
	, int * const restrict staxa
	, int p
	, gn ** const restrict first
	, int * const restrict newn
	, int * const restrict newr
)
{
	// resolve the left-hand side
	if(ffn->needs)
	{
		fatal(list_resolve, ffn->needs, vmap, stax, staxl, staxa, p);
	}
	else
	{
		fatal(list_ensure, stax, staxl, staxa, p);
		fatal(lstack_obj_add, (*stax)[p], defgn, LISTWISE_TYPE_GNLW);
	}

	if(ffn->flags & FFN_SINGLE)
	{
		fatal(dep_add_single, ffn, vmap, stax, staxl, staxa, p, p + 1, first, newn, newr);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(dep_add_multi, ffn, vmap, stax, staxl, staxa, p, p + 1, first, newn, newr);
	}
	else
	{
		fail("bad flags : %hhu", ffn->flags);
	}

	return 1;
}
