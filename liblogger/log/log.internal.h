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

#ifndef _LOGGER_LOG_INTERNAL_H
#define _LOGGER_LOG_INTERNAL_H

#define COLOR_OPT     UINT64_C(0x000d000000000000)
#define COLOR_SET_OPT UINT64_C(0x0001000000000000)

#define PREFIX_OPT    UINT64_C(0x8000000000000000)
#define TRACE_OPT     UINT64_C(0xe000000000000000)
#define DISCOVERY_OPT UINT64_C(0xe000000000000000)

// get the name of a color from its constant
#define LOGGER_COLOR_DEF(a, b, x) (x) == b ? "L_" #a : 
#define LOGGER_COLOR_NAME(x) LOGGER_COLOR_TABLE(x) "UNKNOWN"

// get the name 

#endif
