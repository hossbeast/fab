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

#ifndef _XAPI_INFO_H
#define _XAPI_INFO_H

#define restrict __restrict

/// xapi_info_addw
//
// SUMMARY
//  add an info key/value pair to a calltree frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  [key]  - key string
//  [vbuf] - value buffer
//  [vlen] - vstr length
//
void xapi_info_addw(const char * const restrict key, const char * const restrict vbuf, size_t vlen);

/// xapi_info_add
//
// SUMMARY
//  add an info key/value pair to a calltree frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  [key] - key string
//  vfmt  - format string for value
//
void xapi_info_addf(const char * const restrict key, const char * const restrict vfmt, ...)
  __attribute__((nonnull(2)));

/// xapi_info_add
//
// SUMMARY
//  add an info key/value pair to a calltree frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  [key] - key string
//  vfmt  - format string for value
//  va    - varargs
//
void xapi_info_vaddf(const char * const restrict key, const char * const restrict vfmt, va_list va)
  __attribute__((nonnull(2)));

#undef restrict
#endif
