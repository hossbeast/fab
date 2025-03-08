/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "xmman.h"

#include <string.h>

void * xmmap(void * addr, size_t length, int prot, int flags, int fd, off_t offset)
{
  void *r;

  r = mmap(addr, length, prot, flags, fd, offset);
  RUNTIME_ASSERT(r != MAP_FAILED);

  return r;
}

void xmunmap(void * addr, size_t length)
{
  if(addr == MAP_FAILED)
    return;

  RUNTIME_ASSERT(munmap(addr, length) == 0);
}

void ixmunmap(void * addr, size_t length)
{
  void **addrp;

  addrp = addr;
  xmunmap(*addrp, length);
  *addrp = MAP_FAILED;
}
