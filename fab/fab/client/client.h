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

#include <sys/types.h>

#include "xapi.h"

struct client;
typedef struct client client;

/// client_create
//
// SUMMARY
//  create a client
//
// PARAMETERS
//  client - (returns) client instance
//  ipcdir - 
//
xapi client_create(client ** const restrict client, char * const restrict ipcdir, const char hash[8])
  __attribute__((nonnull));

/// client_dispose
//
// SUMMARY
//  free a client
//
xapi client_dispose(client ** const restrict client)
  __attribute__((nonnull));

/// verify_fabd_state
//
// SUMMARY
//  verify that fabd is still running and has not reported any failure
//
xapi client_verify(client * const restrict client)
  __attribute__((nonnull));

/// client_send_request
//
// SUMMARY
//
xapi client_make_request(client * const restrict client, struct memblk * const restrict mb, struct request * const restrict req)
  __attribute__((nonnull));

/// client_terminatep
//
// SUMMARY
//
xapi client_terminatep(client * const restrict client);

/// client_launchp
//
// SUMMARY
//
xapi client_launchp(client * const restrict client)
  __attribute__((nonnull));
