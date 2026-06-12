/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"

#include "plan.h"
#include "args.h"
#include "command.h"
#include "params.h"


struct plan_args plan_args;
static uint64_t requestid;
static bool any;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"bam plan - show the build plan\n"
"           does not update the buildplan ; use reconcile first, to do that\n"
"\n"
"usage: bam plan\n"
  );
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    { MAPPING STRING("select")
      { LIST
        MAPPING STRING("path")
        { SET
          { MAPPING STRING("pattern")
            LOADF("//bm/sys-plan")
          }
        }
      }
    }

    STRING("tell")
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
  if(msg->code != 0) {
    g_params.shutdown = true;
    return;
  } else if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  if(any) {
    (void)!write(1, "\n", 1);
  }
  (void)!write(1, msg->text, msg->len);
  any = true;
}

static void disconnected(command * restrict cmd)
{
  if(any) {
    (void)!write(1, "\n", 1);
  }
}

//
// public
//

struct command plan_command = {
    usage : usage
  , name : "plan"
  , connected : connected
  , process : process
  , disconnected : disconnected
};
