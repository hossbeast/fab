/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include <stdio.h>
#include <string.h>

#include "listwise.h"

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "gn.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"
#include "traverse.h"

#include "logs.h"
#include "global.h"
#include "xlinux.h"
#include "macros.h"
#include "list.h"
#include "lwutil.h"
#include "map.h"

//
// static
//

#define restrict __restrict

static void bp_freestage(bp_stage * const restrict bps);

static int gn_cmp(const void * _A, const void * _B)
{
	gn * A = *((gn **)_A);
	gn * B = *((gn **)_B);

	return path_cmp(A->path, B->path);
}

static int fmleval_cmp(const void * _A, const void * _B)
{
	fmleval * A = *((fmleval **)_A);
	fmleval * B = *((fmleval **)_B);

	return gn_cmp(&A->products[0], &B->products[0]);
}

static int reset(gn * r, int exact, int nofile)
{
	int logic(gn * gn, int d)
	{
		gn->height = -1;
		gn->stage = -1;

		finally : coda;
	};

	if(exact)
		fatal(logic, r, 0);
	else if(nofile)
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_usenofile, r, logic);
	else
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_nonofile, r, logic);

	finally : coda;
}

static int heights(gn * r, int exact, int nofile, int * change)
{
	int logic(gn * gn, int d)
	{
		int height = 0;

		int x;
		for(x = 0; x < gn->needs.l; x++)
		{
			if(gn->needs.e[x]->weak)
				height = MAX(height, gn->needs.e[x]->B->height - 1);
			else
				height = MAX(height, gn->needs.e[x]->B->height);
		}

		height++;

		if(height > gn->height)
		{
			gn->height = height;
			(*change)++;
		}

		finally : coda;
	};

	if(exact)
		fatal(logic, r, 0);
	else if(nofile)
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_usenofile, r, logic);
	else
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_nonofile, r, logic);

	finally : coda;
}

static int visit(gn * r, int k, gn *** lvs, int * l, int * a, int exact, int nofile)
{
	int logic(gn * gn, int d)
	{
		if(gn->stage == -1)
		{
			if(gn->height == k)
			{
				if((*l) == (*a))
				{
					int ns = (*a) ?: 10;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, lvs, sizeof(**lvs), ns, *a);
					(*a) = ns;
				}

				(*lvs)[(*l)++] = gn;
			}
		}

		finally : coda;
	};

	if(exact)
		fatal(logic, r, 0);
	else if(nofile)
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_usenofile, r, logic);
	else
		fatal(traverse_depth_bynodes_needsward_useweak_usebridge_nonofile, r, logic);

	finally : coda;
}

//
// public
//

