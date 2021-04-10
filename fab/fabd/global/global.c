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

/* boostrap thread inherits this lock */
struct trylock global_system_reconcile_lock = TRYLOCK_INIT_HELD;
enum bam_system_state global_system_state = BAM_SYSTEM_STATE_BOOTSTRAP;
uint16_t global_reconciliation_id;

/* on the first reconciliation, the config and module systems must be bootstrapped */
static bool subsequent;

xapi global_system_reconcile(handler_context * restrict ctx, bool * restrict reconciled)
{
  enter;

  int walk_id;
  graph_invalidation_context invalidation = { };

  global_reconciliation_id++;
  fatal(graph_invalidation_begin, &invalidation);
  walk_id = walker_begin();
  *reconciled = true;

  if(!subsequent) {
    fatal(config_system_bootstrap);
  }

  /* reload config files */
  global_system_state = BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE;
  fatal(config_system_reconcile, walk_id, &invalidation, reconciled);
  if(!*reconciled) {
    fprintf(stderr, "config files were not parsed!\n");
    goto XAPI_FINALLY;
  }

  /* this must come after the first config parse */
  if(!subsequent) {
    subsequent = true;
    fatal(module_system_bootstrap);
  }

  /* reload filesystem trees */
  global_system_state = BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE;
  fatal(walker_descend, 0, g_project_root, 0, g_params.proj_dir, walk_id, &invalidation);
  fatal(walker_ascend, g_project_root, walk_id, &invalidation);
  fatal(extern_system_reconcile, walk_id, &invalidation);

  /* reload modules */
  global_system_state = BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE;
  fatal(module_system_reconcile, &ctx->invalidation, reconciled);
  if(!*reconciled) {
    fprintf(stderr, "module files were not reconciled\n");
    goto XAPI_FINALLY;
  }

  /* cleanup var files not reloaded by any module */
  global_system_state = BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE;
  fatal(var_system_reconcile, reconciled);
  if(!*reconciled) {
    fprintf(stderr, "var files were not reconciled\n");
    goto XAPI_FINALLY;
  }

  /* run rules to quiescence */
  global_system_state = BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE;
  fatal(rule_system_reconcile, &ctx->rule_ctx, reconciled);
  if(!*reconciled) {
    fprintf(stderr, "rules were not reconciled!\n");
    goto XAPI_FINALLY;
  }

  /* cleanup formulas which are no longer referenced */
  global_system_state = BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE;
  fatal(formula_system_reconcile, reconciled);
  if(!*reconciled) {
    fprintf(stderr, "formula files were not parsed!\n");
    goto XAPI_FINALLY;
  }

  global_system_state = BAM_SYSTEM_STATE_OK;
  STATS_INC(g_stats.system_state_ok);

finally:
  graph_invalidation_end(&invalidation);
coda;
}
