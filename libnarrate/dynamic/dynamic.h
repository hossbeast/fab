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

#ifndef _NARRATE_DYNAMIC_H
#define _NARRATE_DYNAMIC_H

#include <stdarg.h>

struct pstring;				// pstring.h

#define restrict __restrict

/// dynamic
//
// SUMMARY
//  write to a dynamically resizing buffer
//
// PARAMETERS
//  ps - pointer to pstr instance to write to
//

int dynamic_vsayf(struct pstring ** restrict ps, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

int dynamic_sayw(struct pstring ** restrict ps, char * const restrict b, size_t l)
	__attribute__((nonnull));

int dynamic_sayc(struct pstring ** restrict ps, int c)
	__attribute__((nonnull));

#undef restrict
#endif
