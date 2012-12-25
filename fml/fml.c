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
#include "map.h"

#include "control.h"
#include "xmem.h"

//
// data
//

union g_fmls_t		g_fmls = { { .size = sizeof(fml) } };

//
// static
//

static int fml_add_single(fml * fml, lstack * ls)
{
	int R = 1;
	int y;
	int go;

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

		gn* t = 0;
		if((t = gn_lookup(s, l, fml->ffn->loc.ff->dir)) == 0)
			fatal(gn_add, fml->ffn->loc.ff->dir, s, l, &t, 0);

		fmlv->products[0] = t;

		if(fml->ffn->flags & FFN_DISCOVERY)
		{
			log(L_DSC | L_FML | L_FMLTARG, "dsc(%s)[%3d,%3d - %3d,%3d] -> %s"
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
				, gn_idstring(t)
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
				, gn_idstring(t)
			);
			t->fabv = fmlv;
		}
	}
	LSTACK_ITEREND;

	return R;
}

static int fml_add_multi(fml * fml, lstack * ls)
{
	int R = 1;

	int x;
	int y;
	int i;

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
			log_start(L_DSC | L_FML | L_FMLTARG, "(%s)[%3d,%3d - %3d,%3d] -> {\n"
				, ff_idstring(fml->ffn->loc.ff)
				, fml->ffn->loc.f_lin + 1
				, fml->ffn->loc.f_col + 1
				, fml->ffn->loc.l_lin + 1
				, fml->ffn->loc.l_col + 1
			);
		}
		else
		{
			log_start(L_FAB | L_FML | L_FMLTARG, "(%s)[%3d,%3d - %3d,%3d] -> {\n"
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
			if((t = gn_lookup(ls->s[x].s[y].s, ls->s[x].s[y].l, fml->ffn->loc.ff->dir)) == 0)
				fatal(gn_add, fml->ffn->loc.ff->dir, ls->s[x].s[y].s, ls->s[x].s[y].l, &t, 0);

			if(y)
				log_add("  , %s\n", gn_idstring(t));
			else
				log_add("    %s\n", gn_idstring(t));

			fmlv->products[y] = t;
			if(fml->ffn->flags & FFN_DISCOVERY)
				t->dscv = fmlv;
			else
				t->fabv = fmlv;
		}
		log_finish("}");
	}

	return R;
}

//
// public
//
int fml_add(ff_node * ffn, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p)
{
	// create fml with ffn - which is an FFN_FORMULA
	fml * fml = 0;
	fatal(coll_doubly_add, &g_fmls.c, 0, &fml);
	fml->ffn = ffn;

	// resolve targets list
	fatal(list_resolve, ffn->targets, vmap, stax, stax_l, stax_a, p);

	// attach graph nodes
	if(fml->ffn->flags & FFN_SINGLE)
	{
		fatal(fml_add_single, fml, (*stax)[p]);
	}
	else if(fml->ffn->flags & FFN_MULTI)
	{
		fatal(fml_add_multi, fml, (*stax)[p]);
	}
	else
	{
		fail("bad flags %hhu (no cardinality)", fml->ffn->flags);
	}

	return 1;
}

int fml_render(ts * ts, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p)
{
	// start with shebang
	fatal(psprintf, &ts->cmd_txt, "#!/bin/bash\n\n");

	ff_node * ffn = ts->fmlv->fml->ffn;

	int k = 0;
	int x;
	for(x = 0; x < ffn->commands_l; x++)
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
			fatal(list_resolve, ffn->commands[x], vmap, stax, stax_l, stax_a, p);

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
		for(x = 3; x < 6; x++)
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
