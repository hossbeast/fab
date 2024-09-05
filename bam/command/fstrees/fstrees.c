/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"

#include "fstrees.h"
#include "params.h"

//
// fstree
//

static void fstree_usage(command * restrict cmd)
{
  printf(
"fstree options (none)\n"
  );
}

static void fstree_identify(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    command_context(cmd, builder_writer);
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              if(cmd->fstree_id_len) {
                LOADF("//fstrees/%.*s", cmd->fstree_id_len, cmd->fstree_id)
              } else if(cmd->target_len) {
                BYTES(cmd->target, cmd->target_len)
              } else {
                STRING(".")
              }
            }
          }
        }
        STRING("fstree-of")
      }
    }

    STRING("identify")
  }
}

static void fstree_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
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
              LOADF("//fstrees/%.*s", cmd->fstree_id_len, cmd->fstree_id)
            }
            { MAPPING
              STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }

    STRING("describe")
  }
}

static void describe_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->fstree_id_len == 0) {
    cmd->identify_id = command_post(cmd, client, fstree_identify);
    return;
  }

  cmd->describe_id = command_post(cmd, client, fstree_describe);
}

static void describe_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->identify_id && msg->id != cmd->describe_id) {
    RTA; // wtf
  }
  if(msg->code != 0) {
    goto end;
  }

  if(msg->id == cmd->identify_id && msg->type == BAMIPC_MSG_RESULT) {
    cmd->fstree_id_len = command_identify_result_parse(msg, cmd->fstree_id, sizeof(cmd->fstree_id));
    cmd->describe_id = command_post(cmd, client, fstree_describe);
  } else if(msg->id == cmd->identify_id && msg->type == BAMIPC_MSG_RESPONSE) {
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->size);
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

static void describe_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

//
// fstrees
//

static void fstrees_usage(command * restrict cmd)
{
  printf(
"\n"
"fstrees options (none)\n"
  );
}

typeof(fstree_command) fstree_command = {
    name : "fstree"
  , usage : fstree_usage
  , connected : describe_connected
  , process : describe_process
  , disconnected : describe_disconnected
//  , prefix : "//fstrees"
//  , visit : "fstree"
};

typeof(fstrees_command) fstrees_command = {
    name : "fstrees"
  , usage : fstrees_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//fstrees"
  , visit : "fstree"
};