int bp_create(
	  gn *** const restrict fabrications
	, int fabricationsl
	, gn *** const restrict fabricationxs
	, int fabricationxsl
	, gn *** const restrict fabricationns
	, int fabricationnsl
	, bp ** const restrict bp
)
{
	gn ** lvs = 0;
	int lvsl = 0;
	int lvsa = 0;

	// reset all depths/heights to -1
	int x;
	for(x = 0; x < fabricationsl; x++)
		fatal(reset, *fabrications[x], 0, 1);

	for(x = 0; x < fabricationxsl; x++)
		fatal(reset, *fabricationxs[x], 1, 0);

	for(x = 0; x < fabricationnsl; x++)
		fatal(reset, *fabricationns[x], 0, 0);

	// calculate node heights and max height
	int maxheight = -1;
	int change = 0;
	for(x = 0; x < fabricationsl + fabricationxsl + fabricationnsl; x++)
	{
		int nowchange = 0;
		if(x < fabricationsl)
		{
			fatal(heights, *fabrications[x], 0, 1, &nowchange);
			maxheight = MAX(maxheight, (*fabrications[x])->height);
		}
		else if(x < (fabricationsl + fabricationxsl))
		{
			fatal(heights, *fabricationxs[x - fabricationsl], 1, 0, &nowchange);
			maxheight = MAX(maxheight, (*fabricationxs[x - fabricationsl])->height);
		}
		else
		{
			fatal(heights, *fabricationns[x - fabricationsl - fabricationxsl], 0, 0, &nowchange);
			maxheight = MAX(maxheight, (*fabricationns[x - fabricationsl - fabricationxsl])->height);
		}

		if(change && nowchange)
		{
			x = -1;
			change = 0;
		}		
		else
		{
			change |= nowchange;
		}
	}

	if(maxheight > -1)
	{
		// allocate stages in the buildplan and assign nodes
		// allocate a new stage
		fatal(xmalloc, bp, sizeof(**bp));
		fatal(xmalloc, &(*bp)->stages, sizeof((*bp)->stages[0]) * maxheight);
		(*bp)->stages_l = maxheight;

		int k;
		for(k = 0; k < maxheight; k++)
		{
			// get list of nodes for this stage - k+1 is stage number
			lvsl = 0;
			for(x = 0; x < fabricationsl; x++)
				fatal(visit, *fabrications[x], k+1, &lvs, &lvsl, &lvsa, 0, 1);

			for(x = 0; x < fabricationxsl; x++)
				fatal(visit, *fabricationxs[x], k+1, &lvs, &lvsl, &lvsa, 1, 0);

			for(x = 0; x < fabricationnsl; x++)
				fatal(visit, *fabricationns[x], k+1, &lvs, &lvsl, &lvsa, 0, 0);

			// ptr to the stage we will add to
			bp_stage * bps = &(*bp)->stages[k];

			// reallocate the stage - the ceiling for these allocations is the number of nodes found
			fatal(xrealloc
				, &bps->evals
				, sizeof(bps->evals[0])
				, lvsl
				, 0
			);
			fatal(xrealloc
				, &bps->nofmls
				, sizeof(bps->nofmls[0])
				, lvsl
				, 0
			);
			fatal(xrealloc
				, &bps->primary
				, sizeof(bps->primary[0])
				, lvsl
				, 0
			);

			// process nodes found on the last visit
			int y;
			for(y = 0; y < lvsl; y++)
			{
				if(lvs[y]->stage == -1)
				{
					if(lvs[y]->fabv)
					{
						// all nodes in an fmlv go into the same stage, the last stage any of them is required to be in
						int mheight = lvs[y]->height;
						int i;
						for(i = 0; i < lvs[y]->fabv->productsl; i++)
							mheight = MAX(mheight, lvs[y]->fabv->products[i]->height);

						if(mheight == k+1)
						{
							for(i = 0; i < lvs[y]->fabv->productsl; i++)
								lvs[y]->fabv->products[i]->stage = k+1;

							for(i = 0; i < bps->evals_l; i++)
							{
								if(bps->evals[i] == lvs[y]->fabv)
									break;
							}

							// add this eval context to the stage
							if(i == bps->evals_l)
								bps->evals[bps->evals_l++] = lvs[y]->fabv;
						}
					}
					else if(lvs[y]->path->canl >= 4 && memcmp(lvs[y]->path->can, "/../", 4) == 0)
					{
						// this is a NOFILE node - no error, but do not add to the stage
					}
					else if(lvs[y]->needs.l)
					{
						// this SECONDARY node is part of the buildplan but is not part of any fml eval context - it cannot
						// be fabricated. we add a dummy node to the plan so that we can defer reporting this error
						// until pruning the buildplan. there are two reasons to do this:
						//  1 - this node may be pruned away and not actually needed after all.
						//  2 - in order to report additional errors before failing out
						//
						logf(L_WARN, "SECONDARY has no formula - %s", lvs[y]->idstring);
						bps->nofmls[bps->nofmls_l++] = lvs[y];
						lvs[y]->stage = k+1;
					}
					else
					{
						// this is a source file
						bps->primary[bps->primary_l++] = lvs[y];
						lvs[y]->stage = k+1;
					}
				}
			}
		}

		// splice out empty stages
		for(x = (*bp)->stages_l - 1; x >= 0; x--)
		{
			if(((*bp)->stages[x].primary_l + (*bp)->stages[x].evals_l + (*bp)->stages[x].nofmls_l) == 0)
			{
				bp_freestage(&(*bp)->stages[x]);

				memmove(
						&(*bp)->stages[x]
					, &(*bp)->stages[x + 1]
					, ((*bp)->stages_l - x - 1) * sizeof((*bp)->stages[0])
				);

				(*bp)->stages_l--;
			}
		}

		// internally sort lists in each stage by name of their first product
		for(x = 0; x < (*bp)->stages_l; x++)
		{
			qsort(
					(*bp)->stages[x].evals
				, (*bp)->stages[x].evals_l
				, sizeof((*bp)->stages[0].evals[0])
				, fmleval_cmp
			);
			qsort(
					(*bp)->stages[x].nofmls
				, (*bp)->stages[x].nofmls_l
				, sizeof((*bp)->stages[0].nofmls[0])
				, gn_cmp
			);
			qsort(
					(*bp)->stages[x].primary
				, (*bp)->stages[x].primary_l
				, sizeof((*bp)->stages[0].primary[0])
				, gn_cmp
			);
		}
	}

finally:
	free(lvs);
coda;
}

