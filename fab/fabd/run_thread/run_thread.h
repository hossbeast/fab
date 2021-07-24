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

#ifndef FABD_AUTORUN_THREAD_H
#define FABD_AUTORUN_THREAD_H

/*

There is up to one run thread

*/

#include "request.internal.h"

struct handler_context;

extern bool run_thread_autorun;

/*
 * launch the run thread
 *
 * [handler] - handler thread to notify if any
 */
void run_thread_launch(struct handler_context * restrict owner, command_type cmd);

#endif
