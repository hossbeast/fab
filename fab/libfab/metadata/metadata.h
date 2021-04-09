/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef _FAB_METADATA_H
#define _FAB_METADATA_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_fab_metadata;

struct attrs16;

#define BAM_SYSTEM_STATE_TABLE                                                   \
  DEF(BAM_SYSTEM_STATE_BOOTSTRAP                  , "bootstrap"             , 1) \
  DEF(BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE    , "config-reconcile"      , 2) \
  DEF(BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE       , "filesystem-reconcile"  , 3) \
  DEF(BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE       , "var-reconcile"         , 4) \
  DEF(BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE   , "formula-reconcile"     , 5) \
  DEF(BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE    , "module-reconcile"      , 6) \
  DEF(BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE      , "rule-reconcile"        , 7) \
  DEF(BAM_SYSTEM_STATE_BUILDING                   , "building"              , 8) \
  DEF(BAM_SYSTEM_STATE_OK                         , "ok"                    , 9) \

typedef enum bam_system_state {
#undef DEF
#define DEF(x, s, v) x = v,
BAM_SYSTEM_STATE_TABLE
} bam_system_state;

extern struct attrs16 * bam_system_state_attrs;

typedef struct fab_metadata {
  int64_t fabd_pid;
  uint64_t uptime;          // uptime

  const char *ipcdir;       // ipc dir
  uint16_t ipcdir_len;

  const char *homedir;      // homedir of the real user
  uint16_t homedir_len;

  const char *project_dir;  // absolute path of the project directory
  uint16_t project_dir_len;

  const char *project_hash; // project hash
  uint16_t project_hash_len;

  enum bam_system_state system_state;
  uint8_t system_error;

  const char *goals;
  uint16_t goals_len;

  uint8_t autorun;
} fab_metadata;

#endif
