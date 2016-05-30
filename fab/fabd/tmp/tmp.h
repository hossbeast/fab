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

#ifndef _TMP_H
#define _TMP_H

#include "xapi.h"

#define restrict __restrict

/// tmp_cleanup
//
// SUMMARY
//  delete directory entries in the tmp dir that have expired
//
// PARAMETERS
//  pid - delete directory for this pid even if it has not yet expired
//
xapi tmp_cleanup(pid_t * const restrict pids, size_t pidsl);

#endif
