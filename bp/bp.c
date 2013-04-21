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

#include <stdio.h>

#include <listwise.h>
#include <listwise/lstack.h>

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "gn.h"
#include "gnlw.h"
#include "args.h"
#include "var.h"
#include "traverse.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
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

static int reset(gn * r)
{
	int logic(gn * gn, int d)
	{
		gn->height = -1;
		gn->stage = -1;

		return 1;
	};

	return traverse_depth_bynodes_needsward_useweak(r, logic);
}

static int heights(gn * r)
{
	int logic(gn * gn, int d)
	{
		int x;
		for(x = 0; x < gn->needs.l; x++)
		{
			if(gn->needs.e[x]->weak)
				gn->height = MAX(gn->height, gn->needs.e[x]->B->height - 1);
			else
				gn->height = MAX(gn->height, gn->needs.e[x]->B->height);
		}

		gn->height++;

		return 1;
	};

	return traverse_depth_bynodes_needsward_useweak(r, logic);
}

static int visit(gn * r, int k, gn *** lvs, int * l, int * a)
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

	return traverse_depth_bynodes_needsward_useweak(r, logic);
}

//
// public
//

int bp_create(gn ** n, int l, bp ** bp)
{
	gn ** lvs = 0;
	int lvsl = 0;
	int lvsa = 0;

	fatal(xmalloc, bp, sizeof(**bp));

	// reset all depths/heights to -1
	int x;
	for(x = 0; x < l; x++)
		fatal(reset, n[x]);

	// calculate node heights and max height
	int maxheight = -1;
	for(x = 0; x < l; x++)
	{
		fatal(heights, n[x]);
		maxheight = MAX(maxheight, n[x]->height);
	}

	// allocate stages in the buildplan and assign nodes
	int k;
	for(k = 0; k <= maxheight; k++)
	{
		// get list of nodes for this stage - k is stage number
		lvsl = 0;
		for(x = 0; x < l; x++)
			fatal(visit, n[x], k, &lvs, &lvsl, &lvsa);

		if((*bp)->stages_l <= k)
		{
			// allocate a new stage
			fatal(xrealloc, &(*bp)->stages, sizeof((*bp)->stages[0]), (*bp)->stages_l + 1, (*bp)->stages_l);
			(*bp)->stages_l++;
		}

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

					if(mheight == k)
					{
						for(i = 0; i < lvs[y]->fabv->productsl; i++)
							lvs[y]->fabv->products[i]->stage = k;

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
					log(L_WARN, "SECONDARY has no formula - %s", lvs[y]->idstring);
					bps->nofmls[bps->nofmls_l++] = lvs[y];
					lvs[y]->stage = k;
				}
				else
				{
					// this is a source file
					bps->primary[bps->primary_l++] = lvs[y];
					lvs[y]->stage = k;
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

finally:
	free(lvs);
coda;
}

int bp_eval(bp * const bp, int * const poison)
{
	int x;
	int y;
	int i;
	int k;

	// begin with an assumption of a good build
	(*poison) = 0;

	// process invalidations, also update node designations
	gn_invalidations();

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
				log(L_ERROR, "[%2d,%2d] %-9s file %s not found", x, y, "PRIMARY", gn->idstring);

				// poison and propagate
				(*poison) = 1;
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->A->poison = 1;
			}

			if(gn->changed || gn->invalid || gn->weak_invalid)
			{
				// change propagation
				for(i = 0; i < gn->feeds.l; i++)
				{
					if(gn->feeds.e[i]->weak)
					{
						gn->feeds.e[i]->A->weak_invalid = 1;
					}
					else if(gn->feeds.e[i]->bridge)
					{
						// no-op
					}
					else
					{
//					gn->feeds.e[i]->A->rebuild = 1;
						gn->feeds.e[i]->A->invalid = 1;
					}
				}
			}

			log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
				, x, c++
				, "PRIMARY"
				, gn->idstring
				, ""
				,   gn->invalid ?               "  invalid"
					: gn->changed ?               "  changed"
					:                             "unchanged"
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
				else
				{
					if(gn->designation == GN_DESIGNATION_TASK)
					{
						// TASK node - must be fabricated every time
						gn->rebuild = 1;
					}
					else if(gn->designation == GN_DESIGNATION_GENERATED)
					{
						// GENERATED file - must be fabricated every time
						gn->rebuild = 1;
					}
					else if(gn->designation == GN_DESIGNATION_SECONDARY)
					{
						// SECONDARY file
						fatal(gn_secondary_reload, gn);

						if(gn->fab_exists == 0)
						{
							gn->rebuild = 1;	// file doesnt exist
						}
						else if(gn->fab_force_ff)
						{
							gn->rebuild = 1;	// rebuild is forced (ff change)
						}
						else if(gn->fab_force_gn)
						{
							gn->rebuild = 1;	// rebuild is forced (source file change)
						}
						else if(gn->invalid)
						{
							gn->rebuild = 1;	// node has been invalidated
						}
					}

					if(gn->rebuild || gn->invalid || gn->weak_invalid)
					{
						// change propagation
						for(i = 0; i < gn->feeds.l; i++)
						{
							if(gn->feeds.e[i]->weak)
							{
								gn->feeds.e[i]->A->weak_invalid = 1;
							}
							else if(gn->feeds.e[i]->bridge)
							{
								// no-op
							}
							else
							{
		//					gn->feeds.e[i]->A->rebuild = 1;
								gn->feeds.e[i]->A->invalid = 1;
							}
						}
					}

					// needs rebuilt
					if(gn->rebuild)
					{
						keep++;
					}
				}
			}

			if(!keep)
			{
				for(k = 0; k < bp->stages[x].evals[y]->productsl; k++)
				{
					gn * gn = bp->stages[x].evals[y]->products[k];

					if(!gn->poison)
					{
						if(gn->needs.l)
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s"
								, x, c++
								, gn_designate(gn)
								, gn->idstring
								, "SKIP"
							);
						}
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

					if(gn->rebuild)
					{
						if(gn->flags & GN_FLAGS_NOFILE)
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s"
								, x, c++
								, gn_designate(gn)
								, gn->idstring
								, "EXECUTE"
							);
						}
						else if(gn->flags & GN_FLAGS_HASNEED)
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
								, x, c++
								, gn_designate(gn)
								, gn->idstring
								, "REBUILD"
								,   gn->invalid      		? "invalidated"
								  : !gn->fab_exists  		? "does not exist"
									: gn->fab_force_ff	  ? "fabfile changed"
									: gn->fab_force_gn		? "sources changed"	// not useful to distinguish between these two cases
									:                    		"sources changed"
							);
						}
						else
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
								, x, c++
								, gn_designate(gn)
								, gn->idstring
								, "REBUILD"
								, "always fab"
							);
						}
					}
					else
					{
						log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
							, x, c++
							, gn_designate(gn)
							, gn->idstring
							, "REBUILD"
							, "eval context product"
						);
					}
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
				(*poison) = 1;
			}

			// propagate the poison
			if(gn->poison)
			{
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->A->poison = 1;
			}

			if(gn->rebuild)
			{
				log(L_ERROR | L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
					, x, c++
					, gn_designate(gn)
					, gn->idstring
					, ""
					, "no formula"
				);
			}
			else
			{
				log(L_WARN | L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
					, x, c++
					, gn_designate(gn)
					, gn->idstring
					, ""
					, "no formula"
				);
			}
		}
	}

	// consolidate stages
	if(!(*poison))
	{
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
	}

	finally : coda;
}

