#include <listwise/lstack.h>

#include "ffproc.h"

#include "fml.h"
#include "list.h"
#include "var.h"
#include "dep.h"
#include "args.h"

#include "control.h"
#include "path.h"
#include "xmem.h"

#define restrict __restrict

///
/// public
///

int ffproc(const ff_parser * const ffp, const path * const restrict inpath, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first, const uint32_t flags)
{
	int x;
	int y;
	int i;
	path * pth = 0;
	pstring * inv = 0;
	map * cmap = 0;

	// parse
	ff_file * ff = 0;
	fatal(ff_reg_load, ffp, inpath, &ff);

	if(!ff)
		qfail();

	// use up one list and populate the * variable (relative directory path to THIS fabfile)
	fatal(list_ensure, stax, staxa, (*staxp));

	if(flags & FFP_MODULE)
	{
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->abs_dir, ff->path->abs_dirl);
	}
	else
	{
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->rel_dir, ff->path->rel_dirl);
	}

	fatal(var_set, vmap, "*", (*stax)[(*staxp)++], 0, 1, 0);

	// process the fabfile tree, construct the graph
	for(x = 0; x < ff->ffn->statementsl; x++)
	{
		ff_node* stmt = ff->ffn->statements[x];

		if(stmt->type == FFN_DEPENDENCY)
		{
			fatal(dep_process, stmt, sstk, vmap, stax, staxa, (*staxp), first, 0, 0, 0);

			if(first && *first)
				first = 0;
		}
		else if(stmt->type == FFN_FORMULA)
		{
			fatal(fml_attach, stmt, sstk, vmap, stax, staxa, (*staxp));
		}
		else if(stmt->type == FFN_VARASSIGN)
		{
			int pn = (*staxp);
			fatal(list_resolve, stmt->definition, vmap, stax, staxa, staxp, 0);

			for(y = 0; y < stmt->varsl; y++)
				fatal(var_set, vmap, stmt->vars[y]->name, (*stax)[pn], 0, 1, stmt);
		}
		else if(stmt->type == FFN_INVOCATION)
		{
			fatal(list_resolve, stmt->module, vmap, stax, staxa, (*staxp), 0);
			fatal(list_render, (*stax)[(*staxp)], &inv);

			// handle module reference
			int ismod = 0;
			if(inv->l >= 5 && memcmp(inv->s, "/../", 4) == 0)
			{
				ismod = 1;

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
				path_xfree(&pth);
				fatal(path_create, &pth, g_args.invokedirs[i]
					, "%.*s/fabfile"
					, inv->l, inv->a
				);

				if(euidaccess(pth->can, F_OK) != 0)
				{
					// otherwise, the invocation string is an exact path to the fabfile
					path_xfree(&pth);
					fatal(path_create
						, &pth
						, ff->path->rel_dir
						, "%.*s"
						, inv->l, inv->s
					);
				}
			}

			map * nmap = 0;

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
								fatal(list_resolve, set->definition, vmap, stax, staxa, (*staxp), 0);

								for(i = 0; i < set->varsl; i++)
									fatal(var_set, cmap, set->vars[i]->name, (*stax)[(*staxp)], 1, 0, set);

								(*staxp)++;
							}
							else if(set->definition->type == FFN_VARREF)
							{
								fatal(var_alias, vmap, set->vars[0]->name, cmap, set->definition->name, set);
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
			else
			{
				nmap = vmap;
			}

			// apply scope for this invocation
			if(stmt->scope)
				fatal(strstack_push, sstk, stmt->scope->text);

			// process the referenced fabfile
			fatal(ffproc, ffp, pth, sstk, nmap, stax, staxa, staxp, 0, ismod);

			// scope pop
			if(stmt->scope)
				strstack_pop(sstk);
		}
	}

finally:
	path_free(pth);
	pstring_free(inv);
	map_free(cmap);
coda;
}
