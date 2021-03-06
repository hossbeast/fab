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

#if __linux__
#include <sys/auxv.h>
#endif

#include <string.h>

#include "xapi.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstring.h"
#include "narrator.h"

#include "logger.h"
#include "arguments.internal.h"
#include "logging.internal.h"
#include "expr.internal.h"
#include "filter.internal.h"

#include "common/snarf.h"
#include "zbuffer.h"

char *      APIDATA g_binary;
char *      APIDATA g_interpreter;
char **     APIDATA g_argv;
size_t      APIDATA g_argc;
char *      APIDATA g_argvs;
size_t      APIDATA g_argvsl;
char **     APIDATA g_logv;
size_t      APIDATA g_logc;
char *      APIDATA g_logvs;
size_t      APIDATA g_logvsl;
char **     APIDATA g_ulogv;
size_t      APIDATA g_ulogc;
char *      APIDATA g_ulogvs;
size_t      APIDATA g_ulogvsl;

static inline size_t zloads(void * restrict dst, const char * const restrict src)
{
  size_t len = strlen(src);
  memcpy(dst, src, len);
  return len;
}

/// list_finalize
//
// SUMMARY
//  complete setup of an arguments list
//
// PARAMETERS
//  list - list
//  len  - elements in list
//  vs   - list, joined by single spaces
//  vsl  - (returns) length of vs
//
static void list_finalize(char ** list, size_t * len, char * vs, size_t * vsl)
{
  int x;
  for(x = (*len) - 1; x >= 0; x--)
  {
    if(list[x] == 0)
    {
      memmove(
          &list[x]
        , &list[x + 1]
        , ((*len) - x - 1) * sizeof(*list)
      );
      (*len)--;
    }
  }

  (*vsl) = 0;
  for(x = 0; x < *len; x++)
  {
    if(x)
      (*vsl) += zloads(vs + (*vsl), " ");

    (*vsl) += zloads(vs + (*vsl), list[x]);
  }
}

//
// public
//

void arguments_teardown()
{
  iwfree(&g_argvs);
  int x;
  for(x = 0; x < g_argc; x++)
    iwfree(&g_argv[x]);
  iwfree(&g_argv);
  g_argc = 0;

  for(x = 0; x < g_logc; x++)
    iwfree(&g_logv[x]);
  iwfree(&g_logv);
  iwfree(&g_logvs);
  g_logc = 0;

  for(x = 0; x < g_ulogc; x++)
    iwfree(&g_ulogv[x]);
  iwfree(&g_ulogv);
  iwfree(&g_ulogvs);
  g_ulogc = 0;

  g_binary = 0;
  g_interpreter = 0;
}

xapi arguments_finalize()
{
  enter;

  int x;

  // move recognized logexprs g_ulogv -> g_logv
  for(x = 0; x < g_ulogc; x++)
  {
    if(expr_validate(g_ulogv[x], 1))
    {
      g_logv[g_logc++] = g_ulogv[x];
      g_ulogv[x] = 0;
    }
  }

  // move unrecognized logexprs g_logv -> g_ulogv
  for(x = 0; x < g_logc; x++)
  {
    if(expr_validate(g_logv[x], 1))
    {
      fatal(logger_expr_push, 0, g_logv[x]);
    }
    else
    {
      g_ulogv[g_ulogc++] = g_logv[x];
      g_logv[x] = 0;
    }
  }

  list_finalize(g_logv, &g_logc, g_logvs, &g_logvsl);
  list_finalize(g_ulogv, &g_ulogc, g_ulogvs, &g_ulogvsl);

  finally : coda;
}

xapi arguments_process(const char * restrict argvs, size_t argvsl, int binaryx, int interpx)
{
  enter;

  int i;
  int x;
  size_t argva = 0;

  // construct g_argv, array of arguments
  i = 0;
  for(x = 0; x <= argvsl; x++)
  {
    if(argvs[x] == 0)
    {
      if(strlen(&argvs[i]))
      {
        argva++;
        i = x + 1;
      }
    }
  }

  fatal(xmalloc, &g_argv, sizeof(*g_argv) * argva);

  i = 0;
  for(x = 0; x <= argvsl; x++)
  {
    if(argvs[x] == 0)
    {
      int len = strlen(&argvs[i]);
      if(len)
      {
        fatal(ixstrdup, &g_argv[g_argc], &argvs[i]);

        if(binaryx == i)
          g_binary = g_argv[g_argc];
        else if(interpx == i)
          g_interpreter = g_argv[g_argc];

        g_argc++;
        i = x + 1;
      }
    }
  }

  fatal(xmalloc, &g_ulogv, sizeof(*g_ulogv) * g_argc);
  fatal(xmalloc, &g_logv, sizeof(*g_logv) * g_argc);

  size_t tot = 0;
  for(x = 0; x < g_argc; x++)
    tot += strlen(g_argv[x]);

  if(g_argc)
    tot += (g_argc - 1);

  fatal(xmalloc, &g_argvs, tot + 2);
  fatal(xmalloc, &g_logvs, tot + 2);
  fatal(xmalloc, &g_ulogvs, tot + 2);

  // move logexpr options g_argv -> g_ulogv
  for(x = 0; x < g_argc; x++)
  {
    if(expr_validate(g_argv[x], 0))
    {
      g_ulogv[g_ulogc++] = g_argv[x];
      g_argv[x] = 0;
    }
  }

  list_finalize(g_argv, &g_argc, g_argvs, &g_argvsl);

  finally : coda;
}

