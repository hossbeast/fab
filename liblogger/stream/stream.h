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
#include <sys/types.h>

#include "xapi.h"

struct narrator;

/*
 * options and modifiers that can be applied to a log message
 */
#define LOGGER_STREAM_TABLE(x)                                                          \
  LOGGER_STREAM_DEF(FD            , 0x01  , x)  /* write to a file descriptor */        \
  LOGGER_STREAM_DEF(RING          , 0x02  , x)  /* write to a ring buffer */            \
  LOGGER_STREAM_DEF(ROLLING       , 0x03  , x)  /* write to a rolling set of files */   \
  LOGGER_STREAM_DEF(NARRATOR      , 0x04  , x)  /* write to a narrator */

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
  char ** exprs;            // e.g. (char *[]) { "+INFO", "+WARN", 0 }
  
  union {
    int fd;                       // LOGGER_STREAM_FD
    struct narrator * narrator;   // LOGGER_STREAM_NARRATOR
    struct {                      // LOGGER_STREAM_ROLLING
      char *    path_base;
      mode_t    file_mode;
      uint32_t  threshold;
      uint16_t  max_files;
    };
  };

  // (returns) unique id
  int id;
} logger_stream;

/// logger_stream_register
//
// SUMMARY
//  provide a list of streams for writing log messages to
//  
// PARAMETERS
//  streams    - sentinel-terminated list of stream definitions
//
xapi logger_stream_register(const logger_stream * restrict streams)
  __attribute__((nonnull));

/// logger_streams_report
//
// SUMMARY
//  log a summary of configured streams to LOGGER
//
xapi logger_streams_report(void);

/// logger_stream_would
//
// SUMMARY
//  returns true if a log with the specified ids would write to the specified stream
//
int logger_stream_would(int stream_id, uint64_t ids);

#endif
