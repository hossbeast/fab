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

#include <listwise/lstack.h>

#include "ffproc.h"

#include "fml.h"
#include "list.h"
#include "lwutil.h"
#include "var.h"
#include "dep.h"
#include "args.h"

#include "control.h"
#include "path.h"
#include "xmem.h"

#define restrict __restrict

static int procfile(const ff_parser * const ffp, const path * const restrict inpath, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first, char * nofile, int nofilel);

static int procblock(ff_file * ff, ff_node* root, const ff_parser * const ffp, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first, int star)
{
	int x;
	int y;
	int i;
	path * pth = 0;
	map * cmap = 0;
	pstring * inv = 0;

	for(x = 0; x < root->statementsl; x++)
	{
		ff_node* stmt = root->statements[x];

		if(stmt->type == FFN_ONCEBLOCK)
		{
			if(ff->count == 1)
			{
				fatal(procblock, ff, stmt, ffp, sstk, vmap, stax, staxa, staxp, first, star);
			}
		}
		else if(stmt->type == FFN_DEPENDENCY)
		{
			fatal(dep_process, stmt, sstk, vmap, ffp->gp, stax, staxa, (*staxp), first, 0, 0, 0);

			if(first && *first)
				first = 0;
		}
		else if(stmt->type == FFN_FORMULA)
		{
			fatal(fml_attach, stmt, sstk, vmap, ffp->gp, stax, staxa, staxp);
		}
		else if(stmt->type == FFN_VARASSIGN)
		{
			int pn = (*staxp);
			fatal(list_resolve, stmt->definition, vmap, ffp->gp, stax, staxa, staxp, 0, 0);

			for(y = 0; y < stmt->varsl; y++)
				fatal(var_set, vmap, stmt->vars[y]->name, (*stax)[pn], 0, 1, stmt);
		}
		else if(stmt->type == FFN_INVOCATION)
		{
			int pn = (*staxp);
			fatal(list_resolve, stmt->module, vmap, ffp->gp, stax, staxa, &pn, 0, 0);
			fatal(list_render, (*stax)[(*staxp)], &inv);

			// handle module reference
			char * nofile = 0;
			int    nofilel = 0;
			if(inv->l >= 5 && memcmp(inv->s, "/../", 4) == 0)
			{
				nofile = inv->s;
				nofilel = inv->l;

				// last component
				char * f = inv->s + inv->l - 1;
				int fl = 1;

				// previous components
				char * p = inv->s + 4;
				int pl = inv->l - 4;

				while(f != inv->s && f[0] != '/')
				{
					f--;
					fl++;
				}

				if(f == inv->s)
				{
					fail("invalid invocation string : '%.*s'", inv->l, inv->s);
				}
				else
				{
					pl -= fl;
					f++;
					fl--;
				}

				for(i = 0; i < g_args.invokedirsl; i++)
				{
					// look for a module
					path_xfree(&pth);
					fatal(path_create, &pth, g_args.invokedirs[i]
						, "%.*s/%.*s.fab"
						, pl, p
						, fl, f
					);

					// using euidaccess indicates a race condition ..
					if(euidaccess(pth->can, F_OK) == 0)
						break;
				}

				if(i == g_args.invokedirsl)
				{
					fail("invocation not found %.*s", inv->l, inv->s);
				}
			}
			else
			{
				// look for a directory
				for(i = 0; i < g_args.invokedirsl; i++)
				{
					path_xfree(&pth);
					fatal(path_create, &pth, g_args.invokedirs[i]
						, "%.*s/fabfile"
						, inv->l, inv->s
					);

					if(euidaccess(pth->can, F_OK) == 0)
					{
						break;
					}
				}

				if(i == g_args.invokedirsl)
				{
					// otherwise, the invocation string is an exact path to the fabfile
					path_xfree(&pth);
					fatal(path_create
						, &pth
						, g_args.init_fabfile_path->abs_dir
						, "%.*s"
						, inv->l, inv->s
					);
				}
			}

			map * nmap = vmap;

			if(stmt->flags & FFN_SUBCONTEXT)
			{
				// clone inherited context into a new map
				map_xfree(&cmap);
				fatal(var_clone, vmap, &cmap);

				// apply additional var settings to context
				for(y = 0; y < stmt->varsettingsl; y++)
				{
					ff_node * set = stmt->varsettings[y];		// VARLOCK or VARLINK

					if(set->type == FFN_VARLOCK)
					{
						if(set->definition)
						{
							if(set->definition->type == FFN_LIST)
							{
								int pn = (*staxp);
								fatal(list_resolve, set->definition, vmap, ffp->gp, stax, staxa, staxp, 0, 0);

								for(i = 0; i < set->varsl; i++)
									fatal(var_set, cmap, set->vars[i]->name, (*stax)[pn], 1, 0, set);
							}
							else if(set->definition->type == FFN_VARREF)
							{
								fatal(var_alias, vmap, set->definition->name, cmap, set->vars[0]->name, set);
							}
						}
						else
						{
							for(i = 0; i < set->varsl; i++)
								fatal(var_alias, vmap, set->vars[i]->name, cmap, set->vars[i]->name, set);
						}
					}
					else if(set->type == FFN_VARLINK)
					{
						if(set->definition)
						{
							fatal(var_link, vmap, set->vars[0]->name, cmap, set->definition->name, set);
						}
						else
						{
							for(i = 0; i < set->varsl; i++)
							{
								fatal(var_link, vmap, set->vars[i]->name, cmap, set->vars[i]->name, set);
							}
						}
					}
				}

				nmap = cmap;
			}

			// apply scope for this invocation
			if(stmt->scope)
			{
				for(i = 1; i < stmt->scope->partsl; i++)
					fatal(strstack_push, sstk, stmt->scope->parts[i]);
			}

			// process the referenced fabfile
			fatal(procfile, ffp, pth, sstk, nmap, stax, staxa, staxp, 0, nofile, nofilel);

			// scope pop
			if(stmt->scope)
			{
				for(i = 1; i < stmt->scope->partsl; i++)
					strstack_pop(sstk);
			}

			// has possibly been overridden
			fatal(var_set, vmap, "*", (*stax)[star], 0, 1, 0);
		}
	}

finally:
	pstring_free(inv);
	path_free(pth);
	map_free(cmap);
coda;
}

