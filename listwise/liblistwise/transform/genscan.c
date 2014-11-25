/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include "internal.h"

#include "transform/genscan.h"
#undef _GENSCAN

#include "transform/transform.states.h"

uint32_t genscan_parse(char * s, int l)
{
	uint32_t * modes = 0;

	int x;
	for(x = 0; x < l; x++)
	{
		if(s[x] == '?' && modes == 0)
			modes = genscan_by_opening_char_dorefs;

		else if(s[x] == '?')
			modes = genscan_by_opening_char_norefs;

		else if(modes && modes[(int)s[x]] != UINT32_MAX)
			return modes[(int)s[x]];
	}

	/* 0 happens to be the default scanmode */
	return 0;
}

/*
** define all of the lookup tables
*/

int genscan_num = sizeof((int[]){
#define _GENSCAN(a, b, c, d, e, f, g) 1,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
}) / sizeof(int);

uint32_t * genscan_by_number = (uint32_t[]){
#define _GENSCAN(a, b, c, d, e, f, g) b,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

uint32_t * genscan_by_opening_char_dorefs = (uint32_t[]){
	/* indexable by any int8_t, yields -1 for chars for which there is no such scanmode */
 [ 0 ... 255 ] = UINT32_MAX,
#define _GENSCAN(a, b, c, d, e, f, g) [ c ] = b,
GENSCAN_TABLE_DOREFS(0)
#undef _GENSCAN
};

uint32_t * genscan_by_opening_char_norefs = (uint32_t[]){
	/* indexable by any int8_t, yields -1 for chars for which there is no such scanmode */
 [ 0 ... 255 ] = UINT32_MAX,
#define _GENSCAN(a, b, c, d, e, f, g) [ c ] = b,
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

char * genscan_opening_char = (char[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = c,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

char * genscan_closing_char = (char[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = d,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

int * genscan_startcondition_initial = (int[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = transform_ ## e,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

int * genscan_startcondition_argscan = (int[]){
#define _GENSCAN(a, b, c, d, e, f, g) [ b ] = transform_ ## f,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};
