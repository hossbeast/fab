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

#ifndef _ARGS_H
#define _ARGS_H

#include "xapi.h"
#include "types.h"

struct command;
struct value_writer;
struct fabipc_message;

extern struct g_args_t {
  bool invalidate;
  bool kill;
} g_args;

/// args_parse
//
// SUMMARY
//  parses command-line options, populating g_args.
//
// REMARKS
//  prints the usage statement and terminates the program if options are
//  invalid, for example required options are not present, or invalid
//  parameters are given to an option
//
// PARAMETERS
//  cmd - (returns) subcommand dispatch
//
xapi args_parse(struct command ** cmd)
  __attribute__((nonnull));

/// args_usage
//
// SUMMARY
//
//
// PARAMETERS
//  cmd - subcommand dispatch
//
void args_usage(struct command * restrict cmd);
void args_version(void);

xapi args_teardown(void);

#endif
