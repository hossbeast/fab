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

#ifndef FAB_COMMAND_DEF_H
#define FAB_COMMAND_DEF_H

#include <sys/types.h>
#include <stdint.h>

#include "command.h"

struct fab_command
{
  uint8_t  type;  // one of FAB_COMMAND_*
  uint32_t attr;  // bitwise combo of FAB_ATTR_*

  union {
    char *  strings[1];

    union {
      char *  text;
      char *  selector;
    };
  };
};

#define FAB_COMMAND_TABLE                               \
  /* select nodes for the request */                    \
  FAB_COMMAND(SELECT         , select         , 0x01)   \
  /* selection based commands */                        \
  FAB_COMMAND(BUILD          , build          , 0x02)   \
  FAB_COMMAND(PLAN           , plan           , 0x03)   \
  FAB_COMMAND(SCRIPT         , script         , 0x04)   \
  /* commands with included node identifier */          \
  FAB_COMMAND(INVALIDATE     , invalidate     , 0x05)   \
  FAB_COMMAND(INVALIDATE_ALL , invalidate-all , 0x06)   \
  FAB_COMMAND(INSPECT        , inspect        , 0x07)   \
  FAB_COMMAND(QUERY          , query          , 0x08)   \
  /* configuration commands */                          \
  FAB_COMMAND(CONFIG_STAGE   , config-stage   , 0x09)   \
  FAB_COMMAND(CONFIG_APPLY   , config-apply   , 0x0a)

enum {
#define FAB_COMMAND(a, b, c) FAB_COMMAND_ ## a = UINT8_C(c),
FAB_COMMAND_TABLE
#undef FAB_COMMAND
};

const char ** fab_command_names;
size_t fab_command_num;

#define COMMAND_NAME(x)  \
  ((x) < 1 || (x) > fab_command_num) ? "(unknown)" : fab_command_names[(x)]

#endif