int bp_eval(bp * const bp)
{
	int x;
	int y;
	int i;
	int k;

	char space[128];

	// begin with the assumption of a good build
	int poison = 0;

	for(x = 0; x < bp->stages_l; x++)
	{
		int c = 0;
		for(y = 0; y < bp->stages[x].primary_l; y++)
		{
			// source files
			gn * gn = bp->stages[x].primary[y];

			if(gn->primary_hb->stathash[1] == 0)		// file does not exist
			{
				// PRIMARY file - not found
				logf(L_ERROR, "[%2d,%3d] %-9s file %s not found", x, y, "PRIMARY", gn->idstring);

				// poison and propagate
				poison = 1;
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->A->poison = 1;
			}

			logf(L_BP | L_BPEVAL, "[%2d,%3d] %9s %-65s |"
				, x, c++
				, "PRIMARY"
				, gn->idstring
			);
		}

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			// whether NONE of the products of this eval context require rebuilding
			int keep = 0;

			for(k = 0; k < bp->stages[x].evals[y]->productsl; k++)
			{
				// TASK, GENERATED, and SECONDARY nodes
				gn * gn = bp->stages[x].evals[y]->products[k];

				// propagate poison
				if(gn->poison)
				{
					for(i = 0; i < gn->feeds.l; i++)
						gn->feeds.e[i]->A->poison = 1;
				}
				else if(GN_IS_INVALID(gn))
				{
					keep++;
				}
			}

			if(!keep)
			{
				for(k = 0; k < bp->stages[x].evals[y]->productsl; k++)
				{
					gn * gn = bp->stages[x].evals[y]->products[k];

					if(!gn->poison)
					{
						logf(L_BP | L_BPEVAL, "[%2d,%3d] %9s %-65s | %-7s"
							, x, c++
							, gn_designation(gn)
							, gn->idstring
							, "SKIP"
						);
					}
				}

				// no products of this eval context are needed - remove from buildplan
				bp->stages[x].evals[y] = 0;
			}
			else
			{
				for(k = 0; k < bp->stages[x].evals[y]->productsl; k++)
				{
					gn * gn = bp->stages[x].evals[y]->products[k];

					logf(L_BP | L_BPEVAL, "[%2d,%3d] %9s %-65s | %-7s (%s)"
						, x, c++
						, gn_designation(gn)
						, gn->idstring
						, "REBUILD"
						, gn_invalid_reason(space, sizeof(space), gn)
					);
				}
			}
		}

		for(y = 0; y < bp->stages[x].nofmls_l; y++)
		{
			// SECONDARY files which have no formula
			gn * gn = bp->stages[x].nofmls[y];

			if(gn->rebuild)
			{
				// file doesn't exist or has changed, is not a PRIMARY file, and cannot be fabricated
				gn->poison = 1;
				poison = 1;
			}

			// propagate the poison
			if(gn->poison)
			{
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->A->poison = 1;
			}

			if(gn->rebuild)
			{
				logf(L_ERROR | L_BP | L_BPEVAL, "[%2d,%3d] %9s %-65s | %-7s (%s)"
					, x, c++
					, gn_designation(gn)
					, gn->idstring
					, ""
					, "no formula"
				);
			}
			else
			{
				logf(L_WARN | L_BP | L_BPEVAL, "[%2d,%3d] %9s %-65s | %-7s (%s)"
					, x, c++
					, gn_designation(gn)
					, gn->idstring
					, ""
					, "no formula"
				);
			}
		}
	}

	if(poison)
		fails(FAB_UNSATISFIED, "buildplan cannot be executed");

	// consolidate stages
	for(x = bp->stages_l - 1; x >= 0; x--)
	{
		for(y = bp->stages[x].evals_l - 1; y >= 0; y--)
		{
			if(bp->stages[x].evals[y] == 0)
			{
				memmove(
						&bp->stages[x].evals[y]
					, &bp->stages[x].evals[y + 1]
					, (bp->stages[x].evals_l - y - 1) * sizeof(bp->stages[0].evals[0])
				);

				bp->stages[x].evals_l--;
			}
		}

		if(bp->stages[x].evals_l == 0)
		{
			bp_freestage(&bp->stages[x]);

			memmove(
					&bp->stages[x]
				, &bp->stages[x + 1]
				, (bp->stages_l - x - 1) * sizeof(bp->stages[0])
			);

			bp->stages_l--;
		}
	}

	finally : coda;
}

