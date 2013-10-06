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
#include <string.h>

#include "listwise/internal.h"

#include <pcre.h>

// error string from libpcre

#if PCRE_MAJOR == 8
#define LIBPCRE_ERR_STR(x) (                                          		\
	(x) == PCRE_ERROR_NOMATCH						? "PCRE_ERROR_NOMATCH"					: 	\
	(x) == PCRE_ERROR_NULL							? "PCRE_ERROR_NULL"							: 	\
	(x) == PCRE_ERROR_BADOPTION					? "PCRE_ERROR_BADOPTION"				: 	\
	(x) == PCRE_ERROR_BADMAGIC					? "PCRE_ERROR_BADMAGIC"					: 	\
	(x) == PCRE_ERROR_UNKNOWN_OPCODE		? "PCRE_ERROR_UNKNOWN_OPCODE"		: 	\
	(x) == PCRE_ERROR_UNKNOWN_NODE			? "PCRE_ERROR_UNKNOWN_NODE"			: 	\
	(x) == PCRE_ERROR_NOMEMORY					? "PCRE_ERROR_NOMEMORY"					: 	\
	(x) == PCRE_ERROR_NOSUBSTRING				? "PCRE_ERROR_NOSUBSTRING"			: 	\
	(x) == PCRE_ERROR_MATCHLIMIT				? "PCRE_ERROR_MATCHLIMIT"				: 	\
	(x) == PCRE_ERROR_CALLOUT						? "PCRE_ERROR_CALLOUT"					: 	\
	(x) == PCRE_ERROR_BADUTF8						? "PCRE_ERROR_BADUTF8"					: 	\
	(x) == PCRE_ERROR_BADUTF8_OFFSET		? "PCRE_ERROR_BADUTF8_OFFSET"		: 	\
	(x) == PCRE_ERROR_PARTIAL						? "PCRE_ERROR_PARTIAL"					: 	\
	(x) == PCRE_ERROR_BADPARTIAL				? "PCRE_ERROR_BADPARTIAL"				: 	\
	(x) == PCRE_ERROR_INTERNAL					? "PCRE_ERROR_INTERNAL"					: 	\
	(x) == PCRE_ERROR_BADCOUNT					? "PCRE_ERROR_BADCOUNT"					: 	\
	(x) == PCRE_ERROR_DFA_UITEM					? "PCRE_ERROR_DFA_UITEM"				: 	\
	(x) == PCRE_ERROR_DFA_UCOND					? "PCRE_ERROR_DFA_UCOND"				: 	\
	(x) == PCRE_ERROR_DFA_UMLIMIT				? "PCRE_ERROR_DFA_UMLIMIT"			: 	\
	(x) == PCRE_ERROR_DFA_WSSIZE				? "PCRE_ERROR_DFA_WSSIZE"				: 	\
	(x) == PCRE_ERROR_DFA_RECURSE				? "PCRE_ERROR_DFA_RECURSE"			: 	\
	(x) == PCRE_ERROR_RECURSIONLIMIT		? "PCRE_ERROR_RECURSIONLIMIT"		: 	\
	(x) == PCRE_ERROR_NULLWSLIMIT				? "PCRE_ERROR_NULLWSLIMIT"			: 	\
	(x) == PCRE_ERROR_BADNEWLINE				? "PCRE_ERROR_BADNEWLINE"				: 	\
	(x) == PCRE_ERROR_BADOFFSET					? "PCRE_ERROR_BADOFFSET"				: 	\
	(x) == PCRE_ERROR_SHORTUTF8					? "PCRE_ERROR_SHORTUTF8"				: "PCRE_UNKNOWN" )
#endif
#if PCRE_MAJOR == 6
#define LIBPCRE_ERR_STR(x) (																					  \
		(x) == PCRE_ERROR_NOMATCH					? "PCRE_ERROR_NOMATCH"					: \
		(x) == PCRE_ERROR_NULL						? "PCRE_ERROR_NULL"							: \
		(x) == PCRE_ERROR_BADOPTION				? "PCRE_ERROR_BADOPTION"				: \
		(x) == PCRE_ERROR_BADMAGIC				? "PCRE_ERROR_BADMAGIC"					: \
		(x) == PCRE_ERROR_UNKNOWN_NODE		? "PCRE_ERROR_UNKNOWN_NODE"			: \
		(x) == PCRE_ERROR_NOMEMORY				? "PCRE_ERROR_NOMEMORY"					: \
		(x) == PCRE_ERROR_NOSUBSTRING			? "PCRE_ERROR_NOSUBSTRING"			: \
		(x) == PCRE_ERROR_MATCHLIMIT			? "PCRE_ERROR_MATCHLIMIT"				: \
		(x) == PCRE_ERROR_CALLOUT					? "PCRE_ERROR_CALLOUT"					: \
		(x) == PCRE_ERROR_BADUTF8					? "PCRE_ERROR_BADUTF8"					: \
		(x) == PCRE_ERROR_BADUTF8_OFFSET	? "PCRE_ERROR_BADUTF8_OFFSET"		: \
		(x) == PCRE_ERROR_PARTIAL					? "PCRE_ERROR_PARTIAL"					: \
		(x) == PCRE_ERROR_BADPARTIAL			? "PCRE_ERROR_BADPARTIAL"				: \
		(x) == PCRE_ERROR_INTERNAL				? "PCRE_ERROR_INTERNAL"					: \
		(x) == PCRE_ERROR_BADCOUNT				? "PCRE_ERROR_BADCOUNT"					: \
		(x) == PCRE_ERROR_DFA_UITEM				? "PCRE_ERROR_DFA_UITEM"				: \
		(x) == PCRE_ERROR_DFA_UCOND				? "PCRE_ERROR_DFA_UCOND"				: \
		(x) == PCRE_ERROR_DFA_UMLIMIT			? "PCRE_ERROR_DFA_UMLIMIT"			: \
		(x) == PCRE_ERROR_DFA_WSSIZE			? "PCRE_ERROR_DFA_WSSIZE"				: \
		(x) == PCRE_ERROR_DFA_RECURSE			? "PCRE_ERROR_DFA_RECURSE"			: "PCRE_UNKNOWN" )
#endif

#include "xmem.h"
#include "macros.h"
#include "liblistwise_control.h"

int API re_compile(char* s, struct re* re, char* mod)
{
	int reopts = 0;
	if(mod && strchr(mod, 'i'))
		reopts |= PCRE_CASELESS;
	if(mod && strchr(mod, 'm'))
		reopts |= PCRE_MULTILINE;
	if(mod && strchr(mod, 's'))
		reopts |= PCRE_DOTALL;

	const char* err;
	int err_off;
	if((re->c_pcre = pcre_compile(s, reopts, &err, &err_off, 0)) == 0)
	{
		fail("pcre: '%s', offset: %d, err: %s", s, err_off, err);
	}

	re->c_pcre_extra = pcre_study(re->c_pcre, 0, &err);
	if(err)
	{
		fail("pcre: '%s', err: %s", s, err);
	}

	int r;
	if((r = pcre_fullinfo(re->c_pcre, re->c_pcre_extra, PCRE_INFO_CAPTURECOUNT, &re->c_caps)) != 0)
	{
		fail("pcre: '%s', err: [%d][%s]", s, r, LIBPCRE_ERR_STR(r));
	}

	finally : coda;
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
		fail("pcre_exec failed: [%d][%s]", (*ovec)[0], LIBPCRE_ERR_STR((*ovec)[0]));
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
