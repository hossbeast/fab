/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "describe.h"
#include "args.h"
#include "command.h"
#include "info.h"
#include "params.h"

#include "zcat.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"describe [<path>...]\n"
  );
}

void command_describe_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  int x;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  cmd->request_id = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    if(cmd->context_module && g_args.context_module_id_len) {
      MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          BYTES(g_args.context_module_id, g_args.context_module_id_len)
        }
      }
    }
    { MAPPING
      STRING("select")
      { LIST
        for(x = 0; x < cmd->targets_len; x++) {
          { MAPPING
            STRING("lookup")
            { SET
              { MAPPING STRING("path")
                BYTES(cmd->targets[x].s, cmd->targets[x].len)
              }
              { MAPPING STRING("hidden")
                BOOL(true)
              }
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
            if(cmd->special == 0) {
              { MAPPING STRING("min-distance") UINT(0) }
              { MAPPING STRING("max-distance") UINT(0) }
            } else if(cmd->special == 1) {
              { MAPPING STRING("min-distance") UINT(0) }
              { MAPPING STRING("max-distance") UINT(1) }
            } else {
              { MAPPING STRING("min-distance") UINT(1) }
              { MAPPING STRING("max-distance") UINT(1) }
            }
          }
        }
      }
    }
    STRING("describe")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

void command_describe_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->request_id) {
    return;
  }
  if(msg->code != 0) {
    g_params.shutdown = true;
    return;
  }
  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }
  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);

  if(cmd->any) {
    (void)!write(1, "\n", 1);
  }
  (void)!write(1, msg->text, msg->size);
  cmd->any = true;
}

void command_describe_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    (void)!write(1, "\n", 1);
  }
}

//
// public
//

typeof(describe_command) describe_command = {
    usage : usage
  , connected : command_describe_connected
  , process : command_describe_process
  , disconnected : command_describe_disconnected
  , context_module : true
};
