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
/// static
///

static int proc_varnode(
	  ff_node * restrict stmt
	, map * const restrict  vmap
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int * const restrict staxp
	, char *** const restrict vrefs
	, int * const restrict vrefsa
	, int * const restrict vrefsl
)
{
	// get the value, if any
	void * v = 0;
	uint8_t t = 0;

	if(stmt->definition)
	{
		if(stmt->definition->type == FFN_LIST)
		{
			fatal(list_resolve, stmt->definition, vmap, stax, staxa, (*staxp), 0);
			v = (*stax)[(*staxp)++];
			t = 1;
		}
		else if(stmt->definition->type == FFN_VARREF)
		{
			v = stmt->definition->text;
			t = 0;
		}
	}
	else if(stmt->type == FFN_VARPUSH)
	{
		v = listwise_identity;
		t = 1;
	}

	// apply to all referenced vars
	int y;
	for(y = 0; y < stmt->varsl; y++)
	{
		char * var = stmt->vars[y]->text;
		int r = 0;

		if(stmt->type == FFN_VARASSIGN)
		{
			// clear the stack for this variable
			fatal(var_undef, vmap, var, &r, stmt);

			// r is whether it was actually cleared, which is prevented by a sticky value on top
			if(r && v)
			{
				if(t)
					fatal(var_push_list, vmap, var, 0, v, stmt);
				else
					fatal(var_push_alias, vmap, var, 0, v, stmt);
			}
		}
		else if(stmt->type == FFN_VARPUSH)
		{
			if(t)
				fatal(var_push_list, vmap, var, 0, v, stmt);
			else
				fatal(var_push_alias, vmap, var, 0, v, stmt);
		}
		else if(stmt->type == FFN_VARPOP || stmt->type == FFN_VARDESIGNATE)
		{
			if(v)
			{
				if(t)
					fatal(var_push_list, vmap, var, 0, v, stmt);
				else
					fatal(var_push_alias, vmap, var, 0, v, stmt);
			}
		}

		if(vrefs)
		{
			if(    (stmt->type == FFN_VARASSIGN && r && v)
					|| (stmt->type == FFN_VARPUSH)
					|| (stmt->type == FFN_VARDESIGNATE)
					|| (stmt->type == FFN_VARPOP && v)
			)
			{
				if((*vrefsl) == (*vrefsa))
				{
					int ns = (*vrefsa) ?: 10;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, vrefs, sizeof(**vrefs), ns, (*vrefsa));
					(*vrefsa) = ns;
				}

				(*vrefs)[(*vrefsl)++] = var;
			}
		}
	}

	finally : coda;
}

///
/// public
///

