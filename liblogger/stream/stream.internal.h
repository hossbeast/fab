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

#ifndef _LOGGER_STREAM_INTERNAL_H
#define _LOGGER_STREAM_INTERNAL_H

#include <inttypes.h>

#include "xapi.h"
#include "stream.h"
#include "locks.h"

struct pstring;
struct array;
struct list;
struct narrator;
struct narrator_fd;
struct narrator_rolling;
struct narrator_record;
struct filter;

/// stream
//
// SUMMARY
//  represents an outlet to which log messages are emitted
//
typedef struct stream
{
  // unique identifier
  uint8_t id;
  uint8_t type;

  // unique identifier for the stream
  char * name;
  size_t namel;

  // stack of logger expressions attached to the stream
  struct list * exprs;

  // effective attributes, given higher precedence than category and log site attrs
  uint32_t attrs;

  // effective filters, which determine which log messages are emitted to the stream
  struct list * filters;

  // log messages are written to this record narrator
  union {
    struct narrator * narrator;
    struct narrator_record * narrator_record;
  };

  // the underlying narrator
  struct futexlock lock;
  union {
    struct narrator * narrator_base;
    struct narrator_fd * narrator_base_fd;
  };
  union {
    struct narrator * narrator_owned;
    struct narrator_fd * narrator_owned_fd;
    struct narrator_rolling * narrator_owned_rolling;
  };
} stream;

/// g_streams
//
// SUMMARY
//  active streams
//
extern stream g_streams[LOGGER_MAX_STREAMS];
extern uint8_t g_streams_l;

/// stream_byid
//
// SUMMARY
//  lookup an active stream by id
//
// PARAMETERS
//  id - id returned by stream_register
//
static inline stream * stream_byid(uint8_t id)
{
  return &g_streams[id - 1];
}

/// stream_setup
//
// SUMMARY
//  module setup
//
xapi stream_setup(void);

/// stream_cleanup
//
// SUMMARY
//  module cleanup
//
xapi stream_cleanup(void);

/// streams_activate
//
// SUMMARY
//  activate the streams registered with logger_stream_register
//
xapi streams_activate(void);

/// streams_finalize
//
// SUMMARY
//  apply the filter expr for each stream
//
xapi streams_finalize(void);

/// streams_would
//
// SUMMARY
//
//
// PARAMETERS
//  ids        -
//  base_attrs -
//  vector     -
//
int streams_would(uint64_t ids, uint32_t attrs, uint64_t * restrict vector);

/// streams_write
//
// SUMAMRY
//
//
// PARAMETERS
//  ids       -
//  attrs     -
//  b         -
//  l         -
//  time_msec -
//  vector    -
//
xapi streams_write(const uint64_t ids, const uint32_t attrs, const char * restrict b, size_t l, const long time_msec, uint64_t vector)
  __attribute__((nonnull));

/// stream_write
//
// SUMMARY
//  write a log message to a stream
//
// PARAMETERS
//  streamp    - stream to write to
//  ids        - bitmask of ids for the message
//  attrs      - category attrs + log site attrs + stream attrs
//  b          -
//  l          -
//  time_msec  -
//
xapi stream_write(stream *  restrict streamp, const uint64_t ids, uint32_t attrs, const char * const restrict b, size_t l, const long time_msec)
  __attribute__((nonnull));

/// stream_say
//
// SUMMARY
//  write a description of the stream to a narrator
//
xapi stream_say(stream * restrict streamp, struct narrator * restrict N)
  __attribute__((nonnull));

/// stream_expr_push
//
// SUMMARY
//  append a filter to the filters for the specified stream
//
// PARAMETERS
//  streamp - stream
//  filterp - filter
//
// REMARKS
//  takes ownership of the filter instance
//
xapi stream_expr_push(stream * restrict streamp, const char * restrict expr)
  __attribute__((nonnull));

xapi stream_expr_pop(stream * restrict streamp)
  __attribute__((nonnull));

xapi stream_expr_unshift(stream * restrict streamp, const char * restrict expr)
  __attribute__((nonnull));

xapi stream_expr_shift(stream * restrict streamp)
  __attribute__((nonnull));

xapi stream_expr_reset(stream * restrict streamp)
  __attribute__((nonnull));

#endif
