#include "ts.h"

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

	return 1;
}

int ts_execwave(ts ** ts, int n, int * waveid, uint64_t hi, uint64_t lo)
{
	int x;

	(*waveid)++;

	// execute all formulas in parallel processes
	for(x = 0; x < n; x++)
		fatal(fml_exec, ts[x], ((*waveid) * 1000) + x);

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

		int e = 0;
		if(ts[x]->r_status || ts[x]->r_signal || ts[x]->stde_txt->l)
			e = 1;

		int k;
		for(k = 0; k < ts[x]->fmlv->products_l; k++)
		{
			char * des = "SECONDARY";
			if(strcmp("/..", ts[x]->fmlv->products[k]->dir) == 0)
				des = "TASK";
			else if(ts[x]->fmlv->products[k]->needs.l == 0)
				des = "GENERATED";

			int R = 0;
			if(k)
				R = log_start(hi | (e ? L_ERROR : 0), "        %-9s %s", des, ts[x]->fmlv->products[k]->path);
			else
				R = log_start(hi | (e ? L_ERROR : 0), "[%2d,%2d] %-9s %s", x, ts[x]->y, des, ts[x]->fmlv->products[k]->path);

			if(ts[x]->r_status)
			{
				log_add("%*sx=%d", 100 - R, "", ts[x]->r_status);
			}
			else if(ts[x]->r_signal)
			{
				log_add("%*ss=%d", 100 - R, "", ts[x]->r_signal);
			}
			else if(ts[x]->stde_txt->l)
			{
				log_add("%*se=%d", 100 - R, "", ts[x]->stde_txt->l);
			}
			else
			{
				log_add("%*sx=0", 100 - R, "");
			}
			log_finish(0);

			if(log_would(lo | L_FML | L_FMLEXEC))
			{
				log_start(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)								, "%15s : "			, "product(s)");
				for(k = 0; k < ts[x]->fmlv->products_l; k++)
				{
					if(k)
						log_add(", ");
					log_add("%s", ts[x]->fmlv->products[k]->path);
				}
				log_finish(0);
			}
			if(log_would(lo | L_FML | L_FMLEXEC) || e)
			{
					log(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)										, "%15s : (%d) @ %s"	, "cmd"				, ts[x]->cmd_txt->l, ts[x]->cmd_path->s);
					write(2, ts[x]->cmd_txt->s, ts[x]->cmd_txt->l);
					if(ts[x]->cmd_txt->l)
						write(2, "\n", 1);
					log(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)										, "%15s : %d"			, "exit status"		, ts[x]->r_status);
					log(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)										, "%15s : %d"			, "exit signal"		, ts[x]->r_signal);
			}
			if(log_would(lo | L_FML | L_FMLEXEC))
			{
					log(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)										, "%15s : (%d) @ %s"	, "stdout"		, ts[x]->stdo_txt->l, ts[x]->stdo_path->s);
					write(2, ts[x]->stdo_txt->s, ts[x]->stdo_txt->l);
					if(ts[x]->stdo_txt->l)
						write(2, "\n", 1);
			}
			if(log_would(lo | L_FML | L_FMLEXEC) || e)
			{
					log(lo | L_FML | L_FMLEXEC | (e ? L_ERROR : 0)										, "%15s : (%d) @ %s"	, "stderr"		, ts[x]->stde_txt->l, ts[x]->stde_path->s);
					write(2, ts[x]->stde_txt->s, ts[x]->stde_txt->l);
					if(ts[x]->stde_txt->l)
						write(2, "\n", 1);
			}
		}
	}
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
