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

#include "listwise.h"

#include "fml.h"
#include "args.h"
#include "params.h"
#include "gn.h"
#include "ts.h"
#include "ff/ff.tokens.h"
#include "list.h"
#include "lwutil.h"
#include "identity.h"
#include "enclose.h"
#include "var.h"
#include "logs.h"

#include "global.h"
#include "xlinux.h"
#include "map.h"
#include "dirutil.h"
#include "macros.h"

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

static int fml_attach_singly(fml * const restrict fml, strstack * const restrict sstk, fmlctx * const restrict ctx, lwx * const restrict ls)
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
		fmlv->ctx = ctx;
		fmlv->flags = fml->ffn->flags;

		// get the target graph node
		char * s = 0;
		int l = 0;
		fatal(lstack_getstring, ls, 0, y, &s, &l);

		gn * t = 0;
		fatal(gn_add
			, g_args->init_fabfile_path->abs_dir
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

			logf(L_DSC | L_FML, "dsc(%s)[%3d,%3d - %3d,%3d] -> %s"
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

			logf(L_FAB | L_FML, "reg(%s)[%3d,%3d - %3d,%3d] -> %s"
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

static int fml_attach_multi(fml * const restrict fml, strstack * const restrict sstk, fmlctx * const restrict ctx, lwx * const restrict ls)
{
	int x;
	int y;

	// multi-target formula
	for(x = 0; x < lwx_lists(ls); x++)
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
		fmlv->ctx = ctx;
		fmlv->flags = fml->ffn->flags;

		if(fmlv->flags & FFN_FABRICATION)
		{
			log_start(L_FAB | L_FML);
			logf(0, "reg(%s)[%3d,%3d - %3d,%3d] -> { "
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
			);
		}

		// get the target graph nodes
		fmlv->productsl = lwx_rows(ls, x);
		fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->productsl);

		for(y = 0; y < fmlv->productsl; y++)
		{
			char * rv;
			int rl;
			fatal(lstack_getstring, ls, x, y, &rv, &rl);

			gn * t = 0;
			fatal(gn_add
				, g_args->init_fabfile_path->abs_dir
				, sstk
				, rv, rl
				, &t
				, 0
			);

			// update affected lists
			fmlv->products[y] = t;
			fatal(ff_regular_enclose_gn, fml->ffn->loc.ff, t);

			if(y)
				logf(0, ", %s", t->idstring);
			else
				logf(0, "%s", t->idstring);

			if(fmlv->flags & FFN_FABRICATION)
			{
				t->fabv = fmlv;
			}
		}
		logs(0, " }");
		log_finish();
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

		for(x = 0; x < fml->ctxsl; x++)
		{
			free(fml->ctxs[x]->locs);
			map_free(fml->ctxs[x]->bag);
			free(fml->ctxs[x]);
		}
		free(fml->ctxs);

		free(fml->closure_vars);
	}
	free(fml);
}

//
// public
//
int fml_attach(ff_node * const restrict ffn, strstack * const restrict sstk, struct ff_loc ** const restrict loc, const int locl, map * const restrict vmap, transform_parser * const gp, lwx *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
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

	// reallocate context list if necessary
	if(fml->ctxsl == fml->ctxsa)
	{
		int ns = fml->ctxsa ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &fml->ctxs, sizeof(*fml->ctxs), ns, fml->ctxsa);
		fml->ctxsa = ns;
	}
	
	// create a new context
	fatal(xmalloc, &fml->ctxs[fml->ctxsl], sizeof(*fml->ctxs[0]));
	fatal(map_create, &fml->ctxs[fml->ctxsl]->bag, 0);
	fatal(xmalloc, &fml->ctxs[fml->ctxsl]->locs, sizeof(*fml->ctxs[0]->locs) * locl);
	memcpy(fml->ctxs[fml->ctxsl]->locs, loc, sizeof(*loc) * locl);
	fml->ctxs[fml->ctxsl]->locsl = locl;
	fml->ctxs[fml->ctxsl]->fml = fml;

	fml->ctxsl++;

/*
	// create the bag
	if(fml->bagsl == fml->bagsa)
	{
		int ns = fml->bagsa ?: 10;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &fml->bags, sizeof(*fml->bags), ns, fml->bagsa);
		fml->bagsa = ns;
	}

	fatal(map_create, &fml->bags[fml->bagsl++], 0);
*/

	// populate the bag
	map * bag = fml->ctxs[fml->ctxsl - 1]->bag;
	lwx * ls = 0;
	int x = 0;
	for(x = 0; x < fml->closure_varsl; x++)
	{
		fatal(var_access, vmap, fml->closure_vars[x]->name->text->s, stax, staxa, staxp, &ls);
		fatal(map_set, bag, fml->closure_vars[x]->name->text->s, fml->closure_vars[x]->name->text->l, MM(ls), 0);
	}

	fatal(var_access, vmap, "#", stax, staxa, staxp, &ls);
	fatal(map_set, bag, MMS("#"), MM(ls), 0);

	fatal(var_access, vmap, "*", stax, staxa, staxp, &ls);
	fatal(map_set, bag, MMS("*"), MM(ls), 0);

	// resolve targets lists
	fatal(lw_reset, stax, staxa, *staxp);

	if(ffn->targets_0)
	{
		int pn = *staxp;
		fatal(list_resolvetoflat, ffn->targets_0, vmap, gp, stax, staxa, &pn, 0, 0);
	}

	if(ffn->targets_1)
	{
		int pn = *staxp;
		fatal(list_resolvetoflat, ffn->targets_1, vmap, gp, stax, staxa, &pn, 0, 0);
	}

	// create fmlv(s) and attach graph nodes
	if(ffn->flags & FFN_SINGLE)
	{
		fatal(fml_attach_singly, fml, sstk, fml->ctxs[fml->ctxsl - 1], (*stax)[*staxp]);
	}
	else if(ffn->flags & FFN_MULTI)
	{
		fatal(fml_attach_multi, fml, sstk, fml->ctxs[fml->ctxsl - 1], (*stax)[*staxp]);
	}

	finally : coda;
}

