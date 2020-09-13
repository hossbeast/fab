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

#ifndef _FAB_IPC_H
#define _FAB_IPC_H

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

#include "xapi.h"
#include "types.h"
#include "macros.h"
#include "barriers.h"
#include "atomics.h"

struct attrs32;

/// FABIPC_DATA
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*   files which need not be executable
//
// DESCRIPTION
//  ugo+rw   world read/write
//
// RATIONALE
//
#define FABIPC_MODE_DATA 0666

/// FABIPC_CODE
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*    files which need to be executable
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//
#define FABIPC_MODE_CODE 0777

/// FABIPC_DIR
//
// LOCATION
//  {FABIPCDIR,FABTMPDIR}/*    directories
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//
#define FABIPC_MODE_DIR  0777

/// ipc_lock_obtain
//
// SUMMARY
//  attempt to obtain an exclusive lock
//
// PARAMETERS
//  pid  - (returns) zero if the lock was obtained, otherwise pid of lock holder
//  fmt  - printf-style format string for the path to the lockfile
//
xapi fabipc_lockfile_obtain(pid_t * restrict pid, int * restrict fd, char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2, 3)))
  __attribute__((format(printf, 3, 4)));

/// ipc_lock_update
//
// SUMMARY
//  write the pid of the current process to a lockfile
//
// PARAMETERS
//  fmt   - printf-style format string for the path to the lockfile
//
xapi fabipc_lockfile_update(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

/// ipc_lock_release
//
// SUMMARY
//  unlink a lockfile
//
// PARAMETERS
//  fmt   - printf-style format string for the path to the lockfile
//
xapi fabipc_lockfile_release(char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

#define FABIPC_SHMSIZE          (16 * 1024 * 1024)
#define FABIPC_PAGESIZE         8096
#define FABIPC_CLIENT_RINGSIZE  1024
#define FABIPC_SERVER_RINGSIZE  1024

#define FABIPC_MSG_TYPE_OPT 0xff

#define FABIPC_MSG_TYPE_TABLE                                                                                  \
  DEF(FABIPC_MSG_REQUEST   , "request"  , FABIPC_MSG_TYPE_OPT, 0x01)  /* request, a bacon-formatted string */  \
  DEF(FABIPC_MSG_RESPONSE  , "response" , FABIPC_MSG_TYPE_OPT, 0x02)  /* response, a bacon-formatted string */ \
  DEF(FABIPC_MSG_RESULT    , "result"   , FABIPC_MSG_TYPE_OPT, 0x03)  /* end of request processing */          \
  DEF(FABIPC_MSG_STDOUT    , "stdout"   , FABIPC_MSG_TYPE_OPT, 0x04)  /* stdout text */                        \
  DEF(FABIPC_MSG_STDERR    , "stderr"   , FABIPC_MSG_TYPE_OPT, 0x05)  /* stderr text */                        \
  DEF(FABIPC_MSG_EVENTSUB  , "eventsub" , FABIPC_MSG_TYPE_OPT, 0x06)  /* subscribe to events */                \
  DEF(FABIPC_MSG_EVENTS    , "events"   , FABIPC_MSG_TYPE_OPT, 0x07)  /* events */                             \

typedef enum fabipc_msg_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
FABIPC_MSG_TYPE_TABLE
} fabipc_msg_type;

extern struct attrs32 * fabipc_msg_type_attrs;

#define FABIPC_EVENT_TYPE_OPT                                                           0xff
#define FABIPC_EVENT_TYPE_TABLE                                                                                                 \
  DEF(FABIPC_EVENT_NODE_STALE          , "node-stale"          , FABIPC_EVENT_TYPE_OPT, 0x01)  /* node marked as stale */       \
  DEF(FABIPC_EVENT_NODE_FRESH          , "node-fresh"          , FABIPC_EVENT_TYPE_OPT, 0x02)  /* node marked as fresh */       \
  DEF(FABIPC_EVENT_GLOBAL_INVALIDATION , "global-invalidation" , FABIPC_EVENT_TYPE_OPT, 0x03)  /* global node invalidation */   \
  DEF(FABIPC_EVENT_FORMULA_EXEC_FORKED , "formula-exec-fork"   , FABIPC_EVENT_TYPE_OPT, 0x04)  /* formula execution - forked */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_WAITED , "formula-exec-wait"   , FABIPC_EVENT_TYPE_OPT, 0x05)  /* formula execution - waited */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDOUT , "formula-exec-stdout" , FABIPC_EVENT_TYPE_OPT, 0x06)  /* formula execution - stdout */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDERR , "formula-exec-stderr" , FABIPC_EVENT_TYPE_OPT, 0x07)  /* formula execution - stderr */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_AUXOUT , "formula-exec-auxout" , FABIPC_EVENT_TYPE_OPT, 0x08)  /* formula execution - auxout */ \
  DEF(FABIPC_EVENT_GOALS               , "goals"               , FABIPC_EVENT_TYPE_OPT, 0x09)  /* goals changed */              \
  DEF(FABIPC_EVENT_BUILD_START         , "build-start"         , FABIPC_EVENT_TYPE_OPT, 0x0a)  /* build started */              \
  DEF(FABIPC_EVENT_BUILD_END           , "build-end"           , FABIPC_EVENT_TYPE_OPT, 0x0b)  /* build ended */                \

