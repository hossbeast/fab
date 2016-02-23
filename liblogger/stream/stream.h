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
#define LOGGER_STREAM_TABLE(x, y)                                                     \
  LOGGER_STREAM_DEF(FD            , 0x01, x , y)  /* write to a file descriptor */    \
  LOGGER_STREAM_DEF(RING          , 0x02, x , y)  /* write to a ring buffer */        \
  LOGGER_STREAM_DEF(FILE          , 0x03, x , y)  /* write to a rolling logfile  */   \

enum {
#define LOGGER_STREAM_DEF(a, b, x, y) L_ ## a = UINT8_C(b),
LOGGER_STREAM_TABLE(0, 0)
#undef LOGGER_STREAM_DEF
};


typedef struct logger_stream
{
  char *      id;

  uint32_t    attr;

  uint8_t     type;
  
  union {
    int     fd;   // LOGGER_STREAM_FD
  };
} logger_stream;

/// logger_stream_register
//
// SUMMARY
//  
// PARAMETERS
//  streams - sentinel-terminated list of stream definitions
//
xapi logger_stream_register(const logger_stream * const restrict streams)
  __attribute__((nonnull));

#endif