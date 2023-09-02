/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "fab/client.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "touch.h"
#include "command.h"
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

static void connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  int x;

  /* if no targets specified, invalidate everything */
  if(args->targets_len == 0) {
    g_args.invalidate = true;
  }

  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  if(g_args.invalidate) {
    STRING("global-invalidate")
    goto end;
  }

  { LIST
    if(g_args.context_module_id) {
      MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          HEX(g_args.context_module_id)
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

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  }
}

static void attached(command * restrict cmd, fab_client * restrict client)
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
