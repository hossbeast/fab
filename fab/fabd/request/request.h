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

#ifndef _FABD_REQUEST_H
#define _FABD_REQUEST_H

#include "types.h"
#include "xapi.h"

#include "moria/traverse.h"

struct narrator;
struct attrs32;
struct array;

typedef struct request {
  uint32_t msg_id;
  struct array * commands;  // struct command - internal commands
  struct command *first_command;
  struct command *last_command;
} request;

#define TRAVERSE_TYPE_OPT 0x3
#define TRAVERSE_TYPE_TABLE                                                                                             \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_UP    , "up"    , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_UP)    /* traverse upwards */    \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_DOWN  , "down"  , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_DOWN)  /* traverse downwards */  \

typedef enum traverse_type {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_TYPE_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_type;

extern struct attrs32 * traverse_type_attrs;

xapi request_create(request ** restrict rv)
  __attribute__((nonnull));;

xapi request_xfree(request * restrict);
xapi request_ixfree(request ** restrict)
  __attribute__((nonnull));

xapi request_say(request * restrict request, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
