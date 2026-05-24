/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"

#include "rules.h"
#include "params.h"
#include "modules.h"


//
// rule
//

static void rule_usage(command * restrict cmd)
{
  printf(
"rule options (none)\n"
  );
}

static void rule_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rules-by/name/%.*s", cmd->rule_name_len, cmd->rule_name)
            }
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }
    STRING("identify")
  }
}

static void rule_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rules/%.*s", cmd->rule_id_len, cmd->rule_id)
            }
            /* above selector selects shadow link to the rule */
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void rule_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->rule_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, rule_identify_by_name);
    return;
  }

  cmd->describe_id = command_post(cmd, client, rule_describe);
}

static void rule_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->identify_id_1 && msg->id != cmd->describe_id) {
    RTA; // wtf
  }
  if(msg->code != 0) {
    goto end;
  }

  if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->rule_id_len = command_identify_result_parse(cmd->rule_id, sizeof(cmd->rule_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->rule_id_len == 0) {
      /* lookup failure */
      if(cmd->rule_id_len) {
        dprintf(2, "rule not found : %.*s\n", (int)cmd->rule_id_len, cmd->rule_id);
      } else if(cmd->rule_name_len) {
        dprintf(2, "rule not found : %.*s\n", (int)cmd->rule_name_len, cmd->rule_name);
      }
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, rule_describe);
    }
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESULT) {
    if(cmd->any) {
      (void)!write(1, "\n", 1);
    }
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESPONSE) {
    goto end;
  } else {
    RTA;
  }
  return;

end:
  g_params.shutdown = true;
}

static void rule_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

//
// rules
//

static void rules_usage(command * restrict cmd)
{
  printf(
"rules options (none)\n"
  );
}

//
// module rules
//

static void module_rules_usage(command * restrict cmd)
{
  printf(
"module rules options\n"
  );
}

static void module_rules_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
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
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/rules", cmd->module_id_len, cmd->module_id);
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

//
// public
//

typeof(rule_command) rule_command = {
    name : "rule"
  , usage : rule_usage
  , connected : rule_connected
  , process : rule_process
  , disconnected : rule_disconnected
};

typeof(rules_command) rules_command = {
    name : "rules"
  , usage : rules_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/rules"
  , visit : "rule"
};

typeof(module_rules_command) module_rules_command = {
    name : "module rules"
  , usage : module_rules_usage
  , connected : module_command_connected
  , process : module_rules_process
  , disconnected : special_list_disconnected
};
