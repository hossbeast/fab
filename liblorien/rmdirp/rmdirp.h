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

#ifndef _RMDIRP_H
#define _RMDIRP_H

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

#define restrict __restrict

/// rmdirp
//
// SUMMARY
//  recursively delete a directory and all of its contents, which consists only
//  of files, directories, symlinks, and dangling symlinks. symlinks are deleted
//  not followed
//
// PARAMETERS
//  dirpath -
//  rmself  - if true, delete directory itself as well
//  errcode - error code to raise if unexpected files are encountered
//
xapi rmdirp(const char * const restrict dirpath, int rmself)
  __attribute__((nonnull));

#undef restrict
#endif
