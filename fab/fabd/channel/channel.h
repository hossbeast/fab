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

#ifndef FABD_CHANNEL_H
#define FABD_CHANNEL_H

/*

*/

#include <sys/ipc.h>
#include <sys/shm.h>

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "fab/ipc.h"

struct config;

xapi channel_create(fabipc_channel ** restrict chanp, pid_t thread_id)
  __attribute__((nonnull));

xapi channel_xfree(fabipc_channel * restrict chan);

xapi channel_setup(void);

xapi channel_cleanup(void);

xapi channel_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

#endif
