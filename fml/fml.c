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
int fml_add(ff_node * ffn, lstack * ls)
{
	int x;
	int y;
	int i;

	// ffn is an FFN_FORMULA
	fml * fml = 0;
	fatal(coll_doubly_add, &g_fmls.c, 0, &fml);
	fml->ffn = ffn;

	// attach graph nodes
	if(ls->l == 1)
	{
		// single-target formula
		fml->evals_l = 1;
		fatal(xmalloc, &fml->evals, sizeof(fml->evals[0]) * fml->evals_l);
		fmleval * fmlv = &fml->evals[0];
		fmlv->fml = fml;
		fmlv->type = FMLEVAL_SINGLE;

		LSTACK_ITERATE(ls, y, go);
		if(go)
		{
			gn* t = 0;
			if(ls->s[0].s[y].l && ls->s[0].s[y].s[0] == '/')
				t = idx_lookup_val(gn_nodes.by_path, &ls->s[0].s[y].s, 0);
			else
			{
				int i;
				for(i = 0; i < gn_nodes.l; i++)
				{
					if(strcmp(gn_nodes.e[i]->dir, ffn->ff_dir) == 0 && strcmp(gn_nodes.e[i]->name, ls->s[0].s[y].s) == 0)
					{
						t = gn_nodes.e[i];
						break;
					}
				}
			}

			if(t)
			{
				log(L_FML | L_FMLTARG, "formula -> %s @ [%3d,%3d - %3d,%3d]"
					, t->path
					, fml->ffn->loc.f_lin + 1
					, fml->ffn->loc.f_col + 1
					, fml->ffn->loc.l_lin + 1
					, fml->ffn->loc.l_col + 1
				);
				t->fmlv = fmlv;
			}
			else
			{
				log(L_ERROR, "formula target unresolved %s @ [%3d,%3d - %3d,%3d]"
					, ls->s[0].s[y].s
					, fml->ffn->loc.f_lin + 1
					, fml->ffn->loc.f_col + 1
					, fml->ffn->loc.l_lin + 1
					, fml->ffn->loc.l_col + 1
				);
			}	
		}
		LSTACK_ITEREND;
	}
	else
	{
		// multi-target formula
		fml->evals_l = ls->l;
		fatal(xmalloc, &fml->evals, sizeof(fml->evals[0]) * fml->evals_l);

		for(x = 0; x < ls->l; x++)
		{
			fmleval * fmlv = &fml->evals[x];
			fmlv->fml = fml;
			fmlv->type = FMLEVAL_MULTI;
			fmlv->products_l = ls->s[x].l;
			fatal(xmalloc, &fmlv->products, sizeof(fmlv->products[0]) * fmlv->products_l);

			for(y = 0; y < ls->s[x].l; y++)
			{
				gn * t = 0;
				if(ls->s[x].s[y].l && ls->s[x].s[y].s[0] == '/')
					t = idx_lookup_val(gn_nodes.by_path, &ls->s[x].s[y].s, 0);
				else
				{
					int i;
					for(i = 0; i < gn_nodes.l; i++)
					{
						if(strcmp(gn_nodes.e[i]->dir, ffn->ff_dir) == 0 && strcmp(gn_nodes.e[i]->name, ls->s[x].s[y].s) == 0)
						{
							t = gn_nodes.e[i];
							break;
						}
					}
				}

				if(t)
				{
					fmlv->products[y] = t;
					t->fmlv = fmlv;
				}
				else
				{
					log(L_ERROR, "formula target unresolved %s @ [%3d,%3d - %3d,%3d]"
						, ls->s[x].s[y].s
						, fml->ffn->loc.f_lin + 1
						, fml->ffn->loc.f_col + 1
						, fml->ffn->loc.l_lin + 1
						, fml->ffn->loc.l_col + 1
					);
				}
			}
		}
	}
}

int fml_render(ts * ts, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p)
{
	// start with shebang
	fatal(psprintf, &ts->cmd_txt, "#!/bin/bash\n\n");

	ff_node * ffn = ts->gn->fml->ffn;

	int k = 0;
	int x;
	for(x = 0; x < ffn->commands_l; x++)
	{
		if(k)
			fatal(pscat, &ts->cmd_txt, " ", 1);

		if(ffn->commands[x]->type == FFN_WORD)
		{
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
