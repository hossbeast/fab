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

#ifndef FABD_HANDLER_H
#define FABD_HANDLER_H

/*

*/

#include "types.h"
#include "xapi.h"

struct fab_request;
struct fab_response;
struct memblk;
struct buildplan;

typedef struct handler_context {
  struct buildplan * bp;
} handler_context;

xapi handler_context_create(handler_context ** restrict ctx)
  __attribute__((nonnull));

xapi handler_context_xfree(handler_context * restrict ctx);

xapi handler_context_ixfree(handler_context ** restrict ctx)
  __attribute__((nonnull));

/// handler_build
//
// SUMMARY
//  build the current targets
//
xapi handler_build(handler_context * restrict ctx)
  __attribute__((nonnull));

/// handler_dispatch
//
// SUMMARY
//  
//
xapi handler_dispatch(
    handler_context * restrict ctx
  , struct fab_request * restrict request
  , struct memblk * restrict mb
  , struct fab_response ** restrict response
)
  __attribute__((nonnull));

#endif
