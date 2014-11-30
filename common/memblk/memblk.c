/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include "xapi/SYS.errtab.h"

#include "xlinux.h"
#include "xlinux/mempolicy.h"

struct memblk_internals
{
	struct memblk_policy
	{
		mempolicy;
		struct memblk * mb;
	} policy;
};

#define MEMBLK_INTERNALS struct memblk_internals;
#include "memblk.h"

#define ALLOC_BASE 256

#define restrict __restrict

//
// static
//

static xapi policy_malloc(mempolicy * restrict plc, void * restrict p, size_t sz)
{
	xproxy(memblk_alloc, ((struct memblk_policy *)plc)->mb, p, sz);
}

static xapi policy_realloc(mempolicy * restrict plc, void * restrict p, size_t es, size_t ec, size_t oec)
{
	xproxy(memblk_realloc, ((struct memblk_policy *)plc)->mb, p, es, ec, oec);
}

//
// api
//
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

xapi memblk_mk(memblk ** mb, size_t sz)
{
	fatal(xmalloc, mb, sizeof(**mb));
	fatal(xmalloc, &(*mb)->s, sz);
	(*mb)->a = sz;

	finally : coda;
}

xapi memblk_alloc(memblk * restrict mb, void * restrict p, size_t sz)
{
	if((mb->l + sz) > mb->a)
		tfail(perrtab_SYS, SYS_ENOMEM);

	*(void**)p = mb->s + mb->l;
	mb->l += sz;

	finally : coda;
}

xapi memblk_realloc(memblk * restrict mb, void * restrict p, size_t es, size_t ec, size_t oec)
{
	xproxy(memblk_alloc, mb, p, es * ec);
}

void memblk_free(memblk * mb)
{
	if(mb)
	{
		free(mb->s);
	}
	free(mb);
}

void memblk_xfree(memblk ** mb)
{
	memblk_free(*mb);
	*mb = 0;
}
