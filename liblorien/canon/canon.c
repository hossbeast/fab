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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "internal.h"
#include "canon.internal.h"

#include "strutil.h"
#include "macros.h"

//
// static
//

#define SLASH   1
#define DOT     2
#define DOTDOT  3

typedef struct
{
  char *        s;
  int           l;
  int           t;

  int           f;  // force
} item;

static xapi breakup(item ** i, int * ia, int * il, int at, char * fmt, ...)
{
  enter;

  char in[512];

  va_list va;
  va_start(va, fmt);
  int inl = vsnprintf(in, sizeof(in), fmt, va);
  va_end(va);

  int x = 0;
  while(x < inl)
  {
    while((*il) >= (*ia))
    {
      int ns = (*ia) ?: 10;
      ns = ns * 2 + ns / 2;

      fatal(xrealloc, i, sizeof(**i), ns, *ia);
      *ia = ns;
    }

    // swap an unused entry into position at
    item T = (*i)[(*ia) - 1];

    memmove(
        &(*i)[at + 1]
      , &(*i)[at]
      , ((*ia) - at - 1) * sizeof((*i)[0])
    );

    (*i)[at] = T;

    // reset it
    (*i)[at].s = 0;
    (*i)[at].l = 0;
    (*i)[at].t = 0;

    if((inl - x) >= 1 && in[x+0] == '/')
    {
      (*i)[at].t = SLASH;
      x++;
    }
    else if((inl - x) >= 1 && in[x+0] == '.' && (in[x+1] == 0 || in[x+1] == '/'))
    {
      (*i)[at].t = DOT;
      x++;
    }
    else if((inl - x) >= 2 && in[x+0] == '.' && in[x+1] == '.' && (in[x+2] == 0 || in[x+2] == '/'))
    {
      (*i)[at].t = DOTDOT;
      x += 2;
    }
    else
    {
      const char * e = &in[x];
      while(e[0] && e[0] != '/')
        e++;

      fatal(ixstrdup, &(*i)[at].s, &in[x]);
      (*i)[at].l = e - &in[x];
      x += (*i)[at].l;
    }

    (*il)++;
    at++;
  }

  finally : coda;
}

static void add(size_t * z, char * const resolved, size_t sz, char * fmt, ...)
{
  if(*z && resolved[(*z) - 1] != '/')
    (*z) += znloadf(resolved + (*z), sz - (*z), "/");

  va_list va;
  va_start(va, fmt);
  (*z) += znvloadf(resolved + (*z), sz - (*z), fmt, va);
  va_end(va);

  resolved[(*z)] = 0;
}

//
// api
//

/*
invocations

 gn.c     : (  7) CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT
 gn.c     : (255) CAN_REALPATH
 path.c   : (255) CAN_REALPATH
 path.c   : ( 15) CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT | CAN_NEXT_SYM
 params.c : (255) CAN_REALPATH
 op/rb.c  : ( 15) CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT | CAN_NEXT_SYM

optimization

 lstat can be expensive, especially on clients with network-mounted file systems. the lstat/readlink
 calls are only necessary to implement the *_SYM options, which are strictly only necessary for
 display of paths to the user. using them, or not, could be a configurable option (probably per-filesystem)

 further, the compression ratio of calls to canon -> calls to canon with unique arguments is large. when
 compiling fab itself, this value is greater than 4 and less than 7. thus, there is a possibility of
 time savings here by memoization
*/

