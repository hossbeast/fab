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

#include "global.h"
#include "config.h"
#include "extern.h"
#include "formula.h"
#include "fsent.h"
#include "handler.h"
#include "module.h"
#include "params.h"
#include "rule_system.h"
#include "stats.h"
#include "var.h"
#include "walker.h"
#include "channel.h"

/* boostrap thread inherits this lock */
struct trylock global_system_reconcile_lock = TRYLOCK_INIT_HELD;
enum bam_system_state global_system_state = BAM_SYSTEM_STATE_BOOTSTRAP;
uint16_t global_reconciliation_id;

xapi global_system_reconcile(channel * restrict chan)
{
  enter;

  graph_invalidation_context invalidation = { };
  rule_run_context rule_ctx;
  bool filesystems_changed;

  fatal(rule_run_context_xinit, &rule_ctx);

  global_reconciliation_id++;
  fatal(graph_invalidation_begin, &invalidation);

  /* in the first reconciliation, so that errors if any may be surfaced to the initial client */
  if(global_reconciliation_id == 1)
  {
    /* creates the global/protected config nodes */
    fatal(config_system_bootstrap);

    /* loads global/protected config nodes only */
    fatal(config_system_reconcile, &invalidation, &filesystems_changed, chan);
    if(chan->error) {
      goto XAPI_FINALLY;
    }

    /* special.module config value is needed here
     * create the protected project module file
     * runs before the first walker reconcile, which would create the project module file as not-protected */
    fatal(module_system_bootstrap);
  }

  /* walk filesystem trees (this can find more config files) */
  global_system_state = BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE;
  fatal(walker_system_reconcile, &invalidation, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  /* reload all config files (this can change filesystem invalidation settings)
   * which are applied by walker reconciliation */
  global_system_state = BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE;
  fatal(config_system_reconcile, &invalidation, &filesystems_changed, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  if(filesystems_changed)
  {
    /* re-walk filesystem trees with updated filesystems settings */
    global_system_state = BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE;
    fatal(walker_system_reconcile, &invalidation, chan);
    if(chan->error) {
      goto XAPI_FINALLY;
    }
  }

  /* reload modules */
  global_system_state = BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE;
  if(global_reconciliation_id == 1)
  fatal(module_system_reconcile, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  /* cleanup var files not reloaded by any module */
  global_system_state = BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE;
  fatal(var_system_reconcile, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  /* run rules to quiescence */
  global_system_state = BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE;
  fatal(rule_system_reconcile, &rule_ctx, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  /* cleanup formulas which are no longer referenced */
  global_system_state = BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE;
  fatal(formula_system_reconcile, chan);
  if(chan->error) {
    goto XAPI_FINALLY;
  }

  global_system_state = BAM_SYSTEM_STATE_OK;
  STATS_INC(g_stats.system_state_ok);

finally:
  graph_invalidation_end(&invalidation);
  fatal(rule_run_context_xdestroy, &rule_ctx);
coda;
}
