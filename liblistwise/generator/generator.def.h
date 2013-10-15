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

#ifndef _GENERATOR_DEF_H
#define _GENERATOR_DEF_H

#include <stdio.h>
#include <stdint.h>

#include "listwise/internal.h"

typedef struct
{
	generator*			g;				// completed generator goes here
	void*						scanner;

	int							states[64];		// start states stack
	int							states_n;

	char						space[256];		// temp space
	char						space2[256];

	int							argsa;
	int							opsa;
	int							opargsa;

	int							r;						// return value of the parse ; zeroed in yyerror
} parse_param;

#endif