//
// api
//

xapi API logger_arguments_setup(char ** restrict envp)
{
  enter;

  int fd = -1;
  char * argvs = 0;
  size_t argvsl = 0; // argvs length
#if __linux__
  char * auxv = 0;
  size_t auxvl = 0;
#endif
  int x;
  int y;

  int binaryx = -1;
  int interpx = -1;
  char * execfn = 0;

  const unsigned long * restrict auxvec = 0;

#if __linux__
  // locate auxiliary vector
  if(envp)
  {
    while(*envp)
      envp++;
    envp++;
    auxvec = (void*)envp;
  }
#endif

  // read into argvs - single string containing entire cmdline
  fatal(snarfs, &argvs, &argvsl, "/proc/self/cmdline");

  // locate binary and interpreter, if any
  for(x = -1; x < (int)argvsl; x = y)
  {
    for(y = x + 1; y < argvsl; y++)
    {
      if(argvs[y] == 0)
        break;
    }

    // the binary is the first argument
    if(binaryx == -1)
    {
      binaryx = x + 1;
#if __linux__
      if(auxvec == 0)
      {
        /*
        requires glibc 2.16. ubuntu 12.10 has glibc 2.15.
        execfn = (char*)(intptr_t)getauxval(AT_EXECFN);
        */

        fatal(snarfs, &auxv, &auxvl, "/proc/self/auxv");
        auxvec = (void*)auxv;
      }

      if(auxvec)
      {
        while(*auxvec)
        {
          unsigned long key = auxvec[0];
          unsigned long val = auxvec[1];
          if(key == AT_EXECFN)
          {
            execfn = (char*)(uintptr_t)val;
            break;
          }
          auxvec += 2;
        }
      }
#endif
    }

    // when interpreting, AT_EXECFN is the interpreter script
    else if(interpx == -1 && execfn && strcmp(&argvs[x + 1], execfn) == 0)
    {
      interpx = x + 1;
    }
  }

#if __linux__
  /*
  * on linux, an interpreter script is executed with the optional-arg passed as a
  * single string. break it up on spaces, so argument parsing will see separate args
  *  see : http://man7.org/linux/man-pages/man2/execve.2.html
  *
  * getauxval is probably not available on all unices
  */
  if(binaryx >= 0)
  {
    for(x = binaryx + strlen(&argvs[binaryx]) + 1; x < (interpx - 1); x++)
    {
      // process the optional-arg in a linux-specific way
      if(argvs[x] == 0x20)
        argvs[x] = 0;
    }
  }
#endif

  fatal(arguments_process, argvs, argvsl, binaryx, interpx);

finally:
  fatal(ixclose, &fd);

#if __linux__
  wfree(auxv);
#endif
  wfree(argvs);
coda;
}

xapi API logger_arguments_report()
{
  enter;

  narrator * N;

  logf(L_LOGGER, "logger cmdline arguments");
  logf(L_LOGGER, " args : %s", g_argvs);
  int x;
  for(x = 0; x < g_argc; x++)
  {
    fatal(log_start, L_LOGGER, &N);
    xsayf("  [%2d] %s", x, g_argv[x]);
    if(g_binary == g_argv[x])
      xsayf("   <-- binary");
    else if(g_interpreter == g_argv[x])
      xsayf("   <-- interpreting");
    fatal(log_finish);
  }

  logf(L_LOGGER, " logs : %s - %s", g_logvs, g_ulogvs);
  for(x = 0; x < g_logc; x++)
    logf(L_LOGGER, "  [%2d] %s", x, g_logv[x]);
  for(x = 0; x < g_ulogc; x++)
    logf(L_LOGGER, "  [%2d] %s", x, g_ulogv[x]);

  finally : coda;
}
