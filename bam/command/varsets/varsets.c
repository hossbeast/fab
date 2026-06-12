/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "varsets.h"
#include "params.h"
#include "modules.h"

static void varsets_usage(command * restrict cmd)
{
  printf(
"bam varsets - list varsets\n"
"\n"
"usage: bam varsets\n"
  );
}

static void varset_usage(command * restrict cmd)
{
  printf(
"bam varset - show varset metadata\n"
"\n"
"usage: bam varset (<id>|<path>)\n"
" <id>                         varset id\n"
" <path>                       path to varset file\n"
  );
}

//
// module varsets
//

static void module_varsets_usage(command * restrict cmd)
{
  printf(
"bam module varsets - list a modules varsets\n"
"\n"
"usage: bam module [<id>|<path>] varsets\n"
" <id>                         module id\n"
" <path>                       path to file or directory owned by the module\n"
  );
}

/*
 * The module varsets command runs in two phases. First the module is identified from
 * the command target. Then the varsets linked under the module are listed. Each request
 * elicits zero or more RESULT messages carrying output, followed by exactly one
 * RESPONSE message marking completion.
 */
static void module_varsets_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(module_identify_msg(cmd, client, msg)) {
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/varsets", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
    return;
  }

  if(msg->id == cmd->list_id) {
    /* list : print the module varsets */
    command_list_msg(cmd, msg);
    return;
  }

  RTA;
}

//
// module varset
//

static void module_varset_usage(command * restrict cmd)
{
  printf(
"bam module varset - show a module varsets metadata\n"
"\n"
"usage: bam module [<id>|<path>] varset [<varset-id>|<varset-path>]\n"
" <id>                         module id\n"
" <path>                       path to file or directory owned by the module\n"
" <varset-id>                  varset id\n"
" <varset-path>                path to varset file\n"
  );
}

static void varset_identify_by_path(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              narrator_sayw(builder_writer->N, cmd->varset_path, cmd->varset_path_len);
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
              STRING("varset")
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

static void module_varset_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/modules/%.*s/varsets/%.*s", cmd->module_id_len, cmd->module_id, cmd->varset_id_len, cmd->varset_id)
            }
            /* above selector selects shadow link to the varset */
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

/*
 * The module varset command runs in up to three phases. First the module is identified
 * from the command target. When invoked with a varset path rather than an id, the path
 * is then resolved to an id (identify). The varset is then looked up under the module
 * and printed (describe). Each request elicits zero or more RESULT messages carrying
 * output, followed by exactly one RESPONSE message marking completion.
 */
static void module_varset_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(!module_identify_msg(cmd, client, msg)) {
      return;
    }
    if(cmd->varset_id_len == 0) {
      cmd->identify_id_2 = command_post(cmd, client, varset_identify_by_path);
    } else {
      cmd->describe_id = command_post(cmd, client, module_varset_describe);
    }
    return;
  }

  if(msg->id == cmd->identify_id_2) {
    /* identify : resolve varset path -> id */
    if(!command_identify_msg(msg, cmd->varset_id, sizeof(cmd->varset_id), &cmd->varset_id_len)) {
      return;
    }
    if(cmd->varset_id_len == 0) {
      command_notfound("varset", cmd->varset_path, cmd->varset_path_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, module_varset_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the varset */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

typeof(varset_command) varset_command = {
    name : "varset"
  , usage : varset_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//bm/varsets"
  , visit : "varset"
};

typeof(varsets_command) varsets_command = {
    name : "varsets"
  , usage : varsets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/varsets"
  , visit : "varset"
};

/* by module */

typeof(module_varsets_command) module_varsets_command = {
    name : "module varsets"
  , usage : module_varsets_usage
  , connected : module_command_connected    // first, identify the module
  , process : module_varsets_process
  , disconnected : special_list_disconnected
};

typeof(module_varset_command) module_varset_command = {
    name : "module varset"
  , usage : module_varset_usage
  , connected : module_command_connected    // first, identify the module
  , process : module_varset_process
  , disconnected : special_describe_disconnected
};
