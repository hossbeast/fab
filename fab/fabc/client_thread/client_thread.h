/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef _CLIENT_THREAD_H
#define _CLIENT_THREAD_H

#include "xapi.h"

struct fab_client;
struct fabipc_message;

xapi client_thread_launch(void);
xapi client_thread_redrive(void);

void client_thread_post(struct fab_client * restrict client, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

#endif
