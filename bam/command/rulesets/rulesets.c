/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "rulesets.h"
#include "params.h"
#include "modules.h"

//
// ruleset
//

static void ruleset_usage(command * restrict cmd)
{
  printf(
"bam ruleset - show a ruleset\n"
"\n"
"usage: bam ruleset [<id>|<path>]\n"
" <id>                    ruleset id\n"
" <path>                  path to ruleset file\n"
  );
}

//
// rulesets
//

static void rulesets_usage(command * restrict cmd)
{
  printf(
"bam rulesets - list rulesets\n"
"\n"
"usage: bam rulesets\n"
  );
}

//
// module rulesets
//

static void module_ruleset_usage(command * restrict cmd)
{
  printf(
"bam module ruleset - show a module rulesets metadata\n"
"\n"
"usage: bam module [<id>|<path>] ruleset [<ruleset-id>|<ruleset-path>]\n"
" <id>                    module id\n"
" <path>                  path to file or directory owned by the module\n"
" <ruleset-id>            ruleset id\n"
" <ruleset-path>          path to ruleset\n"
  );
}

static void module_rulesets_usage(command * restrict cmd)
{
  printf(
"bam module rulesets - list a modules rulesets\n"
"\n"
"usage: bam module [<id>|<path>] vars\n"
" <id>                    module id\n"
" <path>                  path to file or directory owned by the module\n"
  );
}

static void module_rulesets_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(module_identify_msg(cmd, client, msg)) {
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/rulesets", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
    return;
  }

  if(msg->id == cmd->list_id) {
    /* list : print the module rulesets */
    command_list_msg(cmd, msg);
    return;
  }

  RTA;
}

static void ruleset_identify_by_path(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST

    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              narrator_sayw(builder_writer->N, cmd->ruleset_path, cmd->ruleset_path_len);
              bacon_writer_pop_bytes(builder_writer);
            }
          }
        }
        { MAPPING STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("visit")
              STRING("ruleset")
            }
            { MAPPING STRING("min-distance")
              UINT(1)
            }
            { MAPPING STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }

    STRING("identify")
  }
}

static void module_ruleset_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/modules/%.*s/rulesets/%.*s", cmd->module_id_len, cmd->module_id, cmd->ruleset_id_len, cmd->ruleset_id)
            }
            /* above selector selects shadow link to the ruleset */
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

static void ruleset_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rulesets/%.*s", cmd->ruleset_id_len, cmd->ruleset_id)
            }
            /* above selector selects shadow link to the ruleset */
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

static void ruleset_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->ruleset_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, ruleset_identify_by_path);
    return;
  }

  cmd->describe_id = command_post(cmd, client, ruleset_describe);
}

static void ruleset_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_1) {
    /* identify : resolve ruleset path -> id */
    if(!command_identify_msg(msg, cmd->ruleset_id, sizeof(cmd->ruleset_id), &cmd->ruleset_id_len)) {
      return;
    }
    if(cmd->ruleset_id_len == 0) {
      command_notfound("ruleset", cmd->ruleset_path, cmd->ruleset_path_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, ruleset_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the ruleset */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

static void module_ruleset_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(!module_identify_msg(cmd, client, msg)) {
      return;
    }
    if(cmd->ruleset_id_len == 0) {
      cmd->identify_id_2 = command_post(cmd, client, ruleset_identify_by_path);
    } else {
      cmd->describe_id = command_post(cmd, client, module_ruleset_describe);
    }
    return;
  }

  if(msg->id == cmd->identify_id_2) {
    /* identify : resolve ruleset path -> id */
    if(!command_identify_msg(msg, cmd->ruleset_id, sizeof(cmd->ruleset_id), &cmd->ruleset_id_len)) {
      return;
    }
    if(cmd->ruleset_id_len == 0) {
      command_notfound("ruleset", cmd->ruleset_path, cmd->ruleset_path_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, module_ruleset_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the ruleset */
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

typeof(ruleset_command) ruleset_command = {
    name : "ruleset"
  , usage : ruleset_usage
  , connected : ruleset_connected
  , process : ruleset_process
  , disconnected : disconnected
  , prefix : "//bm/rulesets"
  , visit : "ruleset"
};

typeof(rulesets_command) rulesets_command = {
    name : "rulesets"
  , usage : rulesets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : disconnected
  , path : "//bm/rulesets"
  , visit : "ruleset"
};

/* by module */

typeof(module_ruleset_command) module_ruleset_command = {
    name : "module ruleset"
  , usage : module_ruleset_usage
  , connected : module_command_connected
  , process : module_ruleset_process
  , disconnected : disconnected
};

typeof(module_rulesets_command) module_rulesets_command = {
    name : "module rulesets"
  , usage : module_rulesets_usage
  , connected : module_command_connected
  , process : module_rulesets_process
  , disconnected : disconnected
};
