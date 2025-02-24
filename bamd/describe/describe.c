/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bacon/writer.h"
#include "narrator/fixed.h"

#include "handler.h"
#include "request.h"
#include "describe.h"
#include "channel.h"

void describe_handler(handler_context * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  llist_foreach(&ctx->selection->list, sn, lln) {
    msg = channel_produce(ctx->chan);
    msg->type = BAMIPC_MSG_RESULT;

    narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    vertex_describe(sn->v, builder_writer);

    bacon_writer_destroy(&writer);
    msg->size = fixed.l;
    channel_post(ctx->chan, msg);
  }
}
