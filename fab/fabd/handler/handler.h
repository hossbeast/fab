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

struct request;
struct handler_context;
struct fabipc_message;

/* lock for processing any request */
extern int32_t handler_lock;

/* lock for running the build, e.g. build or autobuild commands */
extern int32_t handler_build_lock;

xapi handler_process_request(struct handler_context * restrict ctx, struct request * restrict request)
  __attribute__((nonnull));

void handler_request_completes(struct handler_context * restrict ctx, int code, const char * restrict text, uint16_t text_len)
  __attribute__((nonnull(1)));

void handler_request_complete(struct handler_context * restrict ctx, int code)
  __attribute__((nonnull));

/* send/receive messages over the channel */

struct fabipc_message * handler_produce(struct handler_context * restrict ctx, uint32_t * restrict tail)
  __attribute__((nonnull));

void handler_post(struct handler_context * restrict ctx, uint32_t tail)
  __attribute__((nonnull));

struct fabipc_message * handler_acquire(struct handler_context * restrict ctx)
  __attribute__((nonnull));

void handler_consume(struct handler_context * restrict ctx)
  __attribute__((nonnull));

#endif
