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

#ifndef _XFTW_H
#define _XFTW_H

#include <ftw.h>

/// xftw
//
// SUMMARY
//  proxy for ftw
//
// CALLBACK
//  xnftw uses an internal callback which is passed to nftw. the callback specified for xnftw is invoked by that callback, and is expected
//  to follow the xapi calling convention
//
int xnftw(const char * dirpath, int (*fn)(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf), int nopenfd, int flags);

/// xnftw_nth
//
// SUMMARY
//  proxy for ftw which skips entries other than nth-level entries
//
// CALLBACK
//  as for xnftw
//
int xnftw_nth(const char * dirpath, int (*fn)(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf), int nopenfd, int flags, int level);

#endif