typedef enum fabipc_event_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
FABIPC_EVENT_TYPE_TABLE
} fabipc_event_type;

extern struct attrs32 * fabipc_event_type_attrs;

#define FABIPC_MESSAGE_FIELDS                                 \
  struct {                                                    \
    fabipc_msg_type type;                                     \
    uint32_t id;                                              \
    union {                                                   \
      uint32_t attrs;                                         \
      fabipc_event_type evtype;                               \
    };                                                        \
    int code;                                                 \
    uint32_t size;              /* size of the text field */  \
  }

struct _fabipc_message_fields {
  FABIPC_MESSAGE_FIELDS;
  char __attribute__((aligned(8))) text[];
};

typedef struct fabipc_message {
  FABIPC_MESSAGE_FIELDS;
  char __attribute__((aligned(8))) text[FABIPC_PAGESIZE - offsetof(struct _fabipc_message_fields, text)];
} fabipc_message;

typedef struct fabipc_page {
  union {
    fabipc_message msg;
    char pad[FABIPC_PAGESIZE];
  };
} fabipc_page;
STATIC_ASSERT(sizeof(fabipc_page) == FABIPC_PAGESIZE);

/* created and owned by fabd (server) */
typedef struct fabipc_channel
{
  union {
    struct {
      int shmid;

      pid_t client_pid;  // client
      pid_t client_tid;
      uint16_t client_pulse;
      bool client_exit;

      pid_t server_pid; // server
      pid_t server_tid;
      uint16_t server_pulse;
      bool server_exit;

      // messages client -> server
      struct __attribute__((aligned(8))) {
        struct fabipc_page pages[FABIPC_CLIENT_RINGSIZE];
        uint32_t head;          // updated by server when message(s) consumed
        uint32_t tail;          // updated by client when message(s) posted
        int32_t __attribute__((aligned(4))) waiters;
        uint8_t overflow;
      } client_ring;

      // messages server -> client
      struct __attribute__((aligned(8))) {
        struct fabipc_page pages[FABIPC_SERVER_RINGSIZE];
        uint32_t head;          // updated by client when message(s) consumed
        uint32_t tail;          // updated by server when message(s) posted
        int32_t __attribute__((aligned(4))) waiters;
        uint8_t overflow;
      } server_ring;
    };

    char pad[FABIPC_SHMSIZE];
  };
} fabipc_channel;

STATIC_ASSERT(sizeof(fabipc_channel) == FABIPC_SHMSIZE);

static inline fabipc_message * fabipc_produce(fabipc_page *pages, uint32_t *headp, uint32_t *tailp, uint32_t *nextp, uint8_t *overflow, uint32_t mask, size_t size)
{
  uint32_t head;
  uint32_t tail;
  uint32_t index;
  uint32_t next;
  struct fabipc_page *page;

  RUNTIME_ASSERT(size <= sizeof_member(fabipc_message, text));

  head = *headp;
  tail = *tailp;
  smp_rmb();
  next = tail + 1;
  if(next == head) {
    *overflow = 1;
    return 0;
  }

  *nextp = next;
  index = tail & mask;
  page = &pages[index];
// id is user-supplied
//  page->msg.id = (tail >> 1) + 1;
  return &page->msg;
}

static inline void fabipc_post(uint32_t * restrict tail, uint32_t next, int32_t * restrict waiters)
{
  int32_t one = 1;
  int32_t zero = 0;

  *tail = next;
  if(atomic_cas_i32(waiters, &one, &zero)) {
    smp_wmb();
    syscall(SYS_futex, FUTEX_WAKE, 1, 0, 0, 0);
  }
}

static inline fabipc_message * fabipc_acquire(fabipc_page *pages, uint32_t *headp, uint32_t *tailp, uint32_t mask)
{
  uint32_t head;
  uint32_t tail;
  uint32_t index;
  fabipc_page *page;

  head = *headp;
  tail = *tailp;
  smp_rmb();

  if(head == tail) {
    return 0;
  }

  index = head & mask;
  page = &pages[index];
  return &page->msg;
}

static inline void fabipc_consume(uint32_t *head)
{
  (*head)++;
}

#endif
