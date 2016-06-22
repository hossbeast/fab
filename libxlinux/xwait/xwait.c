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

#include "internal.h"
#include "xwait/xwait.h"
#include "errtab/SYS.errtab.h"

API xapi xwait(int * status)
{
  enter;

  if(wait(status) == -1)
    fail(errno);

  finally : coda;
}

API xapi xwaitpid(pid_t pid, int * status, int options)
{
  enter;

  if(waitpid(pid, status, options) == -1)
    fail(errno);

  finally : coda;
}
