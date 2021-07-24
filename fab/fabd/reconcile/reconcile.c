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


#include "fab/sigutil.h"
#include "xlinux/xpthread.h"

#include "reconcile.h"
#include "channel.h"
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
#include "system_state.h"

#include "atomics.h"

uint16_t reconciliation_id;

void reconcile()
{
  graph_invalidation_context invalidation = { };
  rule_run_context rule_ctx;
  bool work;

  reconciliation_id++;

  rule_run_context_xinit(&rule_ctx);
  graph_invalidation_begin(&invalidation);

  system_error = false;
  if(system_state != BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE)
  {
    system_state_change(BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE);
    config_system_reconcile(&work, &invalidation);
    if(system_error) {
      goto end;
    }
  }

  do
  {
    /* walk filesystem trees (this can find config files in the ancestry of the project root)
     * it can also invalidate config files */
    system_state_change(BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE);
    walker_system_reconcile(&invalidation);
    if(system_error) {
      goto end;
    }

    /* reload all config files (this can change filesystem invalidation settings)
     * it can also change walker include/exclude settings */
    system_state_change(BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE);
    config_system_reconcile(&work, &invalidation);
    if(system_error) {
      goto end;
    }
  } while(work);

  /* reload modules */
  system_state_change(BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE);
  module_system_reconcile();
  if(system_error) {
    goto end;
  }

  /* cleanup var files not reloaded by any module */
  system_state_change(BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE);
  var_system_reconcile();
  if(system_error) {
    goto end;
  }

  /* run rules to quiescence */
  system_state_change(BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE);
  rule_system_reconcile(&rule_ctx);
  if(system_error) {
    goto end;
  }

  /* cleanup formulas which are no longer referenced */
  system_state_change(BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE);
  formula_system_reconcile();
  if(system_error) {
    goto end;
  }

  system_state_change(BAM_SYSTEM_STATE_OK);
  STATS_INC(g_stats.system_state_ok);

end:
  graph_invalidation_end(&invalidation);
  rule_run_context_xdestroy(&rule_ctx);
}
