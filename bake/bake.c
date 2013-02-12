#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <listwise.h>
#include <listwise/lstack.h>
#include <listwise/object.h>

#include "bake.h"
#include "fml.h"
#include "ts.h"
#include "gn.h"
#include "gnlw.h"
#include "var.h"
#include "args.h"

#include "log.h"
#include "control.h"
#include "macros.h"
#include "xmem.h"

int bake_bp(
	  const bp * const bp
	, map * const vmap
	, lstack *** const stax
	, int * const staxa
	, int p
	, ts *** const ts
	, int * const tsa
	, const int * const tsw
	, const char * const dst
)
{
	int tsl				= 0;		// thread count
	int tot				= 0;		// total targets
	int y;
	int x;
	int i;
	int k;
	int fd;

	// determine how many threads are needed
	for(x = 0; x < bp->stages_l; x++)
	{
		tsl = MAX(tsl, bp->stages[x].evals_l);
		tot += bp->stages[x].evals_l;
	}

	// ensure I have enough threadspace
	fatal(ts_ensure, ts, tsa, tsl);

	// open the bakescript
	if((fd = open(dst, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
		fail("open(%s)=[%d][%s]", dst, errno, strerror(errno));

	dprintf(fd, "#!/bin/bash\n\n");
	dprintf(fd, "START=$(date +%%s)\n");

	// render all formulas
	i = 0;
	for(x = 0; x < bp->stages_l; x++)
	{
		log(L_BP | L_BPINFO, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		if(bp->stages[x].evals_l)
			dprintf(fd, "## stage %d\n", x);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			(*ts)[y]->fmlv = bp->stages[x].evals[y];
			(*ts)[y]->y = y;

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
			for(k = 0; k < (*ts)[y]->fmlv->products_l; k++)
				fatal(lstack_obj_add, (*stax)[pn], (*ts)[y]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			fatal(var_set, vmap, "@", (*stax)[pn++], 0);

			// render the formula
			fatal(fml_render, (*ts)[y], vmap, stax, staxa, pn, 0);

			// index in the stage in which this formula is executed
			int index = y;
			if(g_args.concurrency > 0)
				index %= g_args.concurrency;

			// append this formula to the bakescript
			dprintf(fd
				, "fml_%d_%d()\n"						// x, y
				  "{\n"
				  "  exec 1>/dev/null\n"		// discard stdout
				  "  exec 2>&%d\n"					// save stderr to specific fd
					"\n"
					"  # command"
					"  %.*s\n"								// the command
					"\n"
					"  X=$?\n"								// save exit status
					"  echo %d 1>&100\n"			// echo our index
					"  exit $X\n"							// return exit status
				  "}\n"
					"\n"
				, x, y
				, 100+index+1
				, (int)(*ts)[y]->cmd_txt->l, (*ts)[y]->cmd_txt->s
				, index
			);

			// save the name(s) of target(s) produced by this formula
			dprintf(fd, "NAMES[%d]='", i++);
			if((*ts)[y]->fmlv->products_l > 1)
				dprintf(fd, "{");

			for(k = 0; k < (*ts)[y]->fmlv->products_l; k++)
			{
				if(k)
					dprintf(fd, ", ");
				dprintf(fd, "%.*s", (*ts)[y]->fmlv->products[k]->idstringl, (*ts)[y]->fmlv->products[k]->idstring);
			}

			if((*ts)[y]->fmlv->products_l > 1)
				dprintf(fd, "}");

			dprintf(fd, "'\n");
		}
	}

	// create enough temp files for the whole build
	int needtemp = MIN(g_args.concurrency > 0 ? g_args.concurrency : 0xFFFFFFFF, tsl);

	dprintf(fd, "# setup temp files\n");
	dprintf(fd, "tmp[0]=`mktemp --dry-run`\n");
	for(x = 0; x < needtemp; x++)
		dprintf(fd, "tmp[%d]=`mktemp --dry-run`\n", x+1);

	// setup named pipe to manage process multiplexing
	dprintf(fd, "mkfifo --mode=0700 $tmp[0]\n");
	dprintf(fd, "exec 100<>$tmp[0]\n");
	dprintf(fd, "rm -f $tmp[0]\n");
	dprintf(fd, "\n");

	// results aggregation
	dprintf(fd, "WIN=0\n");
	dprintf(fd, "DIE=0\n");

	// emit code to execute all the stages
	i = 0;
	for(x = 0; x < bp->stages_l; x++)
	{
		if(bp->stages[x].evals_l)
		{
			int step = bp->stages[x].evals_l;
			if(g_args.concurrency > 0)
				step = g_args.concurrency;

			int j;
			for(j = 0; j < bp->stages[x].evals_l; j += step)
			{
				// execute fmls in this stage concurrently
				dprintf(fd, "# launch stage %d.%d\n", x, j/step);

				for(y = j; y < (j + step) && y < bp->stages[x].evals_l; y++)
					dprintf(fd, "exec %d>$tmp[%d] ; rm -f $tmp[%d]\n", 100+y-j+1, y-j+1, y-j+1);

				for(y = j; y < (j + step) && y < bp->stages[x].evals_l; y++)
					dprintf(fd, "fml_%d_%d & PIDS[%d]=$!\n", x, y, y-j);
				
				dprintf(fd, "\n");
				dprintf(fd, "# harvest stage %d.%d\n", x, j);
				dprintf(fd, "C=0\n");
				dprintf(fd, "while [[ $C != %d ]]; do\n", MIN(j + step, bp->stages[x].evals_l) - j);
				dprintf(fd, "  read -u 100 idx\n");
				dprintf(fd, "  wait ${PIDS[$idx]}\n");
				dprintf(fd, "  EXTS[$idx]=$?\n");
				dprintf(fd, "  P=${PIDS[$idx]}\n");
				dprintf(fd, "  X=${EXTS[$idx]}\n");
				dprintf(fd, "  I=$((%d+$idx))\n", i+j);
				dprintf(fd, "  N=${NAMES[$I]}\n");
				dprintf(fd, "  [[ $X -eq 0 ]] && ((WIN++))\n");
				dprintf(fd, "  [[ $X -ne 0 ]] && ((DIE++))\n");
				dprintf(fd, "  printf '[%%3d,%%3d] P=%%d X=%%d %%s\\n' %d $(($idx+%d)) $P $X \"$N\"\n", x, j);
				dprintf(fd, "  cat /proc/$$/fd/$((100+$idx+1))\n");
				dprintf(fd, "  ((C++))\n");
				dprintf(fd, "done\n");
				dprintf(fd, "\n");
			}
		}
		i += bp->stages[x].evals_l;
	}
	
	dprintf(fd, "STOP=$(date +%%s)\n");
	dprintf(fd, "printf '%%15s %%ds\\n' elapsed $(($STOP-$START))\n");
	dprintf(fd, "printf '%%15s %%d\\n' succeeded $WIN\n");
	dprintf(fd, "printf '%%15s %%d\\n' failed $DIE\n");

finally:
	close(fd);
coda;
}
