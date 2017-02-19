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

#include "internal.h"
#include "xshm/xshm.h"
#include "errtab/KERNEL.errtab.h"

API xapi uxshmget(key_t key, size_t size, int shmflg, int * const restrict shmid)
{
  enter;

  if((*shmid = shmget(key, size, shmflg)) == -1 && errno != ENOENT)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("key", "0x%08lx", (long)key);
  xapi_infof("size", "%zu", size);
coda;
}

API xapi xshmget(key_t key, size_t size, int shmflg, int * const restrict shmid)
{
  enter;

  if((*shmid = shmget(key, size, shmflg)) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("key", "0x%08lx", (long)key);
  xapi_infof("size", "%zu", size);
coda;
}

API xapi xshmctl(int shmid, int cmd, struct shmid_ds * const restrict buf)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, shmctl, shmid, cmd, buf);

  finally : coda;
}

API xapi xshmat(int shmid, const void * shmaddr, int shmflg, void ** const restrict addr)
{
  enter;

  if((*addr = shmat(shmid, shmaddr, shmflg)) == (void*)-1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xshmdt(const void * shmaddr)
{
  enter;

  if(shmaddr)
    tfatalize(perrtab_KERNEL, errno, shmdt, shmaddr);

  finally : coda;
}

API xapi ixshmdt(void ** shmaddr)
{
  enter;

  fatal(xshmdt, *shmaddr);
  *shmaddr = 0;

  finally : coda;
}