int bp_exec(bp * bp, map * vmap, generator_parser * const gp, lstack *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
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
		log(L_BP | L_BPINFO, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			(*ts)[i]->fmlv = bp->stages[x].evals[y];
			(*ts)[i]->y = y;

			// prepare lstack(s) for variables resident in this context
			fatal(lw_reset, stax, staxa, staxp);

			// @ is a list of expected products of this eval context
			for(k = 0; k < (*ts)[i]->fmlv->productsl; k++)
				fatal(lstack_obj_add, (*stax)[staxp], (*ts)[i]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			// render the formula
			//  note that serialization in this loop is important, because fmlv's may share the same bag
			fatal(map_set, (*ts)[i]->fmlv->bag, MMS("@"), MM((*stax)[staxp]));
			fatal(fml_render, (*ts)[i], gp, stax, staxa, staxp + 1, 1);
			fatal(map_delete, (*ts)[i]->fmlv->bag, MMS("@"));

			i++;
		}

		// execute all formulas in parallel processes - res is true if all formulas
		// executed successfully
		int res = 0;
		fatal(ts_execwave, *ts, i, tsw, x, L_BP | L_BPEXEC, L_FAB, &res);

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
					// secondary rewrite
					if((*ts)[y]->fmlv->products[q]->designation == GN_DESIGNATION_SECONDARY)
					{
						fatal(gn_secondary_rewrite_fab, (*ts)[y]->fmlv->products[q], ws);
					}
				}
			}
		}

		if(!res)
		{
			qfail();
		}
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
		log(L_BP | L_BPDUMP, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			int i;
			for(i = 0; i < bp->stages[x].evals[y]->productsl; i++)
			{
				if(i)
				{
					log(L_BP | L_BPDUMP, "        %-9s %s"
						, gn_designate(bp->stages[x].evals[y]->products[i])
						, bp->stages[x].evals[y]->products[i]->idstring
					);
				}
				else
				{
					log(L_BP | L_BPDUMP, "[%2d,%2d] %-9s %s"
						, x
						, y
						, gn_designate(bp->stages[x].evals[y]->products[i])
						, bp->stages[x].evals[y]->products[i]->idstring
					);
				}
			}
		}
	}
}

int bp_flatten(bp * bp, gn *** gns, int * gnl, int * gna)
{
	int add(gn * gn)
	{
		if(*gnl == *gna)
		{
			int ns = 10;
			if(*gna)
				ns = (*gna) * 2 + (*gna) / 2;

			fatal(xrealloc, gns, sizeof(**gns), ns, *gna);
			*gna = ns;
		}
	
		(*gns)[(*gnl)++] = gn;

		finally : coda;
	};

	*gnl = 0;

	int x;
	for(x = 0; x < bp->stages_l; x++)
	{
		int y;
		for(y = 0; y < bp->stages[x].primary_l; y++)
			fatal(add, bp->stages[x].primary[y]);

		for(y = 0; y < bp->stages[x].nofmls_l; y++)
			fatal(add, bp->stages[x].nofmls[y]);

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			int q;
			for(q = 0; q < bp->stages[x].evals[y]->productsl; q++)
				fatal(add, bp->stages[x].evals[y]->products[q]);
		}
	}

	finally : coda;
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

