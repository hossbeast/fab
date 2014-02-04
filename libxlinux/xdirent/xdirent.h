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

#ifndef _XDIRENT_H
#define _XDIRENT_H

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

/// xdiropen
//
// SUMMARY
//  proxy for diropen
//
int xopendir(const char * name, DIR ** dd);

/// xreaddir_r
//
// SUMMARY
//  proxy for readdir_r
//
int xreaddir_r(DIR * dirp, struct dirent * entry, struct dirent ** result);

#endif
