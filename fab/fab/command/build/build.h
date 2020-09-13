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

#ifndef COMMAND_BUILD_H
#define COMMAND_BUILD_H

#include "xapi.h"
#include "types.h"

#include "command.h"

struct config;
struct narrator;
struct fab_client;
struct fabipc_message;

extern struct command build_command;

xapi build_command_setup(void);
xapi build_command_cleanup(void);
xapi build_command_reconfigure(struct config * restrict cfg);

void build_command_usage(struct command * restrict cmd);
xapi build_command_connected(struct command * restrict cmd, struct fab_client * restrict client);
xapi build_command_args_parse(struct command * restrict cmd, int argc, char ** restrict argv);

xapi build_command_request_collate(narrator * restrict N)
  __attribute__((nonnull));
xapi build_command_process_event(struct fab_client * restrict client, struct fabipc_message * restrict msg);

#endif
