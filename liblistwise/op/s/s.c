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
#include <string.h>
#include <alloca.h>

#include "listwise/operator.h"
#include "listwise/lwx.h"
#include "listwise/re.h"

#include "liblistwise_control.h"

#include "xmem.h"
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

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "s"
		, .optype				= LWOP_SELECTION_STAGE | LWOP_WINDOWS_STAGE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_validate	= op_validate
		, .op_exec			= op_exec
		, .desc					= "substitution by regex"
	}, {}
};

static int append(lwx * lx, int x, char * s, int l)
{
	int i;
	for(i = 0; i < l; i++)
	{
		if(((i + 1) < l) && s[i] == '\\')
		{
			if(s[i + 1] == 'r')
			  fatal(lstack_append, lx, 0, x, "\r", 1);
			else if(s[i + 1] == 'n')
			  fatal(lstack_append, lx, 0, x, "\n", 1);
			else if(s[i + 1] == '0')
			  fatal(lstack_append, lx, 0, x, "\0", 1);
			else if(s[i + 1] == '\\')
			  fatal(lstack_append, lx, 0, x, "\\", 1);
			else if(s[i + 1] == 't')
			  fatal(lstack_append, lx, 0, x, "\t", 1);
			else if(s[i + 1] == 'x')
			{
			  if((i + 3) < l)
			  {
			    int v;
			    if(parseuint(s + i + 2, SCNx8, 0, 0xFF, 2, 2, &v, 0) == 0)
			    {
			      fatal(lstack_appendf, lx, 0, x, "%c", v);
			    }
			
			    i += 2;
			  }
			}

			i++;
		}
		else
		{
			fatal(lstack_append, lx, 0, x, s + i, 1);
		}
	}

	finally : coda;
}

int op_validate(operation* o)
{
	if(o->argsl == 2 || o->argsl == 3)
	{
		if(o->args[0]->l == 0)
			fail("s -- empty first argument");

		if(o->argsl == 2 || o->args[2]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 3)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[2]->s);
	}
	else
		fail("s -- requires 2 or 3 args, actual: %d", o->argsl);

	o->args[0]->itype = ITYPE_RE;

	// validate backreferences in the substitution string
	int x;
	for(x = 0; x < o->args[1]->refsl; x++)
	{
		if(o->args[1]->refs[x].bref > o->args[0]->re.c_caps)
		{
			fail("backref: %d, captures: %d", o->args[1]->refs[x].bref, o->args[0]->re.c_caps);
		}
	}

	finally : coda;
}

int op_exec(operation* o, lwx* lx, int** ovec, int* ovec_len)
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
		fatal(re_exec, &o->args[0]->re, s, l, 0, ovec, ovec_len);

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
			lstack_clear(lx, 0, x);

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
				if(o->args[1]->refsl)
				{
					fatal(append, lx, x, o->args[1]->s, o->args[1]->refs[0].s - o->args[1]->s);
				}
				else
				{
					fatal(append, lx, x, o->args[1]->s, o->args[1]->l);
				}

				// foreach backreference
				int i;
				for(i = 0; i < o->args[1]->refsl; i++)
				{
					// text in the replacement string between this and the previous backreference
					if(i)
					{
						fatal(append, lx, x, o->args[1]->refs[i-1].e, o->args[1]->refs[i].s - o->args[1]->refs[i-1].e);
					}

					// text of the backreference itself, if the corresponding subcapture was populated
					if((*ovec)[0] > o->args[1]->refs[i].bref)
					{
						int a = (*ovec)[1 + (o->args[1]->refs[i].bref * 2) + 0];
						int b = (*ovec)[1 + (o->args[1]->refs[i].bref * 2) + 1];

						if(a >= 0 && b >= 0)
							fatal(lstack_append, lx, 0, x, ss + a, b - a);
					}
				}

				// text in the replacement string following the last backreference
				if(o->args[1]->refsl)
				{
					fatal(append, lx, x, o->args[1]->ref_last->e, o->args[1]->l - (o->args[1]->ref_last->e - o->args[1]->s));
				}

				// end offset of replacement
				int B = lx->s[0].s[x].l;

				fatal(lstack_window_stage, lx, x, A, B - A);

				loff = (*ovec)[2];

				if(isglobal)
				{
					fatal(re_exec, &o->args[0]->re, ss, ssl, loff, ovec, ovec_len);
				}
			} while(isglobal && (*ovec)[0] > 0);

			// text in the subject string following the last match
			fatal(append, lx, x, ss + loff, ssl - loff);

			fatal(lstack_sel_stage, lx, x);
		}
	}
	LSTACK_ITEREND

finally:
	free(ss);
coda;
}
