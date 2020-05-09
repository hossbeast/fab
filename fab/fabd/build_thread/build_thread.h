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

typedef struct build_thread_config {
  int16_t concurrency;

  stream_part capture_stdout;
  uint16_t stdout_buffer_size;

  stream_part capture_stderr;
  uint16_t stderr_buffer_size;

  stream_part capture_auxout;
  uint16_t auxout_buffer_size;

  struct show_settings {
    bool show_path;
    bool show_arguments;
    bool show_command;
    bool show_cwd;
    bool show_sources;
    bool show_targets;
    bool show_environment;
    bool show_status;

    bool show_stdout;
    int16_t show_stdout_limit_lines;
    int16_t show_stdout_limit_bytes;

    bool show_stderr;
    int16_t show_stderr_limit_lines;
    int16_t show_stderr_limit_bytes;

    bool show_auxout;
    int16_t show_auxout_limit_lines;
    int16_t show_auxout_limit_bytes;
  } error;

  struct show_settings success;
} build_thread_config;

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

xapi build_thread_build(bool notify);

#endif
