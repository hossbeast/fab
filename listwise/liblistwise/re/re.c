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

#include <stdio.h>
#include <string.h>

#include <pcre.h>

#include "internal.h"

/* libpcre error table */
#include "PCRE.errtab.h"

#include "xlinux.h"
#include "macros.h"

int API re_compile(char* s, struct re* re, char* mod)
{
	int reopts = 0;
	if(mod && strchr(mod, 'i'))
		reopts |= PCRE_CASELESS;
	if(mod && strchr(mod, 'm'))
		reopts |= PCRE_MULTILINE;
	if(mod && strchr(mod, 's'))
		reopts |= PCRE_DOTALL;

	const char* errstr = 0;
	int off = -1;
	if((re->c_pcre = pcre_compile(s, reopts, &errstr, &off, 0)) == 0)
	{
		tfails(perrtab_PCRE, 0, errstr);
	}

	errstr = 0;
	off = -1;
	re->c_pcre_extra = pcre_study(re->c_pcre, 0, &errstr);
	if(errstr)
	{
		tfails(perrtab_PCRE, 0, errstr);
	}

	int r;
	if((r = pcre_fullinfo(re->c_pcre, re->c_pcre_extra, PCRE_INFO_CAPTURECOUNT, &re->c_caps)) != 0)
	{
		tfail(perrtab_PCRE, r);
	}

finally:
	XAPI_INFOF("pcre", "%s", s);
	if(mod)
		XAPI_INFOF("mod", "%s", mod);
	if(off)
		XAPI_INFOF("off", "%d", off);
coda;
}

int API re_exec(struct re* re, char* s, int l, int o, int** ovec, int* ovec_len)
{
	if((*ovec_len) < ((MIN(re->c_caps, 1) * 3) + 1))
	{
		fatal(xrealloc, ovec, sizeof((*ovec)[0]), (MIN(re->c_caps, 1) * 3) + 10, *ovec_len);
		*ovec_len = (MIN(re->c_caps, 1) * 3) + 10;
	}

	(*ovec)[0] = pcre_exec(
		  re->c_pcre
		, re->c_pcre_extra
		, s
		, l ?: strlen(s)
		, o
		, 0
		, &(*ovec)[1]
		, (*ovec_len) - 1
	);

	if((*ovec)[0] < 0 && (*ovec)[0] != PCRE_ERROR_NOMATCH)
	{
		tfail(perrtab_PCRE, (*ovec)[0]);
	}

#if 0
	int x;
	for(x = 0; x < (*ovec)[0]; x++)
	{
		int a = (*ovec)[1 + (x * 2) + 0];
		int b = (*ovec)[1 + (x * 2) + 1];

		if(a >= 0 && b >= 0)
		{
			printf("[%d] = '%.*s'\n", x, b - a, s + a);
		}
	}
#endif

	finally : coda;
}
