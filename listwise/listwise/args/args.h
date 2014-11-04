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

#include "pstring.h"

#define restrict __restrict

#if DEVEL
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
	char		numbering;									// -n=1(0..n), -N=2(list index)
	char		out_null;										// -z
	char		out_stack;									// -k
	char		out_list;										// -a

	int			stdin_init_list_items;
	int			stdin_linewise;

	struct
	{
		char *	s;
		int			linewise;
#define KIND_INIT_LIST_ITEM	1		/* -i */
#define KIND_INIT_LIST_FILE	2		/* -l */
#define KIND_TRANSFORM_FILE	3		/* -t */
#define KIND_INPUT_FILE			4		/* -f */
		int			kind;
	} *			inputs;
	size_t	inputsl;

/*
	char *	linewise_init_list_files;		// -l
	int			linewise_init_list_filesl;
	char *	nullwise_init_list_files;
	int			nullwise_init_list_filesl;
	char **	linewise_transform_files;		// -t/-
	int			linewise_transform_filesl;
	char **	nullwise_transform_files;
	int			nullwise_transform_filesl;
	char **	linewise_input_files;				// -f/-
	int			linewise_input_filesl;
	char **	nullwise_input_files;
	int			nullwise_input_filesl;

	char **	init_list;									// -i 
	int *		init_list_lens;
	int			init_listl;
	int			init_lista;
*/

#if DEVEL
	int			mode_backtrace;							// backtrace reporting mode
	int			mode_logtrace;							// logtrace mode
#endif
} g_args;

int args_parse(pstring ** remnant)
	__attribute__((nonnull));

void args_teardown();

#endif
