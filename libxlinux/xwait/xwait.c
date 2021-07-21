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

#include <errno.h>
#include <sys/wait.h>

#include "types.h"
#include "macros.h"

#include "xwait/xwait.h"

void API xwait(int * restrict status)
{
  RUNTIME_ASSERT(wait(status) == 0);
}

void API xwaitpid(pid_t pid, int * restrict status, int options)
{
  int r;

  r = waitpid(pid, status, options);
  RUNTIME_ASSERT(r == pid);
}

void API uxwaitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
  int r;

  r = waitid(idtype, id, infop, options);
  RUNTIME_ASSERT(r == 0 || errno == ECHILD);
}
