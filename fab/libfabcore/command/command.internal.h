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

#ifndef _FABCORE_COMMAND_INTERNAL_H
#define _FABCORE_COMMAND_INTERNAL_H

#include <stdint.h>

#include "xapi.h"

#include "command.h"

struct memblk;

#define FABCORE_COMMAND_OPT UINT32_C(0x00000FF)
#define FABCORE_LIST_OP_OPT UINT32_C(0x0000F00)

#define FABCORE(a, b, x, y) ((x) & (y)) == b ? "FABCORE_" #a :
#define FABCORE_COMMAND_STRING(x)   FABCORE_TABLE(x, FABCORE_COMMAND_OPT) "UNKNOWN"
#define FABCORE_LIST_OP_STRING(x)   FABCORE_TABLE(x, FABCORE_LIST_OP_OPT) "UNKNOWN"

#define FABCORE_BUILD(a, b, x) (x) == b ? "FABCORE_BUILD_" #a :
#define FABCORE_BUILD_STRING(x)     FABCORE_BUILD_TABLE(x) "UNKNOWN"

#define FABCORE_LICENSE(a, b, x) (x) == b ? "FABCORE_LICENSE_" #a :
#define FABCORE_LICENSE_STRING(x)   FABCORE_LICENSE_TABLE(x) "UNKNOWN"

#define FABCORE_TRACE(a, b, x) (x) == b ? "FABCORE_TRACE_" #a :
#define FABCORE_TRACE_STRING(x)     FABCORE_TRACE_TABLE(x) "UNKNOWN"

#define FABCORE_TARGET(a, b, x) (x) == b ? "FABCORE_TARGET_" #a :
#define FABCORE_TARGET_STRING(x)     FABCORE_TARGET_TABLE(x) "UNKNOWN"

typedef struct command
{
  uint32_t  attrs;    // bitwise combo of FABCORE_*

  union {
    uint8_t   u8;
  };

  union {
    char *  strings[1];

    union {
      char *  text;
      char *  path;
    };
  };
} command;


void command_free(command * const restrict cmd)
  __attribute__((nonnull));

void command_ifree(command ** const restrict cmd)
  __attribute__((nonnull));

xapi command_create(command ** const restrict cmd, uint32_t attrs)
  __attribute__((nonnull));

xapi command_creates(command ** const restrict ret, uint32_t attrs, const char * const restrict text)
  __attribute__((nonnull));

xapi command_createu8(command ** const restrict cmd, uint32_t attrs, uint8_t value)
  __attribute__((nonnull));

xapi command_create_target(command ** const restrict ret, uint32_t attrs, uint8_t opt, const char * const restrict text)
  __attribute__((nonnull));

/// command_freeze
//
// SUMMARY
//  
//
void command_freeze(command * const restrict cmd, struct memblk * const restrict mb)
  __attribute__((nonnull));

/// command_thaw
//
//
//
void command_thaw(command * const restrict cmd, char * const restrict mb)
  __attribute__((nonnull));

/// command_say
//
// SUMMARY
//  
//
xapi command_say(const command * const restrict cmd, struct narrator * const restrict N)
  __attribute__((nonnull));

#endif