API xapi canon(
    const char * path
  , size_t pathl
  , const char * const base
  , size_t basel
  , char * const dst
  , const size_t siz
  , size_t * z
  , uint32_t opts
)
{
  enter;

  int x;
  opts = opts ?: CAN_REALPATH;
  pathl = pathl ?: strlen(path);

  if(base)
  {
    basel = basel ?: strlen(base);
  }

  size_t local_z;
  if(z == 0)
  {
    z = &local_z;
  }

  (*z) = 0;

  item *  i = 0;
  int     ia = 0;
  int     il = 0;
  int     ix = 0;

  // if the path begins with a DOT or a DOTDOT
  if(pathl >= 1 && path[0] == '.')
  {
    if(pathl == 1 || (path[1] == '.' || path[1] == '/' || path[1] == 0))
    {
      if(pathl == 2 || path[1] == '/' || path[1] == 0 || (path[2] == '/' || path[2] == 0))
      {
        if(opts & CAN_INIT_DOT)
        {
          fatal(breakup, &i, &ia, &il, il, "%.*s/", basel, base);
        }
      }
    }
  }

  // else if the path is not an absolute path
  else if(pathl == 0 || path[0] != '/')
  {
    if(opts & CAN_FORCE_DOT)
    {
      fatal(breakup, &i, &ia, &il, il, "%.*s/", basel, base);
    }
  }

  int init = il;
  fatal(breakup, &i, &ia, &il, il, "%.*s", pathl, path);
  dst[0] = 0;

#if 0
printf("\n");
for(x = 0; x < il; x++)
{
  printf("[%d]", x);
  if(x == ix)
    printf(">");
  else
    printf(" ");

  if(i[x].t == SLASH)
    printf("[%d] SLASH", x);
  else if(i[x].t == DOT)
    printf("[%d] DOT", x);
  else if(i[x].t == DOTDOT)
    printf("[%d] DOTDOT", x);
  else
    printf("[%d] %.*s", x, i[x].l, i[x].s);

  printf("\n");
  printf("%.*s\n", (int)(*z), dst);
}
#endif
  for(ix = 0; ix < il; ix++)
  {

    if(i[ix].t == SLASH)
    {
      if((*z) == 0)
        (*z) += znloadf(dst + (*z), siz - (*z), "/");
    }
    else if(i[ix].t == DOT)
    {
      if(init != -1)
      {
        if((opts & CAN_INIT_DOT) == 0)
          add(z, dst, siz, ".");
      }
      else
      {
        if((opts & CAN_NEXT_DOT) == 0)
          add(z, dst, siz, ".");
      }
    }
    else if(i[ix].t == DOTDOT)
    {
      int no = 0;
      if(init != -1)
      {
        if((opts & CAN_INIT_DOT) == 0)
          no = 1;
      }
      else
      {
        if((opts & CAN_NEXT_DOT) == 0)
          no = 1;
      }

      if(no || (*z) == 0 || ((*z) == 1 && dst[0] == '/'))
      {
        // cannot backup; append the DOTDOT
        add(z, dst, siz, "..");
      }
      else
      {
        // back up
        if(*z)
          (*z)--;
        while((*z) && dst[(*z)] != '/')
          (*z)--;
      }
    }
    else
    {
      if(ix >= init)
        init = -1;

      int isfinal = 1;
      int fix;
      for(fix = ix + 1; fix < il; fix++)
      {
        if(i[fix].t != SLASH)
        {
          isfinal = 0;
        }
      }

      // append the link name to pass to stat
      size_t oldz = (*z);
      add(z, dst, siz, "%.*s", i[ix].l, i[ix].s);

      struct stat stb[2] = {};
      if((*z) >= 4 && memcmp(dst, "/../", 4) == 0)
      {
        /* do not resolve nofile references against the filesystem lest they be resolved against slash (/) */
      }
      else if(lstat(dst, &stb[0]))
      {
        if(errno != ENOENT && errno != EACCES && errno != ENOTDIR)
        {
          tfail(perrtab_KERNEL, errno);
        }
      }
      else
      {
        if(S_ISLNK(stb[0].st_mode))
        {
          // space contains the link target
          char space[512];

          int j = readlink(dst, space, sizeof(space));
          space[j] = 0;

          // space2 contains the resolved path
          char space2[512];
          snprintf(space2, sizeof(space2), "%.*s/%s", (int)oldz, dst, space);

          // stat the resolved path
          int r = lstat(space2, &stb[1]);
          if(r || stb[0].st_dev == stb[1].st_dev)
          {
            // dangling links, and fulfilled links which do not cross mount points
            if((isfinal && (opts & CAN_FINL_SYM)) || (!isfinal && (opts & CAN_NEXT_SYM)))
            {
              // breakup the link target and append to item list
              fatal(breakup, &i, &ia, &il, ix + 1, "%.*s", j, space);

              // if the link target is an absolute path, resume writing at z=0
              if(space[0] == '/')
                (*z) = 0;

              // otherwise, resume writing at the location where the link was encountered
              else
                (*z) = oldz;
            }
          }
          else if((isfinal && (opts & CAN_FINL_SYMMNT)) || (!isfinal && (opts & CAN_NEXT_SYMMNT)))
          {
            fatal(breakup, &i, &ia, &il, ix + 1, "%.*s", j, space);

            if(space[0] == '/')
              (*z) = 0;
            else
              (*z) = oldz;
          }
        }
      }
    }
  }

  dst[(*z)] = 0;

#if 0
printf("\n");
printf("%7s : %s\n", "path", path);
printf("%7s : %s\n", "base", base);
printf("%7s : %u\n", "opts", opts);
printf("%7s : %s\n", "final", dst);
#endif

finally:
  for(x = 0; x < ia; x++)
    wfree(i[x].s);
  wfree(i);
coda;
}
