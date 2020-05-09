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

#ifndef _SERVER_THREAD_H
#define _SERVER_THREAD_H

#include "xapi.h"
#include "types.h"

#if DEVEL
bool g_server_no_initial_client;      // no client right at startup
const char *g_server_initial_request; // initial request
#endif

// true if currently autobuilding
extern bool g_server_autorun;

/// create the server thread
xapi server_thread_launch(void);

#endif
