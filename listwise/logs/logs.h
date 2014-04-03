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

#ifndef _LOGS_H
#define _LOGS_H

#include "logger.h"

#if DEVEL
# define L_LOGGER				0x0000000000000001ULL
#endif
#define L_LWPARSE				0x0000000000000002ULL		/* display the parsed generator */
#define L_LWEXEC				0x0000000000000004ULL		/* step-by-step LW exectution */
#define L_LWOPINFO			0x0000000000000008ULL		/* operator informational messages */

#if DEVEL
# define L_LWTOKEN			0x0000000000000010ULL		/* scanner token stream */
# define L_LWSTATE			0x0000000000000020ULL		/* scanner state changes */
# define L_LWSANITY			0x0000000000000040ULL
#endif

#endif
