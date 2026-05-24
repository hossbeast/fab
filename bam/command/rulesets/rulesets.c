/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"

#include "rulesets.h"
#include "params.h"
#include "modules.h"

//
// ruleset
//

static void ruleset_usage(command * restrict cmd)
{
  printf(
"ruleset options (none)\n"
  );
}

//
// rulesets
//

static void rulesets_usage(command * restrict cmd)
{
  printf(
"rulesets options (none)\n"
  );
}

//
// module rulesets
//

static void module_rulesets_usage(command * restrict cmd)
{
  printf(
"module rulesets options\n"
  );
}

static void module_rulesets_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/rulesets", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
}

typeof(ruleset_command) ruleset_command = {
    name : "ruleset"
  , usage : ruleset_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//bm/rulesets"
  , visit : "ruleset"
};

typeof(rulesets_command) rulesets_command = {
    name : "rulesets"
  , usage : rulesets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/rulesets"
  , visit : "ruleset"
};

typeof(module_rulesets_command) module_rulesets_command = {
    name : "module rulesets"
  , usage : module_rulesets_usage
  , connected : module_command_connected
  , process : module_rulesets_process
  , disconnected : special_list_disconnected
};
