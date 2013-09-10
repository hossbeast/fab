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

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "liblistwise_control.h"

#include "color.h"
#include "macros.h"

/*

m operator - window regex matches

ARGUMENTS
	 1  - offset to start of window
		    if negative, interpreted as offset from the end of the string
		    default : 0
	*2  - length of window
		    default : 0 - entire string

OPERATION

	1. if nothing selected, select all
	2. foreach selected string
		3. replace that string with a slice of itself specified by the arguments

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc[] = {
	{
		  .s						= "hl"
		, .optype				= LWOP_SELECTION_READ | LWOP_ARGS_CANHAVE | LWOP_MODIFIERS_CANHAVE | LWOP_OPERATION_INPLACE
		, .op_exec			= op_exec
		, .desc					= "highlight windows"
	}
	, {}
};

static int op_exec_hl(operation* o, lstack* ls, int** ovec, int* ovec_len, int win, int win_off, int win_len, struct re * re)
{
	char * ss = 0;

	int isglobal = 0;
	if(re)
	{
		isglobal = o->argsl == 2 && o->args[1]->l && strchr(o->args[1]->s, 'g');
	}

	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		// copy of the starting string
		char * zr;
		int ssl;
		fatal(lstack_string, ls, 0, x, &zr, &ssl);
		
		char * ss = 0;
		fatal(xmalloc, &

		char * ss = alloca(ssl + 1);
		memcpy(ss, ls->s[0].s[x].s, ssl);
		ss[ssl] = 0;

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

finally:
	free(ss);
coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	return op_exec_hl(o, ls, ovec, ovec_len, 0, 0, 0, &o->args[0]->re);
}