int fml_render(ts * const restrict ts, transform_parser * const gp, lwx *** const restrict stax, int * const restrict staxa, int staxp, map * const restrict rawvars, int shebang)
{
	// resolve the command list
	int pn = staxp;
	fatal(list_resolve, ts->fmlv->ctx->fml->ffn->command, ts->fmlv->ctx->bag, gp, stax, staxa, &pn, 1, rawvars);

	// psprintf 1) allocates the pstring if necessary, and 2) sets the length
	fatal(psprintf, &ts->cmd_txt, shebang ? "#!/bin/bash\n\n" : "");
	fatal(list_renderto, (*stax)[staxp], &ts->cmd_txt);

char * gn_idstring(struct gn *);

finally:
	if(ts->fmlv->flags & FFN_DISCOVERY)
		XAPI_INFOS("target", gn_idstring(ts->fmlv->target));
	else
		XAPI_INFOS("product[0]", gn_idstring(ts->fmlv->products[0]));
coda;
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
	fatal(psprintf, &ts->stdo_path, XQUOTE(FABTMPDIR) "/pid/%d/fml/%d", g_params.pid, num);
	fatal(mkdirp, ts->stdo_path->s, S_IRWXU | S_IRWXG | S_IRWXO);

	// the directories are fabsys:fabsys, but the files are user:user
	fatal(identity_assume_user);

	// create tmp file for the cmd
	fatal(psprintf, &ts->cmd_path, XQUOTE(FABTMPDIR) "/pid/%d/fml/%d/cmd", g_params.pid, num);
	fatal(xopen_mode, ts->cmd_path->s, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU | S_IRWXG, &ts->cmd_fd);

	// write the cmd to the tmp file
	fatal(xwrite, ts->cmd_fd, ts->cmd_txt->s, ts->cmd_txt->l, 0);
	fatal(xclose, ts->cmd_fd);

	// create tmp file to capture stdout, remain-through-exec
	fatal(psprintf, &ts->stdo_path, XQUOTE(FABTMPDIR) "/pid/%d/fml/%d/out", g_params.pid, num);
	fatal(xopen_mode, ts->stdo_path->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &ts->stdo_fd);

	// create tmp file to capture stderr, remain-through-exec
	fatal(psprintf, &ts->stde_path, XQUOTE(FABTMPDIR) "/pid/%d/fml/%d/err", g_params.pid, num);
	fatal(xopen_mode, ts->stde_path->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &ts->stde_fd);

	// fork off the child
	ts->pid = 0;
	fatal(xfork, &ts->pid);

	if(ts->pid == 0)
	{
		// reopen stdin
		fatal(xclose, 0);
		fatal(xopen, "/dev/null", O_RDONLY, 0);

		// save stdout
		fatal(xdup2, 1, 501);

		// redirect stdout
		fatal(xdup2, ts->stdo_fd, 1);

		// save stderr
		fatal(xdup2, 2, 502);

		// redirect stderr
		fatal(xdup2, ts->stde_fd, 2);

		// irretrievably drop fabsys:fabsys identity
		fatal(xsetresuid, g_params.ruid, g_params.ruid, g_params.ruid);
		fatal(xsetresgid, g_params.rgid, g_params.rgid, g_params.rgid);

		// exec doesnt return
		execl(ts->cmd_path->s, ts->cmd_path->s, (void*)0);

		fail(0);
	}

	finally : coda;
}

void fml_teardown()
{
	int x;
	for(x = 0; x < g_fmls.l; x++)
		fml_free(g_fmls.e[x]);

	free(g_fmls.e);
}
