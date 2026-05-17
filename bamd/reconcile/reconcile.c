/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/sigutil.h"
#include "xlinux/xpthread.h"

#include "reconcile.h"
#include "channel.h"
#include "config_system.h"
#include "formula.h"
#include "fsent.h"
#include "request_handler.h"
#include "module.h"
#include "params.h"
#include "rule_system.h"
#include "stats.h"
#include "walker.h"
#include "system.h"
#include "filesystem.h"
#include "meta_edge.h"
#include "bpe.h"
#include "namespace.h"
#include "varset.h"
#include "plugin_system.h"

#include "atomics.h"


uint16_t reconciliation_epoch;

/*
 * This function is always invoked on the run thread
 */
void reconcile()
{
  graph_invalidation_context invalidation = { };
  rule_run_context rule_ctx;
  bool work;

  reconciliation_epoch++;
  rcu_thread rcu;

  rule_run_context_init(&rule_ctx);
  graph_invalidation_begin(&invalidation);
  rcu_enter(&rcu);

  system_has_error = false;
  do {
    /* walk filesystem trees (this can find config files in the ancestry of the project root)
     * it can also invalidate config files */
    system_state_change(SYSTEM_STATE_WALKER_RECONCILE);
    walker_system_reconcile(&invalidation);
    rcu_quiesce(&rcu);
    if(system_has_error) {
      goto end;
    }

    /* reload all config files (this can change filesystem invalidation settings)
     * it can also change walker include/exclude settings */
    system_state_change(SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE);
    config_system_reconcile(&work, &invalidation);
    rcu_quiesce(&rcu);
    if(system_has_error) {
      goto end;
    }
  } while(work);

  /* prune no-longer-mounted filesystem trees */
  /* refresh fse -> fst mappings */
  /* refresh the index */
  /* refresh inotify watches */
  system_state_change(SYSTEM_STATE_FILESYSTEM_RECONCILE);
  fsent_fstree_reconcile(&invalidation);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* resolve symlinks */
  symlink_system_reconcile();
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* create the default namespace */
  /* cleanup unreferenced namespaces */
  /* refresh fse -> namespace mappings */
  system_state_change(SYSTEM_STATE_NAMESPACE_RECONCILE);
  namespace_reconcile(&invalidation);
  rcu_quiesce(&rcu);

  /* create directories */
  system_state_change(SYSTEM_STATE_FILESYSTEM_RECONCILE);
  fsent_dir_reconcile();
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* rebuild modules */
  system_state_change(SYSTEM_STATE_MODULE_SYSTEM_RECONCILE);
  module_system_reconcile(&rule_ctx);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* run rules to quiescence */
  system_state_change(SYSTEM_STATE_RULE_SYSTEM_RECONCILE);
  rule_system_reconcile(&rule_ctx);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* check for bpe conflicts */
  system_state_change(SYSTEM_STATE_BPE_SYSTEM_RECONCILE);
  bpe_system_reconcile(&invalidation);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* create directories */
  system_state_change(SYSTEM_STATE_FILESYSTEM_RECONCILE);
  fsent_dir_reconcile();
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* resolve plugin config entries */
  /* parse plugin files */
  /* re-launch plugins */
  system_state_change(SYSTEM_STATE_PLUGIN_SYSTEM_RECONCILE);
  plugin_system_reconcile(&invalidation, &rcu);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* unlink primary files in managed build directories */
  system_state_change(SYSTEM_STATE_FSENT_RECONCILE);
  fsent_managed_reconcile(&invalidation);
  rcu_quiesce(&rcu);
  if(system_has_error) {
    goto end;
  }

  /* discard invalidations - no errors are possible here */
  system_state_change(SYSTEM_STATE_FSENT_RECONCILE);
  fsent_primary_reconcile(&invalidation);
  rcu_quiesce(&rcu);

end:
  rcu_leave(&rcu);
  graph_invalidation_end(&invalidation);
  rule_run_context_destroy(&rule_ctx);
}
