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

#ifndef _FAB_CLIENT_H
#define _FAB_CLIENT_H

#include "xapi.h"
#include "types.h"

#include "macros.h"

#if DEVEL
extern const char *g_fab_client_fabd_path;
#endif

struct fabipc_channel;
struct fabipc_message;

typedef struct fab_client {
  int       lockfd;
  char *    projdir;    // canonicalized
  char *    ipcdir;
  char      hash[16 + 1];
  pid_t     fabd_pid;   // fabd process group id
  int32_t   futex;

  uint32_t local_tail;
  struct fabipc_channel *shm;
} fab_client;

/// fab_client_create
//
// SUMMARY
//  create a fab client
//
// PARAMETERS
//  client      - (returns) client instance
//  projdir     - project directory path (contains fabfile), e.g. "."
//  ipcdir      - base ipc dir
//
xapi fab_client_create(
    fab_client ** restrict client
  , const char * restrict projdir
  , const char * restrict ipcdir
)
  __attribute__((nonnull(1, 2, 3)));

/// fab_client_xfree
//
// SUMMARY
//  free a client
//
xapi fab_client_xfree(fab_client * restrict client);
xapi fab_client_ixfree(fab_client ** restrict client)
  __attribute__((nonnull));

xapi fab_client_prepare(fab_client * restrict client)
  __attribute__((nonnull));

xapi fab_client_kill(fab_client * restrict client)
  __attribute__((nonnull));

/*
 * launch fabd if its not already running
 */

xapi fab_client_solicit(fab_client * restrict client)
  __attribute__((nonnull));

xapi fab_client_attach(fab_client * restrict client, int channel_id)
  __attribute__((nonnull));


void fab_client_disconnect(fab_client * restrict client)
  __attribute__((nonnull));


struct fabipc_message * fab_client_produce(fab_client * restrict client, uint32_t * restrict tail)
  __attribute__((nonnull));

void fab_client_post(fab_client * restrict client, uint32_t tail)
  __attribute__((nonnull));

struct fabipc_message * fab_client_acquire(fab_client * restrict client)
  __attribute__((nonnull));

void fab_client_consume(fab_client * restrict client)
  __attribute__((nonnull));

#endif
