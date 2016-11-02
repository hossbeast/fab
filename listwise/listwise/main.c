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
#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "xapi.h"
#include "listwise/load.h"
#include "logger/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"

#include "logger.h"
#include "xapi/trace.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"
#include "valyria/pstring.h"

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/transform.h"
#include "listwise/object.h"
#include "listwise/lstack.h"
#include "listwise/exec.h"

#include "args.h"
#include "logging.h"
#include "errtab/MAIN.errtab.h"

/// snarf
//
// SUMMARY
//  read a file into memory
//
// PARAMETERS
//  path - path to file to load
//  mem  - append contents here
//  bang - true if this file is an interpreter script
//
static int snarf(char * path, int bang, pstring * mem)
{
  enter;

  int fd = -1;
  struct stat st;

  if(strcmp(path, "-") == 0)
    path = "/dev/fd/0";

  fatal(xopens, &fd, O_RDONLY | O_NONBLOCK, path);
  fatal(xfstat, fd, &st);

  if(S_ISREG(st.st_mode) || S_ISFIFO(st.st_mode))
  {
    char blk[512];
    ssize_t r = 1;
    while(r != 0)
    {
      fatal(uxread, fd, blk, sizeof(blk) / sizeof(*blk), &r);

      if(r > 0)
      {
        // discard the first line of an interpreter script
        if(bang && r > 2 && memcmp(blk, "#!", 2) == 0)
        {
          int x;
          for(x = 2; x < r; x++)
          {
            if(blk[x] == '\n')
              break;
          }

          if(x < r)
          {
            fatal(pscatw, mem, &blk[x + 1], r - x - 1);
            bang = 0;
            continue;
          }
        }

        fatal(pscatw, mem, blk, r);
      }
    }
  }
  else if(S_ISCHR(st.st_mode))
  {
    /*
    ** /dev/fd/0 is connected to a character device (/dev/pts/x) when nothing is piped to stdin of the process
    */
  }
  else
  {
    failf(MAIN_BADFILE, "type : %s (%d)"
      ,   S_ISREG(st.st_mode)     ? "REG"
        : S_ISDIR(st.st_mode)     ? "DIR"
        : S_ISCHR(st.st_mode)     ? "CHR"
        : S_ISBLK(st.st_mode)     ? "BLK"
        : S_ISFIFO(st.st_mode)    ? "FIFO"
        : S_ISLNK(st.st_mode)     ? "LNK"
        : S_ISSOCK(st.st_mode)    ? "SOCK"
        :                           "UNKNOWN"
      , st.st_mode & S_IFMT
    );
  }
 
finally:
  fatal(ixclose, &fd);

  xapi_infof("path", "%s", path);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  char space[4096];

  xapi R = 0;
  pstring * temp = 0;
  pstring * trans = 0;
  transform* g = 0;           // transform
  lwx * lx = 0;               // list stack

  // load libraries
  fatal(listwise_load);
  fatal(logger_load);
  fatal(valyria_load);
  fatal(xlinux_load);

  // logging
  fatal(logging_setup);
  fatal(logger_arguments_setup, envp);
  fatal(logger_finalize);

  // locals
  fatal(lwx_alloc, &lx);
  fatal(pscreate, &temp);
  fatal(pscreate, &trans);

  // parse cmdline args
  fatal(args_parse);
  fatal(args_report);

  // read transform-expr from stdin
  if(g_args.stdin_init_list_items)
  {
    psclear(temp);
    fatal(snarf, "-", 0, temp);

    char delim = g_args.stdin_linewise ? '\n' : 0;

    char * s[2] = { temp->s, 0 };
    while((s[1] = memchr(s[0], delim, temp->l - (s[0] - ((char*)temp->s)))))
    {
      if(s[1] - s[0])
        fatal(lstack_addw, lx, s[0], s[1] - s[0]);

      s[0] = s[1] + 1;
    }
  }

  // read transform-expr and init-items
  int x;
  for(x = 0; x < g_args.inputsl; x++)
  {
    if(g_args.inputs[x].type == INPUT_TYPE_INIT_LIST_ITEM)
    {
      fatal(lstack_adds, lx, g_args.inputs[x].s);
    }
    else if(g_args.inputs[x].type == INPUT_TYPE_INIT_LIST_FILE)
    {
      psclear(temp);
      fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, temp);

      char delim = g_args.inputs[x].linewise ? '\n' : 0;

      char * s[2] = { temp->s, 0 };
      while((s[1] = memchr(s[0], delim, temp->l - (s[0] - ((char*)temp->s)))))
      {
        if(s[1] - s[0])
          fatal(lstack_addw, lx, s[0], s[1] - s[0]);

        s[0] = s[1] + 1;
      }
    }
    else if(g_args.inputs[x].type == INPUT_TYPE_TRANSFORM_ITEM)
    {
      if(trans && trans->l)
        fatal(pscats, trans, " ");

      fatal(pscats, trans, g_args.inputs[x].s);
    }
    else if(g_args.inputs[x].type == INPUT_TYPE_TRANSFORM_FILE)
    {
      if(trans && trans->l)
        fatal(pscats, trans, " ");
      
      fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, trans);
    }
    else if(g_args.inputs[x].type == INPUT_TYPE_HYBRID_FILE)
    {
      psclear(temp);
      fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, temp);

      // read transform-expr until 3 consecutive newlines
      char * s[2] = { temp->s, 0 };
      while((s[1] = memchr(s[0], '\n', temp->l - (s[0] - ((char*)temp->s)))))
      {
        int dl = 0;
        while(s[1][dl] == '\n' && temp->l > ((s[1] - ((char*)temp->s)) + dl))
          dl++;

        if(s[1] - s[0])
        {
          if(s[0] != temp->s || (s[1] - s[0]) <= 2 || memcmp(s[0], "#!", 2))
          {
            if(trans && trans->l)
              fatal(pscats, trans, " ");

            fatal(pscatw, trans, s[0], s[1] - s[0]);
          }
        }

        s[0] = s[1] + 1;

        if(dl > 2)
          break;
      }

      // read init-list-items
      char delim = g_args.inputs[x].linewise ? '\n' : 0;
      while((s[1] = memchr(s[0], delim, temp->l - (s[0] - ((char*)temp->s)))))
      {
        if(s[1] - s[0])
          fatal(lstack_addw, lx, s[0], s[1] - s[0]);

        s[0] = s[1] + 1;
      }
    }
  }

  // parse
  if(trans->l)
    fatal(transform_parse, 0, trans->s, trans->l, &g);

  // execute
  if(g)
    fatal(listwise_exec_transform, g, 0, 0, 0, &lx);

  // OUTPUT
  int i = 0;
  int y = 0;
  for(y = 0; lx && y < LSTACK_LISTS(lx); y++)
  {
    if(y)
    {
      if(g_args.mode_separator == MODE_OUTPUT_SEPARATOR_NULL)
        printf("%c", 0);
      else // MODE_OUTPUT_SEPARATOR_NEWLINE
        printf("\n");

      i = 0;
    }

    LSTACK_ITERATE_LIST(lx, y, x, go)
    {
      if(go)
      {
        int spacel = 0;
        space[0] = 0;

        // numbering
        if(g_args.mode_numbering != MODE_OUTPUT_NUMBERING_NONE)
        {
          int j = i++;
          if(g_args.mode_numbering == MODE_OUTPUT_NUMBERING_INDEX)
            j = x;

          if(g_args.mode_output == MODE_OUTPUT_STACK)
            spacel = snprintf(space, sizeof(space), "%3d %3d ", y, j);
          else
            spacel = snprintf(space, sizeof(space), "%3d ", j);
        }

        // string value
        char * ss = 0;
        int    ssl = 0;
        fatal(lstack_getstring, lx, y, x, &ss, &ssl);

        // delimiter
        char delim[1];
        if(g_args.mode_separator == MODE_OUTPUT_SEPARATOR_NULL)
          delim[0] = 0;
        else // MODE_OUTPUT_SEPARATOR_NEWLINE
          delim[0] = '\n';

        int __attribute__((unused)) rr = writev(1
          , (struct iovec[]) {
              { .iov_base = space , .iov_len = spacel }
            , { .iov_base = ss    , .iov_len = ssl }
            , { .iov_base = delim , .iov_len = 1 }
          }, 3
        );
      }
    }
    LSTACK_ITEREND

    if(g_args.mode_output != MODE_OUTPUT_STACK)
      break;
  }

finally:
#if XAPI_STACKTRACE
  if(XAPI_UNWINDING)
  {
    size_t tracesz = 0;

#if DEBUG || DEVEL
    if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
    {
#endif
      tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEBUG || DEVEL
    }
    else
    {
      tracesz = xapi_trace_full(space, sizeof(space));
    }
#endif

    xlogw(L_ERROR, L_RED, space, tracesz);
  }
#endif

  // locals
  lwx_free(lx);
  transform_free(g);
  psfree(temp);
  psfree(trans);

  // modules
  args_teardown();

  // libraries
  fatal(listwise_unload);
  fatal(logger_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);

conclude(&R);

#ifndef XAPI_MODE_ERRORCODE
  xapi_teardown();
#endif

  return R;
}
