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

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "internal.h"
#include "xstdlib/xstdlib.h"
#include "KERNEL.errtab.h"
#include "XLINUX.errtab.h"
#include "mempolicy/mempolicy.internal.h"

#include "fmt.h"

//
// static
//

struct xqsort_context
{
  xapi (*xcompar)(const void *, const void *, void *, int *);
  void * arg;
  int hasfailed;
};

static int xqsort_compar(const void * A, const void * B, void * arg)
{
  enter_nochecks;

  struct xqsort_context * ctx = arg;

  int r = 0;
  if(!ctx->hasfailed)
    fatal(ctx->xcompar, A, B, ctx->arg, &r);

  int R = 0;
  finally : conclude(&R);

  if(R)
  {
    ctx->hasfailed = 1;
    r = 0;
  }

  return r;
};

//
// api
//

API xapi xmalloc(void* target, size_t size)
{
  enter;

  //void * mem = 0;

  if(policy)
  {
    fatal(policy->malloc, policy, target, size);
  }
  else
  {
    posix_memalign(target, sizeof(void*) * 2, size);
    memset(*(void**)target, 0, size);
  }

finally:
  xapi_infof("size", "%zu", size);
coda;
}

API xapi xrealloc(void* target, size_t es, size_t ec, size_t oec)
{
  enter;

  void** t = ((void**)target);

  if(policy)
  {
    fatal(policy->realloc, policy, target, es, ec, oec);
  }
  else if(!es || !ec)
  {
    *t = 0;
  }
  else
  {
    void * mem = 0;
    if((mem = realloc(*t, es * ec)) == 0)
      tfail(perrtab_KERNEL, errno);
    *t = mem;

    if(*t)
    {
      if(((ssize_t)ec - (ssize_t)oec) > 0)
        memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);
    }
    else
    {
      tfail(perrtab_KERNEL, errno);
    }
  }

finally :
  xapi_infof("size", "%zu", es * ec);
coda;
}

void API iwfree(void* target)
{
  void** t = (void**)target;

  if(policy)
  {
    if(policy->ifree)
      policy->ifree(policy, target);
  }
  else
  {
    wfree(*t);
  }

  *t = 0;
}

void API wfree(void* target)
{
  if(policy)
  {
    if(policy->free)
      policy->free(policy, target);
  }
  else
  {
    free(target);
  }
}

API xapi xqsort_r(void * base, size_t nmemb, size_t size, xapi (*xcompar)(const void *, const void *, void *, int * r), void * arg)
{
  enter;

  struct xqsort_context ctx = { 0 };
  ctx.xcompar = xcompar;
  ctx.arg = arg;

  qsort_r(base, nmemb, size, xqsort_compar, &ctx);

  if(ctx.hasfailed)
    fail(0);

  finally : coda;
}

API xapi xreadlinks(const char * pathname, char * buf, size_t bufsiz, ssize_t * r)
{
  enter;

  ssize_t lr;
  if(!r)
    r = &lr;

  if((*r = readlink(pathname, buf, bufsiz)) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("pathname", pathname);
coda;
}

API xapi xreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ssize_t * r, ...)
{
  enter;

  va_list va;
  va_start(va, r);

  fatal(xreadlinkvf, pathname_fmt, buf, bufsiz, r, va);

finally:
  va_end(va);
coda;
}

API xapi xreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, ssize_t * r, va_list va)
{
  enter;

  char pathname[512];

  fatal(fmt_apply, pathname, sizeof(pathname), pathname_fmt, va);
  fatal(xreadlinks, pathname, buf, bufsiz, r);

  finally : coda;
}

API xapi xrealpaths(char ** restrict r, char * restrict resolved_path, const char * restrict path)
{
  enter;

  if(r && (*r= realpath(path, resolved_path)) == 0)
    tfail(perrtab_KERNEL, errno);
  else if(!r && realpath(path, resolved_path) == 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xrealpathf(char ** restrict r, char * restrict resolved_path, const char * restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xrealpathvf, r, resolved_path, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xrealpathvf(char ** restrict r, char * restrict resolved_path, const char * restrict path_fmt, va_list va)
{
  enter;

  char pathname[512];

  fatal(fmt_apply, pathname, sizeof(pathname), path_fmt, va);
  fatal(xrealpaths, r, resolved_path, pathname);

finally:
  va_end(va);
coda;
}

API xapi xsystem(const char * restrict command, int * restrict status)
{
  enter;

  if(status && ((*status) = system(command)) == -1)
    fail(XLINUX_SYSFAIL);
  else if(system(command) == -1)
    fail(XLINUX_SYSFAIL);

finally:
  xapi_infos("command", command);
coda;
}
