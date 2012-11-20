#include <stdio.h>

#include "bp.h"
#include "ts.h"
#include "fml.h"
#include "gnlw.h"
#include "args.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"

/*
** BPEVAL graph node designations
**
** GENERATED - has no dependencies, can be fabricated by some formula
** PRIMARY   - has no dependencies, has no formula
** SECONDARY - has dependencies, and can be fabricated by some formula
*/

//
// static
//

static int gn_cmp(const void * _A, const void * _B)
{
	gn * A = *((gn **)_A);
	gn * B = *((gn **)_B);

	return strcmp(A->path, B->path);
}

static int fmleval_cmp(const void * _A, const void * _B)
{
	fmleval * A = *((fmleval **)_A);
	fmleval * B = *((fmleval **)_B);

	return gn_cmp(&A->products[0], &B->products[0]);
}

static void reset(gn * n)
{
	n->mark = 0;

	int x;
	for(x = 0; x < n->needs.l; x++)
		reset(n->needs.e[x]);
}

static int visit(gn * n, gn *** lvs, int * l, int * a)
{
	if(!n->mark)
	{
		// I am a leaf if I have no dependencies which were visited
		int leaf = 1;

		int x;
		for(x = 0; x < n->needs.l; x++)
		{
			if(n->needs.e[x]->mark == 0)
			{
				if(visit(n->needs.e[x], lvs, l, a) == 0)
					return 0;

				leaf = 0;
			}
		}

		if(leaf)
		{
			n->mark = 1;

			if(*l == *a)
			{
				fatal(xrealloc, lvs, sizeof(**lvs), (*a) + 1, *a);
				(*a)++;
			}

			(*lvs)[(*l)++] = n;
		}
	}

	return 1;
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

	// reset tracking
	int x;
	for(x = 0; x < l; x++)
		reset(n[x]);

	// traverse graph from each target node
	for(x = 0; x < l; x++)
	{
		int k;
		for(k = 0; k == 0 || lvsl; k++)
		{
			// visit - get all leaves from this node
			lvsl = 0;
			fatal(visit, n[x], &lvs, &lvsl, &lvsa);

			if(lvsl)
			{
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
					, bps->evals_l
				);
				fatal(xrealloc
					, &bps->nofmls
					, sizeof(bps->nofmls[0])
					, bps->nofmls_l + lvsl
					, bps->nofmls_l
				);
				fatal(xrealloc
					, &bps->primary
					, sizeof(bps->primary[0])
					, bps->primary_l + lvsl
					, bps->primary_l
				);

				// process nodes found on the last visit
				int evals_cnt = 0;
				int nofmls_cnt = 0;
				int primary_cnt = 0;
				int y;
				for(y = 0; y < lvsl; y++)
				{
					if(lvs[y]->mark == 1)
					{
						if(lvs[y]->fmlv)
						{
							// explicitly mark each target of this eval context : it is possible that a single
							// eval context produces more than one of the leaf nodes found, and we want to avoid
							// adding it to this stage twice. marking also excludes it from future stages
							int i;
							for(i = 0; i < lvs[y]->fmlv->products_l; i++)
								lvs[y]->fmlv->products[i]->mark++;

							// add this eval context to the stage
							bps->evals[bps->evals_l + (evals_cnt++)] = lvs[y]->fmlv;
						}
						else if(lvs[y]->needs.l)
						{
							// this node is part of the buildplan but is not part of any fml eval context - it cannot
							// be fabricated. we add a dummy node to the plan so that we can defer reporting this error
							// until pruning the buildplan. there are two reasons to do this:
							//  1 - this node may be pruned away and not actually needed after all.
							//  2 - in order to report additional errors before failing out
							//
							log(L_WARN, "no formula - %s", lvs[y]->path);

							bps->nofmls[bps->nofmls_l + (nofmls_cnt++)] = lvs[y];
						}
						else
						{
							// this is a source file
							bps->primary[bps->primary_l + (primary_cnt++)] = lvs[y];
						}
					}
				}

				bps->evals_l += evals_cnt;
				bps->nofmls_l += nofmls_cnt;
				bps->primary_l += primary_cnt;
			}
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

	return 1;
}

int bp_prune(bp * bp)
{
	int x;
	int y;
	int i;
	int k;

	// process invalidations
	if(g_args.invalidate_all)
	{
		for(x = 0; x < gn_nodes.l; x++)
			gn_nodes.e[x]->changed = 1;
	}
	else
	{
		for(x = 0; x < g_args.invalidate_len; x++)
		{
			gn * gn = 0;
			if((gn = idx_lookup_val(gn_nodes.by_path, (char*[]) { g_args.invalidate[x] }, 0)))
			{
				gn->changed = 1;
			}
		}
	}

	int poisoned = 0;
	for(x = 0; x < bp->stages_l; x++)
	{
		int c = 0;
		for(y = 0; y < bp->stages[x].primary_l; y++)
		{
			// source files
			gn * gn = bp->stages[x].primary[y];

			if(gn->prop_hash[1] == 0)		// file does not exist
			{
				// SOURCE file - not found
				log(L_ERROR, "[%2d,%2d] %-9s file %s not found - feeds %d", x, y, "PRIMARY", gn->path, gn->feeds.l);
				gn->poison = 1;
			}
			else
			{
				gn_hashes_read(gn);

				if(gn_hashes_cmp(gn))		// hashes do not agree; file has changed
					gn->changed = 1;
			}

			// needs rebuilt
			if(gn->changed)
			{
				// : mark all nodes that depend on me as needing rebuilt, too
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->rebuild = 1;
			}

			// propagate the poison
			if(gn->poison)
			{
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->poison = 1;

				poisoned = 1;
			}

			log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
				, x, c++
				, "PRIMARY"
				, gn->path
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
				// GENERATED and SECONDARY files
				gn * gn = bp->stages[x].evals[y]->products[k];

				if(!gn->poison)
				{
					if(gn->needs.l)
					{
						// SECONDARY file
						if(gn->prop_hash[1] == 0)
							gn->rebuild = 1;	// file doesn't exist
					}
					else if(gn->fmlv)
					{
						// GENERATED file - must be fabricated every time
						gn->rebuild = 1;
					}

					// needs rebuilt
					if(gn->rebuild)
					{
						// : mark all nodes that depend on me as needing rebuilt, too
						for(i = 0; i < gn->feeds.l; i++)
							gn->feeds.e[i]->rebuild = 1;

						keep++;
					}
				}

				// propagate the poison
				if(gn->poison)
				{
					for(i = 0; i < gn->feeds.l; i++)
						gn->feeds.e[i]->poison = 1;
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
								, "SECONDARY"
								, gn->path
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
						if(gn->needs.l)
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
								, x, c++
								, "SECONDARY"
								, gn->path
								, "REBUILD"
								, gn->prop_hash[1] == 0 ? "does not exist" : "sources changed"
							);
						}
						else
						{
							log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s"
								, x, c++
								, "GENERATED"
								, gn->path
								, "REBUILD"
							);
						}
					}
					else if(gn->needs.l)
					{
						log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
							, x, c++
							, "SECONDARY"
							, gn->path
							, "REBUILD"
							, "eval context product"
						);
					}
					else
					{
						log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
							, x, c++
							, "GENERATED"
							, gn->path
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
			gn * gn = bp->stages[x].evals[y]->products[k];

			if(!gn->poison)
			{
				if(gn->rebuild)
				{
					// file doesn't exist or has changed, is not a SOURCE file, and cannot be fabricated
					gn->poison = 1;
					poisoned = 1;
				}
			}

			// propagate the poison
			if(gn->poison)
			{
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->poison = 1;
			}

			if(gn->rebuild)
			{
				log(L_ERROR | L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
					, x, c++
					, "SECONDARY"
					, gn->path
					, ""
					, "no formula"
				);
			}
			else
			{
				log(L_BP | L_BPEVAL, "[%2d,%2d] %9s %-65s | %-7s (%s)"
					, x, c++
					, "SECONDARY"
					, gn->path
					, ""
					, "no formula"
				);
			}
		}
	}

	// consolidate stages
	if(!poisoned)
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
				memmove(
					  &bp->stages[x]
					, &bp->stages[x + 1]
					, (bp->stages_l - x - 1) * sizeof(bp->stages[0])
				);

				bp->stages_l--;
			}
		}
	}

	return !poisoned;
}

