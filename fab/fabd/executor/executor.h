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

#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include <sys/types.h>
#include <stdint.h>

#define restrict __restrict

// opaque context
struct executor_context;
typedef struct executor_context executor_context;

// logging profile
typedef struct executor_logging
{
  uint64_t cmd_cat;         // category for logging commands as they are executed
  uint64_t exec_cat;        // category for EXEC logging
  void (*log_exec)(         // callback for EXEC logging
      struct executor_logging * const restrict logging
    , int num         // command number
    , int bad         // whether executor thinks the command succeeded
    , int r           // return value from wait
  );
} executor_logging;

/// executor_execute
//
// SUMMARY
//  execute a set of commands in parallel
//
// PARAMETERS
//  ctx        - executor context instance
//  subdir     - subdirectory of ipcdir containing the commands
//  cmdsl      - number of commands
//  [logging]  - logging profile
//  [success]  - (returns) whether all commands completed successfully
//
// REMARKS
//  the commands have previously been written to a numbered set of files
//  in ipcdir/{subdir}
//
int executor_execute(
  /*  1 */    executor_context * const restrict ctx
  /*  2 */  , const char * const restrict subdir
  /*  3 */  , const int cmdsl
  /*  4 */  , const executor_logging * const restrict logging
  /*  5 */  , int * const restrict success
)
  __attribute__((nonnull(1, 2)));

/// executor_context_mk
//
// SUMMARY
//
// PARAMETERS
//  ctx - (returns) executor context instance
//
int executor_context_mk(executor_context ** const restrict ctx)
  __attribute__((nonnull));

/// executor_context_recycle
//
// SUMMARY
//  reset an executor_context instance
//
// RETURNS
//  xapi semantics
//
int executor_context_recycle(executor_context * const restrict ctx)
  __attribute__((nonnull));

/// executor_context_dispose
//
// SUMMARY
//  release resources in an idempotent way
//
// RETURNS
//  xapi semantics
//
int executor_context_dispose(executor_context * ctx);

#endif
