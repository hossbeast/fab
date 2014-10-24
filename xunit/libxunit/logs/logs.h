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

#ifndef _XUNIT_LOGS_H
#define _XUNIT_LOGS_H

#include "logger.h"

#if DEVEL
# define L_LOGGER				0x0000000000000001ULL
#endif
#define L_ERROR					0x0000000000000002ULL
#define L_WARN					0x0000000000000004ULL
#define L_INFO					0x0000000000000008ULL
#define L_ARGS					0x0000000000000010ULL
#define L_PARAMS				0x0000000000000020ULL
#define L_XUNIT					0x0000000000000040ULL
#define L_INPUT					0x0000000000000080ULL
#define L_OUTPUT				0x0000000000000100ULL

#endif
