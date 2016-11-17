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

#ifndef _FAB_RESPONSE_H
#define _FAB_RESPONSE_H

#include <sys/types.h>

#include "xapi.h"

struct memblk;
struct narrator;

typedef struct fab_response
{
  xapi    exit;     // 0 on success
} fab_response;

#define restrict __restrict

/// fab_response_create
//
// SUMMARY
//  create a response
//
// PARAMETERS
//  response - (returns) response instance
//  exit     - exit value, zero is success
//
xapi fab_response_create(fab_response ** const restrict response, xapi exit)
  __attribute__((nonnull));

/// fab_response_free
//
// SUMMARY
//  free a response with free semantics
//
void fab_response_free(fab_response * const restrict response);

/// fab_response_ifree
//
// SUMMARY
//  free a response with iwfree semantics
//
void fab_response_ifree(fab_response ** const restrict response)
  __attribute__((nonnull));

/// fab_response_freeze
//
// SUMMARY
//  
//
void fab_response_freeze(fab_response * const restrict response, struct memblk * const restrict mb)
  __attribute__((nonnull));

/// fab_response_thaw
//
// SUMMARY
//
void fab_response_thaw(fab_response * const restrict response, char * const restrict mb)
  __attribute__((nonnull));

/// fab_response_say
//
// SUMMARY
//
xapi fab_response_say(const fab_response * const restrict response, struct narrator * const restrict N)
  __attribute__((nonnull));

#undef restrict
#endif
