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

#ifndef _SERVER_H
#define _SERVER_H

#include <sys/types.h>

#include "xapi.h"

struct request;
struct response;
struct memblk;

typedef struct server
{
  char *    ipcdir;
  char      hash[8];
  pid_t     pid;        // client process id

  int       shmid;
  void *    shmaddr;    // request shm, after a request has been received, and before a response has been sent
} server;

#define restrict __restrict

/// server_create
//
// SUMMARY
//  create a server
//
// PARAMETERS
//  server - (returns) server instance
//  ipcdir - 
//
xapi server_create(server ** const restrict server, char * const restrict ipcdir, const char hash[8])
  __attribute__((nonnull));

xapi server_dispose(server ** const restrict server)
  __attribute__((nonnull));

xapi server_ready(server * const restrict server)
  __attribute__((nonnull));

xapi server_validate(server * const restrict server, pid_t pid)
  __attribute__((nonnull));

xapi server_redirect(server * const restrict server)
  __attribute__((nonnull));

xapi server_receive(server * const restrict server, struct request ** const restrict req)
  __attribute__((nonnull));

xapi server_respond(server * const restrict server, struct memblk * const restrict mb, struct response * const restrict resp)
  __attribute__((nonnull));

xapi server_release(server * const restrict server)
  __attribute__((nonnull));

xapi server_dispatch(server * const restrict server, struct request * const restrict req, struct memblk * const restrict mb, struct response ** const restrict resp)
  __attribute__((nonnull));

#undef restrict
#endif
