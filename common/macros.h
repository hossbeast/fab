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

#ifndef _MACROS_H
#define _MACROS_H

#include "listwise.h"

#define MIN(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _b : _a; })

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

/// znprintf
//
// summary
//  allows the following idiom without buffer overflow (which snprintf would permit)
//
// z += znprintf(dst + z, sz - z, fmt, ...)
// z += znprintf(dst + z, sz - z, fmt, ...)
//
#define znprintf(dst, sz, fmt, ...) ({										\
	size_t _z = 0;																					\
	if(sz)																									\
	{																												\
		_z = MIN(sz, snprintf(dst, sz, fmt, ##__VA_ARGS__));	\
	}																												\
	_z; 																										\
})

#endif
