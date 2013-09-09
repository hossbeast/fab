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

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "parseint.h"

int parseint(const char* const s, char* f, intmax_t lo, intmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
	intmax_t	tr = 0;
	int		tn = 0;

#if DBUG_PARSEINT
printf("parseint('%s', '%s', %"PRIdMAX", %"PRIdMAX", %hhu, %hhu, %p, %p)", s, f, lo, hi, min, max, r, rn);
#endif

	errno = 0;
	if(strchr(f, 'd'))
	{
		if(sscanf(s, "%"SCNdMAX"%n", &tr, &tn) != 1)
		{
#if DBUG_PARSEINT
	printf("not numeric\n");
#endif
			return 1;
		}
	}
	if(strchr(f, 'i'))
	{
		if(sscanf(s, "%"SCNiMAX"%n", &tr, &tn) != 1)
		{
#if DBUG_PARSEINT
	printf("not numeric\n");
#endif
			return 1;
		}
	}

	if(errno == ERANGE)					/* larger, or smaller, than intmax can hold	*/
	{
#if DBUG_PARSEINT
	printf(" = ERANGE\n");
#endif
		return 1;
	}
	else if(tr < lo)						/* minimum value threshold	*/
	{
#if DBUG_PARSEINT
	printf(" < lo\n");
#endif
		return 1;
	}
	else if(tr > hi)						/* maximum value threshold	*/
	{
#if DBUG_PARSEINT
	printf(" > hi\n");
#endif
		return 1;
	}
	else if(tn < min)						/* minimum length threshold	*/
	{
#if DBUG_PARSEINT
	printf(" < min\n");
#endif
		return 1;
	}
	else if(tn > max)						/* maximum length threshold	*/
	{
#if DBUG_PARSEINT
	printf(" > max\n");
#endif
		return 1;
	}

	if(strstr(f, "ll") == f)
	{
#if DBUG_PARSEINT
printf(" = %"PRIdMAX"\n", tr);
#endif
		if(r)
			*(signed long long*)r = tr;
	}
	else if(strstr(f, "l") == f)
	{
#if DBUG_PARSEINT
printf(" = %" "l" "d" "\n", (signed long)tr);
#endif
		if(r)
			*(signed long*)r = tr;
	}
	else if(strstr(f, "hh") == f)
	{
#if DBUG_PARSEINT
printf(" = %" "hh" "d" "\n", (signed char)tr);
#endif
		if(r)
			*(signed char*)r = tr;
	}
	else if(strstr(f, "h") == f)
	{
#if DBUG_PARSEINT
printf(" = %" "h" "d" "\n", (signed short)tr);
#endif
		if(r)
			*(signed short*)r = tr;
	}
	else
	{
#if DBUG_PARSEINT
printf(" = %" "" "d" "\n", (signed int)tr);
#endif
		if(r)
			*(signed int*)r = tr;
	}

	if(rn)
		*rn = tn;

	return 0;
}

int parseuint(const char* const s, char* f, uintmax_t lo, uintmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
	uintmax_t	tr = 0;
	int		tn = 0;

#if DBUG_PARSEINT
printf("parseuint('%s', '%s', %"PRIuMAX", %"PRIuMAX", %hhu, %hhu, %p, %p)", s, f, lo, hi, min, max, r, rn);
#endif

	errno = 0;
	if(strchr(f, 'o'))
	{
		if(sscanf(s, "%"SCNoMAX"%n", &tr, &tn) != 1)
		{
#if DBUG_PARSEINT
	printf("not numeric\n");
#endif
			return 1;
		}
	}
	if(strchr(f, 'u'))
	{
		if(sscanf(s, "%"SCNuMAX"%n", &tr, &tn) != 1)
		{
#if DBUG_PARSEINT
	printf("not numeric\n");
#endif
			return 1;
		}
	}
	if(strchr(f, 'x'))
	{
		if(sscanf(s, "%"SCNxMAX"%n", &tr, &tn) != 1)
		{
#if DBUG_PARSEINT
	printf("not numeric\n");
#endif
			return 1;
		}
	}

	if(errno == ERANGE)					/* larger, or smaller, than intmax can hold	*/
	{
#if DBUG_PARSEINT
	printf(" = ERANGE\n");
#endif
		return 1;
	}
	else if(tr < lo)						/* minimum value threshold	*/
	{
#if DBUG_PARSEINT
	printf(" %"PRIuMAX" < lo\n", tr);
#endif
		return 1;
	}
	else if(tr > hi)						/* maximum value threshold	*/
	{
#if DBUG_PARSEINT
	printf(" %"PRIuMAX" > hi\n", tr);
#endif
		return 1;
	}
	else if(tn < min)						/* minimum length threshold	*/
	{
#if DBUG_PARSEINT
	printf(" %hhu < min\n", tn);
#endif
		return 1;
	}
	else if(tn > max)						/* maximum length threshold	*/
	{
#if DBUG_PARSEINT
	printf(" %hhu > max\n", tn);
#endif
		return 1;
	}

	if(r)
	{
		if(strstr(f, "ll") == f)
		{
#if DBUG_PARSEINT
	printf(" = %"PRIdMAX"\n", tr);
#endif
			*(unsigned long long*)r = tr;
		}
		else if(strstr(f, "l") == f)
		{
#if DBUG_PARSEINT
	printf(" = %" "l" "d" "\n", (unsigned long)tr);
#endif
			*(unsigned long*)r = tr;
		}
		else if(strstr(f, "hh") == f)
		{
#if DBUG_PARSEINT
	printf(" = %" "hh" "d" "\n", (unsigned char)tr);
#endif
			*(unsigned char*)r = tr;
		}
		else if(strstr(f, "h") == f)
		{
#if DBUG_PARSEINT
	printf(" = %" "h" "d" "\n", (unsigned short)tr);
#endif
			*(unsigned short*)r = tr;
		}
		else
		{
#if DBUG_PARSEINT
	printf(" = %" "" "d" "\n", (unsigned int)tr);
#endif
			*(unsigned int*)r = tr;
		}
	}

	if(rn)
		*rn = tn;

	return 0;
}
