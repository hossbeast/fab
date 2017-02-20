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

#include <sys/types.h>

#include "xapi.h"

typedef struct command
{
  xapi (*args_parse)(const char ** restrict argv, size_t argc);
  xapi __attribute__((nonnull)) (*usage_say)(struct narrator * restrict);
  xapi __attribute__((nonnull)) (*command_say)(struct narrator * restrict);
  xapi __attribute__((nonnull)) (*collate)(struct fab_request * restrict req, int * restrict config_applied);
} command;

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
xapi args_parse(const struct command ** cmd)
  __attribute__((nonnull));

/// args_usage
//
// SUMMARY
//
//
// PARAMETERS
//  cmd - subcommand dispatch
//
xapi args_usage(const struct command * restrict cmd, int version, int logcats);

/// args_report
//
// SUMMARY
//  log a summary of args as-parsed
//
xapi args_report(const struct command * restrict cmd)
  __attribute__((nonnull));

/// args_request_collate
//
// SUMMARY
//  build a fab request from g_args
//
xapi args_collate(const command * restrict cmd, struct memblk * const restrict mb, struct fab_request ** const restrict req)
  __attribute__((nonnull));

#endif
