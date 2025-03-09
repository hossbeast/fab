/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/ipc.h"
#include "bacon/writer.h"
#include "narrator/fixed.h"

#include "request_handler.h"
#include "request.h"
#include "command.h"
#include "describe.h"
#include "channel.h"

void describe_handler(request_handler * restrict ctx, command * restrict cmd)
{
  bamipc_message *msg;
  selected *sn;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  llist_foreach(&ctx->selection->list, sn, lln) {
    msg = request_handler_tx(ctx, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);

    narrator_fixed_init(&fixed, msg->text, msg->size);
    bacon_writer_init(&writer);
    builder_writer = bacon_writer_open(&writer, &fixed.base);

    vertex_describe(sn->v, builder_writer);

    bacon_writer_destroy(&writer);
    msg->len = fixed.l;
    request_handler_post(ctx, msg);
  }
}
