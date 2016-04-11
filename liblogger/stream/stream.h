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

#ifndef _LOGGER_STREAM_H
#define _LOGGER_STREAM_H

#include <stdint.h>

#include "xapi.h"

/*
 * options and modifiers that can be applied to a log message
 */
#define LOGGER_STREAM_TABLE(x)                                                    \
  LOGGER_STREAM_DEF(FD            , 0x01, x)  /* write to a file descriptor */    \
  LOGGER_STREAM_DEF(RING          , 0x02, x)  /* write to a ring buffer */        \
  LOGGER_STREAM_DEF(FILE          , 0x03, x)  /* write to a rolling logfile  */   \

enum {
#define LOGGER_STREAM_DEF(a, b, x) LOGGER_STREAM_ ## a = UINT8_C(b),
LOGGER_STREAM_TABLE(0)
#undef LOGGER_STREAM_DEF
};

#define LOGGER_STREAM_DEF(a, b, x) (x) == b ? #a :
#define LOGGER_STREAM_STR(x) LOGGER_STREAM_TABLE(x) "UNKNOWN"

/// logger_stream
//
// user definition
//
typedef struct logger_stream
{
  // (optional) unique name for the stream
  char * name;              // e.g. console
  size_t namel;

  uint8_t type;             // e.g. LOGGER_STREAM_FD

  // options and modifiers
  uint32_t attr;            // e.g. L_RED | L_TRACE

  char * expr;              // e.g. +INFO
  
  union {
    int fd;                 // LOGGER_STREAM_FD
  };

  // (returns) unique id
  int id;
} logger_stream;

/// logger_stream_register
//
// SUMMARY
//  provide a list of streams for writingn log messages to
//  
// PARAMETERS
//  streams    - sentinel-terminated list of stream definitions
//
xapi logger_stream_register(const logger_stream * restrict streams)
  __attribute__((nonnull));

#endif
