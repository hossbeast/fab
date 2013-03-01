#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <listwise.h>
#include <listwise/lstack.h>

#include "fml.h"
#include "args.h"
#include "gn.h"
#include "ts.h"
#include "ff.tokens.h"
#include "list.h"
#include "identity.h"

#include "control.h"
#include "xmem.h"
#include "map.h"
#include "dirutil.h"

//
// data
//

union g_fmls_t		g_fmls = { { .size = sizeof(fml) } };

//
// static
//

static int fml_add_single(fml * fml, strstack * sstk, lstack * ls)
{
	int y;

	fml->evals_l = ls->s[0].l;
	fatal(xmalloc, &fml->evals, sizeof(fml->evals[0]) * fml->evals_l);

	LSTACK_ITERATE(ls, y, go);
	if(go)
	{
		fmleval * fmlv = &fml->evals[y];
		fmlv->fml = fml;
		fmlv->products_l = 1;
		fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->products_l);

		char * s = 0;
		int l = 0;

		fatal(lstack_string, ls, 0, y, &s, &l);

		gn * t = 0;
		fatal(gn_add
//			, fml->ffn->loc.ff->path->abs_dir
			, g_args.init_fabfile_path->abs_dir
			, sstk
			, s
			, l
			, &t
			, 0
		);

		fmlv->products[0] = t;

		// update affected lists
		fatal(ff_regular_affecting_gn, fml->ffn->loc.ff, t);

		if(fml->ffn->flags & FFN_DISCOVERY)
		{
			log(L_DSC | L_FML | L_FMLTARG, "dsc(%s)[%3d,%3d - %3d,%3d] -> %s"
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
				, t->idstring
			);
			t->dscv = fmlv;
		}
		else
		{
			log(L_FAB | L_FML | L_FMLTARG, "reg(%s)[%3d,%3d - %3d,%3d] -> %s"
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
				, t->idstring
			);
			t->fabv = fmlv;
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}

static int fml_add_multi(fml * fml, strstack * sstk, lstack * ls)
{
	int x;
	int y;

	// multi-target formula
	fml->evals_l = ls->l;
	fatal(xmalloc, &fml->evals, sizeof(fml->evals[0]) * fml->evals_l);

	for(x = 0; x < ls->l; x++)
	{
		fmleval * fmlv = &fml->evals[x];
		fmlv->fml = fml;
		fmlv->products_l = ls->s[x].l;
		fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->products_l);

		if(fml->ffn->flags & FFN_DISCOVERY)
		{
			log_start(L_DSC | L_FML | L_FMLTARG, "dsc(%s)[%3d,%3d - %3d,%3d] -> { "
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
			);
		}
		else
		{
			log_start(L_FAB | L_FML | L_FMLTARG, "reg(%s)[%3d,%3d - %3d,%3d] -> { "
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
			);
		}

		for(y = 0; y < ls->s[x].l; y++)
		{
			gn * t = 0;
			fatal(gn_add
//				, fml->ffn->loc.ff->path->abs_dir
				, g_args.init_fabfile_path->abs_dir
				, sstk
				, ls->s[x].s[y].s
				, ls->s[x].s[y].l
				, &t
				, 0
			);

			// update affected lists
			fatal(ff_regular_affecting_gn, fml->ffn->loc.ff, t);

			if(y)
				log_add(", %s", t->idstring);
			else
				log_add("%s", t->idstring);

			fmlv->products[y] = t;
			if(fml->ffn->flags & FFN_DISCOVERY)
				t->dscv = fmlv;
			else
				t->fabv = fmlv;
		}
		log_finish(" }");
	}

	finally : coda;
}

//
// public
//
int fml_add(ff_node * ffn, strstack * sstk, map * vmap, lstack *** stax, int * staxa, int p)
{
	// create fml with ffn - which is an FFN_FORMULA
	fml * fml = 0;
	fatal(coll_doubly_add, &g_fmls.c, 0, &fml);
	fml->ffn = ffn;

	fatal(list_ensure, stax, staxa, p);

	// resolve targets lists
	if(ffn->targets_0)
		fatal(list_resolveto, ffn->targets_0, vmap, stax, staxa, p);

	if(ffn->targets_1)
		fatal(list_resolveto, ffn->targets_1, vmap, stax, staxa, p);

	// attach graph nodes
	if(fml->ffn->flags & FFN_SINGLE)
	{
		fatal(fml_add_single, fml, sstk, (*stax)[p]);
	}
	else if(fml->ffn->flags & FFN_MULTI)
	{
		fatal(fml_add_multi, fml, sstk, (*stax)[p]);
	}
	else
	{
		fail("bad flags %hhu (no cardinality)", fml->ffn->flags);
	}

	finally : coda;
}

