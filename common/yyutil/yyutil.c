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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yyutil.h"

#include "macros.h"

void yyu_locreset(yyu_extra * xtra, yyu_location * lloc)
{
	// increment line number
	xtra->loc.f_lin++;
	xtra->loc.l_lin = xtra->loc.f_lin;

	// reset columns
	xtra->loc.f_col = 0;
	xtra->loc.l_col = 0;
}

void yyu_locwrite(yyu_extra* xtra, yyu_location* lloc, char * text, int leng, int del)
{
	// update running location track
	xtra->loc.f_col = xtra->loc.l_col;
	xtra->loc.l_col = xtra->loc.f_col + leng;
	xtra->loc.s     = text;
	xtra->loc.e     = text + leng;

	// assign location to the current token
	lloc->f_col = xtra->loc.f_col + del;
	lloc->l_col = xtra->loc.l_col + del - 1;
	lloc->f_lin = xtra->loc.f_lin;
	lloc->l_lin = xtra->loc.l_lin;
	lloc->s = xtra->loc.s + del;
	lloc->e = xtra->loc.s - del - 1;

	xtra->loc.f_col += del;
	xtra->loc.l_col += del;
	xtra->loc.s += del;
	xtra->loc.e -= del;
}

void yyu_error(yyu_location * loc, void * scanner, yyu_extra * xtra, char const * err)
{
	xtra->r = 0;
	YYU_ESAY(
}

void yyu_pushstate(yyu_extra * xtra, int state, const int line)
{
	if(xtra->info)
	{
		int al = snprintf(xtra->space, sizeof(xtra->space), "%s -> %s"
			, STATENAME(TOPSTATE)
			, STATENAME(a)
		);

		xtra->info("(%2d) %.*s %*s @ %*s %d\n"
			, xtra->states_n
			, al
			, xtra->space
			, MAX(58 - al, 0)
			, ""
			, 21
			, ""
			, line
		);
	}

	yyextra->states[yyextra->states_n++] = state;
}

void yyu_popstate(yyu_extra * xtra, const int line)
{
	int x = TOPSTATE;
	yy_pop_state(xtra->scanner);
	xtra->states_n--;

	if(xtra->info)
	{
		int al = snprintf(xtra->space, sizeof(xtra->space), "%s <- %s"
			, STATENAME(TOPSTATE)
			, STATENAME(x)
		);

		xtra->info("(%2d) %.*s %*s @ %*s %d\n"
			, xtra->states_n
			, al
			, xtra->space
			, MAX(58 - al, 0)
			, ""
			, 21
			, ""
			, line
		);
	}
}

int yyu_topstate(yyu_extra * const xtra)
{
	return xtra->states_n ? xtra->states[xtra->states_n-1] : 0;
}

void yyu_ptoken(const int token, void * const lval, yyu_location * const lloc, yyu_extra * const xtra, char * text, const int leng, const int line)
{
	int alen = xstresc(xtra->space, sizeof(xtra->space), text, leng);

	char * bbuf = 0;
	size_t blen = 0;
	if(xtra->lvalstr)
		xtra->lvalstr(token, lval, xtra, &bbuf, &blen);

	if(xtra->info)
	{
		xtra->info("%8s ) '%.*s'%.*s %*s @ [%3d,%3d - %3d,%3d] - %d\n"
			, extra->tokname(token)
			, alen
			, xtra->space
			, blen
			, bbuf
			, MAX(50 - al - blen, 0)
			, ""
			, lloc->f_lin
			, lloc->f_col
			, lloc->l_lin
			, lloc->l_col
			, line
		);
	}
}

void yyu_error(yyu_location * const restrict lloc, void * const restrict scanner, yyu_extra * const restrict xtra, char const * err)
{
	xtra->r = 1;

	if(xtra->error)
		xtra->error("%s", err);

	if(xtra->error)
	{
		int t           = xtra->last_tok;
		const char * s  = xtra->last_loc->s;
		const char * e  = xtra->last_loc->e;
		int l           = e - s;
		ff_loc * lc     = pp->last_loc;
		
		// log last good token
		log(L_ERROR | L_FF, "last token - %s '%.*s' @ (%s)[%3d,%3d - %3d,%3d]"
			, xtra->tokname(xtra->last_tok)
		  , MIN(t == ff_LF ? 0 : t == ff_WS ? 0 : l, 50)
		  , t == ff_LF ? "" : t == ff_WS ? "" : s
		  , ff_idstring(pp->ff)
		  , xtra->last_loc.f_lin + 1
		  , xtra->last_loc.f_col + 1
		  , xtra->last_loc.l_lin + 1
		  , xtra->last_loc.l_col + 1
		);
	}
}

#undef restrict
#endif
