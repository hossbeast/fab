/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "ipc.h"
#include "pipe.h"
#include "common/attrs.h"

attrs32 bamipc_msg_type_attrs = {
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      BAMIPC_MSG_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
BAMIPC_MSG_TYPE_TABLE
#undef DEF
  }
};

attrs32 bamipc_msg_format_attrs = {
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      BAMIPC_MSG_FORMAT_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
BAMIPC_MSG_FORMAT_TABLE
#undef DEF
  }
};

attrs32 bamipc_msg_code_attrs = {
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      BAMIPC_MSG_CODE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
BAMIPC_MSG_CODE_TABLE
#undef DEF
  }
};

attrs32 bamipc_counter_attrs = {
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      BAMIPC_COUNTER_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
BAMIPC_COUNTER_TABLE
#undef DEF
  }
};

static void __attribute__((constructor)) init()
{
  attrs32_init(&bamipc_msg_type_attrs);
  attrs32_init(&bamipc_msg_code_attrs);
  attrs32_init(&bamipc_counter_attrs);
}

void bamipc_channel_init(struct bamipc_channel * restrict channel)
{
  bamipc_pipe_init(&channel->rx);
  bamipc_pipe_init(&channel->tx);
}

void bamipc_channel_reset(struct bamipc_channel * restrict channel)
{
  bamipc_pipe_reset(&channel->rx);
  bamipc_pipe_reset(&channel->tx);

  channel->client_pid = 0;
  channel->client_tid = 0;
  channel->client_exit = 0;
  channel->server_pid = 0;
  channel->server_tid = 0;
  channel->server_exit = 0;
}
