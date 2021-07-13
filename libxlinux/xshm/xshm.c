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

#include "xshm/xshm.h"

int API uxshmget(key_t key, size_t size, int shmflg)
{
  int r;

  r = shmget(key, size, shmflg);
  RUNTIME_ASSERT(r == 0 || errno == ENOENT);

  return r;
}

int API xshmget(key_t key, size_t size, int shmflg)
{
  int r;

  r = shmget(key, size, shmflg);
  RUNTIME_ASSERT(r == 0);

  return r;
}

void API xshmctl(int shmid, int cmd, struct shmid_ds * const restrict buf)
{
  RUNTIME_ASSERT(shmctl(shmid, cmd, buf) == 0);
}

void * API xshmat(int shmid, const void * shmaddr, int shmflg)
{
  void *r;

  r = shmat(shmid, shmaddr, shmflg);
  RUNTIME_ASSERT(r != (void*)-1);

  return r;
}

void API xshmdt(const void * restrict shmaddr)
{
  if(!shmaddr)
    return;

  RUNTIME_ASSERT(shmdt(shmaddr) == 0);
}

void API ixshmdt(void ** shmaddr)
{
  xshmdt(*shmaddr);
  *shmaddr = 0;
}
