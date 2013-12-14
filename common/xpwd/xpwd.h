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

#include <sys/types.h>
#include <pwd.h>

/// xgetpwuid_r
//
// SUMMARY
//  fatalize-able wrapper for getpwuid_r
//
int xgetpwuid_r(uid_t uid, struct passwd * pwd, char * buf, size_t buflen, struct passwd ** result);

#endif
