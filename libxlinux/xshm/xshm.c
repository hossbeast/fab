/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xshm.h"

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
  RUNTIME_ASSERT(r != -1);

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
