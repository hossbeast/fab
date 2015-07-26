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

#ifndef EXECUTOR_DEF_H
#define EXECUTOR_DEF_H

#include <stdint.h>
#include <sys/types.h>

struct pstring;         // pstring.h

typedef struct executor_context
{
  struct pstring *      tmp;      // reusable temp space

  struct
  {
    pid_t   pid;                  // pid of child
    int     num;                  // command number
    int     cmd_fd;               // file descriptors
    int     stdo_fd;
    int     stde_fd;
#if 0
    struct pstring ** prod_type;  // product types
    struct pstring ** prod_id;    // product ids
    size_t  prodl;                // number of products - occupied
    size_t  proda;                // number of products - allocated
#endif
  } *     cmds;
  size_t  cmdsl;
  size_t  cmdsa;
} executor_context;

#endif
