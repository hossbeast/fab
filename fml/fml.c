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
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <listwise.h>
#include <listwise/lstack.h>

#include "fml.h"
#include "args.h"
#include "gn.h"
#include "ts.h"
#include "ff/ff.tokens.h"
#include "list.h"
#include "lwutil.h"
#include "identity.h"
#include "enclose.h"
#include "var.h"

#include "control.h"
#include "xmem.h"
#include "map.h"
#include "dirutil.h"

#define restrict __restrict

//
// data
//

union g_fmls_t		g_fmls = { { .size = sizeof(fml) } };

//
// static
//

static int dscv_attach(gn * t, fmleval * fmlv)
{
/*
	int x;
	for(x = 0; x < t->dscvsl; x++)
	{
		if(t->dscvs[x]->fml == fmlv->fml)
		{
			log(L_WARN | L_DSC | L_FML | L_FMLTARG, "dsc(%s)[%3d,%3d - %3d,%3d] -> %s already associated"
				, ff_idstring(fmlv->fml->ffn->loc.ff)
				, fmlv->fml->ffn->loc.f_lin + 1
				, fmlv->fml->ffn->loc.f_col + 1
				, fmlv->fml->ffn->loc.l_lin + 1
				, fmlv->fml->ffn->loc.l_col + 1
				, t->idstring
			);

			return 1;
		}
	}
*/

	if(t->dscvsl == t->dscvsa)
	{
		int ns = t->dscvsa ?: 2;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &t->dscvs, sizeof(*t->dscvs), ns, t->dscvsa);
		t->dscvsa = ns;
	}
	t->dscvs[t->dscvsl++] = fmlv;

	finally : coda;
}

static int fml_attach_singly(fml * const restrict fml, strstack * const restrict sstk, map * const restrict bag, lstack * const restrict ls)
{
	int y;

	LSTACK_ITERATE(ls, y, go);
	if(go)
	{
		// create a new fmleval
		if(fml->evalsl == fml->evalsa)
		{
			int ns = fml->evalsa ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &fml->evals, sizeof(*fml->evals), ns, fml->evalsa);
			fml->evalsa = ns;
		}
		
		fatal(xmalloc, &fml->evals[fml->evalsl++], sizeof(*fml->evals[0]));
		fmleval * fmlv = fml->evals[fml->evalsl - 1];
		fmlv->fml = fml;
		fmlv->bag = bag;
		fmlv->flags = fml->ffn->flags;

		// get the target graph node
		char * s = 0;
		int l = 0;
		fatal(lstack_string, ls, 0, y, &s, &l);

		gn * t = 0;
		fatal(gn_add
			, g_args.init_fabfile_path->abs_dir
			, sstk
			, s
			, l
			, &t
			, 0
		);

		// update affected lists
		fatal(ff_regular_enclose_gn, fml->ffn->loc.ff, t);

		if(fmlv->flags & FFN_DISCOVERY)
		{
			fmlv->target = t;

			log(L_DSC | L_FML | L_FMLTARG, "dsc(%s)[%3d,%3d - %3d,%3d] -> %s"
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
				, t->idstring
			);

			fatal(dscv_attach, t, fmlv);
		}
		else if(fmlv->flags & FFN_FABRICATION)
		{
			fmlv->productsl = 1;
			fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->productsl);
			fmlv->products[0] = t;

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

static int fml_attach_multi(fml * const restrict fml, strstack * const restrict sstk, map * const restrict bag, lstack * const restrict ls)
{
	int x;
	int y;

	// multi-target formula
	for(x = 0; x < ls->l; x++)
	{
		// create a new fmleval
		if(fml->evalsl == fml->evalsa)
		{
			int ns = fml->evalsa ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &fml->evals, sizeof(*fml->evals), ns, fml->evalsa);
			fml->evalsa = ns;
		}
		
		fatal(xmalloc, &fml->evals[fml->evalsl++], sizeof(*fml->evals[0]));
		fmleval * fmlv = fml->evals[fml->evalsl - 1];
		fmlv->fml = fml;
		fmlv->bag = bag;
		fmlv->flags = fml->ffn->flags;

		if(fmlv->flags & FFN_FABRICATION)
		{
			log_start(L_FAB | L_FML | L_FMLTARG, "reg(%s)[%3d,%3d - %3d,%3d] -> { "
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
			);
		}

		// get the target graph nodes
		fmlv->productsl = ls->s[x].l;
		fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->productsl);

		for(y = 0; y < ls->s[x].l; y++)
		{
			gn * t = 0;
			fatal(gn_add
				, g_args.init_fabfile_path->abs_dir
				, sstk
				, ls->s[x].s[y].s
				, ls->s[x].s[y].l
				, &t
				, 0
			);

			// update affected lists
			fmlv->products[y] = t;
			fatal(ff_regular_enclose_gn, fml->ffn->loc.ff, t);

			if(y)
				log_add(", %s", t->idstring);
			else
				log_add("%s", t->idstring);

			if(fmlv->flags & FFN_FABRICATION)
			{
				t->fabv = fmlv;
			}
		}
		log_finish(" }");
	}

	finally : coda;
}

