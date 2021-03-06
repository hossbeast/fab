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

#include "xwait/xwait.h"
#include "errtab/KERNEL.errtab.h"

xapi API xwait(int * status)
{
  enter;

  if(wait(status) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API xwaitpid(pid_t pid, int * status, int options)
{
  enter;

  if(waitpid(pid, status, options) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

xapi API uxwaitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
  enter;

  if(waitid(idtype, id, infop, options) == -1 && errno != ECHILD)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}
