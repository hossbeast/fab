#include "ffproc.h"

#include "fml.h"
#include "list.h"
#include "var.h"
#include "dep.h"
#include "args.h"

#include "control.h"

///
/// static
///

int proc_varnode(ff_node * stmt, map * const vmap, lstack *** const stax, int * const staxa, int staxp)
{
	// get the value, if any
	void * v = 0;
	uint8_t t = 0;

	if(stmt->definition)
	{
		if(stmt->definition->type == FFN_LIST)
		{
			fatal(list_resolve, stmt->definition, vmap, stax, staxa, staxp);
			v = (*stax)[staxp++];
			t = VV_LS;
		}
		else if(stmt->definition->type == FFN_WORD)
		{
			v = stmt->definition->text;
			t = VV_AL;
		}
	}
	else
	{
		if(stmt->type == FFN_VARPUSH || stmt->type == FFN_VARDESIGNATE)
		{
			fatal(list_empty, stax, staxa, staxp);
			v = (*stax)[staxp++];
			t = VV_LS;
		}
	}

	// apply to all referenced vars
	int y;
	for(y = 0; y < stmt->vars_l; y++)
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

int ffproc(const ff_node * const ffn, const ff_parser * const ffp, const char * const dir, strstack * const sstk, map * const vmap, lstack *** const stax, int * const staxa, int staxp, gn ** const first)
{
	int x;
	int y;
	int i;

	// containing fabfile
	ff_file * ff = ffn->loc.ff;

	// use up one list and populate the ^ variable (relative directory path to this fabfile)
	fatal(list_ensure, stax, staxa, staxp);
	fatal(lstack_add, (*stax)[staxp], dir, strlen(dir));
	fatal(var_push, vmap, "^", (*stax)[staxp++], VV_LS, 0);

	// process the fabfile tree, construct the graph
	for(x = 0; x < ffn->statements_l; x++)
	{
		ff_node* stmt = ffn->statements[x];

		if(stmt->type == FFN_INVOCATION)
		{
			int pn = staxp;
			fatal(list_resolve, stmt->modules, vmap, stax, staxa, pn++);

			// iterate all referenced modules
			LSTACK_ITERATE(ls, y, go);
			if(go)
			{
				char * s = 0;
				int l = 0;

				fatal(lstack_string, ls, 0, y, &s, &l);

				// locate the actual file
				if(l >= 4 && memcmp(s, "/../") == 0)
				{
					for(i = 0; i < g_args.invokedirsl; i++)
					{
						
					}
				}

				// for a gated invocation, push an empty value into all affecting variables
				if(stmt->flags & FFN_GATED)
				{
					for(i = 0; i < ff->affecting_varsl; i++)
						fatal(var_push, vmap, ff->affecting_vars[i], 0, 0, 0);
				}

				// push values for all designations
				for(i = 0; i < stmt->designationsl; i++)
					fatal(proc_varnode, stmt->designations[i], stax, staxa, staxp);

				// apply scope for this invocation
				if(stmt->scope)
					fatal(strstack_push, sstk, stmt->scope->text);

				// process the referenced fabfile
				fatal(ffproc);

				// scope pop
				if(stmt->scope)
					fatal(strstack_pop, sstk);

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
						fatal(var_pop, vmap, ff->affecting_vars[i]);
				}
			}
			LSTACK_ITEREND;
		}
		else if(stmt->type == FFN_DEPENDENCY)
		{
			fatal(dep_process, stmt, vmap, stax, staxa, staxp, first, 0, 0, 0);
		}
		else if(stmt->type == FFN_FORMULA)
		{
			fatal(fml_add, stmt, vmap, stax, staxa, staxp);
		}
		else if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARPUSH || stmt->type == FFN_VARPOP)
		{
			fatal(proc_varnode, stmt, vmap, stax, staxa, staxp);
		}
	}

	finally : coda;
}
