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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ts.h"
#include "fml.h"
#include "args.h"

#include "xmem.h"
#include "pstring.h"

#include "fab_control.h"

char * gn_designation(struct gn * const);
char * gn_idstring(struct gn * const);

void ts_reset(ts * ts)
{
	ts->fmlv			= 0;
	ts->pid				= 0;
	ts->cmd_fd		= 0;
	ts->stdo_fd		= 0;
	ts->stde_fd		= 0;
	ts->r_status	= 0;
	ts->r_signal	= 0;
	ts->y					= 0;

	ffn_xfree(&ts->ffn);

	if(ts->cmd_path)
		ts->cmd_path->l		= 0;
	if(ts->cmd_txt)
		ts->cmd_txt->l		= 0;
	if(ts->stdo_path)
		ts->stdo_path->l	= 0;
	if(ts->stdo_txt)
		ts->stdo_txt->l		= 0;
	if(ts->stde_path)
		ts->stde_path->l	= 0;
	if(ts->stde_txt)
		ts->stde_txt->l		= 0;
}

int ts_ensure(ts *** ts, int * tsa, int tsl)
{
	int x;

	if(tsl > (*tsa))
	{
		// reallocate to appropriate size
		fatal(xrealloc, ts, sizeof(**ts), tsl, *tsa);

		// deep allocate
		for(x = (*tsa); x < tsl; x++)
		{
			fatal(xmalloc, &(*ts)[x], sizeof(*(*ts)[0]));
			fatal(ff_mkparser, &(*ts)[x]->ffp);
		}

		(*tsa) = tsl;
	}

	finally : coda;
}

