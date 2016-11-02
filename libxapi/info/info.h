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

#if XAPI_STACKTRACE
#define restrict __restrict

/// xapi_fail_intent
//
// SUMMARY
//  Before failing a frame, but after calling xapi_fail_intent, infos are staged to be applied to
//  the frame for the current function call. There is no way to unstage, so infos should only be
//  staged when a fail call in the current frame is imminent.
//
//  When staging infos in this way, call xapi_info_add* directly, instead of xapi_info*, which can only
//  be used to apply infos to the current frame while failing
//
//  xapi_fail_intent may safely be called more than once
//
void xapi_fail_intent(void);

/// xapi_info_adds
//
// SUMMARY
//  add an info key/value pair to a calltree frame (no-op if the key or value is null or 0-length)
//
// PARAMETERS
//  [key]  - key string
//  [vstr] - value string
//
// REMARKS
//  to stage infos for inclusion in the base frame using xapi_fail_intent
//
void xapi_info_adds(const char * const restrict key, const char * const restrict vstr);

/// xapi_info_addw
//
// SUMMARY
//  add an info key/value pair to a calltree frame
//
// PARAMETERS
//  [key]  - key string
//  [vbuf] - value buffer
//  [vlen] - vstr length
//
// REMARKS
//  see xapi_info_adds
//
void xapi_info_addw(const char * const restrict key, const char * const restrict vbuf, size_t vlen);

/// xapi_info_add
//
// SUMMARY
//  add an info key/value pair to a calltree frame
//
// PARAMETERS
//  [key] - key string
//  vfmt  - format string for value
//
// REMARKS
//  see xapi_info_adds
//
void xapi_info_addf(const char * const restrict key, const char * const restrict vfmt, ...)
  __attribute__((nonnull(2)));

/// xapi_info_add
//
// SUMMARY
//  add an info key/value pair to a calltree frame
//
// PARAMETERS
//  [key] - key string
//  vfmt  - format string for value
//  va    - varargs
//
// REMARKS
//  see xapi_info_adds
//
void xapi_info_vaddf(const char * const restrict key, const char * const restrict vfmt, va_list va)
  __attribute__((nonnull(2)));

#undef restrict
#endif
#endif