static void fml_free(fml * fml)
{
	if(fml)
	{
		int x;
		for(x = 0; x < fml->evalsl; x++)
		{
			if(fml->evals[x]->flags & FFN_FABRICATION)
				free(fml->evals[x]->products);

			free(fml->evals[x]);
		}
		free(fml->evals);

		for(x = 0; x < fml->bagsl; x++)
		{
			map_free(fml->bags[x]);
		}
		free(fml->bags);

		free(fml->closure_vars);
	}
	free(fml);
}

//
// public
//
int fml_attach(ff_node * const restrict ffn, strstack * const restrict sstk, map * const restrict vmap, generator_parser * const gp, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
{
	// create fml if necessary
	fml * fml = ffn->fml;
	if(fml == 0)
	{
		fatal(coll_doubly_add, &g_fmls.c, 0, &ffn->fml);
		fml = ffn->fml;
		fml->ffn = ffn;

		// compute VARREF closure
		fatal(enclose_vars, ffn, &fml->closure_vars, &fml->closure_varsa, &fml->closure_varsl);
	}

	// create the bag
	if(fml->bagsl == fml->bagsa)
	{
		int ns = fml->bagsa ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &fml->bags, sizeof(*fml->bags), ns, fml->bagsa);
		fml->bagsa = ns;
	}

	fatal(map_create, &fml->bags[fml->bagsl++], 0);

	// populate the bag
	lstack * ls = 0;
	int x = 0;
	for(x = 0; x < fml->closure_varsl; x++)
	{
		fatal(var_access, vmap, fml->closure_vars[x]->text, stax, staxa, staxp, &ls);
		fatal(map_set, fml->bags[fml->bagsl - 1], MMS(fml->closure_vars[x]->text), MM(ls));
	}

	fatal(var_access, vmap, "#", stax, staxa, staxp, &ls);
	fatal(map_set, fml->bags[fml->bagsl - 1], MMS("#"), MM(ls));

	fatal(var_access, vmap, "*", stax, staxa, staxp, &ls);
	fatal(map_set, fml->bags[fml->bagsl - 1], MMS("*"), MM(ls));

	// resolve targets lists
	fatal(lw_reset, stax, staxa, *staxp);

	if(ffn->targets_0)
	{
		int pn = *staxp;
		fatal(list_resolvetoflat, ffn->targets_0, vmap, gp, stax, staxa, &pn, 0);
	}

	if(ffn->targets_1)
	{
		int pn = *staxp;
		fatal(list_resolvetoflat, ffn->targets_1, vmap, gp, stax, staxa, &pn, 0);
	}

	// create fmlv(s) and attach graph nodes
	if(ffn->flags & FFN_SINGLE)
	{
		fatal(fml_attach_singly, fml, sstk, fml->bags[fml->bagsl - 1], (*stax)[*staxp]);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(fml_attach_multi, fml, sstk, fml->bags[fml->bagsl - 1], (*stax)[*staxp]);
	}

	finally : coda;
}

int fml_render(ts * const restrict ts, generator_parser * const gp, lstack *** const restrict stax, int * const restrict staxa, int staxp, int standalone)
{
	// resolve the list of command
	int pn = staxp;
	fatal(list_resolve, ts->fmlv->fml->ffn->command, ts->fmlv->bag, gp, stax, staxa, &pn, 1);

	if(standalone)
	{
		fatal(psprintf, &ts->cmd_txt, "#!/bin/bash\n\n");
		fatal(list_renderto, (*stax)[staxp], &ts->cmd_txt);
	}
	else
	{
		fatal(list_render, (*stax)[staxp], &ts->cmd_txt);
	}

	finally : coda;
}

int fml_exec(ts * const restrict ts, int num)
{
	// assume fabsys identity
	fatal(identity_assume_fabsys);

	//
	// create pid-fml-dir
	//
	//  note : all but the last component of this dir were created in tmp_setup
	//  note : the directory itself cannot already exist, because num is process-unique
	//
	fatal(psprintf, &ts->stdo_path, TMPDIR_BASE "/pid/%d/fml/%d", g_args.pid, num);
	fatal(mkdirp, ts->stdo_path->s, S_IRWXU | S_IRWXG | S_IRWXO);

	// create tmp file for the cmd
	fatal(psprintf, &ts->cmd_path, TMPDIR_BASE "/pid/%d/fml/%d/cmd", g_args.pid, num);
	if((ts->cmd_fd = open(ts->cmd_path->s, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG)) == -1)
		fail("open(%s)=[%d][%s]", ts->cmd_path->s, errno, strerror(errno));

	// write the cmd to the tmp file
	if(write(ts->cmd_fd, ts->cmd_txt->s, ts->cmd_txt->l) == -1)
		fail("write failed : [%d][%s]", errno, strerror(errno));

	close(ts->cmd_fd);

	// create tmp file to capture stdout, remain-through-exec
	fatal(psprintf, &ts->stdo_path, TMPDIR_BASE "/pid/%d/fml/%d/out", g_args.pid, num);
	if((ts->stdo_fd = open(ts->stdo_path->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
		fail("open(%s)=[%d][%s]", ts->stdo_path->s, errno, strerror(errno));

	// create tmp file to capture stderr, remain-through-exec
	fatal(psprintf, &ts->stde_path, TMPDIR_BASE "/pid/%d/fml/%d/err", g_args.pid, num);
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

void fml_teardown()
{
	int x;
	for(x = 0; x < g_fmls.l; x++)
		fml_free(g_fmls.e[x]);

	free(g_fmls.e);
}