int ts_execwave(ts ** ts, int n, int * waveid, int waveno, uint64_t hi, uint64_t lo, int * bad)
{
	int x;
	int j;

	int __attribute__((unused)) __r;

	(*waveid)++;

	int step = n;
	if(g_args.concurrency > 0)
		step = g_args.concurrency;

	int local_bad;
	if(!bad)
		bad = &local_bad;

	(*bad) = 0;

	// execute all formulas in parallel processes
	for(j = 0; (*bad) == 0 && j < n; j += step)
	{
		for(x = j; x < (j + step) && x < n; x++)
			fatal(fml_exec, ts[x], ((*waveid) * 1000) + x);

		// wait for each of them to complete
		pid_t pid = 0;
		int r = 0;
		while((pid = wait(&r)) != -1)
		{
			for(x = j; x < (j + step) && x < n; x++)
			{
				if(ts[x]->pid == pid)
					break;
			}

			if(WIFEXITED(r))
				ts[x]->r_status = WEXITSTATUS(r);
			else if(WIFSIGNALED(r))
				ts[x]->r_signal = WTERMSIG(r);

			// snarf stderr
			if(1)
			{
				off_t sz = lseek(ts[x]->stde_fd, 0, SEEK_END);
				lseek(ts[x]->stde_fd, 0, SEEK_SET);
				psgrow(&ts[x]->stde_txt, sz + 2);
				__r = read(ts[x]->stde_fd, ts[x]->stde_txt->s, sz);
				ts[x]->stde_txt->s[sz+0] = 0;
				ts[x]->stde_txt->s[sz+1] = 0;
				ts[x]->stde_txt->l = sz;
			}

			// snarf stdout
			if(1)
			{
				off_t sz = lseek(ts[x]->stdo_fd, 0, SEEK_END);
				lseek(ts[x]->stdo_fd, 0, SEEK_SET);
				psgrow(&ts[x]->stdo_txt, sz + 2);
				__r = read(ts[x]->stdo_fd, ts[x]->stdo_txt->s, sz);
				ts[x]->stdo_txt->s[sz+0] = 0;
				ts[x]->stdo_txt->s[sz+1] = 0;
				ts[x]->stdo_txt->l = sz;
			}

			close(ts[x]->stde_fd);
			close(ts[x]->stdo_fd);

			uint64_t e_stat = 0;	// whether there has been an error as a result of a nonzero exit status
			uint64_t e_sign = 0;	// whether there has been an error as a result of a nonzero exit signal
			uint64_t e_stde = 0;	// whether there has been an error as a result of a nonzero-length std error output

			if(ts[x]->r_status)
				e_stat = L_ERROR;
			if(ts[x]->r_signal)
				e_sign = L_ERROR;
			if(ts[x]->stde_txt->l)
				e_stde = L_ERROR;

			uint64_t e = e_stat | e_sign | e_stde; // whether there has been an error

			if(e)
				(*bad)++;

			int k = 0;
			while(1)
			{
				struct gn * t = 0;
				if(ts[x]->fmlv->flags & FFN_FABRICATION)
					t = ts[x]->fmlv->products[k];
				else if(ts[x]->fmlv->flags & FFN_DISCOVERY)
					t = ts[x]->fmlv->target;

				if(k == 0)
				{
					int R = log_start(hi | e, "[%2d,%3d] %-9s %s", waveno, ts[x]->y, gn_designation(t), gn_idstring(t));

					if(e_stat)
					{
						log_add("%*s%d", 100 - R, "", ts[x]->r_status);
					}
					else if(e_sign)
					{
						log_add("%*ss=%d", 98 - R, "", ts[x]->r_signal);
					}
					else if(e_stde)
					{
						log_add("%*se=%d", 98 - R, "", ts[x]->stde_txt->l);
					}
					else
					{
						log_add("%*s0", 100 - R, "");
					}
					log_finish(0);
				}
				else
				{
					log(hi | e, "         %-9s %s", gn_designation(t), gn_idstring(t));
				}

				if(ts[x]->fmlv->flags & FFN_FABRICATION)
				{
					if(++k == ts[x]->fmlv->productsl)
						break;
				}
				else if(ts[x]->fmlv->flags & FFN_DISCOVERY)
				{
					break;
				}
			}

			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				log(lo | L_FML | L_FMLEXEC 													, "%15s : (%d) @ %s"	, "cmd"				, ts[x]->cmd_txt->l, ts[x]->cmd_path->s);
				__r = write(2, ts[x]->cmd_txt->s, ts[x]->cmd_txt->l);

				if(ts[x]->cmd_txt->l && ts[x]->cmd_txt->s[ts[x]->cmd_txt->l - 1] != '\n')
					__r = write(2, "\n", 1);
			}
			log(lo | L_FML | L_FMLEXEC | e_stat										, "%15s : %d"			, "exit status"		, ts[x]->r_status);
			log(lo | L_FML | L_FMLEXEC | e_sign										, "%15s : %d"			, "exit signal"		, ts[x]->r_signal);
			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				log(lo | L_FML | L_FMLEXEC													, "%15s : (%d) @ %s"	, "stdout"		, ts[x]->stdo_txt->l, ts[x]->stdo_path->s);
				__r = write(2, ts[x]->stdo_txt->s, ts[x]->stdo_txt->l);

				if(ts[x]->stdo_txt->l && ts[x]->stdo_txt->s[ts[x]->stdo_txt->l - 1] != '\n')
					__r = write(2, "\n", 1);
			}
			if(log_would(lo | L_FML | L_FMLEXEC | e_stde))
			{
				log(lo | L_FML | L_FMLEXEC | e_stde									, "%15s : (%d) @ %s"	, "stderr"		, ts[x]->stde_txt->l, ts[x]->stde_path->s);
				__r = write(2, ts[x]->stde_txt->s, ts[x]->stde_txt->l);

				if(ts[x]->stde_txt->l && ts[x]->stde_txt->s[ts[x]->stde_txt->l - 1] != '\n')
					__r = write(2, "\n", 1);
			}
		}
	}

	finally : coda;
}

void ts_free(ts * ts)
{
	if(ts)
	{
		ff_xfreeparser(&ts->ffp);
		ffn_xfree(&ts->ffn);

		pstring_xfree(&ts->cmd_path);
		pstring_xfree(&ts->cmd_txt);
		pstring_xfree(&ts->stdo_path);
		pstring_xfree(&ts->stdo_txt);
		pstring_xfree(&ts->stde_path);
		pstring_xfree(&ts->stde_txt);
	}

	free(ts);
}

void ts_xfree(ts ** ts)
{
	ts_free(*ts);
	*ts = 0;
}
