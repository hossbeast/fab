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

#include <stdarg.h>
#include <inttypes.h>

#include "listwise/internal.h"

#include "generator/genscan.h"
#undef _GENSCAN

uint32_t genscan_parse(char * s, int l)
{
	int bslash = 0;
	int x;	
	for(x = 0; x < l; x++)
	{
		if(s[x] == '\\')
			bslash++;
	}

	for(x = 0; x < l; x++)
	{
		int i;
		for(i = 0; i < genscan_num; i++)
		{
			if(s[l] == genscan_opener[genscan_modes[i]])
				break;
		}

		if(i < genscan_num)
		{
			if(bslash)
				return genscan_modes[i + 1];
			else
				return genscan_modes[i];
		}
	}

	return 0;
}

char * genscan_opener = (char[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = c,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

char * genscan_closer = (char[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = d,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

int * genscan_basestate = (int[]){
#define _GENSCAN(a, b, c, d, e, f, g) generator_ ## e,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

int * genscan_argstate = (int[]){
#define _GENSCAN(a, b, c, d, e, f, g) generator_ ## e ## _ ## f,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

int genscan_num = sizeof((int[]){
#define _GENSCAN(a, b, c, d, e, f, g) 1,
GENSCAN_TABLE(0)
#undef _GENSCAN
}) / sizeof(int);

uint32_t * genscan_modes = (uint32_t[]){
#define _GENSCAN(a, b, c, d, e, f, g) b,
GENSCAN_TABLE(0)
#undef _GENSCAN
};
