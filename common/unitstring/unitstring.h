/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _UNITSTRING_H
#define _UNITSTRING_H

struct timespec;

#define restrict __restrict

/// durationstring
//
// accepts number of seconds, produces string describing the interval in terms of
// years, days, hours, minutes, and seconds
//
// parameters
//  base - number of seconds
//  [s]  - receives result string
//  [l]  - max bytes to write to s
//
// returns
//  s, if provided, otherwise a pointer to internal, statically allocated space
//
char* durationstring(int base);
char* durationstring_r(int base, char* s, int l);

/// bytestring
//
// accepts number of bytes, produces string describing the magnitude in terms of
// gigabytes, megabytes, kilobytes, and bytes
//
// parameters
//  base - number of bytes
//  [s]  - receives result string
//  [l]  - max bytes to write to s
// 
// returns
//  s, if provided, otherwise a pointer to internal, statically allocated space
//
char* bytestring(int base);
char* bytestring_r(int base, char* s, int l);

/// elapsed_string_timespec
//
// SUMMARY
//  write a string describing the elapsed duration from start to end followed by a null byte
//
// PARAMETERS
//  start - start of duration
//  end   - end of duration
//  dst   - (returns) duration string
//  sz    - size of *dst
//
// RETURNS
//  number of bytes written, not including the terminating null byte
//
size_t elapsed_string_timespec(const struct timespec * const restrict start, const struct timespec * const restrict end, char * const restrict dst, size_t sz)
	__attribute__((nonnull));

#undef restrict
#endif