int fml_render(ts * ts, map * vmap, lstack *** stax, int * staxa, int p, int standalone)
{
	if(standalone)
	{
		// start with shebang
		fatal(psprintf, &ts->cmd_txt, "#!/bin/bash\n\n");
	}
	else if(ts->cmd_txt)
	{
		ts->cmd_txt->l = 0;
	}

	ff_node * ffn = ts->fmlv->fml->ffn;

	int k = 0;
	int x;
	for(x = 0; x < ffn->commandsl; x++)
	{
		if(ffn->commands[x]->type == FFN_WORD)
		{
			if(k)
				fatal(pscat, &ts->cmd_txt, " ", 1);

			fatal(pscatf, &ts->cmd_txt, ffn->commands[x]->text);
			k++;
		}
		else if(ffn->commands[x]->type == FFN_LF)
		{
			fatal(pscatf, &ts->cmd_txt, "\n");
			k = 0;
		}
		else if(ffn->commands[x]->type == FFN_LIST)
		{
			fatal(list_resolve, ffn->commands[x], vmap, stax, staxa, p);

			int i;
			LSTACK_ITERATE((*stax)[p], i, go);
			if(go)
			{
				if(k)
					fatal(pscat, &ts->cmd_txt, " ", 1);

				char * s = 0;
				int l = 0;
				lstack_string((*stax)[p], 0, i, &s, &l);

				fatal(pscat, &ts->cmd_txt, s, l);
				k++;
			}
			LSTACK_ITEREND;
		}
	}

	finally : coda;
}

int fml_exec(ts * ts, int num)
{
	// assume fabsys identity
	fatal(identity_assume_fabsys);

	//
	// create pid-fml-dir
	//
	//  note : all but the last component of this dir were created in tmp_setup
	//  note : the directory itself cannot already exist, because num is process-unique
	//
	fatal(psprintf, &ts->stdo_path, PID_DIR_BASE "/%d/fml/%d", g_args.pid, num);
	fatal(mkdirp, ts->stdo_path->s, S_IRWXU | S_IRWXG | S_IRWXO);

	// create tmp file for the cmd
	fatal(psprintf, &ts->cmd_path, PID_DIR_BASE "/%d/fml/%d/cmd", g_args.pid, num);
	if((ts->cmd_fd = open(ts->cmd_path->s, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG)) == -1)
		fail("open(%s)=[%d][%s]", ts->cmd_path->s, errno, strerror(errno));

	// write the cmd to the tmp file
	if(write(ts->cmd_fd, ts->cmd_txt->s, ts->cmd_txt->l) == -1)
		fail("write failed : [%d][%s]", errno, strerror(errno));

	close(ts->cmd_fd);

	// create tmp file to capture stdout, remain-through-exec
	fatal(psprintf, &ts->stdo_path, PID_DIR_BASE "/%d/fml/%d/out", g_args.pid, num);
	if((ts->stdo_fd = open(ts->stdo_path->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
		fail("open(%s)=[%d][%s]", ts->stdo_path->s, errno, strerror(errno));

	// create tmp file to capture stderr, remain-through-exec
	fatal(psprintf, &ts->stde_path, PID_DIR_BASE "/%d/fml/%d/err", g_args.pid, num);
	if((ts->stde_fd = open(ts->stde_path->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
		fail("open(%s)=[%d][%s]", ts->stde_path->s, errno, strerror(errno));

	// fork off the child
	ts->pid = 0;
	if((ts->pid = fork()) == -1)
		fail("fork failed");

	if(ts->pid == 0)
	{
		// reopen stdin
		close(0);
		if(open("/dev/null", O_RDONLY) != 0)
			fail("open(/dev/stderr)=[%d][%s]", errno, strerror(errno));

		// redirect stdout
		if(dup2(ts->stdo_fd, 1) == -1)
			fail("dup2 failed : [%d][%s]", errno, strerror(errno));

		// redirect stderr
		if(dup2(ts->stde_fd, 2) == -1)
			fail("dup2 failed : [%d][%s]", errno, strerror(errno));

		// irretrievably drop fabsys:fabsys identity
		fatal_os(setresuid, g_args.ruid, g_args.ruid, g_args.ruid);
		fatal_os(setresgid, g_args.rgid, g_args.rgid, g_args.rgid);

		// exec doesnt return
		fatal_os(execl, ts->cmd_path->s, ts->cmd_path->s, (void*)0);
	}

	// reassume user identity
	fatal(identity_assume_user);

	finally : coda;
}

static void fml_free(fml * fml)
{
	if(fml)
	{
		int x;
		for(x = 0; x < fml->evals_l; x++)
		{
			free(fml->evals[x].products);
		}
		free(fml->evals);
	}
	free(fml);
}

void fml_teardown()
{
	int x;
	for(x = 0; x < g_fmls.l; x++)
		fml_free(g_fmls.e[x]);

	free(g_fmls.e);
}
