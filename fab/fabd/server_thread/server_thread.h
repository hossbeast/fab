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

#ifndef _SERVER_THREAD_H
#define _SERVER_THREAD_H

/*

*/

#include "xapi.h"

#if DEBUG || DEVEL
// dont acknowledge the client if true
int g_server_no_acknowledge;
#endif

#define restrict __restrict

extern bool server_thread_rebuild;

xapi server_thread_launch(void);

#if 0

struct fab_request;
struct fab_response;
struct memblk;

typedef struct fab_server
{
  char *    ipcdir;
  char      hash[8];
  pid_t     client_pid; // client process id

  void *    request_shm;
  void *    response_shm;

  int       client_cwd_dirfd;
} fab_server;

/// server_create
//
// SUMMARY
//  create a server
//
// PARAMETERS
//  server - (returns) server instance
//
xapi server_create(fab_server ** const restrict server)
  __attribute__((nonnull));

/// server_xfree
//
// SUMMARY
//
//
xapi server_xfree(fab_server * const restrict server);

xapi server_ixfree(fab_server ** const restrict server)
  __attribute__((nonnull));

/// server_ready
//
// SUMMARY
//  notify the client that we are ready to begin processing requests
//
xapi server_ready(fab_server * const restrict server)
  __attribute__((nonnull));

xapi server_receive(fab_server * const restrict server, int daemon, struct fab_request ** const restrict req)
  __attribute__((nonnull));

xapi server_respond(fab_server * const restrict server, struct memblk * const restrict mb, struct fab_response * const restrict response)
  __attribute__((nonnull));

xapi server_dispatch(fab_server * const restrict server, struct fab_request * const restrict req, struct memblk * const restrict mb, struct fab_response ** const restrict response)
  __attribute__((nonnull));

/// server_client_cwd_dirfd
//
// SUMMARY
//  get the file descriptor for the clients cwd
//
int server_client_cwd_dirfd(fab_server * const restrict server)
  __attribute__((nonnull));

/// server_project_dirfd
//
// SUMMARY
//  get the file descriptor for the project dir
//
int server_project_dirfd(fab_server * const restrict server)
  __attribute__((nonnull));
#endif

#undef restrict
#endif
