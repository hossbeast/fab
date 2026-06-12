/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator.h"

#include "fstrees.h"
#include "params.h"

//
// usage
//

static void fstrees_usage(command * restrict cmd)
{
  printf(
"bam fstrees - list fstrees\n"
"\n"
"usage: bam fstrees\n"
  );
}

static void fstree_usage(command * restrict cmd)
{
  printf(
"bam fstree - show fstree metadata\n"
"\n"
"usage: bam fstree [<id>|<path>]\n"
" <id>                         fstree id\n"
" <path>                       path to file or directory owned by the fstree\n"
  );
}

static void fstree_identify(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    command_context(builder_writer);
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              if(cmd->fstree_id_len) {
                LOADF("//bm/fstrees/%.*s", cmd->fstree_id_len, cmd->fstree_id)
              } else if(cmd->target_len) {
                bacon_writer_push_bytes(builder_writer);
                narrator_sayw(builder_writer->N, cmd->target, cmd->target_len);
                bacon_writer_pop_bytes(builder_writer);
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
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              LOADF("//bm/fstrees/%.*s", cmd->fstree_id_len, cmd->fstree_id)
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
  if(msg->id == cmd->identify_id) {
    /* identify : resolve the target -> fstree id */
    if(!command_identify_msg(msg, cmd->fstree_id, sizeof(cmd->fstree_id), &cmd->fstree_id_len)) {
      return;
    }
    if(cmd->fstree_id_len == 0) {
      command_notfound("fstree", cmd->target, cmd->target_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, fstree_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the fstree */
    command_list_msg(cmd, msg);
    return;
  }

  RTA;
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

typeof(fstree_command) fstree_command = {
    name : "fstree"
  , usage : fstree_usage
  , connected : describe_connected
  , process : describe_process
  , disconnected : describe_disconnected
};

typeof(fstrees_command) fstrees_command = {
    name : "fstrees"
  , usage : fstrees_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/fstrees"
  , visit : "fstree"
};
