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
#include "xapi.h"

struct FTW;
struct stat;

#define restrict __restrict

/// xftw
//
// SUMMARY
//  proxy for ftw
//
// CALLBACK
//  xnftw uses an internal callback which is passed to nftw. That callback fatal-invokes the xnftw callback
//
// REMARKS
//  this function uses static storage and is therefore not threadsafe
//
xapi xnftw(
    const char * restrict dirpath
  , xapi (* xfn)(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf, void * arg)
  , int nopenfd
  , int flags
  , void * arg
)
  __attribute__((nonnull));

/// xnftw_nth
//
// SUMMARY
//  proxy for ftw which skips entries other than nth-level entries
//
// CALLBACK
//  as for xnftw
//
// REMARKS
//  this function uses static storage and is therefore not threadsafe
//
xapi xnftw_nth(
    const char * restrict dirpath
  , xapi (* xfn)(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf, void * arg)
  , int nopenfd
  , int flags
  , int level
  , void * arg
)
  __attribute__((nonnull));

#undef restrict
#endif
