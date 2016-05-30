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

#include <sys/types.h>

#include "xapi.h"

/// fabd_launchp
//
// SUMMARY
//  launch the specified process for this ipc-hash if it is not already running
//
// PARAMETERS
//  pgid  - (returns) pgid of the process
//
xapi launchp_fabd(int fablockfd, pid_t * const restrict pgid)
  __attribute__((nonnull));
