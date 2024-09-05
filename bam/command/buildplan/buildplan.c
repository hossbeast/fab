/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "buildplan.h"
#include "args.h"
#include "command.h"
#include "info.h"
#include "params.h"

#include "zcat.h"

struct buildplan_args buildplan_args;
static uint64_t requestid;
static bool any;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"buildplan options (none)\n"
  );
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    { MAPPING
      STRING("select")
      { LIST
        MAPPING
        STRING("lookup")
        { SET
          { MAPPING
            STRING("path")
            LOADF("//buildplan")
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

  msg->size = fixed.l;
  client_post(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  (void)!write(1, msg->text, msg->size);
  (void)!write(1, "\n", 1);
  any = true;
}

static void disconnected(command * restrict cmd)
{
  if(any) {
    printf("\n");
  }
}

//
// public
//

struct command buildplan_command = {
    usage : usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
