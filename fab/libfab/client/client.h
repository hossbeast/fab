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

struct fab_request;
struct memblk;

struct fab_client;
typedef struct fab_client fab_client;

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
xapi fab_client_create(fab_client ** const restrict client, const char * const restrict projdir, const char * const restrict ipcdir, const char * const restrict fabw_path)
  __attribute__((nonnull(1, 2, 3)));

/// fab_client_gethash
//
// SUMMARY
//  get the hash
//
char * fab_client_gethash(fab_client * const restrict client)
  __attribute__((nonnull));

/// fab_client_prepare
//
// SUMMARY
//  prepare to make requests
//
xapi fab_client_prepare(fab_client * const restrict client)
  __attribute__((nonnull));

/// fab_client_dispose
//
// SUMMARY
//  free a client
//
xapi fab_client_dispose(fab_client ** const restrict client)
  __attribute__((nonnull));

/// verify_fabd_state
//
// SUMMARY
//  verify that fabd is still running and has not reported any failure
//
xapi fab_client_verify(fab_client * const restrict client)
  __attribute__((nonnull));

/// fab_client_make_request
//
// SUMMARY
//
xapi fab_client_make_request(fab_client * const restrict client, struct memblk * const restrict mb, struct fab_request * const restrict request)
  __attribute__((nonnull));

/// fab_client_terminatep
//
// SUMMARY
//
xapi fab_client_terminatep(fab_client * const restrict client);

/// fab_client_launchp
//
// SUMMARY
//
xapi fab_client_launchp(fab_client * const restrict client)
  __attribute__((nonnull));
