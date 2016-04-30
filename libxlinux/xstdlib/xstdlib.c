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
#include <string.h>

#include "internal.h"
#include "xstdlib/xstdlib.h"
#include "errtab/SYS.errtab.h"
#include "mempolicy/mempolicy.internal.h"

API xapi xmalloc(void* target, size_t size)
{
  enter;

  void * mem = 0;

  if(policy)
  {
    fatal(policy->malloc, policy, target, size);
  }
  else if((mem = calloc(size, 1)) == 0)
  {
    fail(errno);
  }
  else
  {
    *(void**)target = mem;
  }
  
finally :
  if(XAPI_UNWINDING)
  {
    xapi_infof("size", "%zu", size);
  }
coda;
}

API xapi xrealloc(void* target, size_t es, size_t ec, size_t oec)
{
  enter;

  if(policy)
  {
    fatal(policy->realloc, policy, target, es, ec, oec);
  }
  else
  {
    void** t = ((void**)target);
    void * mem = 0;
    if((mem = realloc(*t, es * ec)) == 0)
      fail(errno);
    *t = mem;

    if(es * ec)
    {
      if(*t)
      {
        if(((ssize_t)ec - (ssize_t)oec) > 0)
          memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);
      }
      else
      {
        fail(errno);
      }
    }
  }

finally :
  if(XAPI_UNWINDING)
  {
    xapi_infof("size", "%zu", es * ec);
  }
coda;
}

void API ifree(void* target)
{
  void** t = (void**)target;

  if(policy)
  {
    if(policy->ifree)
      policy->ifree(policy, target);
  }
  else
  {
    free(*t);
  }

  *t = 0;
}

void API xfree(void* target)
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

  int hasfailed = 0;

  int compar(const void * A, const void * B, void * T)
  {
    enter_nochecks;

    int r = 0;
    if(!hasfailed)
      fatal(xcompar, A, B, T, &r);

    int R = 0;
    finally : conclude(&R);

    if(R)
    {
      hasfailed = 1;
      r = 0;
    }

    return r;
  };

  qsort_r(base, nmemb, size, compar, arg);

  if(hasfailed)
    fail(0);

  finally : coda;
}

API xapi xreadlink(const char * pathname, char * buf, size_t bufsiz, ssize_t * r)
{
  enter;

  ssize_t lr;
  if(!r)
    r = &lr;

  if((*r = readlink(pathname, buf, bufsiz)) == -1)
    fail(errno);

finally:
  xapi_infos("path", pathname);
coda;
}
