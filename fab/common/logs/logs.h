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

#ifndef _FAB_LOGS_H
#define _FAB_LOGS_H

#include "logger.h"

#define L_ERROR        (0x0000000000000001ULL | L_RED)
#define L_WARN         (0x0000000000000002ULL | L_YELLOW)
#define L_INFO          0x0000000000000004ULL
#define L_ARGS 					0x0000000000000008ULL
#define L_PARAMS				0x0000000000000010ULL
#if DEBUG || DEVEL
# define L_USAGE				0x0000000000000020ULL
# define L_FFTOKEN 			0x0000000000000040ULL
# define L_FFSTATE 			0x0000000000000080ULL
# define L_FFTREE				0x0000000000000100ULL
# define L_FFFILE				0x0000000000000200ULL
#endif
#define L_INVALID				0x0000000000000400ULL
#define L_BPINFO				0x0000000000000800ULL
#define L_BPEXEC				0x0000000000001000ULL
#define L_BPEVAL				0x0000000000002000ULL
#define L_BPDUMP				0x0000000000004000ULL
#define L_BPCMD					0x0000000000008000ULL
#define L_DSCINFO				0x0000000000080000ULL
#define L_DSCEXEC				0x0000000000100000ULL
#define L_DSCCMD				0x0000000000200000ULL
#define L_DSCRES				0x0000000000400000ULL
#define L_NODE	 				0x0000000000800000ULL
#define L_VARAUTO				0x0000000002000000ULL
#define L_VARUSER				0x0000000004000000ULL
#define L_VAR						0x0000000008000000ULL
#define L_INVOKE				0x0000000010000000ULL
#define L_SELECT				0x0000000020000000ULL
#define L_LISTS					0x0000000040000000ULL
#if DEBUG || DEVEL
# define L_LWEXEC				0x0000000100000000ULL	/* step-by-step LW execution */
# define L_LWOPINFO			0x0000000200000000ULL /* operator informational messages */
# define L_LWPARSE			0x0000000400000000ULL	/* display the parsed transform */
# define L_LWTOKEN			0x0000000800000000ULL	/* scanner token stream */
# define L_LWSTATE			0x0000001000000000ULL	/* scanner state changes */
#endif
#if SANITY
# define L_LWSANITY			0x0000002000000000ULL
#endif

#endif
