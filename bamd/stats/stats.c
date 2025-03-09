/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "descriptor.h"
#include "bacon/builder.h"
#include "bam/ipc.h"
#include "narrator/fixed.h"

#include "stats.h"
#include "request_handler.h"
#include "request.h"
#include "command.h"
#include "channel.h"

//
// public
//

void stats_build(
    const stats_record * restrict record
  , const descriptor_type * restrict descriptor
  , bacon_writer * restrict builder_writer
)
{
  const descriptor_field *member;
  int x;

  for(x = 0; x < descriptor->members_len; x++)
  {
    member = descriptor->members[x];
    if(member->code != DESCRIPTOR_UINT) {
      continue;
    }
    { MAPPING
      BYTES(member->name, member->name_len)
      switch(member->size) {
        case 8:
          UINT(record->u64[member->offset / 8]);
          break;
        case 4:
          UINT(record->u32[member->offset / 4]);
          break;
        case 2:
          UINT(record->u16[member->offset / 2]);
          break;
        case 1:
          UINT(record->u8[member->offset / 1]);
          break;
        default:
          break;
      }
    }
  }
}

void stats_handler(request_handler * restrict ctx, command * restrict cmd, bool reset)
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

    vertex_stats_describe(sn->v, reset, builder_writer);

    bacon_writer_destroy(&writer);
    msg->len = fixed.l;
    request_handler_post(ctx, msg);
  }
}
