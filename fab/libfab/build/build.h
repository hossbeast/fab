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

#ifndef _FAB_BUILD_H
#define _FAB_BUILD_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

struct attrs16;
extern descriptor_type descriptor_fab_build;

#define FAB_BUILD_STATE_TABLE                 \
  DEF(FAB_BUILD_PENDING     , "pending")      \
  DEF(FAB_BUILD_IN_PROGRESS , "in-progress")  \
  DEF(FAB_BUILD_SUCCEEDED   , "succeeded")    \
  DEF(FAB_BUILD_FAILED      , "failed")       \
  DEF(FAB_BUILD_NONE        , "none")         \

typedef enum fab_build_state {
  FAB_BUILD_STATE_RANGE_BEFORE,
#undef DEF
#define DEF(x, s) x,
FAB_BUILD_STATE_TABLE
} fab_build_state;

extern struct attrs16 * fab_build_state_attrs;

typedef struct fab_build {
  enum fab_build_state state;
  uint16_t numranks;
  uint16_t numexecs;
} fab_build;

#endif
