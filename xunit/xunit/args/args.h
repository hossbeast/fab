/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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


#ifndef _ARGS_H
#define _ARGS_H

#define restrict __restrict

#if DEBUG || DEVEL
# define DEFAULT_MODE_BACKTRACE		MODE_BACKTRACE_FULL
# define DEFAULT_MODE_LOGTRACE		MODE_LOGTRACE_NONE

#define MODE_TABLE_DEVEL(x)																																											\
/* error reporting modes */																																											\
	_MODE(MODE_BACKTRACE_FULL							, 0x0b	, x)		/* report on immediate error condition only */					\
	_MODE(MODE_BACKTRACE_PITHY	 					, 0x0c	, x)		/* unwind stack when reporting errors */								\
/* logging modes */																																															\
	_MODE(MODE_LOGTRACE_NONE							, 0x0d	, x)		/* disable log trace */																	\
	_MODE(MODE_LOGTRACE_FULL							, 0x0e	, x)		/* enable log trace */

#define _MODE(a, b, c) a = b,
enum {
MODE_TABLE_DEVEL(0)
};
#undef _MODE

#define _MODE(a, b, c) (c) == b ? #a :
# define MODE_STR(x) MODE_TABLE_DEVEL(x) "UNKNWN"
#endif

struct g_args_t
{
	char **	test_objects;
	int			test_objectsl;

#if DEBUG || DEVEL
	int			mode_backtrace;		// backtrace reporting mode
	int			mode_logtrace;		// logtrace mode
#endif

	long		procs;
	int			concurrency;			// concurrently limiting factor
} g_args;

int args_parse();
int args_summarize();
void args_teardown();

#endif
