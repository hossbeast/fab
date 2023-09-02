/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/foreach.h"
#include "fab/stats.desc.h"
#include "valyria/map.h"

#include "stats.h"
#include "config.h"
#include "config.h"
#include "formula.h"
#include "fsent.h"
#include "vertex.h"
#include "marshal.h"
#include "module.h"
#include "var.h"
#include "rule_module_association.h"
#include "block.h"
#include "rule.h"
#include "rules.h"
#include "var.h"
#include "system_state.h"
#include "run_thread.h"
#include "edge.h"
#include "fsent_index.h"
#include "notify_thread.h"

#include "zcat.h"

fab_global_stats g_stats;

typedef struct stat_def {
  uint32_t offset;
  uint8_t size;
} stat_def;

//
// public
//

static size_t block_rules(block *b)
{
  size_t z;
  block *blk;

  z = 0;
  z += llist_count(&b->rules);

  llist_foreach(&b->blocks, blk, lln) {
    z += block_rules(blk);
  }

  return z;
}

size_t stats_global_collate(void *dst, size_t sz, bool reset)
{
  size_t z;
  fab_global_stats *stats;
  fab_global_stats tmp;
  module *mod;
  fsent *fse;
  uint32_t count;
  block *block;

  memcpy(&tmp, &g_stats, sizeof(tmp));
  if(reset) {
    memset(&g_stats, 0, sizeof(g_stats));
  }
  stats = &tmp;

  /* state */
  stats->system_state = system_state;
  stats->system_error = system_error;
  stats->autorun = run_thread_autorun;

  /* rules */
  count = 0;
  llist_foreach(&module_list, mod, vertex.owner) {
    llist_foreach(&mod->blocks, block, lln) {
      count += block_rules(block);
    }
  }
  stats->rules = count;

  /* special */
  stats->modules = llist_count(&module_list);
  stats->config_files = llist_count(&config_list);
  stats->rules_files = llist_count(&rules_list);
  stats->var_files = llist_count(&var_list);

  /* fsents */
  stats->fsents = llist_count(&fsent_list);

  stats->fsent_index_count = fsent_index_count;
  stats->notify_index_count = notify_thread_fsent_by_in_wd->size;

  /* fsents_shadow */
  count = 0;
  llist_foreach(&fsent_list, fse, vertex.owner) {
    if((fse->vertex.attrs & VERTEX_SHADOWTYPE_OPT)) {
      count++;
    }
  }
  stats->fsents_shadow = count;

  z = 0;
  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_fab_global_stats, &stats->record, 0);

  return z;
}
