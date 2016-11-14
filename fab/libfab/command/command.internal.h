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

#ifndef FAB_COMMAND_INTERNAL_H
#define FAB_COMMAND_INTERNAL_H

#include <stdint.h>

#include "xapi.h"

#include "command.h"
#include "command.def.h"

struct narrator;
struct memblk;

typedef struct fab_command fab_command;

void command_free(fab_command * const restrict cmd)
  __attribute__((nonnull));

void command_ifree(fab_command ** const restrict cmd)
  __attribute__((nonnull));

/// command_freeze
//
// SUMMARY
//
//
void command_freeze(fab_command * const restrict cmd, struct memblk * const restrict mb)
  __attribute__((nonnull));

/// command_thaw
//
//
//
void command_thaw(fab_command * const restrict cmd, char * const restrict mb)
  __attribute__((nonnull));

/// command_say
//
// SUMMARY
//
//
xapi command_say(const fab_command * const restrict cmd, struct narrator * const restrict N)
  __attribute__((nonnull));

xapi command_create(fab_command ** const restrict cmd, uint8_t type, uint32_t attrs)
  __attribute__((nonnull));

xapi command_creates(fab_command ** const restrict ret, uint8_t type, uint32_t attrs, const char * const restrict text)
  __attribute__((nonnull));

xapi command_createvf(fab_command ** const restrict ret, uint8_t type, uint32_t attrs, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(1, 4)));

xapi command_createu8(fab_command ** const restrict cmd, uint8_t type, uint32_t attrs, uint8_t value)
  __attribute__((nonnull));

xapi command_create_target(fab_command ** const restrict ret, uint8_t type, uint32_t attrs, uint8_t opt, const char * const restrict text)
  __attribute__((nonnull));

#endif
