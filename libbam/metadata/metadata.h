/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_metadata;

struct attrs16;

#define BAM_SYSTEM_STATE_TABLE                                                    \
  DEF(BAM_SYSTEM_STATE_BOOTSTRAP                  , "bootstrap"             , 1)  \
  DEF(BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE    , "config-reconcile"      , 2)  \
  DEF(BAM_SYSTEM_STATE_WALKER_RECONCILE           , "walker-reconcile"      , 3)  \
  DEF(BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE       , "var-reconcile"         , 4)  \
  DEF(BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE    , "module-reconcile"      , 5)  \
  DEF(BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE      , "rule-reconcile"        , 6)  \
  DEF(BAM_SYSTEM_STATE_BPE_SYSTEM_RECONCILE       , "bpe-reconcile"         , 7)  \
  DEF(BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE   , "formula-reconcile"     , 8)  \
  DEF(BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE       , "filesystem-reconcile"  , 9)  \
  DEF(BAM_SYSTEM_STATE_BUILDING                   , "building"              , 10) \
  DEF(BAM_SYSTEM_STATE_OK                         , "ok"                    , 11) \

typedef enum bam_system_state {
#undef DEF
#define DEF(x, s, v) x = v,
BAM_SYSTEM_STATE_TABLE
} bam_system_state;

extern struct attrs16 * bam_system_state_attrs;

typedef struct bam_metadata {
  int64_t bamd_pid;
  uint64_t uptime;          // uptime
  uint64_t timeout;         // remaining until timeout

  const char *ipcdir;       // ipc dir
  uint16_t ipcdir_len;

  const char *homedir;      // homedir of the real user
  uint16_t homedir_len;

  const char *base_path;    // absolute path of the working directory
  uint16_t base_path_len;

  const char *hash;         // hash over base
  uint16_t hash_len;

  enum bam_system_state system_state;
  uint8_t system_error;

  const char *goals;
  uint16_t goals_len;

  uint8_t autorun;
} bam_metadata;
