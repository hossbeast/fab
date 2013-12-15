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

#ifndef _XUNISTD_H
#define _XUNISTD_H

#include <unistd.h>

/// xread
//
// SUMMARY
//  proxy for read
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
int xread(int fd, void * buf, size_t count, ssize_t * bytes);


/// xwrite
//
// SUMMARY
//  proxy for write
//
// PARAMETERS
//  [bytes] - returns the number of bytes written
//
int xwrite(int fd, const void * buf, size_t count, ssize_t * bytes);

/// xgetcwd
//
// SUMMARY
//  proxy for getcwd
//
// PARAMETERS
//  [res] - *res is set to the return from getcwd
//
int xgetcwd(char * buf, size_t size, char ** res);

#endif
