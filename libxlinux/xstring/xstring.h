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

#ifndef _XSTRING_H
#define _XSTRING_H

#include "types.h"
#include "xapi.h"


/// xstrncat
//
// reallocates *s1, appends s2 onto *s1
//
xapi ixstrncat(char ** s1, const char * s2, int s2len);

/// xstrcat
//
// reallocates *s1, appends s2 onto *s1
//
xapi ixstrcat(char ** s1, const char * s2);

/// xstrcatf
//
// calculates size required to vprintf, reallocates *s and appends the new string onto *s
//
xapi ixstrcatf(char ** s, char * fmt, ...);

/// xstrdup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
xapi ixstrdup(char ** s1, const char * s2);

/// xstrndup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
xapi ixstrndup(char ** s1, const char * s2, const size_t l);

/// ixsprintf
//
// - wfree(*s)
// - allocate(*s)
// - sprintf(fmt, ...) -> *s
//
xapi ixsprintf(char ** s, char * fmt, ...);

#endif
