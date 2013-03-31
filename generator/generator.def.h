/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _GENERATOR_DEF_H
#define _GENERATOR_DEF_H

#include <stdio.h>
#include <stdint.h>

#include "listwise/internal.h"

typedef struct
{
	int							r;			// return value of the parse
	void*						scanner;

	int							argsa;
	int							opsa;
	int							opargsa;

	generator*			g;			// completed generator goes here
} parse_param;

#endif






