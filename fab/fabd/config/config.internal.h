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

#ifndef FABD_CONFIG_INTERNAL_H
#define FABD_CONFIG_INTERNAL_H

#include "types.h"
#include "xapi.h"

#include "config.h"
#include "filesystem.h"
#include "build_thread.h"

struct box;
struct box_int;
struct box_bool;
struct map;
struct set;
struct list;
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

struct config_filesystem_entry {
  CONFIGBASE;
  struct box_int * invalidate;
};

struct config {
  CONFIGBASE;

  struct config_build {
    CONFIGBASE;
    struct box_int16 * concurrency;
  } build;

  struct config_extern_section {
    CONFIGBASE;
    struct set * entries;    // box_string
  } extern_section;

  struct config_filesystems {
    CONFIGBASE;
    struct map * entries;  // keys are box_string, values are config_filesystem_entry
  } filesystems;

  struct config_formula {
    CONFIGBASE;

    struct config_formula_path {
      CONFIGBASE;

      struct box_bool * copy_from_env;
      struct config_formula_path_dirs {
        CONFIGBASE;
        struct set * entries;    // box_string
      } dirs;
    } path;
  } formula;

  struct config_logging {
    CONFIGBASE;

    struct config_logging_section {
      CONFIGBASE;

      struct config_logging_exprs {
        CONFIGBASE;
        struct list * items;
      } exprs;
    } console;

    struct config_logging_section logfile;
  } logging;

  struct config_var {
    CONFIGBASE;

    struct value * value;
  } var;

  struct config_channels {
    CONFIGBASE;

    struct box_uint16 * max_channels;
  } channels;
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

/// config_compare
//
// SUMMARY
//  mark sections of a new config struct as different from those of another config struct
//
void config_compare(config * restrict new, config * restrict old)
  __attribute__((nonnull(1)));

xapi config_writer_write(config * const restrict cfg, struct value_writer * const restrict writer)
  __attribute__((nonnull));

xapi config_merge(config * restrict dst, config * restrict src)
  __attribute__((nonnull));

#endif
