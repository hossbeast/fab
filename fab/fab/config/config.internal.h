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

#ifndef CONFIG_INTERNAL_H
#define CONFIG_INTERNAL_H

#include "types.h"
#include "xapi.h"

#include "config.h"

struct box;
struct box_int;
struct box_bool;
struct narrator;
struct value_writer;
struct value;

#define CONFIG_CHANGED            0x1
#define CONFIG_MERGE_SIGNIFICANT  0x2
#define CONFIG_MERGE_SET          0x4

#define CONFIGDEF {               \
  union {                         \
    struct {                      \
      bool changed:1;             \
      bool merge_significant:1;   \
      bool merge_overwrite:1;     \
    };                            \
    uint8_t attrs;                \
  };                              \
}                                 \

struct config_base;
typedef struct config_base CONFIGDEF config_base;

#define CONFIGBASE union { config_base cb ; struct CONFIGDEF; }

struct config {
  CONFIGBASE;

  struct config_formula {
    CONFIGBASE;

    // output stream processing
    struct box_int * capture_stdout;
    struct box_uint16 * stdout_buffer_size;
    struct box_int * capture_stderr;
    struct box_uint16 * stderr_buffer_size;
    struct box_int * capture_auxout;
    struct box_uint16 * auxout_buffer_size;

    struct config_formula_show_settings {
      CONFIGBASE;

      struct box_bool * show_path;
      struct box_bool * show_cwd;
      struct box_bool * show_command;
      struct box_bool * show_arguments;
      struct box_bool * show_sources;
      struct box_bool * show_targets;
      struct box_bool * show_environment;
      struct box_bool * show_status;
      struct box_bool * show_stdout;
      struct box_int16 * show_stdout_limit_bytes;
      struct box_int16 * show_stdout_limit_lines;
      struct box_bool * show_stderr;
      struct box_int16 * show_stderr_limit_bytes;
      struct box_int16 * show_stderr_limit_lines;
      struct box_bool * show_auxout;
      struct box_int16 * show_auxout_limit_bytes;
      struct box_int16 * show_auxout_limit_lines;
    } success;

    struct config_formula_show_settings error;
  } formula;
};

/// config_throw
//
// SUMMARY
//  fail with CONFIG_ILLEGAL
//
// PARAMETERS
//  error  - error, CONFIG_*
//  [val]  - throw config object
//  [path] - path from the root of the config tree
//
xapi config_throw(struct box * restrict val)
  __attribute__((nonnull));

/// config_create
//
//
//
//
xapi config_create(config ** restrict cfg)
  __attribute__((nonnull));

/// config_say
//
// SUMMARY
//
// PARAMETERS
//  cfg - config instance
//  N   - narrator
//
xapi config_say(config * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

/// config_xfree
//
// SUMMARY
//  free a config struct with free semantics
//
xapi config_xfree(config * restrict cfg);

/// config_ixfree
//
// SUMMARY
//  free a config struct with ifree semantics
//
xapi config_ixfree(config ** restrict cfg)
  __attribute__((nonnull));

xapi config_writer_write(config * const restrict cfg, struct value_writer * const restrict writer)
  __attribute__((nonnull));

xapi config_merge(config * restrict dst, config * restrict src)
  __attribute__((nonnull));

#endif
