/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/events.h"
#include "bam/ipc.h"

#include "adhoc.h"

#include "zcat.h"
#include "params.h"

struct adhoc_args adhoc_args;
static uint64_t requestid;

static void usage(command * restrict cmd)
{
  printf(
"adhoc [request]\n"
  );
}

static void adhoc_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  size_t z;

  /* send the request */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  z = 0;
  z += zcatw(msg->text + z, sizeof(msg->text) - z, adhoc_args.request.s, adhoc_args.request.len);
  z += zcatw(msg->text + z, sizeof(msg->text) - z, (char[]) { 0x00, 0x00 }, 2);
  msg->size = z;

  client_post(client, msg);
}

static void adhoc_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  RUNTIME_ASSERT(msg->id == requestid);

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    if(msg->code != 0) {
      fprintf(stderr, "response code %d msg %.*s\n", msg->code, msg->size, msg->text);
    }
    g_params.shutdown = true;
    return;
  }

  if(msg->type == BAMIPC_MSG_RESULT) {
    write(1, msg->text, msg->size);
    write(1, "\n", 1);
  }
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

command adhoc_command = {
    name : "adhoc"
  , attached : attached
  , usage : usage
  , connected : adhoc_connected
  , process : adhoc_process
};
