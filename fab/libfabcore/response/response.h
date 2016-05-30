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

#ifndef _FABCORE_RESPONSE_H
#define _FABCORE_RESPONSE_H

#include <sys/types.h>

#include "xapi.h"

struct memblk;
struct narrator;

typedef struct response
{
  xapi    exit;     // 0 on success
  char *  errtab;   
  char *  errname;
  char *  trace;    // error trace
  size_t  tracesz;
} response;

#define restrict __restrict

/// response_create
//
// SUMMARY
//  create a response
//
// PARAMETERS
//  response - (returns) response instance
//
xapi response_create(response ** const restrict response)
  __attribute__((nonnull));

/// response_free
//
// SUMMARY
//  free a response with free semantics
//
void response_free(response * const restrict response);

/// response_ifree
//
// SUMMARY
//  free a response with ifree semantics
//
void response_ifree(response ** const restrict response)
  __attribute__((nonnull));

/// response_freeze
//
// SUMMARY
//  
//
void response_freeze(response * const restrict res, struct memblk * const restrict mb)
  __attribute__((nonnull));

/// response_thaw
//
//
//
void response_thaw(response * const restrict res, char * const restrict mb)
  __attribute__((nonnull));

/// response_say
//
//
//
xapi response_say(const response * const restrict res, struct narrator * const restrict N)
  __attribute__((nonnull));

/// response_error
//
// SUMMARY
//  build an error response
//
xapi response_error(response * const restrict resp, xapi exit, const char * const restrict trace, size_t tracesz)
  __attribute__((nonnull));

#undef restrict
#endif
