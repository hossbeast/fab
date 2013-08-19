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

#include "listwise/internal.h"
#include "liblistwise_control.h"

int listwise_exec(char* s, int l, char** init, int* initls, int initl, lstack** ls)
{
	// generator parser
	generator_parser* p = 0;

	// generator
	generator* g = 0;

	if(generator_mkparser(&p) != 0)
		fail("mkparser failed\n");

	if(generator_parse(p, s, l, &g) != 0)
		fail("parse failed\n");

	if(lstack_exec(g, init, initls, initl, ls) != 0)
		fail("lstack_exec failed");

	generator_free(g);
	generator_parser_free(p);

	finally : coda;
}
