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

#ifndef _PARSEINT
#define _PARSEINT

#include <inttypes.h>

/// parseint
//
// parse an integer from a string
//
// parameters
//  s     - string to parse from
//  f     - one of the SCNxX macros from inttypes.h
//          (a string like "lld", "hhx", "d", etc, which describes both the format
//           of the number to parse, and the size and signedness of the receiving pointer, r)
//  lo    - smallest acceptable value
//  hi    - largest acceptable value
//  min   - smallest acceptable number of characters to parse
//  max   - largest acceptable number of characters to parse
//  [r]   - if not null, receives parsed value
//  [rn]  - if not null, receives number of characters parsed
//          only written to if parseint returns 1
//
// returns 0 if an integer was parsed with the given constraints, nonzero otherwise
//
int parseint(
	  const char* const	s
	, char*							f
	, intmax_t					lo
	, intmax_t					hi
	, uint8_t						min
	, uint8_t						max
	, void*							r
	, int*							rn
) __attribute__((nonnull(2)));

int parseuint(
	  const char* const	s
	, char*							f
	, uintmax_t					lo
	, uintmax_t					hi
	, uint8_t						min
	, uint8_t						max
	, void*							r
	, int*							rn
) __attribute__((nonnull(2)));

#endif
