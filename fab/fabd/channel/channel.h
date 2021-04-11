/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FABD_CHANNEL_H
#define FABD_CHANNEL_H

/*

*/

#include "types.h"
#include "xapi.h"

#include "fab/ipc.h"

struct configblob;
struct fabipc_channel;

typedef struct channel {
  union {
    struct {
      struct fabipc_channel ipc;

      /* whether an error has been returned on this channel */
      bool error;

      /* message id to use for messages on this channel */
      uint32_t msgid;
    };

    char pad[FABIPC_SHMSIZE];
  };
} channel;

STATIC_ASSERT(sizeof(channel) == FABIPC_SHMSIZE);

/* allocate an ipc channel
 *
 * chanp     - (returns) the channel
 * thread_id - thread id for the handler of this channel
 */
xapi channel_create(struct channel ** restrict chanp, pid_t thread_id)
  __attribute__((nonnull));

void channel_release(struct channel * restrict chan)
  __attribute__((nonnull));

/* send/receive messages over the channel */

struct fabipc_message * channel_produce(channel * restrict ctx)
  __attribute__((nonnull));

void channel_post(channel * restrict ctx, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

struct fabipc_message * channel_acquire(channel * restrict ctx)
  __attribute__((nonnull));

void channel_consume(channel * restrict ctx, struct fabipc_message * restrict msg)
  __attribute__((nonnull));

void channel_response(channel * restrict ctx, int code)
  __attribute__((nonnull));

void channel_responses(channel * restrict ctx, int code, const char * restrict text)
  __attribute__((nonnull(1, 3)));

void channel_responsew(channel * restrict ctx, int code, const char * restrict text, uint16_t text_len)
  __attribute__((nonnull(1)));

/* module level */

xapi channel_setup(void);

xapi channel_cleanup(void);

xapi channel_reconfigure(struct configblob * restrict cfg, bool dry)
  __attribute__((nonnull));

#endif
