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

#ifndef _LOG_H
#define _LOG_H

#include "logger.h"

// E
#define L_INFO					0x0000000000000001ULL
#define L_ERROR					0x0000000000000002ULL | L_RED
#define L_LWPARSE				0x0000000800000000ULL		/* dump the generator */

#if DEBUG
# define L_LWEXEC				0x0000000200000000ULL		/* step-by-step LW exectution */
# define L_LWOPINFO			0x0000000400000000ULL		/* operator informational messages */
#else
# define L_LWEXEC				0
# define L_LWOPINFO			0
#endif
#if DEVEL
# define L_LWTOKEN			0x0000001000000000ULL		/* scanner token stream */
# define L_LWSTATE			0x0000002000000000ULL		/* scanner state changes */
#else
# define L_LWTOKEN			0
# define L_LWSTATE			0
#endif
#if SANITY
# define L_LWSANITY			0x0000004000000000ULL
#else
# define L_LWSANITY			0
#endif

#endif
