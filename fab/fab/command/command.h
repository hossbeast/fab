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

#ifndef _COMMAND_H
#define _COMMAND_H

#include "xapi.h"
#include "types.h"

struct fab_client;
struct fabipc_message;
struct narrator;

typedef struct command {
  char *name;

  void (*usage)(struct command *cmd);
  xapi __attribute__((nonnull)) (*connected)(struct command *cmd, struct fab_client * restrict client);
  xapi __attribute__((nonnull)) (*process)(
      struct command *cmd
    , struct fab_client * restrict client
    , struct fabipc_message * restrict msg
  );
} command;

void client_post(struct fab_client * restrict client, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

#endif
