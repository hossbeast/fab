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

struct pstring;     // pstring.h

#define restrict __restrict

typedef struct executor_context
{
  struct pstring *      tmp;      // reusable temp space

  int			num;			// stage number
  struct
  {
    pid_t		pid;
    int			num;		
    int			cmd_fd;
    int			stdo_fd;
    int			stde_fd;
    struct pstring ** prod_type;  // product types
    struct pstring ** prod_id;    // product ids
    size_t	prodl;                // number of products - occupied
    size_t  proda;                // number of products - allocated
  } * 		cmds;
  size_t	cmdsl;
  size_t	cmdsa;
} executor_context;

/// executor_execute_stage
//
// SUMMARY
//  execute a stage in a buildplan
//
// PARAMETERS
//  ctx       - executor context instance
//  stagesl   - total number of stages in plan
//  commandsl - total number of commands in plan
//  success   - (returns) whether the stage executed to completion
//
int executor_execute_stage(executor_context * const restrict ctx, int stagesl, int commandsl, int stagex, int * const restrict success)
  __attribute__((nonnull));

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
