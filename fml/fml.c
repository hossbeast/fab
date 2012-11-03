#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "fml.h"
#include "args.h"
#include "gn.h"
#include "ts.h"
#include "ff.tokens.h"

#include "control.h"
#include "xmem.h"

//
// data
//

union g_fmls_t		g_fmls = { { .size = sizeof(fml) } };

//
// static
//

static int resolve_path(pstring ** p, gn * gn)
{
	return pscatf(p, gn->path);
}

static int do_resolve_edepso(pstring ** p, gn * gn, int k)
{
	if(strcmp(gn->ext, "o") == 0)
	{
		if(k)
			fatal(pscatf, p, " ");

		fatal(pscatf, p, gn->path);
		k++;
	}

	int x;
	for(x = 0; x < gn->needs.l; x++)
		fatal(do_resolve_edepso, p, gn->needs.e[x], k);

	return 1;
}

static int resolve_edepso(pstring ** p, gn * gn)
{
	int x;
	for(x = 0; x < gn->needs.l; x++)
		fatal(do_resolve_edepso, p, gn->needs.e[x], 0);

	return 1;
}

static int resolve_idepsc(pstring ** p, gn * gn)
{
	int k = 0;
	int x;
	for(x = 0; x < gn->needs.l; x++)
	{
		if(strcmp(gn->needs.e[x]->ext, "c") == 0)
		{
			if(k++)
				fatal(pscatf, p, " ");

			fatal(pscatf, p, gn->needs.e[x]->path);
		}
	}

	return 1;
}

//
// public
//
int fml_add(ff_node * ffn, fml ** fml)
{
	// ffn is an FFN_FORMULA
	fatal(coll_doubly_add, &g_fmls.c, 0, fml);

	(*fml)->ffn = ffn;
}

int fml_render(ts * ts)
{
	// start with shebang
	fatal(psprintf, &ts->cmd_txt, "#!/bin/bash\n\n");

	ff_node * ffn = ts->gn->fml->ffn;

	int x;
	for(x = 0; x < ffn->commands_l; x++)
	{
		if(ffn->commands[x]->type == FFN_COMMAND_TEXT)
		{
			fatal(pscatf, &ts->cmd_txt, ffn->commands[x]->text);
		}
		if(ffn->commands[x]->type == FFN_COMMAND_REF)
		{
			if(ffn->commands[x]->ref == ff_FF_REF_EDEPSO)
			{
				fatal(resolve_edepso, &ts->cmd_txt, ts->gn);
			}
			if(ffn->commands[x]->ref == ff_FF_REF_IDEPSC)
			{
				fatal(resolve_idepsc, &ts->cmd_txt, ts->gn);
			}
			if(ffn->commands[x]->ref == ff_FF_REF_PATH)
			{
				fatal(resolve_path, &ts->cmd_txt, ts->gn);
			}
		}
	}

	return 1;
}

int fml_exec(ts * ts,  int num)
{
	// create tmp file to capture stdout
	psprintf(&ts->stdo_path, "%s/%u.out", g_args.execdir, num);

	unlink(ts->stdo_path->s);
	if((ts->stdo_fd = open(ts->stdo_path->s, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
		fail("open(%s)=[%d][%s]", ts->stdo_path->s, errno, strerror(errno));

	// create tmp file to capture stderr
	psprintf(&ts->stde_path, "%s/%u.err", g_args.execdir, num);

	unlink(ts->stde_path->s);
	if((ts->stde_fd = open(ts->stde_path->s, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
		fail("open(%s)=[%d][%s]", ts->stde_path->s, errno, strerror(errno));

	// create tmp file to capture the cmd
	psprintf(&ts->cmd_path, "%s/%u.cmd", g_args.execdir, num);

	unlink(ts->cmd_path->s);
	if((ts->cmd_fd = open(ts->cmd_path->s, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
		fail("open(%s)=[%d][%s]", ts->cmd_path->s, errno, strerror(errno));

	// write the cmd to the tmp file
	if(write(ts->cmd_fd, ts->cmd_txt->s, ts->cmd_txt->l) == -1)
		fail("write failed : [%d][%s]", errno, strerror(errno));

	close(ts->cmd_fd);

	// fork off the child
	ts->pid = 0;
	if((ts->pid = fork()) == -1)
		fail("fork failed");

	if(ts->pid == 0)
	{
		int x;
		for(x = 3; x < 6; x++)//4192; x++)
		{
			if(x != ts->stdo_fd && x != ts->stde_fd)
				close(x);
		}

		// redirect stdin
		

		// redirect stdout
		if(dup2(ts->stdo_fd, 1) == -1)
			fail("dup2 failed : [%d][%s]", errno, strerror(errno));

		// redirect stderr
		if(dup2(ts->stde_fd, 2) == -1)
			fail("dup2 failed : [%d][%s]", errno, strerror(errno));

		// exec doesnt return
		execv(ts->cmd_path->s, 0);
		fail("exec(%s) failed [%d][%s]", ts->cmd_path->s, errno, strerror(errno));
	}

	return 1;
}
