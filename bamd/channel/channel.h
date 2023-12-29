/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

*/

#include "types.h"

#include "bam/ipc.h"

struct config;
struct bamipc_channel;

typedef struct channel {
  union {
    struct {
      struct bamipc_channel ipc;

      /* subscribed events */
      uint32_t event_mask;

      /* whether a response with nonzero code has been txd on this channel */
      bool error;

      /* message id to use for messages on this channel */
      uint64_t msgid;
    };

    char pad[BAMIPC_SHMSIZE];
  };
} channel;

STATIC_ASSERT(sizeof(channel) == BAMIPC_SHMSIZE);

/* allocate an ipc channel
 *
 * chanp     - (returns) the channel
 * thread_id - thread id for the handler of this channel
 */
void channel_create(struct channel ** restrict chanp, pid_t thread_id)
  __attribute__((nonnull));

void channel_release(struct channel * restrict chan)
  __attribute__((nonnull));

/* send/receive messages over the channel */

struct bamipc_message * channel_produce(channel * restrict ctx)
  __attribute__((nonnull));

void channel_post(channel * restrict ctx, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

struct bamipc_message * channel_acquire(channel * restrict ctx)
  __attribute__((nonnull));

void channel_consume(channel * restrict ctx, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void channel_response(channel * restrict ctx, int code)
  __attribute__((nonnull));

void channel_responses(channel * restrict ctx, int code, const char * restrict text)
  __attribute__((nonnull(1, 3)));

void channel_responsew(channel * restrict ctx, int code, const char * restrict text, uint16_t text_len)
  __attribute__((nonnull(1)));

/* module level */

void channel_setup(void);

void channel_cleanup(void);

void channel_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));
