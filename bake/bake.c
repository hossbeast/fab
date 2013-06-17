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

#include "list.h"
#include "lwutil.h"
#include "log.h"
#include "control.h"
#include "macros.h"
#include "xmem.h"
#include "map.h"

int bake_bp(
	  const bp * const bp
	, map * const vmap
	, generator_parser * const gp
	, lstack *** const stax
	, int * const staxa
	, int staxp
	, ts *** const ts
	, int * const tsa
	, const int * const tsw
	, const char * const dst
)
{
	int fd				= 0;
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

	// open the bakescript
	if((fd = open(dst, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
		fail("open(%s)=[%d][%s]", dst, errno, strerror(errno));

	dprintf(fd, "#!/bin/bash\n");

#ifdef DEVEL
	if(g_args.bslic_mode == MODE_BSLIC_FAB)
	{
		dprintf(fd,
/* this is for producing the bakescripts distributed with the fab source code itself */
"\n"
"# Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>\n"
"#\n"
"# This file is part of fab.\n"
"#\n"
"# fab is free software: you can redistribute it and/or modify\n"
"# it under the terms of the GNU General Public License as published by\n"
"# the Free Software Foundation, either version 3 of the License, or\n"
"# (at your option) any later version.\n"
"#\n"
"# fab is distributed in the hope that it will be useful,\n"
"# but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"# GNU General Public License for more details.\n"
"#\n"
"# You should have received a copy of the GNU General Public License\n"
"# along with fab.  If not, see <http://www.gnu.org/licenses/>.\n"
"\n"
		);
	}
	else
	{
#endif
/* include licensing exception in bakescript output */
	dprintf(fd,
"\n"
"# A build script made by fab " XQUOTE(FABVERSION) "\n"
"#  fab is free software released under the GNU General Public License.\n"
"#\n"
"#  As a special exception, build scripts made by fab " XQUOTE(FABVERSION) " (including this\n"
"#  build script) are excluded from the license covering fab itself, even\n"
"#  if substantial portions of the fab source code are copied verbatim into\n"
"#  the build script. You may create a larger work that contains part or all\n"
"#  of the build script and distribute that work under terms of your choice\n"
"\n"
	);
#if DEVEL
	}
#endif

	dprintf(fd, "# re-exec under time\n");
	dprintf(fd, "if [[ $1 != \"timed\" ]]; then\n");
	dprintf(fd, "  time $0 timed\n");
	dprintf(fd, "  exit $?\n");
	dprintf(fd, "fi\n");
	dprintf(fd, "\n");

	// render all formulas
	i = 0;
	for(x = 0; x < bp->stages_l; x++)
	{
		log(L_BP | L_BPINFO, "STAGE %d of %d executes %d of %d", x, bp->stages_l - 1, bp->stages[x].evals_l, tot);

		if(x)
			dprintf(fd, "\n");
		dprintf(fd, "# formulas and names for stage %d\n", x);

		// render formulas for each eval context on this stage
		for(y = 0; y < bp->stages[x].evals_l; y++)
		{
			(*ts)[y]->fmlv = bp->stages[x].evals[y];
			(*ts)[y]->y = y;

			// prepare lstack(s) for variables resident in this context
			fatal(lw_reset, stax, staxa, staxp);

			// @ is a list of expected products of this eval context
			for(k = 0; k < (*ts)[y]->fmlv->productsl; k++)
				fatal(lstack_obj_add, (*stax)[staxp], (*ts)[y]->fmlv->products[k], LISTWISE_TYPE_GNLW);

			// render the formula
			fatal(map_set, (*ts)[y]->fmlv->bag, MMS("@"), MM((*stax)[staxp]));
			fatal(fml_render, (*ts)[y], gp, stax, staxa, staxp + 1, 0);
			fatal(map_delete, (*ts)[y]->fmlv->bag, MMS("@"));

			// index occupied by this formula in the stage.stage in which this formula is executed
			int index = y;
			if(g_args.concurrency > 0)
				index %= g_args.concurrency;

			// save the name(s) of target(s) produced by this formula
			dprintf(fd, "NAMES[%d]='", i++);
			if((*ts)[y]->fmlv->productsl > 1)
				dprintf(fd, "{");

			for(k = 0; k < (*ts)[y]->fmlv->productsl; k++)
			{
				if(k)
					dprintf(fd, ", ");
				dprintf(fd, "%.*s", (*ts)[y]->fmlv->products[k]->idstringl, (*ts)[y]->fmlv->products[k]->idstring);
			}

			if((*ts)[y]->fmlv->productsl > 1)
				dprintf(fd, "}");

			dprintf(fd, "'\n");

			// append this formula to the bakescript
			dprintf(fd
				, "fml_%d_%d()\n"						// x, y
				  "{\n"
				  "  exec 1>/dev/null\n"		// discard stdout
				  "  exec 2>&%d\n"					// save stderr to specific fd
					"\n"
					"  %.*s\n"								// the command
					"\n"
					"  X=$?\n"								// save exit status
					"  echo %d 1>&99\n"				// echo our index
					"  exit $X\n"							// return exit status
				  "}\n"
					"\n"
				, x, y
				, 100+index
				, (int)(*ts)[y]->cmd_txt->l, (*ts)[y]->cmd_txt->s
				, index
			);
		}
	}

	dprintf(fd, "# temp filename - only need the 1\n");
	dprintf(fd, "tmp=`mktemp --dry-run`\n");

	dprintf(fd, "# setup named pipe to manage process multiplexing\n");
	dprintf(fd, "\n");
	dprintf(fd, "mkfifo --mode=0700 $tmp\n");
	dprintf(fd, "exec 99<>$tmp\n");
	dprintf(fd, "rm -f $tmp\n");
	dprintf(fd, "\n");

	dprintf(fd, "# results aggregation\n");
	dprintf(fd, "WIN=0\n");
	dprintf(fd, "DIE=0\n");
	dprintf(fd, "SKP=0\n");
	dprintf(fd, "\n");

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
				dprintf(fd, "# early termination \n");
				dprintf(fd, "if [[ $DIE -ne 0 ]]; then\n");
				dprintf(fd, "  ((SKP+=%d))\n", MIN(j + step, bp->stages[x].evals_l) - j);
				dprintf(fd, "else\n");

				// execute fmls in this stage concurrently
				dprintf(fd, "  # launch stage %d.%d\n", x, j/step);

				for(y = j; y < (j + step) && y < bp->stages[x].evals_l; y++)
					dprintf(fd
						, "  exec %d>$tmp ; rm -f $tmp ; fml_%d_%d & PIDS[%d]=$!\n"
						, 100+y-j
						, x
						, y
						, y-j
					);

				dprintf(fd, "\n");
				dprintf(fd, "  # harvest stage %d.%d\n", x, j/step);
				dprintf(fd, "  C=0\n");
				dprintf(fd, "  while [[ $C != %d ]]; do\n", MIN(j + step, bp->stages[x].evals_l) - j);
				dprintf(fd, "    read -u 99 idx\n");
				dprintf(fd, "    wait ${PIDS[$idx]}\n");
				dprintf(fd, "    EXITS[$idx]=$?\n");
				dprintf(fd, "    P=${PIDS[$idx]}\n");
				dprintf(fd, "    X=${EXITS[$idx]}\n");
				dprintf(fd, "    I=$((%d+$idx))\n", i+j);
				dprintf(fd, "    N=${NAMES[$I]}\n");
				dprintf(fd, "    [[ $X -eq 0 ]] && ((WIN++))\n");
				dprintf(fd, "    [[ $X -ne 0 ]] && ((DIE++))\n");
				dprintf(fd, "    printf '[%%3d,%%3d] X=%%d %%s\\n' %d $((idx+%d)) $X \"$N\"\n", x, j);
				dprintf(fd, "    cat /proc/$$/fd/$((100+idx))\n");
				dprintf(fd, "    ((C++))\n");
				dprintf(fd, "  done\n");
				dprintf(fd, "fi\n");
				dprintf(fd, "\n");
			}
		}

		i += bp->stages[x].evals_l;
	}
	
	dprintf(fd, "printf '%%15s %%d\\n' succeeded $WIN\n");
	dprintf(fd, "printf '%%15s %%d\\n' failed $DIE\n");
	dprintf(fd, "printf '%%15s %%d\\n' skipped $SKP\n");
	dprintf(fd, "# no failures=0, and 1 otherwise\n");
	dprintf(fd, "exit $((!(DIE==0)))\n");

finally:
	close(fd);
coda;
}