int procfile(const ff_parser * const ffp, const path * const inpath, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first, char * nofile, int nofilel)
{
	ff_file * ff = 0;

	if(log_would(L_INVOKE))
	{
		char * sstr = 0;
		fatal(strstack_string, sstk, "/", "/", &sstr);

		log(L_INVOKE, "%s @ %s", inpath->can, sstr);
	}

	// parse
	fatal(ff_reg_load, ffp, inpath, nofile, nofilel, &ff);

	if(!ff)
		qfail();

	ff->count++;

	// use up one list and populate the * variable (path to the directory of the fabfile)
	fatal(lw_reset, stax, staxa, (*staxp));

	if(g_args.mode_paths == MODE_ABSOLUTE)
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->abs_dir, ff->path->abs_dirl);
	else if(g_args.mode_paths == MODE_RELATIVE_FABFILE_DIR)
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->rel_fab_dir, ff->path->rel_fab_dirl);

	int star = (*staxp)++;
	fatal(var_set, vmap, "*", (*stax)[star], 0, 1, 0);

	// process the fabfile tree, construct the graph
	fatal(procblock, ff, ff->ffn, ffp, sstk, vmap, stax, staxa, staxp, first, star);

	finally : coda;
}

///
/// public
///

int ffproc(const ff_parser * const ffp, const path * const restrict inpath, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first)
{
	strstack * sstk = 0;

	// create stack for scope resolution
	fatal(strstack_create, &sstk);
	fatal(strstack_push, sstk, "..");

	fatal(procfile, ffp, inpath, sstk, vmap, stax, staxa, staxp, first, 0, 0);

finally :
	strstack_free(sstk);
coda;
}
