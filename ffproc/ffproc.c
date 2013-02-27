#include <listwise/lstack.h>

#include "ffproc.h"

#include "fml.h"
#include "list.h"
#include "var.h"
#include "dep.h"
#include "args.h"

#include "control.h"
#include "path.h"

#define restrict __restrict

///
/// static
///

static int proc_varnode(ff_node * restrict stmt, map * const restrict  vmap, lstack *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
{
	// get the value, if any
	void * v = 0;
	uint8_t t = 0;

	if(stmt->definition)
	{
		if(stmt->definition->type == FFN_LIST)
		{
			fatal(list_resolve, stmt->definition, vmap, stax, staxa, (*staxp));
			v = (*stax)[(*staxp)++];
			t = VV_LS;
		}
		else if(stmt->definition->type == FFN_VARREF)
		{
			v = stmt->definition->text;
			t = VV_AL;
		}
	}
	else
	{
		if(stmt->type == FFN_VARPUSH || stmt->type == FFN_VARDESIGNATE)
		{
			fatal(list_empty, stax, staxa, (*staxp));
			v = (*stax)[(*staxp)++];
			t = VV_LS;
		}
	}

	// apply to all referenced vars
	int y;
	for(y = 0; y < stmt->varsl; y++)
	{
		char * var = stmt->vars[y]->text;

		if(stmt->type == FFN_VARASSIGN)
		{
			// clear the stack for this variable
			int r;
			fatal(var_undef, vmap, var, &r);

			// r is whether it was actually cleared, which is prevented by a sticky value on top
			if(r && v)
			{
				fatal(var_push, vmap, var, v, t, 0);
			}
		}
		else if(stmt->type == FFN_VARPUSH || stmt->type == FFN_VARDESIGNATE)
		{
			fatal(var_push, vmap, var, v, t, 0);
		}
		else if(stmt->type == FFN_VARPOP)
		{
			if(v)
			{
				fatal(var_push, vmap, var, v, t, 0);
			}
		}
	}

	finally : coda;
}

///
/// public
///

int ffproc(const ff_file * const ff, const ff_parser * const ffp, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int staxp, gn ** const first, const uint32_t flags)
{
	path * pth = 0;
	int x;
	int y;
	int i;

	// use up one list and populate the ^ variable (relative directory path to this fabfile)
	fatal(list_ensure, stax, staxa, staxp);

	if(flags & FFP_MODULE)
	{
		fatal(lstack_add, (*stax)[staxp], ff->path->abs_dir, ff->path->abs_dirl);
	}
	else
	{
		fatal(lstack_add, (*stax)[staxp], ff->path->rel_dir, ff->path->rel_dirl);
	}

	fatal(var_push, vmap, "^", (*stax)[staxp++], VV_LS, 0);

	// process the fabfile tree, construct the graph
	for(x = 0; x < ff->ffn->statementsl; x++)
	{
		ff_node* stmt = ff->ffn->statements[x];

		if(stmt->type == FFN_INVOCATION)
		{
			int p = staxp;
			int pn = p + 1;
			fatal(list_resolve, stmt->modules, vmap, stax, staxa, p);

			// iterate all referenced modules
			LSTACK_ITERATE((*stax)[p], y, go);
			if(go)
			{
				// module reference string
				char * s;
				int l;
				fatal(lstack_string, (*stax)[p], 0, y, &s, &l);

				// handle module notation 
				int ismod = 0;
				if(l >= 4 && memcmp(s, "/../", 4) == 0)
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

				// parse the referenced fabfile
				ff_file * iff = 0;
				fatal(ff_parse_path, ffp, pth, &iff);

				// for a gated invocation, push an empty value into all affecting variables
				if(stmt->flags & FFN_GATED)
				{
					for(i = 0; i < ff->affecting_varsl; i++)
						fatal(var_push, vmap, ff->affecting_vars[i]->text, 0, 0, 0);
				}

				// push values for all designations
				for(i = 0; i < stmt->designationsl; i++)
					fatal(proc_varnode, stmt->designations[i], vmap, stax, staxa, &pn);

				// apply scope for this invocation
				if(stmt->scope)
					fatal(strstack_push, sstk, stmt->scope->text);

				// process the referenced fabfile
				fatal(ffproc, iff, ffp, sstk, vmap, stax, staxa, pn, 0, ismod);

				// scope pop
				if(stmt->scope)
					strstack_pop(sstk);

				// pop designations
				for(i = 0; i < stmt->designationsl; i++)
				{
					int k;
					for(k = 0; k < stmt->designations[i]->varsl; k++)
						fatal(var_pop, vmap, stmt->designations[i]->vars[k]->text);
				}

				// gate pop
				if(stmt->flags & FFN_GATED)
				{
					for(i = 0; i < ff->affecting_varsl; i++)
						fatal(var_pop, vmap, ff->affecting_vars[i]->text);
				}
			}
			LSTACK_ITEREND;
		}
		else if(stmt->type == FFN_DEPENDENCY)
		{
			fatal(dep_process, stmt, sstk, vmap, stax, staxa, staxp, first, 0, 0, 0);
		}
		else if(stmt->type == FFN_FORMULA)
		{
			fatal(fml_add, stmt, sstk, vmap, stax, staxa, staxp);
		}
		else if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARPUSH || stmt->type == FFN_VARPOP)
		{
			int pn = staxp;
			fatal(proc_varnode, stmt, vmap, stax, staxa, &pn);
		}
	}

	fatal(var_pop, vmap, "^");

finally:
	path_free(pth);
coda;
}
