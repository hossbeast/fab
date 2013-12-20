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

#include <stdlib.h>

#include "ffn.h"

#include "global.h"
#include "xmem.h"

#define restrict __restrict

//
// [[ API ]]
//

int enclose_vars(ff_node * const restrict root, ff_node *** restrict closure_vars, int * const restrict closure_varsa, int * const restrict closure_varsl)
{
	int find(ff_node * const restrict ffn)
	{
		if(ffn)
		{
			if(ffn->type == FFN_VARREF)
			{
				// skip autovars
				if((ffn->text[0] >= 'a' && ffn->text[0] <= 'z') || (ffn->text[0] >= 'A' && ffn->text[0] <= 'Z') || ffn->text[0] == '_')
				{
					if((*closure_varsl) == (*closure_varsa))
					{
						int ns = (*closure_varsa) ?: 10;
						ns = ns + 2 * ns / 2;

						fatal(xrealloc, closure_vars, sizeof(**closure_vars), ns, (*closure_varsa));
						(*closure_varsa) = ns;
					}

					(*closure_vars)[(*closure_varsl)++] = ffn;
				}
			}

			int x;
			for(x = 0; x < ffn->listl; x++)
				find(ffn->list[x]);

			for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
				find(ffn->nodes_owned[x]);
		}

		finally : coda;
	};

	// find all varrefs
	fatal(find, root);

	// sort
	int cmp(const void * A, const void * B)
	{
		return strcmp((*(ff_node**)A)->text, (*(ff_node**)B)->text);
	};
	qsort(*closure_vars, (*closure_varsl), sizeof(*closure_vars), cmp);

	// uniq
	int j;
	for(j = (*closure_varsl) - 1; j > 0; j--)
	{
		int i;
		for(i = j - 1; i >= 0; i--)
		{
			if(strcmp((*closure_vars)[i]->text, (*closure_vars)[j]->text))
				break;
		}

		if((++i) < j)
		{
			memmove(
				  &(*closure_vars)[i]
				, &(*closure_vars)[j]
				, ((*closure_varsl) - j) * sizeof((*closure_vars)[0])
			);

			(*closure_varsl) -= (j - i);
			j = i;
		}
	}

	finally : coda;
}
