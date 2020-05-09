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

typedef struct fab_client fab_client;

typedef struct fab_message {
  uint32_t len;
  char text[]; // terminates with two null bytes
} fab_message;

/// fab_client_create
//
// SUMMARY
//  create a fab client
//
// PARAMETERS
//  client      - (returns) client instance
//  projdir     - project directory path (contains fabfile), e.g. "."
//  ipcdir      - base ipc dir
//  [fabw_path] - path to fabw binary
//
xapi fab_client_create(fab_client ** restrict client, const char * restrict projdir, const char * restrict ipcdir, const char * restrict fabw_path)
  __attribute__((nonnull(1, 2, 3)));

/// fab_client_gethash
//
// SUMMARY
//  get the hash
//
char * fab_client_gethash(fab_client * restrict client)
  __attribute__((nonnull));

/// fab_client_prepare
//
// SUMMARY
//  prepare to make requests
//
xapi fab_client_prepare(fab_client * restrict client)
  __attribute__((nonnull));

/// fab_client_xfree
//
// SUMMARY
//  free a client
//
xapi fab_client_xfree(fab_client * restrict client);

xapi fab_client_ixfree(fab_client ** restrict client)
  __attribute__((nonnull));

/// verify_fabd_state
//
// SUMMARY
//  verify that fabd is still running and has not reported any failure
//
xapi fab_client_verify(fab_client * restrict client)
  __attribute__((nonnull));

/// fab_client_prepare_request_shm
//
// SUMMARY
//
xapi fab_client_prepare_request_shm(fab_client * restrict client, void ** restrict request_shm)
  __attribute__((nonnull));

/// fab_client_make_request
//
// SUMMARY
//
xapi fab_client_make_request(
    fab_client * restrict client
  , void * restrict request_shm
  , void ** response_shm
)
  __attribute__((nonnull));

/// fab_client_release_response
//
// SUMMARY
//
xapi fab_client_release_response(fab_client * restrict client, void * restrict response_shm)
  __attribute__((nonnull));

/// fab_client_terminatep
//
// SUMMARY
//
xapi fab_client_terminatep(fab_client * restrict client);

/// fab_client_launchp
//
// SUMMARY
//
xapi fab_client_launchp(fab_client * restrict client)
  __attribute__((nonnull));

xapi fab_client_unlock(fab_client * restrict client)
  __attribute__((nonnull));

#endif
