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

#ifndef _FAB_STATS_H
#define _FAB_STATS_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_fab_global_stats;
extern descriptor_type descriptor_fab_fsent_stats;
extern descriptor_type descriptor_fab_module_stats;
extern descriptor_type descriptor_fab_module_file_stats;
extern descriptor_type descriptor_fab_formula_stats;
extern descriptor_type descriptor_fab_var_stats;

typedef union fab_global_stats {
    struct {
      /* object counts */
      uint32_t rules;                   /* extant rules */
      uint32_t rmas;                    /* extant rmas */
      uint32_t models;                  /* extant models */
      uint32_t modules;                 /* extant modules */
      uint32_t fsents;                  /* extant fsents */
      uint32_t fsents_shadow;           /* extant shadow fsents */

      /* event counters */
      uint32_t stathash;                /* file is stat-hashed */
      uint32_t contenthash;             /* file is content-hash */
      uint16_t bsexec;                  /* build slot is executed */
      uint32_t fstree_refresh;          /* fstree is refreshed */
      uint16_t model_parsed_try;        /* model parse attempt */
      uint16_t model_parsed;            /* model is reloaded */
      uint16_t model_reloaded;          /* model is reloaded */
      uint16_t module_parsed_try;       /* module parse attempt */
      uint16_t module_parsed;           /* module is parsed */
      uint16_t module_reloaded;         /* module is reloaded */
      uint16_t formula_parsed_try;      /* formula parse attempt */
      uint16_t formula_parsed;          /* formula is parsed */
      uint16_t var_parsed_try;          /* var.bam parse attempt */
      uint16_t var_parsed;              /* var.bam is parsed */
      uint16_t rule_run;                /* a rule is run */
      uint32_t rcu_synchronize_nobody;
      uint32_t rcu_synchronize_wait;
      uint32_t rcu_quiesce_nograce;
      uint32_t rcu_quiesce_notlast;
      uint32_t rcu_quiesce_wake;
      uint32_t fsent_stale;             /* fsent marked as stale */
      uint32_t fsent_fresh;             /* fsent marked as fresh */
      uint16_t system_state_ok;         /* system-state set to ok */

      /* operation latencies */
      uint64_t global_system_reconcile;
      uint64_t config_system_reconcile;
      uint64_t filesystem_system_reconcile;
      uint64_t var_system_reconcile;
      uint64_t formula_system_reconcile;
      uint64_t module_system_reconcile;
      uint64_t rule_system_reconcile;
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_global_stats;

typedef union fab_fsent_stats {
  struct {
    /* current state */
    const char *abspath;    /* node absolute path */
    uint16_t abspath_len;
    uint32_t type;          /* node type */
    uint32_t state;         /* node state */

    /* event counters */
    uint32_t fsent_stale;    /* node marked as stale */
    uint32_t fsent_fresh;    /* node marked as fresh */
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_fsent_stats;

/* FAB_NODE_TYPE_DIR - directory containing module.bam */
typedef union fab_module_stats {
  struct {
    /* relation/entity counts */
    uint32_t uses_up;
    uint32_t uses_down;
    uint32_t imports_up;
    uint32_t imports_down;
    uint32_t requires_up;
    uint32_t requires_down;
    uint32_t modules_up;
    uint32_t modules_down;
    uint16_t variants;
    uint16_t rules;           /* rules defined in module.bam */
    uint32_t rules_effective; /* number of rules in effect for this module */
    uint32_t rules_used_by;   /* number of modules which use a rule defined in module.bam */

    /* event counters */
    uint16_t reloaded;        /* module was reloaded */
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_module_stats;

/* FAB_NODE_MODULE_FILE - module.bam file */
typedef union fab_module_file_stats {
  struct {
    /* event counters */
    uint16_t parsed_try;      /* parsed successfully */
    uint16_t parsed;          /* parse attempted */
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_module_file_stats;

/* FAB_NODE_FORMULA_FILE - cc.bam file - referenced as formula from a rule */
typedef union fab_formula_stats {
  struct {
    /* event counters */
    uint16_t parsed_try;      /* parsed successfully */
    uint16_t parsed;          /* parse attempted */
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_formula_stats;

/* FAB_NODE_TYPE_VAR_FILE - var.bam file */
typedef union fab_var_stats {
  struct {
    /* event counters */
    uint16_t parsed_try;      /* parsed successfully */
    uint16_t parsed;          /* parse attempted */
  };

  uint16_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} fab_var_stats;

#endif
