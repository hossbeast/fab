#include <stdio.h>

#include "bp.h"
#include "ts.h"

#include "log.h"
#include "control.h"
#include "xmem.h"

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

//
// static
//

static int target_cmp(const void * _A, const void * _B)
{
	gn * A = *((gn **)_A);
	gn * B = *((gn **)_B);

	return strcmp(A->path, B->path);
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
			// get all leaves from this node
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

				// add all leaf nodes to this stage
				bp_stage * bps = &(*bp)->stages[k];

				// reallocate the stage
				fatal(xrealloc, &bps->targets, sizeof(bps->targets[0]), bps->targets_l + lvsl, bps->targets_l);

				int y;
				for(y = 0; y < lvsl; y++)
					bps->targets[bps->targets_l + y] = lvs[y];

				bps->targets_l += lvsl;
			}
		}
	}

	// sort each stage by name
	for(x = 0; x < (*bp)->stages_l; x++)
	{
		qsort(
			  (*bp)->stages[x].targets
			, (*bp)->stages[x].targets_l
			, sizeof((*bp)->stages[0].targets[0])
			, target_cmp
		);
	}

	return 1;
}

int bp_prune(bp * bp)
{
	int x;
	int y;
	int i;

	char bp_bad = 0;

	// mark all nodes as not needing to be rebuilt
	for(x = 0; x < bp->stages_l; x++)
	{
		for(y = 0; y < bp->stages[x].targets_l; y++)
		{
			bp->stages[x].targets[y]->changed = 0;
			bp->stages[x].targets[y]->rebuild = 0;
			bp->stages[x].targets[y]->poison = 0;
		}
	}

	for(x = 0; x < bp->stages_l; x++)
	{
		for(y = 0; y < bp->stages[x].targets_l; y++)
		{
			gn * gn = bp->stages[x].targets[y];

			if(!gn->poison)
			{
				if(gn->needs.l)
				{
					if(gn->prop_hash[1] == 0)
						gn->changed= 1;	// file doesn't exist

					if(gn->changed)
						gn->rebuild = 1;
				}
				else
				{
					// SOURCE file
					if(gn->prop_hash[1] == 0)		// file does not exist
					{
						// SOURCE file - not found
						log(L_ERROR, "SOURCE file %s not found - required by %d", gn->path, gn->feeds.l);
						for(i = 0; i < gn->feeds.l; i++)
							log(L_BPEVAL, "  ---> %s", gn->feeds.e[i]->path);

						gn->poison = 1;
					}
					else
					{
						gn_hashes_read(gn);

						if(gn_hashes_cmp(gn))		// hashes do not agree; file has changed
							gn->changed = 1;
					}
				}

				if(gn->rebuild && !gn->fml)
				{
					// file doesn't exist or has changed, is not a SOURCE file, and cannot be fabricated

					log(L_ERROR, "file %s has no formula - required by %d", gn->path, gn->feeds.l);
					for(i = 0; i < gn->feeds.l; i++)
						log(L_BPEVAL, "  ---> %s", gn->feeds.e[i]->path);

					gn->poison = 1;
				}

				// needs rebuilt
				if(gn->changed)
				{
					// : mark all nodes that depend on me as needing rebuilt, too
					for(i = 0; i < gn->feeds.l; i++)
						gn->feeds.e[i]->changed = 1;
				}

				// does not need rebuilt : remove it from the buildplan
				if(!gn->rebuild)
					bp->stages[x].targets[y] = 0;
			}

			if(gn->needs.l)
			{
				if(gn->rebuild)
					log(L_BPEVAL, "%9s %-50s | %7s (%s)", "SECONDARY", gn->path, "REBUILD"
						, gn->prop_hash[1] == 0 ? "does not exist" : "sources changed"
					);
				else
					log(L_BPEVAL, "%9s %-50s | %7s", "SECONDARY", gn->path, "SKIP");
			}
			else
			{
				log(L_BPEVAL, "%9s %-50s | %7s (%s)", "SOURCE", gn->path, ""
					, gn->changed ? "  changed" : "unchanged"
				);
			}

			// propagate the poison
			if(gn->poison)
			{
				for(i = 0; i < gn->feeds.l; i++)
					gn->feeds.e[i]->poison = 1;

				bp_bad = 1;
			}
		}
	}

	// consolidate stages
	if(!bp_bad)
	{
		for(x = bp->stages_l - 1; x >= 0; x--)
		{
			for(y = bp->stages[x].targets_l - 1; y >= 0; y--)
			{
				if(bp->stages[x].targets[y] == 0)
				{
					memmove(
						  &bp->stages[x].targets[y]
						, &bp->stages[x].targets[y + 1]
						, (bp->stages[x].targets_l - y - 1) * sizeof(bp->stages[0].targets[0])
					);

					bp->stages[x].targets_l--;
				}
			}

			if(bp->stages[x].targets_l == 0)
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

	return !bp_bad;
}

int bp_exec(bp * bp)
{
	ts ** ts			= 0;
	int tsl				= 0;		// thread count
	int y;
	int x;
	int i;

	// determine how many threads are needed
	for(x = 0; x < bp->stages_l; x++)
		tsl = MAX(tsl, bp->stages[x].targets_l);

	fatal(xmalloc, &ts, sizeof(*ts) * tsl);
	for(x = 0; x < tsl; x++)
		fatal(ts_mk, &ts[x]);

	for(x = 0; x < bp->stages_l; x++)
	{
		i = 0;
		log(L_BPEXEC, "STAGE %d/%d : %d/%d", x, bp->stages_l - 1, bp->stages[x].targets_l - 1, 0);

		// determine which nodes to fabricate on this stage
		for(y = 0; y < bp->stages[x].targets_l; y++)
		{
			ts[i]->gn = bp->stages[x].targets[y];
			fatal(fml_render, ts[i]);
			
			// save hash of the cmd used to render the gn
			gn_hashcmd(ts[i]->gn, ts[i]->cmd_txt->s, ts[i]->cmd_txt->l);

			log(L_BPEXEC, " -- %s", ts[i++]->gn->path);
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
			if(log_would(L_FMEXEC))
			{
				off_t sz = lseek(ts[y]->stdo_fd, 0, SEEK_END);
				lseek(ts[y]->stdo_fd, 0, SEEK_SET);
				psgrow(&ts[y]->stdo_txt, sz);
				read(ts[y]->stdo_fd, ts[y]->stdo_txt->s, sz);
				ts[y]->stdo_txt->l = sz;
			}

			close(ts[y]->stde_fd);
			close(ts[y]->stdo_fd);

			uint64_t tag = L_FMEXEC;
			if(ts[y]->r_status || ts[y]->stde_txt->l)
				tag |= L_ERROR;

			log(tag			, "%15s : %s"			, "target"				, ts[y]->gn->path);
			if(log_would(L_FMEXEC))
			{
				log(L_FMEXEC, "%15s : (%d)"	, "cmd text"			, ts[y]->cmd_txt->l);
				write(2, ts[y]->cmd_txt->s, ts[y]->cmd_txt->l);
			}
			else
			{
				log(L_FMEXEC, "%15s"				, "cmd text");
			}
			log(tag			, "%15s : %d"			, "exit status"		, ts[y]->r_status);
			log(tag			, "%15s : %d"			, "exit signal"		, ts[y]->r_signal);
			log(L_FMEXEC, "%15s : %s"			, "stdout path"		, ts[y]->stdo_path->s);
			if(log_would(L_FMEXEC))
			{
				log(L_FMEXEC, "%15s : (%d)"	, "stdout text"		, ts[y]->stdo_txt->l);
				write(2, ts[y]->stdo_txt->s, ts[y]->stdo_txt->l);
			}
			else
			{
				log(L_FMEXEC, "%15s"				, "stdout text");
			}
			log(L_FMEXEC, "%15s : %s"			, "stderr path"		, ts[y]->stde_path->s);
			if(log_would(L_FMEXEC))
			{
				log(L_FMEXEC, "%15s : (%d)"	, "stderr text"		, ts[y]->stde_txt->l);
				write(2, ts[y]->stde_txt->s, ts[y]->stde_txt->l);
			}
			else
			{
				log(L_FMEXEC, "%15s"				, "stderr text");
			}

			// SUCCESS if - exit status 0 and wrote nothing to stderr
			if(ts[y]->r_status == 0 && ts[y]->stde_txt->l == 0)
			{
				ts[y]->gn->fab_success = 1;

				// check each of my SOURCE dependencies - if all nodes which it feeds
				// to have been fabricated, we are done with it - update its hashfile

				int k;
				for(k = 0; k < ts[y]->gn->needs.l; k++)
				{
					if(ts[y]->gn->needs.e[k]->needs.l == 0)
					{
						int j;
						for(j = 0; j < ts[y]->gn->needs.e[k]->feeds.l; j++)
						{
							if(!ts[y]->gn->needs.e[k]->feeds.e[j]->fab_success)
								break;
						}

						if(j == ts[y]->gn->needs.e[k]->feeds.l)
							fatal(gn_hashes_write, ts[y]->gn->needs.e[k]);
					}
				}
			}
		}

		for(y = 0; y < i; y++)
		{
			if(!ts[y]->gn->fab_success)
				return 0;
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
	for(x = 0; x < bp->stages_l; x++)
	{
		log(L_BPDUMP, "STAGE %d", x);

		int y;
		for(y = 0; y < bp->stages[x].targets_l; y++)
		{
			log(L_BPDUMP, " -- %s %s", bp->stages[x].targets[y]->path, bp->stages[x].targets[y]->mark ? "*" : "");
		}
	}
}
