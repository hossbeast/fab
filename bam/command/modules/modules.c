/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "modules.h"
#include "params.h"
#include "zcat.h"

static void module_usage(command * restrict cmd)
{
  printf(
"bam module - show metadata about a module\n"
"\n"
"usage: bam module [<id>|<path>] [subcommand]\n"
" <id>                         module id\n"
" <path>                       path to file or directory owned by the module\n"
"\n"
"module subcommands\n"
" envs                         show effective module envs\n"
" vars                         show effective module vars\n"
" rule(s)                      list or show the modules rules\n"
" ruleset(s)                   list or show the modules rulesets\n"
" varset(s)                    list or show the modules varsets\n"
" rma(s)                       list or show the modules rmas\n"
  );
}

static void modules_usage(command * restrict cmd)
{
  printf(
"modules commands (none)\n"
  );
}

static void module_identify_by_path(struct command * restrict cmd, bacon_writer * restrict builder_writer, const char * restrict s, uint16_t len)
{
  { LIST
    command_context(builder_writer);
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              narrator_sayw(builder_writer->N, s, len);
              bacon_writer_pop_bytes(builder_writer);
            }
            { MAPPING STRING("expect")
              STRING("any")
            }
          }
        }
        STRING("module-of")
      }
    }

    STRING("identify")
  }
}

void module_command_identify_dot(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  module_identify_by_path(cmd, builder_writer, MMS("."));
}

void module_command_identify_id(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  char target[128];
  size_t target_len;

  target_len = zcatf(target, sizeof(target), "//bm/modules/%.*s", cmd->target_len, cmd->target);
  module_identify_by_path(cmd, builder_writer, target, target_len);
}

void module_command_identify_target(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  module_identify_by_path(cmd, builder_writer, cmd->target, cmd->target_len);
}

static void module_command_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              LOADF("//bm/modules/%.*s", cmd->module_id_len, cmd->module_id)
            }
            { MAPPING
              STRING("nofollow")
              BOOL(false)
            }
          }
        }
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING
              STRING("direction")
              STRING("up")
            }
            { MAPPING
              STRING("graph")
              STRING("special")
            }
            { MAPPING
              STRING("min-distance")
              UINT(1)
            }
            { MAPPING
              STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }

    STRING("describe")
  }
}

void module_command_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->target_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_dot);
  } else if(memmem(cmd->target, cmd->target_len, MMS("/")) != 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  } else {
    cmd->identify_id_0 = command_post(cmd, client, module_command_identify_id);
  }
}

bool module_identify_msg(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {  /* shorter results cannot contain an id */
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
    return false;
  }
  if(msg->type != BAMIPC_MSG_RESPONSE) {
    return false;
  }

  /* identify by id complete, possibly in error ; also identify by path */
  if(msg->id == cmd->identify_id_0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
    return false;
  }

  /* identify complete */
  if(cmd->module_id_len == 0) {
    command_notfound("module", cmd->target, cmd->target_len);
    return false;
  }

  return true;
}

static void module_describe_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 || msg->id == cmd->identify_id_1) {
    /* identify : resolve the module target -> module id */
    if(module_identify_msg(cmd, client, msg)) {
      cmd->module_describe_id = command_post(cmd, client, module_command_describe);
    }
    return;
  }

  if(msg->id == cmd->module_describe_id) {
    /* describe : print the module */
    command_list_msg(cmd, msg);
    return;
  }

  RTA;
}

static void module_describe_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

/* list */
static void list_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              STRING(cmd->path)
            }
          }
        }
        /* listing of module directories */
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING
              STRING("nofollow")
              BOOL(false) // follow links
            }
            { MAPPING
              STRING("direction")
              STRING("down")
            }
            { MAPPING
              STRING("graph")
              STRING("fs")
            }
            { MAPPING
              STRING("min-distance")
              UINT(1)
            }
            { MAPPING
              STRING("max-distance")
              UINT(1)
            }
          }
        }
        /* upwards to the modules themselves */
        { MAPPING STRING("traverse")
          { SET
            { MAPPING
              STRING("nofollow")
              BOOL(false) // follow links
            }
            { MAPPING
              STRING("direction")
              STRING("up")
            }
            { MAPPING
              STRING("graph")
              STRING("special")
            }
            { MAPPING
              STRING("min-distance")
              UINT(1)
            }
            { MAPPING
              STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }
    STRING("list")
  }
}

static void list_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->list_id = command_post(cmd, client, list_build);
}

typeof(modules_command) modules_command = {
    name : "modules"
  , usage : modules_usage
  , connected : list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/modules"
  , visit : "module"
};

typeof(module_command) module_command = {
    name : "module"
  , usage : module_usage
  , connected : module_command_connected
  , process : module_describe_process
  , disconnected : module_describe_disconnected
};
