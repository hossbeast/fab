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

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"

//
// static
//

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

	return gn_depth_traversal_nodes_needsward(r, logic);
}

static int heights(gn * r)
{
	int logic(gn * gn, int d)
	{
		if(gn->height == -1)
		{
			int x;
			for(x = 0; x < gn->needs.l; x++)
			{
				if(!gn->needs.e[x]->weak)
					gn->height = MAX(gn->height, gn->needs.e[x]->B->height);
			}

			gn->height++;
		}

		return 1;
	};

	return gn_depth_traversal_nodes_needsward(r, logic);
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
					int ns = (*a) ? (*a) * 2 + (*a) / 2 : 10;
					fatal(xrealloc, lvs, sizeof(**lvs), ns, *a);
					(*a) = ns;
				}

				(*lvs)[(*l)++] = gn;
			}
		}

		finally : coda;
	};

	return gn_depth_traversal_nodes_needsward(r, logic);
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
			, bps->evals_l + lvsl
			, 0
		);
		fatal(xrealloc
			, &bps->nofmls
			, sizeof(bps->nofmls[0])
			, bps->nofmls_l + lvsl
			, 0
		);
		fatal(xrealloc
			, &bps->primary
			, sizeof(bps->primary[0])
			, bps->primary_l + lvsl
			, 0
		);

		// process nodes found on the last visit
		int evals_cnt = 0;
		int nofmls_cnt = 0;
		int primary_cnt = 0;
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
					for(i = 0; i < lvs[y]->fabv->products_l; i++)
						mheight = MAX(mheight, lvs[y]->fabv->products[i]->height);

					if(mheight == k)
					{
						for(i = 0; i < lvs[y]->fabv->products_l; i++)
							lvs[y]->fabv->products[i]->stage = k;

						for(i = 0; i < (bps->evals_l + evals_cnt); i++)
						{
							if(bps->evals[i] == lvs[y]->fabv)
								break;
						}

						// add this eval context to the stage
						if(i == (bps->evals_l + evals_cnt))
							bps->evals[bps->evals_l + (evals_cnt++)] = lvs[y]->fabv;
					}
				}
				else if(strcmp("/..", lvs[y]->path->abs_dir) == 0)
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
					bps->nofmls[bps->nofmls_l + (nofmls_cnt++)] = lvs[y];
					lvs[y]->stage = k;
				}
				else
				{
					// this is a source file
					bps->primary[bps->primary_l + (primary_cnt++)] = lvs[y];
					lvs[y]->stage = k;
				}
			}
		}

		bps->evals_l += evals_cnt;
		bps->nofmls_l += nofmls_cnt;
		bps->primary_l += primary_cnt;
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

			// check if file has changed
			fatal(gn_primary_reload, gn);

			if(gn->hb->stathash[1] == 0)		// file does not exist
			{
				// PRIMARY file - not found
				log(L_ERROR, "[%2d,%2d] %-9s file %s not found", x, y, "PRIMARY", gn->idstring);

				// poison and propagate
				(*poison) = 1;
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->A->poison = 1;
			}
			else if(hashblock_cmp(gn->hb))
			{
				gn->changed = 1;
			}

			if(gn->changed)
			{
				// propagate through strong dependencies
				for(i = 0; i < gn->feeds.l; i++)
				{
					if(!gn->feeds.e[i]->weak)
						gn->feeds.e[i]->A->rebuild = 1;
				}
			}

			log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
				, x, c++
				, "PRIMARY"
				, gn->idstring
				, ""
				, gn->changed ? "  changed" : "unchanged"
			);
		}

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			// whether NONE of the products of this eval context require rebuilding
			int keep = 0;

			for(k = 0; k < bp->stages[x].evals[y]->products_l; k++)
			{
				// TASK, GENERATED, and SECONDARY nodes
				gn * gn = bp->stages[x].evals[y]->products[k];

				if(!gn->poison)
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
						fatal(gn_secondary_exists, gn);

						if(gn->exists == 0)
						{
							gn->rebuild = 1;	// file doesn't exist
						}
						else
						{
							// rebuild if the ff containing the formula has changed
							if((gn->changed = hashblock_cmp(gn->fabv->fml->ffn->loc.ff->hb)))
								gn->rebuild = 1;
						}
					}

					// needs rebuilt
					if(gn->rebuild)
					{
						// propagate to strong dependencies
						for(i = 0; i < gn->feeds.l; i++)
						{
							if(!gn->feeds.e[i]->weak)
								gn->feeds.e[i]->A->rebuild = 1;
						}

						keep++;
					}
				}

				// propagate the poison
				if(gn->poison)
				{
					for(i = 0; i < gn->feeds.l; i++)
						gn->feeds.e[i]->A->poison = 1;
				}
			}

			if(!keep)
			{
				for(k = 0; k < bp->stages[x].evals[y]->products_l; k++)
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
				for(k = 0; k < bp->stages[x].evals[y]->products_l; k++)
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
								, !gn->exists ? "does not exist" : gn->changed ? "fabfile changed" : "sources changed"
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

int bp_exec(bp * bp, map * vmap, lstack *** stax, int * staxa, int p, ts *** ts, int * tsa, int * tsw)
{
	int tsl				= 0;		// thread count
	int tot				= 0;		// total targets
	int y;
	int x;
	int i;
	int k;

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
		int bad = 0;
		i = 0;
		log(L_BP | L_BPINFO, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			(*ts)[i]->fmlv = bp->stages[x].evals[y];
			(*ts)[i]->y = y;

			// prepare lstack(s) for variables resident in this context
			int pn = p;
			if((*staxa) <= pn)
			{
				fatal(xrealloc, stax, sizeof(**stax), pn + 1, (*staxa));
				(*staxa) = pn + 1;
			}
			if(!(*stax)[pn])
				fatal(lstack_create, &(*stax)[pn]);
			lstack_reset((*stax)[pn]);

			// @ is a list of expected products of this eval context
			for(k = 0; k < (*ts)[i]->fmlv->products_l; k++)
				fatal(lstack_obj_add, (*stax)[pn], (*ts)[i]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			fatal(var_set, vmap, "@", (*stax)[pn++], 0);

			// render the formula
			fatal(fml_render, (*ts)[i], vmap, stax, staxa, pn);

			i++;
		}

		// execute all formulas in parallel processes
		int res = 0;
		fatal(ts_execwave, *ts, i, tsw, x, L_BP | L_BPEXEC, L_FAB, &res);

		if(!res)
			qfail();

		// harvest the results
		for(y = 0; y < i; y++)
		{
			// SUCCESS if - exit status 0, not killed by signal, and wrote nothing to stderr
			if((*ts)[y]->r_status == 0 && (*ts)[y]->r_signal == 0 && (*ts)[y]->stde_txt->l == 0)
			{
				int q;
				for(q = 0; q < (*ts)[y]->fmlv->products_l; q++)
				{
					gn * prod = (*ts)[y]->fmlv->products[q];

					prod->fab_success = 1;

					// check each of my PRIMARY dependencies - if all nodes which it feeds
					// have now been fabricated, we are done with it - update its hashfile

					for(k = 0; k < prod->needs.l; k++)
					{
						gn * pri = prod->needs.e[k]->B;

						if(pri->designation == GN_DESIGNATION_PRIMARY)
						{
							int j;
							for(j = 0; j < pri->feeds.l; j++)
							{
								if(!pri->feeds.e[j]->A->fab_success)
									break;
							}

							if(j == pri->feeds.l)
								fatal(gn_primary_rewrite, pri);
						}
					}
				}
			}
			else
			{
				bad = 1;
			}
		}

		if(bad)
			return 0;
	}

	finally : coda;
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
			for(i = 0; i < bp->stages[x].evals[y]->products_l; i++)
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
			for(q = 0; q < bp->stages[x].evals[y]->products_l; q++)
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
