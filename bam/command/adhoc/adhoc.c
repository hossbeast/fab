/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"

#include "adhoc.h"

#include "zcat.h"
#include "params.h"

struct adhoc_args adhoc_args;
static uint64_t requestid;
static bool any;

static void usage(command * restrict cmd)
{
  printf(
"adhoc [request]\n"
  );
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  size_t z;

  /* send the request */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += zcatw(msg->text + z, msg->size - z, adhoc_args.request, adhoc_args.request_len);
  z += zcatw(msg->text + z, msg->size - z, (char[]) { 0x00, 0x00 }, 2);
  msg->len = z;

  client_publish(client, msg);
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
  if(msg->code != 0) {
    return;
  }

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

command adhoc_command = {
    name : "adhoc"
  , usage : usage
  , connected : connected
  , disconnected : disconnected
  , process : process
};
