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

#ifndef _MACROS_H
#define _MACROS_H

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

#define sentinel(x) ({																\
	typeof((x)[0]) * p = (x);														\
	while(memcmp(p, (char[sizeof(*p)]){}, sizeof(*p)))	\
		p++;																							\
	p - (x);																						\
})

/// NARGS
//
// evaluates to the quantity of arguments passed to it
//
#define NARGS(...) ((sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int)) - 1)

/// LSIGX / MSIGX
//
// SUMMARY
//  access the nth byte in ascending(LSIGX) or descending(MSIGX) order of significance
//
// PARAMETERS
//  x - size of the value
//  n - byte to access
//
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define LSIGX(l, x) (x)
# define MSIGX(l, x) ((l) - (x) - 1)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define LSIGX(l, x) ((l) - (x) - 1)
# define MSIGX(l, x) (l)
#else
# error unable to determine endianness
#endif

#endif
