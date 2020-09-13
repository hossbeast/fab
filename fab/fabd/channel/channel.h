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

#include <sys/ipc.h>
#include <sys/shm.h>

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "fab/ipc.h"

struct config;

xapi channel_create(fabipc_channel ** restrict chanp, pid_t thread_id)
  __attribute__((nonnull));

xapi channel_xfree(fabipc_channel * restrict chan);

xapi channel_setup(void);

xapi channel_cleanup(void);

xapi channel_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

fabipc_message * channel_produce(fabipc_channel * restrict chan, uint32_t * restrict tail_next, size_t sz)
  __attribute__((nonnull));

void channel_post(fabipc_channel * restrict chan, uint32_t tail_next)
  __attribute__((nonnull));

fabipc_message * channel_acquire(fabipc_channel * restrict chan)
  __attribute__((nonnull));

void channel_consume(fabipc_channel * restrict chan)
  __attribute__((nonnull));

#endif

#if 0
channel_msg * channel_msg_alloc(channel * restrict chan, size_t sz)
  __attribute__((nonnull));

void channel_msg_post(channel * restrict chan)
  __attribute__((nonnull));

channel_msg * channel_msg_acquire(channel * restrict chan)
  __attribute__((nonnull));

void channel_msg_release(channel * restrict chan)
  __attribute__((nonnull));

/* see libfab/ipc.h */
#define CHANNEL_SHMSIZE          (16 * 1024 * 1024)
#define CHANNEL_ENTRYSIZE        8096
#define CHANNEL_CLIENT_RINGSIZE  1024
#define CHANNEL_SERVER_RINGSIZE  1024

#define CHANNEL_MSG_TYPE_OPT  0xff
#define CHANNEL_MSG_TYPE_TABLE                                                \
  DEF(CHANNEL_MSG_REQUEST   , 0x01)  /* request, a bacon-formatted string */  \
  DEF(CHANNEL_MSG_RESPONSE  , 0x02)  /* response, a bacon-formatted string */ \
  DEF(CHANNEL_MSG_STDOUT    , 0x03)  /* stdout text */                        \
  DEF(CHANNEL_MSG_STDERR    , 0x04)  /* stderr text */                        \
  DEF(CHANNEL_MSG_LOGGER    , 0x05)  /* logger configuration */               \
  DEF(CHANNEL_MSG_LOGMSG    , 0x06)  /* log message */                        \

typedef enum channel_msg_type {
#undef DEF
#define DEF(x, y) x = y,
CHANNEL_MSG_TYPE_TABLE
} channel_msg_type;

#define CHANNEL_MSG_ATTRS_FLAGS       0x01
#define CHANNEL_MSG_ATTRS_TABLE              \
  DEF(CHANNEL_MSG_REQUEST_COMPLETE  , 0x01)  \

enum {
#undef DEF
#define DEF(x, y) x = y,
CHANNEL_MSG_ATTRS_TABLE
};

typedef struct channel_msg {
  uint32_t size;
  uint8_t type;
  uint32_t attrs;
  int code;
  char text[];
} channel_msg;

typedef struct channel_entry {
  union {
    channel_msg msg;
    char pad[CHANNEL_ENTRYSIZE];
  };
} channel_entry;

typedef struct channel
{
  union {
    struct {
      int shmid;

      pid_t client_pid;   // fab
      pid_t client_tid;
      uint64_t client_pulse;
      bool client_exit;

      pid_t server_pid;   // fabd
      pid_t server_tid;
      uint64_t server_pulse;
      bool server_exit;

      // messages client -> server
      struct {
        channel_entry entries[CHANNEL_CLIENT_RINGSIZE];
        uint32_t head;     // updated by server when message(s) consumed
        uint32_t tail;     // updated by client when message(s) posted
        int32_t __attribute__((aligned(4))) waiters;
        uint8_t overflow;
      } client_ring;

      // messages server -> client
      struct {
        channel_entry entries[CHANNEL_SERVER_RINGSIZE];
        uint32_t head;    // updated by client when message(s) consumed
        uint32_t tail;    // updated by server when message(s) posted
        int32_t __attribute__((aligned(4))) waiters;
        uint8_t overflow;
      } server_ring;
    };

    char pad[CHANNEL_SHMSIZE];
  };
} channel;

STATIC_ASSERT(sizeof(channel) == CHANNEL_SHMSIZE);
STATIC_ASSERT(sizeof(channel_entry) == CHANNEL_ENTRYSIZE);
#endif
