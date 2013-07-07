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

#ifndef _DIRUTIL_H
#define _DIRUTIL_H

#include <sys/types.h>

#define restrict __restrict

/// rmdir_recursive
//
// SUMMARY
//  recursively delete a directory and all of its contents, which consists only
//  of files, directories, symlinks, and dangling symlinks. symlinks are deleted
//  not followed
//
// PARAMETERS
//  dirpath - 
//  rmself  - if true, delete directory itself as well
//
int rmdir_recursive(const char * const restrict dirpath, int rmself)
	__attribute__((nonnull));

/// mkdirp
//
// SUMMARY
//  ensure a directory path exists, iteratively creating each component of the path
//  if it does not already exist
//
// PARAMETERS
//  dirpath - path to directory to create
//  mode    - mode to create directories with
// 
// RETURNS
//  0 if an error OTHER THAN EEXIST was encountered, 1 otherwise
//
int mkdirp(const char * const restrict dirpath, mode_t mode)
	__attribute__((nonnull));

#undef restrict
#endif

