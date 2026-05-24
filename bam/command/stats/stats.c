/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"

#include "stats.h"
#include "params.h"

typeof(stats_command) stats_command;

static void usage(command * restrict cmd)
{
  printf(
"stats positional arguments\n"
" <path>            (optional) select the subject node to show stats for\n"
"                              with no subject, show global stats e.g. //bm/sys-system\n"
"\n"
"stats options\n"
" --reset | -r                 read and clear stats for visited nodes\n"
" --no-reset         (default) read but do not clear stats\n"
"\n"
" --special                    also visit nodes attached via the specials graph\n"
" --no-special       (default) dont traverse into the specials graph\n"
" -s                           also visit nodes attached via the specials graph\n"
" -S                           only visit nodes attached via the specials graph\n"
" --hidden           (default) show files not yet created\n"
" --no-hidden\n"
" --follow                     follow symlinks\n"
" --no-follow        (default) visit, dont follow, symlinks\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message *msg;
  int x;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  /* send the request */
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
        if(cmd->targets_len == 0) {
          { MAPPING
            STRING("path")
            { SET
              { MAPPING STRING("pattern")
                STRING("//bm/sys-system")
              }
            }
          }
        }
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
    if(cmd->reset) {
      STRING("stats-reset")
    } else {
      STRING("stats-read")
    }
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->len = fixed.l;
  client_publish(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->request_id) {
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

static void disconnected(command * restrict cmd)
{
  if(cmd->any) {
    (void)!write(1, "\n", 1);
  }
}

//
// public
//

typeof(stats_command) stats_command = {
    usage : usage
  , name : "stats"
  , connected : connected
  , process : process
  , disconnected : disconnected

  /* defaults */
  , hidden : true
  , nofollow : true
};
