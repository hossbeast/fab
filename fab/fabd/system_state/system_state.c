/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "fab/metadata.h"
#include "fab/ipc.h"
#include "fab/events.h"
#include "channel.h"
#include "common/attrs.h"

#include "system_state.h"
#include "events.h"
#include "zcat.h"
#include "stats.h"

enum bam_system_state system_state = BAM_SYSTEM_STATE_BOOTSTRAP;
bool system_error;

void system_state_change(bam_system_state state)
{
  channel *chan;
  fabipc_message *msg;

  if(state == system_state) {
    return;
  }

  switch(state) {
    case BAM_SYSTEM_STATE_BOOTSTRAP:
      STATS_INC(g_stats.system_state_bootstrap);
      break;
    case BAM_SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_config_reconcile);
      break;
    case BAM_SYSTEM_STATE_WALKER_RECONCILE:
      STATS_INC(g_stats.system_state_walker_reconcile);
      break;
    case BAM_SYSTEM_STATE_VAR_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_var_reconcile);
      break;
    case BAM_SYSTEM_STATE_MODULE_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_module_reconcile);
      break;
    case BAM_SYSTEM_STATE_RULE_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_rule_reconcile);
      break;
    case BAM_SYSTEM_STATE_BPE_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_bpe_reconcile);
      break;
    case BAM_SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_formula_reconcile);
      break;
    case BAM_SYSTEM_STATE_FILESYSTEM_RECONCILE:
      STATS_INC(g_stats.system_state_filesystem_reconcile);
      break;
    case BAM_SYSTEM_STATE_BUILDING:
      STATS_INC(g_stats.system_state_building);
      break;
    case BAM_SYSTEM_STATE_OK:
      STATS_INC(g_stats.system_state_ok);
      break;
  }

  printf("%-20s -> %s\n"
    , attrs16_name_byvalue(bam_system_state_attrs, system_state)
    , attrs16_name_byvalue(bam_system_state_attrs, state)
  );

  if(!events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
    goto end;
  }

  msg->size = zcatf(msg->text, sizeof(msg->text), "%-20s -> %s"
    , attrs16_name_byvalue(bam_system_state_attrs, system_state)
    , attrs16_name_byvalue(bam_system_state_attrs, state)
  );
  events_publish(chan, msg);

end:
  system_state = state;
}

fabipc_message *system_error_event(channel * restrict * restrict first)
{
  fabipc_message *msg;

  system_error = true;
  if(events_would(FABIPC_EVENT_SYSTEM_STATE, first, &msg)) {
    return msg;
  }

  /* fabricate a message for rendering onto */
  *first = 0;
  xmalloc(&msg, sizeof(*msg));
  return msg;
}
