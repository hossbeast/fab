/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "fab/client.h"
#include "fab/vertex.h"
#include "fab/ipc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "fstree.h"
#include "args.h"
#include "command.h"
#include "params.h"

#include "zcat.h"

struct fstree_args fstree_args;
static uint64_t requestid;
static bool any;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"fstree options (none)\n"
  );
}

static void connected(command * restrict cmd, fab_client * restrict client)
{
  fabipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    STRING("fstree-describe")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  if(msg->id != requestid) {
    return;
  }

  if(msg->type == FABIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == FABIPC_MSG_RESULT);
  write(1, msg->text, msg->size);
  any = true;
}

static void disconnected(command * restrict cmd)
{
  if(any) {
    printf("\n");
  }
}

static void attached(command * restrict cmd, fab_client * restrict client)
{

}

//
// public
//

struct command fstree_command = {
    usage : usage
  , attached : attached
  , connected : connected
  , process : process
  , disconnected : disconnected
};
