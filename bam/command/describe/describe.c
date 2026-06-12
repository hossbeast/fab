/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"

#include "args.h"
#include "command.h"
#include "info.h"
#include "params.h"


//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"bam describe - show nodes metadata\n"
"\n"
"usage: bam describe <path>... [option]...\n"
" <path>                       path to target to describe\n"
"\n"
"describe options\n"
" --special | -s               also show nodes attached via the specials graph\n"
" --no-special       (default) dont traverse into the specials graph\n"
" -S                           only show nodes attached via the specials graph\n"
" --hidden           (default) show files which have not yet been created\n"
" --no-hidden                  dont visit hidden files\n"
" --follow                     follow symlinks\n"
" --no-follow        (default) visit, dont follow, symlinks\n"
  );
}

void command_describe_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  int x;

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  cmd->request_id = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    command_context(builder_writer);
    { MAPPING
      STRING("select")
      { LIST
        for(x = 0; x < cmd->targets_len; x++) {
          { MAPPING
            STRING("path")
            { SET
              { MAPPING STRING("pattern")
                bacon_writer_push_bytes(builder_writer);
                narrator_sayw(builder_writer->N, cmd->targets[x], cmd->targets_lens[x]);
                bacon_writer_pop_bytes(builder_writer);
              }
              { MAPPING STRING("nofollow")
                BOOL(cmd->nofollow)
              }
              { MAPPING STRING("hidden")
                BOOL(cmd->hidden)
              }
            }
          }
        }
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("nofollow")
              BOOL(true)
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

  msg->len = fixed.l;
  client_publish(client, msg);
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
  (void)!write(1, msg->text, msg->len);
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
  , name : "describe"
  , connected : command_describe_connected
  , process : command_describe_process
  , disconnected : command_describe_disconnected

  /* defaults */
  , hidden : true
  , nofollow : true
};
