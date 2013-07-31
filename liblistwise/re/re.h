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

#ifndef _RE_H
#define _RE_H

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
#endif




