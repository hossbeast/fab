/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/re.h"
#include "listwise/lwx.h"

#include "xlinux.h"
#include "parseint.h"

/*

s operator - substitution by regex (search and replace)

ARGUMENTS

  0  - regex
	1  - replacement string
	*2 - options string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. if regex matches, apply replacement
	3. if the y modifier is present, change the selection to be those items which matched

REMARKS
	if windows exist on a row, the substitution takes place only on the windowed section, and the
	entire row (not just the windowed section) is replaced with the result of that operation
	(? could it be otherwise ?)

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*, void**);

operator op_desc[] = {
	{
		  .s						= "s"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_WINDOWS_STAGE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .mnemonic			= "substitution"
		, .desc					= "search and replace by regex"
	}, {}
};

static int append(lwx * lx, int x, char * s, int l)
{
	fatal(lstack_catw, lx, 0, x, s, l);

	finally : coda;
}

int op_validate(operation* o)
{
	if(o->argsl == 2 || o->argsl == 3)
	{
		if(o->args[0]->l == 0)
			fails(LW_ARGSDOM, "empty argument");

		if(o->argsl == 2 || o->args[2]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 3)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[2]->s);
	}
	else
	{
		failf(LW_ARGSNUM, "expected : 2 or 3, actual : %d", o->argsl);
	}

	o->args[0]->itype = ITYPE_RE;

	// validate backreferences in the substitution string
	int x;
	for(x = 0; x < o->args[1]->refs.l; x++)
	{
		if(o->args[1]->refs.v[x].ref > o->args[0]->re.c_caps)
		{
			failf(LW_ILLREF, "captures : %d, backref: %d", o->args[0]->re.c_caps, o->args[1]->refs.v[x].ref);
		}
	}

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len, void ** udata)
{
	char * ss = 0;
	int ssl = 0;
	int ssa = 0;

	int isglobal = o->argsl == 3 && o->args[2]->l && strchr(o->args[2]->s, 'g');

	int x;
	LSTACK_ITERATE(lx, x, go)
	if(go)
	{
		char * s;
		int l;
		fatal(lstack_getbytes, lx, 0, x, &s, &l);

		// lx->s[0].s[x].s - the string to check and modify
		fatal(re_execute, &o->args[0]->re, s, l, 0, ovec, ovec_len);

		if((*ovec)[0] > 0)
		{
			// copy of the starting string
			if(l >= ssa)
			{
				fatal(xrealloc, &ss, 1, l + 1, ssa);
				ssa = l + 1;
			}
			memcpy(ss, s, l);
			ss[l] = 0;
			ssl = l;

			// offset to the end of the last match
			int loff = 0;

			// clear this string on the stack
			fatal(lstack_clear, lx, 0, x);

			// text in the subject string before the first match
			fatal(append, lx, x, ss, (*ovec)[1]);

			do
			{
				// text in the subject string following the previous match, if any, and preceeding the current match
				if(loff)
				{
					fatal(append, lx, x, ss + loff, (*ovec)[1] - loff);
				}

				// start offset of replacement
				int A = lx->s[0].s[x].l;

				// text in the replacement string before the first backreference
				if(o->args[1]->refs.l)
				{
					fatal(append, lx, x, o->args[1]->s, o->args[1]->refs.v[0].s - o->args[1]->s);
				}
				else
				{
					fatal(append, lx, x, o->args[1]->s, o->args[1]->l);
				}

				// foreach backreference
				int i;
				for(i = 0; i < o->args[1]->refs.l; i++)
				{
					// text in the replacement string between this and the previous backreference
					if(i)
					{
						fatal(append, lx, x, o->args[1]->refs.v[i-1].e, o->args[1]->refs.v[i].s - o->args[1]->refs.v[i-1].e);
					}

					// text of the backreference itself, if the corresponding subcapture was populated
					if((*ovec)[0] > o->args[1]->refs.v[i].ref)
					{
						int a = (*ovec)[1 + (o->args[1]->refs.v[i].ref * 2) + 0];
						int b = (*ovec)[1 + (o->args[1]->refs.v[i].ref * 2) + 1];

						if(a >= 0 && b >= 0)
							fatal(lstack_catw, lx, 0, x, ss + a, b - a);
					}
				}

				// text in the replacement string following the last backreference
				if(o->args[1]->refs.l)
				{
					fatal(append, lx, x, o->args[1]->refs.last->e, o->args[1]->l - (o->args[1]->refs.last->e - o->args[1]->s));
				}

				// end offset of replacement
				int B = lx->s[0].s[x].l;

				if(B - A)
					fatal(lstack_windows_stage, lx, x, A, B - A);

				loff = (*ovec)[2];

				if(isglobal)
				{
					fatal(re_execute, &o->args[0]->re, ss, ssl, loff, ovec, ovec_len);
				}
			} while(isglobal && (*ovec)[0] > 0);

			// text in the subject string following the last match
			fatal(append, lx, x, ss + loff, ssl - loff);

			fatal(lstack_selection_stage, lx, x);
		}
	}
	LSTACK_ITEREND

finally:
	free(ss);
coda;
}
