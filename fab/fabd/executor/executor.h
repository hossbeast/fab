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

#ifndef EXECUTOR_CONTEXT
#define EXECUTOR_CONTEXT struct executor_context
#endif

#define restrict __restrict

// opaque context
struct executor_context;

// logging profile
typedef struct executor_logging
{
  uint64_t cmd_cat;         // category for CMD logging
  uint64_t exec_cat;        // category for EXEC logging

  /// log_exec
  //
  // SUMMARY
  //  callback for EXEC logging, invoked once per command upon completion if log_would
  //
  void (*log_exec)(
    /*  1 */    struct executor_context * const restrict ctx
    /*  2 */  , const struct executor_logging * const restrict logging
    /*  3 */  , const char * const restrict subdir
    /*  4 */  , int num         // command index
    /*  5 */  , int bad         // whether executor thinks the command succeeded (includes writing to stderr)
    /*  6 */  , int r           // return value from wait
  )
  __attribute__((nonnull));
} executor_logging;

/// executor_execute
//
// SUMMARY
//  execute a set of commands in parallel
//
// PARAMETERS
//  ctx        - executor context instance
//  [logging]  - logging profile
//  subdir     - subdirectory of ipcdir containing the commands
//  cmdsl      - number of commands
//  [success]  - (returns) whether all commands completed successfully
//
// REMARKS
//  the commands have previously been written to a numbered set of files
//  in ipcdir/{subdir}
//
int executor_execute(
  /*  1 */    struct executor_context * const restrict ctx
  /*  2 */  , const executor_logging * const restrict logging
  /*  3 */  , const char * const restrict subdir
  /*  4 */  , const int cmdsl
  /*  5 */  , int * const restrict success
)
  __attribute__((nonnull(1, 3)));

/// executor_context_mk
//
// SUMMARY
//
// PARAMETERS
//  ctx - (returns) executor context instance
//
int executor_context_mk(struct executor_context ** const restrict ctx)
  __attribute__((nonnull));

/// executor_context_recycle
//
// SUMMARY
//  reset an executor_context instance
//
// RETURNS
//  xapi semantics
//
int executor_context_recycle(struct executor_context * const restrict ctx)
  __attribute__((nonnull));

/// executor_context_dispose
//
// SUMMARY
//  release resources in an idempotent way
//
// RETURNS
//  xapi semantics
//
int executor_context_dispose(struct executor_context * restrict ctx);

#endif
