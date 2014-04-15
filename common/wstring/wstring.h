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

#ifndef _WSTRING_H
#define _WSTRING_H

#define restrict __restrict

/// iwstrcat
//
// reallocates *s1, appends s2 onto *s1
//
// returns nonzero on malloc failure
//
int iwstrcat(char** s1, const char* s2);

/// iwstrcatf
//
// calculates size required to vprintf, reallocates *s and appends the new string onto *s
//
// returns nonzero on malloc failure
//
int iwstrcatf(char** s, char* fmt, ...);

/// iwstrdup
//
// copies s2 to *s1, reallocating *s1 if necessary, deallocating it if it was already allocated
//
// returns nonzero on malloc failure
//
int iwstrdup(char** s1, const char* s2);

/// iwsprintf
//
// - free(*s)
// - allocate(*s)
// - sprintf(fmt, ...) -> *s
//
int iwsprintf(char** s, char* fmt, ...);

#undef restrict
#endif
