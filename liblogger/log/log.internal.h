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

#define COLOR_OPT     UINT32_C(0x0000000F)
#define PREFIX_OPT    UINT32_C(0x000000C0)
#define TRACE_OPT     UINT32_C(0x00000030)
#define DISCOVERY_OPT UINT32_C(0x00000300)

// get the name of a color from its constant
#define LOGGER_ATTR_DEF(a, b, x, m) (x & m) == b ? "L_" #a : 
#define LOGGER_COLOR_VALUE(x) LOGGER_ATTR_TABLE(x, COLOR_OPT) "NONE"
#define LOGGER_PREFIX_VALUE(x) LOGGER_ATTR_TABLE(x, PREFIX_OPT) "NONE"
#define LOGGER_TRACE_VALUE(x) LOGGER_ATTR_TABLE(x, TRACE_OPT) "NONE"
#define LOGGER_DISCOVERY_VALUE(x) LOGGER_ATTR_TABLE(x, DISCOVERY_OPT) "NONE"

// get the name 

#endif
