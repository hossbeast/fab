/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "modules.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

static void module_usage(command * restrict cmd)
{
  printf(
"module commands\n"
" envs                  show effective module envs\n"
" vars                  show effective module vars\n"
" rule(s)               list module rules\n"
" rma(s)                list module rmas\n"
  );
}

static void modules_usage(command * restrict cmd)
{
  printf(
"\n"
"modules commands (none)\n"
  );
}

static void module_command_identify_path(struct command * restrict cmd, bacon_writer * restrict builder_writer, const char * restrict s, uint16_t len)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              BYTES(s, len)
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
  module_command_identify_path(cmd, builder_writer, MMS("."));
}

void module_command_identify_id(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  char target[128];
  size_t target_len;

  target_len = zcatf(target, sizeof(target), "//modules/%.*s", cmd->target_len, cmd->target);
  module_command_identify_path(cmd, builder_writer, target, target_len);
}

void module_command_identify_target(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  module_command_identify_path(cmd, builder_writer, cmd->target, cmd->target_len);
}

static void module_command_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              LOADF("//modules/%.*s", cmd->module_id_len, cmd->module_id)
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

static void module_describe_connected(command * restrict cmd, bam_client * restrict client)
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

static void module_describe_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
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
      cmd->module_describe_id = command_post(cmd, client, module_command_describe);
    }
  } else if(msg->id == cmd->module_describe_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->size);
    cmd->any = true;
  } else if(msg->id == cmd->module_describe_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
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
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              STRING(cmd->path)
            }
          }
        }
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
        { MAPPING
          STRING("traverse")
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

typeof(module_command) module_command = {
    name : "module"
  , usage : module_usage
  , connected : module_describe_connected
  , process : module_describe_process
  , disconnected : module_describe_disconnected
};

typeof(modules_command) modules_command = {
    name : "modules"
  , usage : modules_usage
  , connected : list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//modules"
  , visit : "module"
};

void module_command_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
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
    }
  }
}
