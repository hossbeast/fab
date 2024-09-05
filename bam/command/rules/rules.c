/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "rules.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

//
// rule
//

static void rule_usage(command * restrict cmd)
{
  printf(
"rule options (none)\n"
  );
}

//
// rules
//

static void rules_usage(command * restrict cmd)
{
  printf(
"\n"
"rules options (none)\n"
  );
}

//
// module rules
//

static void module_rules_usage(command * restrict cmd)
{
  printf(
"\n"
"module rules options\n"
  );
}

static void module_rules_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->target_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_dot);
  } else if(memmem(cmd->target, cmd->target_len, MMS("/")) != 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  } else {
    cmd->identify_id_0 = command_post(cmd, client, module_command_identify_id);
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  }

  message_no_error_list[0] = cmd->identify_id_0;
  message_no_error_list[1] = cmd->identify_id_1;
  message_no_error_list_len = 2;
}

static void module_rules_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->size > 10) {
      cmd->module_id_len = command_identify_result_parse(msg, cmd->module_id, sizeof(cmd->module_id));
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->size > 10) {
      cmd->module_id_len = command_identify_result_parse(msg, cmd->module_id, sizeof(cmd->module_id));
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      snprintf(cmd->path, sizeof(cmd->path), "//modules/%.*s/rules", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->size);
    cmd->any = true;
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
}

//
// public
//

typeof(rule_command) rule_command = {
    name : "rule"
  , usage : rule_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected

  , prefix : "//rules"
  , visit : "rule"
};

typeof(rules_command) rules_command = {
    name : "rules"
  , usage : rules_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//rules"
  , visit : "rule"
};

typeof(module_rules_command) module_rules_command = {
    name : "rules"
  , usage : module_rules_usage
  , connected : module_rules_connected
  , process : module_rules_process
  , disconnected : special_list_disconnected
};
