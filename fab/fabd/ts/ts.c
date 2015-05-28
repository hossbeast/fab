/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "global.h"
#include "ts.h"
#include "args.h"
#include "params.h"
#include "fml.h"
#include "gn.h"

#include "macros.h"

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

int ts_ensure(ts *** ts, size_t * tsa, size_t tsl)
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

int ts_execwave(ts ** ts, int n, size_t * waveid, int waveno, uint64_t hi, int * bad)
{
	int x;
	int j;

	int __attribute__((unused)) __r;

	(*waveid)++;

	int step = n;
	if(g_args->concurrency > 0)
		step = g_args->concurrency;

	int local_bad;
	if(!bad)
		bad = &local_bad;

	*bad = 0;

	// execute all formulas in parallel processes
	for(j = 0; !*bad && j < n; j += step)
	{
		// launch all of the commands
		for(x = j; x < (j + step) && x < n; x++)
			fatal(fml_exec, ts[x], x);

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

			// snarf stdout (discovery results)
			off_t sz = lseek(ts[x]->stdo_fd, 0, SEEK_END);
			lseek(ts[x]->stdo_fd, 0, SEEK_SET);
			fatal(psgrow, &ts[x]->stdo_txt, sz + 2);
			__r = read(ts[x]->stdo_fd, ts[x]->stdo_txt->s, sz);
			ts[x]->stdo_txt->s[sz+0] = 0;
			ts[x]->stdo_txt->s[sz+1] = 0;
			ts[x]->stdo_txt->l = sz;

			// snarf stderr
			sz = lseek(ts[x]->stde_fd, 0, SEEK_END);
			lseek(ts[x]->stde_fd, 0, SEEK_SET);
			fatal(psgrow, &ts[x]->stde_txt, sz + 2);
			__r = read(ts[x]->stde_fd, ts[x]->stde_txt->s, sz);
			ts[x]->stde_txt->s[sz+0] = 0;
			ts[x]->stde_txt->s[sz+1] = 0;
			ts[x]->stde_txt->l = sz;

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

			// build products
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
					if(log_start(hi | e))
					{
						logf(0, "[%2d,%3d] %s %s", waveno, ts[x]->y, GN_TYPE_STR(t->type), t->idstring);
						if(e_stat)
						{
							logf(0, ", exit status : %d",  ts[x]->r_status);
						}
						else if(e_sign)
						{
							logf(0, ", signal : %d",  ts[x]->r_signal);
						}

						if(e)
						{
							logf(0, ", details @ %s/dsc/%d", g_params.ipcstem, ts[x]->y);
						}

						log_finish();
					}
				}
				else
				{
					logf(hi | e, "         %s %s", GN_TYPE_STR(t->type), t->idstring);
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

			if(log_would(L_DSCCMD))
			{
				// cmd : chomp trailing whitespace
				size_t end = ts[x]->cmd_txt->l - 1;
				while(end >= 0 && (ts[x]->cmd_txt->s[end] == ' ' || ts[x]->cmd_txt->s[end] == '\t' || ts[x]->cmd_txt->s[end] == '\r' || ts[x]->cmd_txt->s[end] == '\n'))
					end--;
				end++;

				fatal(axwrite, 1, ts[x]->cmd_txt->s, end);
				fatal(axwrite, 1, "\n", 1);
			}

			if(log_would(L_DSCRES))
			{
				// stdout : chomp trailing whitespace
				size_t end = ts[x]->stdo_txt->l - 1;
				while(end >= 0 && (ts[x]->stdo_txt->s[end] == ' ' || ts[x]->stdo_txt->s[end] == '\t' || ts[x]->stdo_txt->s[end] == '\r' || ts[x]->stdo_txt->s[end] == '\n'))
					end--;
				end++;

				fatal(axwrite, 1, ts[x]->stdo_txt->s, end);
				fatal(axwrite, 1, "\n", 1);
			}

			if(e_stde)
			{
				// chomp trailing whitespace
				size_t end = ts[x]->stde_txt->l - 1;
				while(end >= 0 && (ts[x]->stde_txt->s[end] == ' ' || ts[x]->stde_txt->s[end] == '\t' || ts[x]->stde_txt->s[end] == '\r' || ts[x]->stde_txt->s[end] == '\n'))
					end--;
				end++;

				fatal(axwrite, 1, ts[x]->stde_txt->s, end);
				fatal(axwrite, 1, "\n", 1);
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

		pswfree(&ts->cmd_path);
		pswfree(&ts->cmd_txt);
		pswfree(&ts->stdo_path);
		pswfree(&ts->stdo_txt);
		pswfree(&ts->stde_path);
		pswfree(&ts->stde_txt);
	}

	free(ts);
}

void ts_xfree(ts ** ts)
{
	ts_free(*ts);
	*ts = 0;
}
