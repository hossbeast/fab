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

static void module_rule_usage(command * restrict cmd)
{
  printf(
"bam rule - show a rules metadata\n"
"\n"
"usage: bam rule [<id>|<name>]\n"
" <id>                    rule id\n"
" <name>                  name of rule\n"
  );
}

static void rule_usage(command * restrict cmd)
{
  printf(
"bam rule - show a rules metadata\n"
"\n"
"usage: bam rule [<id>|<name>]\n"
" <id>                    rule id\n"
" <name>                  name of rule\n"
  );
}

static void module_rule_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/modules/%.*s/rules-by/name/%.*s", cmd->module_id_len, cmd->module_id, cmd->rule_name_len, cmd->rule_name)
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
  if(msg->id == cmd->identify_id_1) {
    /* identify : resolve rule name -> id */
    if(!command_identify_msg(msg, cmd->rule_id, sizeof(cmd->rule_id), &cmd->rule_id_len)) {
      return;
    }
    if(cmd->rule_id_len == 0) {
      command_notfound("rule", cmd->rule_name, cmd->rule_name_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, rule_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the rule */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

static void module_rule_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(!module_identify_msg(cmd, client, msg)) {
      return;
    }
    if(cmd->rule_id_len == 0) {
      cmd->identify_id_2 = command_post(cmd, client, module_rule_identify_by_name);
    } else {
      cmd->describe_id = command_post(cmd, client, rule_describe);
    }
    return;
  }

  if(msg->id == cmd->identify_id_2) {
    /* identify : resolve rule name -> id */
    if(!command_identify_msg(msg, cmd->rule_id, sizeof(cmd->rule_id), &cmd->rule_id_len)) {
      return;
    }
    if(cmd->rule_id_len == 0) {
      command_notfound("rule", cmd->rule_name, cmd->rule_name_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, rule_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the rule */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

static void disconnected(command * restrict cmd)
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
"bam rules - list rules\n"
"\n"
"usage: bam module [<id>|<path>] rules\n"
" <id>                    module id\n"
" <path>                  path to file or directory owned by the module\n"
  );
}
//
// module rules
//

static void module_rules_usage(command * restrict cmd)
{
  printf(
"bam module rules - list a modules rules\n"
"\n"
"usage: bam module [<id>|<path>] vars\n"
" <id>                    module id\n"
" <path>                  path to file or directory owned by the module\n"
  );
}

static void module_rules_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(module_identify_msg(cmd, client, msg)) {
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/rules", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
    return;
  }

  if(msg->id == cmd->list_id) {
    /* list : print the module rules */
    command_list_msg(cmd, msg);
    return;
  }

  RTA;
}

//
// public
//


typeof(rule_command) rule_command = {
    name : "rule"
  , usage : rule_usage
  , connected : rule_connected
  , process : rule_process
  , disconnected : disconnected
};

typeof(rules_command) rules_command = {
    name : "rules"
  , usage : rules_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : disconnected
  , path : "//bm/rules"
  , visit : "rule"
};

/* by module */

typeof(rule_command) module_rule_command = {
    name : "module rule"
  , usage : module_rule_usage
  , connected : module_command_connected  // first, identify the module
  , process : module_rule_process
  , disconnected : disconnected
};

typeof(module_rules_command) module_rules_command = {
    name : "module rules"
  , usage : module_rules_usage
  , connected : module_command_connected  // first, identify the module
  , process : module_rules_process
  , disconnected : disconnected
};
