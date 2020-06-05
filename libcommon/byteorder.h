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

#ifndef _BYTEORDER_H
#define _BYTEORDER_H

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
