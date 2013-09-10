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

#include <listwise/operator.h>

#include "liblistwise_control.h"

/*

re operator - execute regex : select by regex, and window regex matches
m operator  - window regex matches
fm operator - window by fields delimited by regex matches

ARGUMENTS

  0  - regex
	1* - options string

OPERATION

	1. foreach item in selection, or, if no selection, in top list
	2. select that item if regex matches, subject to options

*/

static int op_validate_re(operation* o);
static int op_validate_fm(operation* o);
static int op_exec_re(operation*, lstack*, int**, int*);
static int op_exec_m(operation*, lstack*, int**, int*);
static int op_exec_fm(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "re"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_re
		, .op_exec			= op_exec_re
		, .desc					= "select by regex and window regex matches"
	}
	, {
		  .s						= "m"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_MODIFIERS_CANHAVE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_m
		, .op_exec			= op_exec_re
		, .desc					= "window by regex"
	}
	, {
		  .s						= "f"
		, .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
		, .op_validate	= op_validate_f
		, .op_exec			= op_exec_f
		, .desc					= "window by substring"
	}
	, {}
};

static int op_validate(operation* o, char * name)
{
	if(o->argsl == 1 || o->argsl == 2)
	{
		if(o->args[0]->l == 0)
			fail("%s -- empty first argument", name);

		if(o->argsl == 1 || o->args[1]->l == 0)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, 0);
		else if(o->argsl == 2)
			fatal(re_compile, o->args[0]->s, &o->args[0]->re, o->args[1]->s);
	}
	else
		fail("%s -- arguments : %d", name, o->argsl);

	o->args[0]->itype = ITYPE_RE;

	finally : coda;
}

static int op_validate_f(operation* o)
{
	if(o->argsl == 1) { }
	else
		fail("f -- arguments : %d", name, o->argsl);

	finally : coda;
}

static int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len, int win, int win_off, int win_len)
{
	int isglobal = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'g');

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		char * ss;
		int ssl;
		fatal(lstack_string, ls, 0, x, &ss, &ssl);

		int off = 0;
		int len = 0;

		if(re)
		{
			fatal(re_exec, &o->args[0]->re, ss, ssl, 0, ovec, ovec_len);

			if((*ovec)[0] > 0)
			{
				off = (*ovec)[1];
				len = (*ovec)[2] - (*ovec)[1];
			}
		}
		else if(win)
		{
			off = win_off;
			len = win_len;

			if(win_off < 0)
				off = ssl + win_off;
			if(win_len == 0)
				len = ssl - off;
			else
				len = MIN(len, ssl - off);
		}

		if(off < ssl && len > 0)
		{
			// offset to the end of the last match
			int loff = 0;

			// clear this string on the stack
			lstack_clear(ls, 0, x);

			// text in the subject before the first match
			fatal(lstack_append, ls, 0, x, ss, off);

			do
			{
				if(loff)
				{
					fatal(lstack_append, ls, 0, x, ss + loff, (*ovec)[1] - loff);
				}

				// write new string using the window
				fatal(lstack_append, ls, 0, x, COLOR(RED));
				fatal(lstack_append, ls, 0, x, ss + off, len);
				fatal(lstack_append, ls, 0, x, NOCOLOR);

				loff = off + len;
				off = len = 0;

				if(isglobal)
				{
					fatal(re_exec, &o->args[0]->re, ss, ssl, loff, ovec, ovec_len);

					if((*ovec)[0])
					{
						off = (*ovec)[1];
						len = (*ovec)[2] - (*ovec)[1];
					}
				}
			} while(off < ssl && len > 0);

			// text in the subject following the last match
			fatal(lstack_append, ls, 0, x, ss + loff, ssl - loff);

			// record this index was hit
			fatal(lstack_last_set, ls, x);
		}
	}
	LSTACK_ITEREND
}

int op_validate_re(operation* o)
{
	return op_validate(o, "re");
}

int op_validate_m(operation* o)
{
	return op_validate(o, "m");
}

int op_validate_f(operation* o)
{
	return op_validate_f(o);
}

int op_exec_re(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len);
}

int op_exec_m(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec(o, ls, ovec, ovec_len);
}
