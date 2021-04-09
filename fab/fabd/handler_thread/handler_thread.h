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

#ifndef FABD_HANDLER_THREAD_H
#define FABD_HANDLER_THREAD_H

/*

There is one handler thread per client, plus up to one autorun handler thread when autorun is enabled

*/

#include "types.h"
#include "xapi.h"

/*
 * launch a handler thread
 *
 * client_pid - pid of the client, if any
 * client_tid - thread id of the client, if any
 * autorun    - true for autorun (there is no client)
 */
xapi handler_thread_launch(pid_t client_pid, pid_t client_tid, bool autorun)
  __attribute__((nonnull));

#endif
