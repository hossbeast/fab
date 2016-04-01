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

struct pstring;
struct array;
struct list;
struct narrator;

/// stream
//
// SUMMARY
//  represents an outlet to which log messages are emitted
//
typedef struct stream
{
  // unique identifier
  int id;

  // unique identifier for the stream
  char * name;
  size_t namel;

  // attributes for logs emitted to the stream, at higher precedence than both
  // category definitions and attributes at the log site
  uint32_t attr;

  // the filters determine which log messages are emitted to the stream
  struct list * filters;

  struct pstring * buffer;

  // emitted log messages are written to the narrator
  struct narrator * narrator;
} stream;

/// g_streams
//
// SUMMARY
//  
//
extern struct array * g_streams;

#define restrict __restrict

xapi stream_setup();
void stream_teardown();

int stream_would(const stream * const restrict streamp, const uint64_t ids)
  __attribute__((nonnull));

int streams_would(const uint64_t ids);

/// streams_write
//
// SUMAMRY
//
//
// PARAMETERS
//  ids       - 
//  attrs     - 
//  message   - 
//  time_msec - 
//
xapi streams_write(const uint64_t ids, const uint32_t attrs, const struct pstring * restrict message, const long time_msec)
  __attribute__((nonnull));

/// stream_byid
//
// SUMMARY
//  lookup an active stream by id
//
// PARAMETERS
//  id       - id
//  category - (returns) category definition
//
xapi stream_byid(int id, stream ** const restrict streamp)
  __attribute__((nonnull));

#undef restrict
#endif
