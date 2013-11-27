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

#ifndef _XOPEN_H
#define _XOPEN_H

/// xopen
//
// SUMMARY
//  call open xapi-style
//
int xopen(const char * path, int flags, int * const fd)
	__attribute__((nonnull));

/// xopen_mode
//
// SUMMARY
//  call open xapi-style
//
int xopen_mode(const char * path, int flags, mode_t mode, int * const fd)
	__attribute__((nonnull));

#endif