int bp_exec(bp * bp, map * vmap, generator_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
{
	int tsl				= 0;		// thread count
	int tot				= 0;		// total targets
	int y;
	int x;
	int i;
	int k;

	map * ws = 0;

	fatal(map_create, &ws, 0);

	// determine how many threads are needed
	for(x = 0; x < bp->stages_l; x++)
	{
		tsl = MAX(tsl, bp->stages[x].evals_l);
		tot += bp->stages[x].evals_l;
	}

	// ensure I have enough threadspace
	fatal(ts_ensure, ts, tsa, tsl);

	// execute stages
	for(x = 0; x < bp->stages_l; x++)
	{
		i = 0;
		logf(L_BP | L_BPINFO, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			(*ts)[i]->fmlv = bp->stages[x].evals[y];
			(*ts)[i]->y = y;

			// prepare lstack(s) for variables resident in this context
			fatal(lw_reset, stax, staxa, staxp);

			// $@ is a list of expected products of this eval context
			for(k = 0; k < (*ts)[i]->fmlv->productsl; k++)
				fatal(lstack_obj_add, (*stax)[staxp], (*ts)[i]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			// render the formula
			//  note that serialization in this loop is important, because fmlv's may share the same bag
			fatal(map_set, (*ts)[i]->fmlv->bag, MMS("@"), MM((*stax)[staxp]), 0);
			fatal(fml_render, (*ts)[i], gp, stax, staxa, staxp + 1, 0, 1);
			map_delete((*ts)[i]->fmlv->bag, MMS("@"));

			i++;
		}

		// execute all formulas in parallel processes - res is true if all formulas
		// executed successfully
		int bad = 0;
		fatal(ts_execwave, *ts, i, tsw, x, L_BP | L_BPEXEC, L_FAB, &bad);

		// harvest the results
		for(y = 0; y < i; y++)
		{
			// SUCCESS if
			//  pid      - it was executed
			//  r_status - exit status of 0
			//  r_signal - not killed by a signal
			//  std_txt  - wrote nothing to stderr
			if((*ts)[y]->pid && (*ts)[y]->r_status == 0 && (*ts)[y]->r_signal == 0 && (*ts)[y]->stde_txt->l == 0)
			{
				int q;
				for(q = 0; q < (*ts)[y]->fmlv->productsl; q++)
				{
					fatal(gn_reconcile_fab, (*ts)[y]->fmlv->products[q], ws);
				}
			}
		}

		if(bad)
			fail(FAB_FMLFAIL);
	}

finally:
	map_free(ws);
coda;
}

void bp_dump(bp * bp)
{
	int x;
	int y;
	
	int tot = 0;
	for(x = 0; x < bp->stages_l; x++)
	{
		tot += bp->stages[x].evals_l;
	}

	for(x = 0; x < bp->stages_l; x++)
	{
		logf(L_BP | L_BPDUMP, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			int i;
			for(i = 0; i < bp->stages[x].evals[y]->productsl; i++)
			{
				if(i)
				{
					logf(L_BP | L_BPDUMP, "         %-9s %s"
						, gn_designation(bp->stages[x].evals[y]->products[i])
						, bp->stages[x].evals[y]->products[i]->idstring
					);
				}
				else
				{
					logf(L_BP | L_BPDUMP, "[%2d,%3d] %-9s %s"
						, x
						, y
						, gn_designation(bp->stages[x].evals[y]->products[i])
						, bp->stages[x].evals[y]->products[i]->idstring
					);
				}
			}
		}
	}
}

void bp_freestage(bp_stage * const restrict bps)
{
	if(bps)
	{
		free(bps->primary);
		free(bps->evals);
		free(bps->nofmls);
	}
}

void bp_free(bp * const restrict bp)
{
	if(bp)
	{
		int x;
		for(x = 0; x < bp->stages_l; x++)
			bp_freestage(&bp->stages[x]);

		free(bp->stages);
	}

	free(bp);
}

void bp_xfree(bp ** const restrict bp)
{
	bp_free(*bp);
	*bp = 0;
}

