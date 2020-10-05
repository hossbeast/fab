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

#ifndef _BUILD_THREAD_H
#define _BUILD_THREAD_H

#include "xapi.h"

struct attrs32;
struct config;
struct handler_context;

// configuration
#define STREAM_PART_OPT 0xf
#define STREAM_PART_TABLE                                                 \
  STREAM_PART_DEF(STREAM_PART_LEADING   , leading , STREAM_PART_OPT, 0x1) \
  STREAM_PART_DEF(STREAM_PART_TRAILING  , trailing, STREAM_PART_OPT, 0x2) \
  STREAM_PART_DEF(STREAM_PART_NONE      , none    , STREAM_PART_OPT, 0x3)

typedef enum stream_part {
#define STREAM_PART_DEF(x, n, r, y) x = UINT32_C(y),
STREAM_PART_TABLE
#undef STREAM_PART_DEF
} stream_part;

extern struct attrs32 * stream_part_attrs;

xapi build_thread_setup(void);

xapi build_thread_cleanup(void);

xapi build_thread_launch(void);

/// build_thread_reconfigure
//
// SUMMARY
//  reapply configuration to the build module
//
// PARAMETERS
/// ctx    - reconfiguration context
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi build_thread_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

xapi build_thread_build(struct handler_context * restrict ctx)
  __attribute__((nonnull));

#endif
