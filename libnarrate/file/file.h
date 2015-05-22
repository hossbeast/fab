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

#ifndef _NARRATE_FILE_H
#define _NARRATE_FILE_H

#define restrict __restrict

/// file
//
// SUMMARY
//  write to a file descriptor
//
// PARAMETERS
//  fd - file descriptor to write to
//

int file_vsayf(int fd, const char * const restrict fmt, va_list va)
	__attribute__((nonnull));

int file_says(int fd, char * const restrict s)
	__attribute__((nonnull));

int file_sayw(int fd, char * const restrict b, size_t l)
	__attribute__((nonnull));

int file_sayc(int fd, int c)
	__attribute__((nonnull));

#undef restrict
#endif
