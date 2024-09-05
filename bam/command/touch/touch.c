/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "bam/client.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "touch.h"
#include "command.h"
#include "info.h"
#include "params.h"

static uint64_t requestid;
struct touch_args touch_args;
static struct touch_args *args = &touch_args;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"\n"
"touch [<path>...]\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  int x;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  /* if no targets specified, invalidate everything */
  if(args->targets_len == 0) {
    STRING("invalidate-primary")
    goto end;
  }

  { LIST
    if(g_args.context_module_id_len) {
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
        for(x = 0; x < args->targets_len; x++)
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              BYTES(args->targets[x].s, args->targets[x].len)
            }
            { MAPPING
              STRING("hidden")
              BOOL(true)
            }
          }
        }
      }
    }
    STRING("invalidate")
  }

end:
  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  }
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

//
// public
//
static void disconnected(command * restrict cmd) { }

struct command touch_command = {
    usage : usage
  , attached : attached
  , connected : connected
  , process : process
  , disconnected : disconnected
};