int ffproc(const ff_parser * const ffp, const path * const restrict inpath, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int * const staxp, gn ** first, const uint32_t flags)
{
	int x;
	int y;
	int i;
	path * pth = 0;

	char ** vrefs = 0;
	int vrefsa = 0;
	int vrefsl = 0;

	// parse
	ff_file * ff = 0;
	fatal(ff_reg_load, ffp, inpath, &ff);

	if(!ff)
		qfail();

	// use up one list and populate the ^ variable (relative directory path to THIS fabfile)
	fatal(list_ensure, stax, staxa, (*staxp));

	if(flags & FFP_MODULE)
	{
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->abs_dir, ff->path->abs_dirl);
	}
	else
	{
		fatal(lstack_add, (*stax)[(*staxp)], ff->path->rel_dir, ff->path->rel_dirl);
	}

	fatal(var_push_list, vmap, "^", 0, (*stax)[(*staxp)++], 0);

	// process the fabfile tree, construct the graph
	for(x = 0; x < ff->ffn->statementsl; x++)
	{
		ff_node* stmt = ff->ffn->statements[x];

		if(stmt->type == FFN_INVOCATION)
		{
			int pn = *staxp;
			fatal(list_resolve, stmt->modules, vmap, stax, staxa, pn, 0);

			// iterate all referenced modules
			LSTACK_ITERATE((*stax)[pn], y, go);
			if(go)
			{
				// module reference string
				char * s;
				int l;
				fatal(lstack_string, (*stax)[pn], 0, y, &s, &l);

//printf("INVOKE %.*s @ %d\n", l, s, y);

				// handle module notation 
				int ismod = 0;
				if(l >= 5 && memcmp(s, "/../", 4) == 0)
				{
					ismod = 1;
					for(i = 0; i < g_args.invokedirsl; i++)
					{
						// last component is understood as a filename
						char * f = s + l - 1;
						int fl = 1;

						// previous components understood as a path
						char * p = s + 4;
						int pl = l - 4;

						while(f != s && f[0] != '/')
						{
							f--;
							fl++;
						}

						if(f == s)
							fail("invalid invocation %.*s", l, s);
						else
						{
							pl -= fl;
							f++;
							fl--;
						}

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
						fail("invocation not found %.*s", l, s);
					}
				}
				else
				{
					fatal(path_create, &pth, ff->path->rel_dir, "%.*s", l, s);
				}

				// for a gated invocation, push an empty value onto all enclosed variables
				// which are not the target of some designation
				if(stmt->flags & FFN_GATED)
				{
					// push all designations, create list of vars actually pushed onto (vrefs)
					vrefsl = 0;
					for(i = 0; i < stmt->designationsl; i++)
						fatal(proc_varnode, stmt->designations[i], vmap, stax, staxa, staxp, &vrefs, &vrefsa, &vrefsl);

					for(i = 0; i < ff->closure_varsl; i++)
					{
						int j;
						for(j = 0; j < vrefsl; j++)
						{
							if(strcmp(vrefs[j], ff->closure_vars[i]->text) == 0)
								break;
						}

						if(j == vrefsl)
							fatal(var_push_list, vmap, ff->closure_vars[i]->text, 0, listwise_identity, stmt);
					}
				}
				else
				{
					// push values for all designations
					for(i = 0; i < stmt->designationsl; i++)
					{
						fatal(proc_varnode, stmt->designations[i], vmap, stax, staxa, staxp, 0, 0, 0);
					}
				}

				// apply scope for this invocation
				if(stmt->scope)
					fatal(strstack_push, sstk, stmt->scope->text);

				// process the referenced fabfile
				fatal(ffproc, ffp, pth, sstk, vmap, stax, staxa, staxp, 0, ismod);

				// scope pop
				if(stmt->scope)
					strstack_pop(sstk);

				// gate pop
				if(stmt->flags & FFN_GATED)
				{
					for(i = 0; i < ff->closure_varsl; i++)
					{
						int j;
						for(j = 0; j < vrefsl; j++)
						{
							if(strcmp(vrefs[j], ff->closure_vars[i]->text) == 0)
								break;
						}

						if(j == vrefsl)
						{
							if(ff->closure_vars[i]->type != FFN_VARDESIGNATE)
								fatal(var_pop, vmap, ff->closure_vars[i]->text, stmt);
						}
					}
				}
				else
				{
					// pop designations only
					for(i = 0; i < stmt->designationsl; i++)
					{
						int k;
						for(k = 0; k < stmt->designations[i]->varsl; k++)
							fatal(var_pop, vmap, stmt->designations[i]->vars[k]->text, stmt->designations[i]);
					}
				}
			}
			LSTACK_ITEREND;
		}
		else if(stmt->type == FFN_DEPENDENCY)
		{
			fatal(dep_process, stmt, sstk, vmap, stax, staxa, (*staxp), first, 0, 0, 0);
			if(first && *first)
				first = 0;
		}
		else if(stmt->type == FFN_FORMULA)
		{
			fatal(fml_attach, stmt, sstk, vmap, stax, staxa, (*staxp));
		}
		else if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARPUSH || stmt->type == FFN_VARPOP)
		{
			fatal(proc_varnode, stmt, vmap, stax, staxa, staxp, 0, 0, 0);
		}
	}

	fatal(var_pop, vmap, "^", 0);

finally:
	path_free(pth);
	free(vrefs);
coda;
}
