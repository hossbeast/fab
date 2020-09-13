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

#ifndef FABD_EVENTS_H
#define FABD_EVENTS_H

#include "types.h"
#include "fab/ipc.h"

struct handler_context;
struct fabipc_message;

bool events_would(fabipc_event_type type, struct handler_context ** restrict first, struct fabipc_message ** restrict msg)
  __attribute__((nonnull));

void events_publish(struct handler_context * first, fabipc_message * msg)
  __attribute__((nonnull));

#endif