int bp_exec(bp * bp, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p)
{
	ts ** ts			= 0;
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

	fatal(xmalloc, &ts, sizeof(*ts) * tsl);
	for(x = 0; x < tsl; x++)
		fatal(ts_mk, &ts[x]);

	for(x = 0; x < bp->stages_l; x++)
	{
		i = 0;
		log(L_BP | L_BPEXEC, "STAGE %d/%d : %d/%d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			ts[i]->fmlv = bp->stages[x].evals[y];

			// prepare lstack(s) for variables resident in this context
			int pn = p;
			if((*stax_a) <= pn)
			{
				fatal(xrealloc, stax, sizeof(**stax), pn + 1, (*stax_a));
				(*stax_a) = pn + 1;
			}
			if(!(*stax)[pn])
				fatal(xmalloc, &(*stax)[pn], sizeof(*(*stax)[0]));
			lstack_reset((*stax)[pn]);

			// @ is a list of the products of this eval context
			for(k = 0; k < ts[i]->fmlv->products_l; k++)
				fatal(lstack_obj_add, (*stax)[pn], ts[i]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			fatal(var_set, vmap, "@", (*stax)[pn++]);

			// render the formula
			fatal(fml_render, ts[i], vmap, stax, stax_l, stax_a, pn);
			
			// save hash of the cmd used to render the gn
			for(k = 0; k < ts[i]->fmlv->products_l; k++)
				gn_hashcmd(ts[i]->fmlv->products[k], ts[i]->cmd_txt->s, ts[i]->cmd_txt->l);

			log_start(L_BP | L_BPEXEC, " -- ");
			for(k = 0; k < ts[i]->fmlv->products_l; k++)
			{
				if(k)
					log_add(", ");
				log_add("%s", ts[i]->fmlv->products[k]->path);
			}
			log_finish(0);
			i++;
		}

		// execute all formulas in parallel processes
		for(y = 0; y < i; y++)
			fatal(fml_exec, ts[y], (x * 1000) + y);

		// wait for formulas to complete
		for(y = 0; y < i; y++)
		{
			ts[y]->r_status = -1;
			ts[y]->r_signal = 0;

			int r = 0;
			if(waitpid(ts[y]->pid, &r, 0) == ts[y]->pid)
			{
				if(WIFEXITED(r))
					ts[y]->r_status = WEXITSTATUS(r);
				else if(WIFSIGNALED(r))
					ts[y]->r_signal = WTERMSIG(r);
			}

			// snarf stderr
			if(1)
			{
				off_t sz = lseek(ts[y]->stde_fd, 0, SEEK_END);
				lseek(ts[y]->stde_fd, 0, SEEK_SET);
				psgrow(&ts[y]->stde_txt, sz);
				read(ts[y]->stde_fd, ts[y]->stde_txt->s, sz);
				ts[y]->stde_txt->l = sz;
			}

			// snarf stdout
			if(log_would(L_FML | L_FMLEXEC))
			{
				off_t sz = lseek(ts[y]->stdo_fd, 0, SEEK_END);
				lseek(ts[y]->stdo_fd, 0, SEEK_SET);
				psgrow(&ts[y]->stdo_txt, sz);
				read(ts[y]->stdo_fd, ts[y]->stdo_txt->s, sz);
				ts[y]->stdo_txt->l = sz;
			}

			close(ts[y]->stde_fd);
			close(ts[y]->stdo_fd);

			uint64_t tag = L_FML | L_FMLEXEC;
			if(ts[y]->r_status || ts[y]->r_signal || ts[y]->stde_txt->l)
				tag |= L_ERROR;

			log_start(tag								, "%15s : "			, "product(s)");
			for(k = 0; k < ts[y]->fmlv->products_l; k++)
			{
				if(k)
					log_add(", ");
				log_add("%s", ts[y]->fmlv->products[k]->path);
			}
			log_finish(0);
			if(log_would(tag))
			{
				log(tag										, "%15s : (%d)"		, "cmd text"			, ts[y]->cmd_txt->l);
				write(2, ts[y]->cmd_txt->s, ts[y]->cmd_txt->l);
			}
			if(log_would(L_FML | L_FMLEXEC) || ts[y]->r_status)
				log(tag										, "%15s : %d"			, "exit status"		, ts[y]->r_status);
			if(log_would(L_FML | L_FMLEXEC) || ts[y]->r_signal)
				log(tag										, "%15s : %d"			, "exit signal"		, ts[y]->r_signal);
			if(log_would(L_FML | L_FMLEXEC))
			{
				log(tag										, "%15s : %s"			, "stdout path"		, ts[y]->stdo_path->s);
				log(tag										, "%15s : (%d)"		, "stdout text"		, ts[y]->stdo_txt->l);
				write(2, ts[y]->stdo_txt->s, ts[y]->stdo_txt->l);
			}
			if(log_would(L_FML | L_FMLEXEC) || ts[y]->stde_txt->l)
			{
				log(tag										,  "%15s : %s"		, "stderr path"		, ts[y]->stde_path->s);
				log(tag										, "%15s : (%d)"		, "stderr text"		, ts[y]->stde_txt->l);
				write(2, ts[y]->stde_txt->s, ts[y]->stde_txt->l);
			}

			// SUCCESS if - exit status 0, not killed by signal, and wrote nothing to stderr
			if(ts[y]->r_status == 0 && ts[y]->r_signal == 0 && ts[y]->stde_txt->l == 0)
			{
				int q;
				for(q = 0; q < ts[y]->fmlv->products_l; q++)
				{
					gn * gn = ts[y]->fmlv->products[q];

					gn->fab_success = 1;

					// check each of my SOURCE dependencies - if all nodes which it feeds
					// have been fabricated, we are done with it - update its hashfile

					for(k = 0; k < gn->needs.l; k++)
					{
						if(gn->needs.e[k]->needs.l == 0)
						{
							int j;
							for(j = 0; j < gn->needs.e[k]->feeds.l; j++)
							{
								if(!gn->needs.e[k]->feeds.e[j]->fab_success)
									break;
							}

							if(j == gn->needs.e[k]->feeds.l)
								fatal(gn_hashes_write, gn->needs.e[k]);
						}
					}
				}
			}
			else
			{
				return 0;
			}
		}
	}

	for(x = 0; x < tsl; x++)
		ts_free(ts[x]);
	free(ts);

	return 1;
}

void bp_dump(bp * bp)
{
	int x;
	int y;
	int c;
	for(x = 0; x < bp->stages_l; x++)
	{
		log(L_BP | L_BPDUMP, "STAGE %d", x);

/*
		c = 0;
		for(y = 0; y < bp->stages[x].primary_l; y++)
		{
			log(L_BP | L_BPDUMP, "[%2d,%2d] %-9s %s", x, c++, "PRIMARY", bp->stages[x].primary[y]->path);
		}
*/

		// so numbering lines up with BPEVAL
		c = bp->stages[x].primary_l;

		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			int i;
			for(i = 0; i < bp->stages[x].evals[y]->products_l; i++)
			{
				char * des = "SECONDARY";
				if(bp->stages[x].evals[y]->products[i]->needs.l == 0)
					des = "GENERATED";

				if(i)
					log(L_BP | L_BPDUMP, "        %-9s %s", des, bp->stages[x].evals[y]->products[i]->path);
				else
					log(L_BP | L_BPDUMP, "[%2d,%2d] %-9s %s", x, c++, des, bp->stages[x].evals[y]->products[i]->path);
			}
		}

/*
		for(y = 0; y < bp->stages[x].nofmls_l; y++)
		{
			log(L_WARN | L_BP | L_BPDUMP, "[%2d,%2d] %-9s %s (no formula)", x, c++, "SECONDARY", bp->stages[x].nofmls[y]->path);
		}
*/
	}
}
