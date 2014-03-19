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


#ifndef _ARGS_H
#define _ARGS_H

#define restrict __restrict

#if DEBUG
# define DEFAULT_MODE_BACKTRACE		MODE_BACKTRACE_PITHY
# define DEFAULT_MODE_LOGTRACE		MODE_LOGTRACE_NONE
#endif

#if SANITY
# define DEFAULT_MODE_SANITY			MODE_SANITY_DISABLE
#endif

#if DEBUG
#define MODE_TABLE_DEBUG(x)																																											\
/* error reporting modes */																																											\
	_MODE(MODE_BACKTRACE_FULL							, 0x0b	, x)		/* report on immediate error condition only */					\
	_MODE(MODE_BACKTRACE_PITHY	 					, 0x0c	, x)		/* unwind stack when reporting errors */								\
	_MODE(MODE_LOGTRACE_NONE							, 0x0d	, x)		/* disable log trace */																	\
	_MODE(MODE_LOGTRACE_FULL							, 0x0e	, x)		/* enable log trace */
#endif
#if SANITY
#define MODE_TABLE_SANITY(x)																																										\
/* sanity checking modes */																																											\
	_MODE(MODE_SANITY_DISABLE							, 0x13	, x)		/* disable sanity checks for liblistwise invocations */	\
	_MODE(MODE_SANITY_ENABLE							, 0x14	, x)		/* enable sanity checks for liblistwise invocations */
#endif

#if DEBUG || SANITY
#define _MODE(a, b, c) a = b,
enum {
#if DEBUG
MODE_TABLE_DEBUG(0)
#endif
#if SANITY
MODE_TABLE_SANITY(0)
#endif
};
#undef _MODE
#endif

#define _MODE(a, b, c) (c) == b ? #a :
#if DEVEL
# if SANITY
#  define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) MODE_TABLE_DEVEL(x) MODE_TABLE_SANITY(x) "UNKNWN"
# else
#  define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) MODE_TABLE_DEVEL(x) "UNKNWN"
# endif
#elif DEBUG
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) "UNKNWN"
#endif

struct g_args_t
{
	char		numbering;				// -n=1(0..n), -N=2(list index)
	char		in_null;					// -0
	char		out_null;					// -z
	char		out_stack;				// -k
	char		out_list;					// -a
	char *	generator_file;		// -f/-
	char *	init_file;				// -l

	char **	init_list;				// -i 
	int *		init_list_lens;		// -i 
	int			init_listl;				// -i
	int			init_lista;				// -i

#if DEBUG
	int			mode_backtrace;		// backtrace reporting mode
	int			mode_logtrace;		// backtrace reporting mode
#endif
#if SANITY
	int			mode_sanity;			// liblistwise sanity checks
#endif
} g_args;

int parse_args(const int argc, char ** const restrict argv, int * const restrict genx)
	__attribute__((nonnull));

void args_teardown();

#endif
