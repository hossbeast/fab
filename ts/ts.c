#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ts.h"
#include "fml.h"

#include "xmem.h"
#include "pstring.h"

#include "control.h"

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

	ff_xfreenode(&ts->ffn);

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

int ts_ensure(ts *** ts, int * tsa, int n)
{
	int x;

	if(n > (*tsa))
	{
		// reallocate to appropriate size
		fatal(xrealloc, ts, sizeof(**ts), n, *tsa);

		// deep allocate
		for(x = (*tsa); x < n; x++)
		{
			fatal(xmalloc, &(*ts)[x], sizeof(*(*ts)[0]));
			fatal(ff_mkparser, &(*ts)[x]->ffp);
		}

		(*tsa) = n;
	}

	for(x = 0; x < n; x++)
		ts_reset((*ts)[x]);

	finally : coda;
}

int ts_execwave(ts ** ts, int n, int * waveid, int waveno, uint64_t hi, uint64_t lo, int * res)
{
	int x;

	(*waveid)++;

	// execute all formulas in parallel processes
	for(x = 0; x < n; x++)
		fatal(fml_exec, ts[x], ((*waveid) * 1000) + x);

	(*res) = 1;

	// wait for each of them to complete
	pid_t pid = 0;
	int r = 0;
	while((pid = wait(&r)) != -1)
	{
		for(x = 0; x < n; x++)
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
			read(ts[x]->stde_fd, ts[x]->stde_txt->s, sz);
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
			read(ts[x]->stdo_fd, ts[x]->stdo_txt->s, sz);
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
			(*res) = 0;

		int k;
		for(k = 0; k < ts[x]->fmlv->products_l; k++)
		{
			int R = 0;
			if(k)
				R = log_start(hi | e, "        %-9s %s", gn_designate(ts[x]->fmlv->products[k]), gn_idstring(ts[x]->fmlv->products[k]));
			else
				R = log_start(hi | e, "[%2d,%2d] %-9s %s", waveno, ts[x]->y, gn_designate(ts[x]->fmlv->products[k]), gn_idstring(ts[x]->fmlv->products[k]));

			if(k == 0)
			{
				if(e_stat)
				{
					log_add("%*sx=%d", 100 - R, "", ts[x]->r_status);
				}
				else if(e_sign)
				{
					log_add("%*ss=%d", 100 - R, "", ts[x]->r_signal);
				}
				else if(e_stde)
				{
					log_add("%*se=%d", 100 - R, "", ts[x]->stde_txt->l);
				}
				else
				{
					log_add("%*sx=0", 100 - R, "");
				}
			}
			log_finish(0);

			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				// this section must actually be enabled
				log_start(lo | L_FML | L_FMLEXEC 										, "%15s : "			, "product(s)");
				for(k = 0; k < ts[x]->fmlv->products_l; k++)
				{
					if(k)
						log_add(", ");
					log_add("%s", gn_idstring(ts[x]->fmlv->products[k]));
				}
				log_finish(0);
			}
			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				log(lo | L_FML | L_FMLEXEC 													, "%15s : (%d) @ %s"	, "cmd"				, ts[x]->cmd_txt->l, ts[x]->cmd_path->s);
				write(2, ts[x]->cmd_txt->s, ts[x]->cmd_txt->l);

				if(ts[x]->cmd_txt->l && ts[x]->cmd_txt->s[ts[x]->cmd_txt->l - 1] != '\n')
					write(2, "\n", 1);
			}
			log(lo | L_FML | L_FMLEXEC | e_stat										, "%15s : %d"			, "exit status"		, ts[x]->r_status);
			log(lo | L_FML | L_FMLEXEC | e_sign										, "%15s : %d"			, "exit signal"		, ts[x]->r_signal);
			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				log(lo | L_FML | L_FMLEXEC													, "%15s : (%d) @ %s"	, "stdout"		, ts[x]->stdo_txt->l, ts[x]->stdo_path->s);
				write(2, ts[x]->stdo_txt->s, ts[x]->stdo_txt->l);

				if(ts[x]->stdo_txt->l && ts[x]->stdo_txt->s[ts[x]->stdo_txt->l - 1] != '\n')
					write(2, "\n", 1);
			}
			if(log_would(lo | L_FML | L_FMLEXEC | e_stde))
			{
				log(lo | L_FML | L_FMLEXEC | e_stde									, "%15s : (%d) @ %s"	, "stderr"		, ts[x]->stde_txt->l, ts[x]->stde_path->s);
				write(2, ts[x]->stde_txt->s, ts[x]->stde_txt->l);

				if(ts[x]->stde_txt->l && ts[x]->stde_txt->s[ts[x]->stde_txt->l - 1] != '\n')
					write(2, "\n", 1);
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
		ff_xfreenode(&ts->ffn);

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
