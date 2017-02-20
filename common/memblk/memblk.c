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

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xmman.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xuio.h"

struct memblk_internals
{
  struct memblk_policy
  {
    mempolicy;
    struct memblk * mb;
  } policy;

  int mapped;
  int prot;
  int flags;
};

#define MEMBLK_INTERNALS struct memblk_internals
#include "memblk.h"
#include "memblk.def.h"

// optimization parameters
#define MEMBLOCK_SMALL      0x4000  /* 16k : first THRESHOLD blocks */
#define MEMBLOCK_THRESHOLD  0x4
#define MEMBLOCK_LARGE      0x10000 /* 1m : additional blocks */

#define restrict __restrict

//
// static
//

static int policy_malloc(mempolicy * restrict plc, void * restrict p, size_t sz)
{
  xproxy(memblk_alloc, ((struct memblk_policy *)plc)->mb, p, sz);
}

static int policy_realloc(mempolicy * restrict plc, void * restrict p, size_t es, size_t ec, size_t oec)
{
  xproxy(memblk_realloc, ((struct memblk_policy *)plc)->mb, p, es, ec, oec);
}

//
// api
//

xapi memblk_mk(memblk ** mb)
{
  enter;

  fatal(xmalloc, mb, sizeof(**mb));

  finally : coda;
}

xapi memblk_mk_mapped(memblk ** mb, int prot, int flags)
{
  enter;

  fatal(xmalloc, mb, sizeof(**mb));

  (*mb)->mapped = 1;
  (*mb)->prot = prot;
  (*mb)->flags = flags;

  finally : coda;
}

xapi memblk_alloc(memblk * restrict mb, void * restrict p, size_t sz)
{
  enter;

  // save the active policy, but the memblk itself should use the default mm
  mempolicy * mm = mempolicy_pop(0);

  // request is too large to satisfy
  if(sz > MEMBLOCK_LARGE)
    tfail(perrtab_KERNEL, KERNEL_ENOMEM);

  /* current block is full */
  if(mb->blocksl == 0 || ((mb->blocks[mb->blocksl - 1].l + sz) > mb->blocks[mb->blocksl - 1].a))
  {
    /* reallocate the block container */
    if(mb->blocksl == mb->blocksa)
    {
      size_t ns = mb->blocksa ?: 3;
      ns = ns * 2 + ns / 2;
      fatal(xrealloc, &mb->blocks, sizeof(*mb->blocks), ns, mb->blocksa);
      mb->blocksa = ns;
    }

    if((mb->blocksl < MEMBLOCK_THRESHOLD) && (sz <= MEMBLOCK_SMALL))
      mb->blocks[mb->blocksl].a = MEMBLOCK_SMALL;
    else
      mb->blocks[mb->blocksl].a = MEMBLOCK_LARGE;

    // allocate the block
    if(mb->mapped)
    {
      fatal(xmmap, 0, mb->blocks[mb->blocksl].a, mb->prot, mb->flags, 0, 0, (void*)&mb->blocks[mb->blocksl].s);
    }
    else
    {
      fatal(xmalloc, &mb->blocks[mb->blocksl].s, sizeof(*mb->blocks[0].s) * mb->blocks[mb->blocksl].a);
    }

    // running offset
    if(mb->blocksl)
      mb->blocks[mb->blocksl].o = mb->blocks[mb->blocksl - 1].o + mb->blocks[mb->blocksl - 1].l;

    mb->blocksl++;
  }

  *(void**)p = mb->blocks[mb->blocksl - 1].s + mb->blocks[mb->blocksl - 1].l;
  mb->blocks[mb->blocksl - 1].l += sz;

finally:
  // restore the active mm if any
  // although this is/must be invoked with fatal because it is a xapi function
  // it cannot in fact fail ; the allocation is guaranteed due to the previous pop
  fatal(mempolicy_push, mm, 0);
coda;
}

xapi memblk_realloc(memblk * restrict mb, void * restrict p, size_t es, size_t ec, size_t oec)
{
  enter;

  void * old = *(void**)p;
  fatal(memblk_alloc, mb, p, es * ec);
  if(old)
    memcpy(*(void**)p, old, es * oec);

  finally : coda;
}

void memblk_free(memblk * mb)
{
  if(mb)
  {
    int x;
    for(x = 0; x < mb->blocksl; x++)
    {
      if(mb->mapped)
      {
        munmap(mb->blocks[x].s, mb->blocks[x].a);
      }
      else
      {
        wfree(mb->blocks[x].s);
      }
    }

    wfree(mb->blocks);
  }
  wfree(mb);
}

void memblk_xfree(memblk ** mb)
{
  memblk_free(*mb);
  *mb = 0;
}

mempolicy * memblk_getpolicy(memblk * mb)
{
  if(mb->policy.mb == 0)
  {
    mb->policy.malloc = policy_malloc;
    mb->policy.realloc = policy_realloc;
    mb->policy.mb = mb;
  }

  return &mb->policy;
}

xapi memblk_writeto(memblk * const restrict mb, const int fd)
{
  enter;

  struct iovec * iov = 0;
  fatal(xmalloc, &iov, sizeof(*iov) * mb->blocksl);

  int x;
  for(x = 0; x < mb->blocksl; x++)
  {
    iov[x].iov_base = mb->blocks[x].s;
    iov[x].iov_len = mb->blocks[x].l;
  }

  fatal(axwritev, fd, iov, mb->blocksl);

finally:
  wfree(iov);
coda;
}
