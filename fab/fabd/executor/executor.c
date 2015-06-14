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

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "global.h"
#include "executor.h"
#include "args.def.h"
#include "params.h"
#include "bp.h"

#include "parseint.h"
#include "macros.h"

#define restrict __restrict

typedef struct executor_context
{
  struct pstring *      tmp;      // reusable temp space

  struct
  {
    pid_t   pid;                  // pid of child
    int     num;                  // command number
    int     cmd_fd;               // file descriptors
    int     stdo_fd;
    int     stde_fd;
#if 0
    struct pstring ** prod_type;  // product types
    struct pstring ** prod_id;    // product ids
    size_t  prodl;                // number of products - occupied
    size_t  proda;                // number of products - allocated
#endif
  } *     cmds;
  size_t  cmdsl;
  size_t  cmdsa;
} executor_context;

//
// public
//

int executor_context_mk(executor_context ** const restrict ctx)
{
  fatal(xmalloc, ctx, sizeof(**ctx));

  finally : coda;
}

int executor_context_recycle(executor_context * ctx)
{
  int x;
  for(x = 0; x < ctx->cmdsl; x++)
  {
    fatal(ixclose, &ctx->cmds[x].cmd_fd);
    fatal(ixclose, &ctx->cmds[x].stdo_fd);
    fatal(ixclose, &ctx->cmds[x].stde_fd);

//    ctx->cmds[x].prodl = 0;
  }

  ctx->cmdsl = 0;

  finally : coda;
}

int executor_context_dispose(executor_context * ctx)
{
  if(ctx)
  {
    int x;
    for(x = 0; x < ctx->cmdsa; x++)
    {
      fatal(ixclose, &ctx->cmds[x].cmd_fd);
      fatal(ixclose, &ctx->cmds[x].stdo_fd);
      fatal(ixclose, &ctx->cmds[x].stde_fd);

#if 0
      int y;
      for(y = 0; y < ctx->cmds[x].proda; y++)
      {
        psfree(ctx->cmds[x].prod_type[y]);
        psfree(ctx->cmds[x].prod_id[y]);
      }
      free(ctx->cmds[x].prod_type);
      free(ctx->cmds[x].prod_id);
#endif
    }

    free(ctx->cmds);
    psfree(ctx->tmp);
  }

  free(ctx);

  finally : coda;
}

int executor_execute(
    executor_context * const restrict ctx
  , const char * const restrict subdir
  , const int cmdsl
	, const executor_logging * const restrict logging
  , int * const restrict success
)
{
  int fd = -1;
  int i;
  struct stat stb;

  // context reset
  fatal(executor_context_recycle, ctx);

	// step size is subject to concurrency restrictions
  int step = cmdsl;
  if(g_args->concurrency > 0)
    step = g_args->concurrency;

	// process command-set one wave at a time
  int j;
  for(j = 0; j < cmdsl; j += step)
  {
		// wavel is the number of commands to be processed on this iteration
    int wavel = step;
    if((j + step) > cmdsl)
      wavel = cmdsl - j;

    // launch the wave
    for(i = j; i < (j + wavel); i++)
    {
      // open file for stdout
			fatal(psloadf, &ctx->tmp, "%s/%s/%d/out", g_params.ipcdir, subdir, i);
      fatal(ixclose, &ctx->cmds[i].stdo_fd);
      fatal(xopen_mode, ctx->tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, FABIPC_DATA, &ctx->cmds[i].stdo_fd);

      // open file for stderr
			fatal(psloadf, &ctx->tmp, "%s/%s/%d/err", g_params.ipcdir, subdir, i);
      fatal(ixclose, &ctx->cmds[i].stde_fd);
      fatal(xopen_mode, ctx->tmp->s, O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC, FABIPC_DATA, &ctx->cmds[i].stde_fd);

      // path to the command
			fatal(psloadf, &ctx->tmp, "%s/%s/%d/cmd", g_params.ipcdir, subdir, i);

      // open the file if we will later need to examine it
      if(logging && log_would(logging->cmd_cat))	// L_BPCMD
      {
        fatal(ixclose, &ctx->cmds[i].cmd_fd);
        fatal(xopen, ctx->tmp->s, O_RDONLY | O_CLOEXEC, &ctx->cmds[i].cmd_fd);
      }

      // fork off the child
      fatal(xfork, &ctx->cmds[i].pid);
      if(ctx->cmds[i].pid == 0)
      {
        // reopen stdin
        fatal(xclose, 0);
        fatal(xopen, "/dev/null", O_RDONLY, 0);

        // save stdout @ 501, then redirect to the file (dup2 calls close)
        fatal(xdup2, 1, 501);
        fatal(xdup2, ctx->cmds[i].stdo_fd, 1);

        // save stderr @ 502, then redirect to the file (dup2 calls close)
        fatal(xdup2, 2, 502);
        fatal(xdup2, ctx->cmds[i].stde_fd, 2);

        // irretrievably drop fabsys:fabsys identity
        fatal(xsetresuid, g_params.ruid, g_params.ruid, g_params.ruid);
        fatal(xsetresgid, g_params.rgid, g_params.rgid, g_params.rgid);

        // new process group
        fatal(xsetpgid, 0, 0);

        // exec doesnt return
        execl(ctx->tmp->s, ctx->tmp->s, (void*)0);
        fail(0);
      }
    }

    // harvest the wave
    int good = 0;
    int fail = 0;
    pid_t pid = 0;
    int r = 0;
    while((pid = wait(&r)) != -1)
    {
      for(i = j; i < (j + step) && i < cmdsl; i++)
      {
        if(ctx->cmds[i].pid == pid)
          break;
      }

      // save the exit status
			fatal(psloadf, &ctx->tmp, "%s/%s/%d/exit", g_params.ipcdir, subdir, i);
      fatal(ixclose, &fd);
      fatal(xopen_mode, ctx->tmp->s, O_CREAT | O_EXCL | O_WRONLY, FABIPC_DATA, &fd);
      fatal(axwrite, fd, &r, sizeof(r));

      // not a clean exit
      int bad = 0;
      if((WIFEXITED(r) && WEXITSTATUS(r)) || WIFSIGNALED(r))
      {
        bad = 1;
      }
      else
      {
        // wrote to stderr
        off_t fsiz;
        fatal(xlseek, ctx->cmds[i].stde_fd, 0, SEEK_END, &fsiz);
        if(fsiz)
        {
          bad = 1;
        }
        else
        {
          good++;
        }
      }

      uint64_t tag = logging->exec_cat; //L_BPEXEC;
      if(bad)
        tag |= L_ERROR;

      if(logging && log_would(tag))
      {
				logging->log_exec(logging, i, bad, r);

/*
        int y;
        for(y = 0; y < ctx->cmds[i].prodl; y++)
        {
          if(y == 0)
          {
            log_start(tag);
            logf(0, "[%2d,%3d] %s %s", stagex, i, ctx->cmds[i].prod_type[y], ctx->cmds[i].prod_id[y]);

            if(WIFEXITED(r) && WEXITSTATUS(r))
              logf(0, ", exit status : %d", stagex, i, ctx->cmds[i].prod_type[y], ctx->cmds[i].prod_id[y], WEXITSTATUS(r));
            else if(WIFSIGNALED(r))
              logf(0, ", signal : %d", stagex, i, ctx->cmds[i].prod_type[y], ctx->cmds[i].prod_id[y], WTERMSIG(r));

            if(bad)
            {
              logf(0, ", details @ %s/bp/%d/%d", g_params.ipcdir, stagex, i);
            }

            log_finish();
          }
          else
          {
            logf(tag, "%*s %s %s", 8, "", ctx->cmds[i].prod_type[y], ctx->cmds[i].prod_id[y]);
          }
        }
*/
      }

      if(logging && log_would(logging->cmd_cat))	// L_BPCMD
      {
        // snarf cmd
        fatal(xfstat, ctx->cmds[i].cmd_fd, &stb);
        fatal(psgrow, &ctx->tmp, stb.st_size);
        fatal(xlseek, ctx->cmds[i].cmd_fd, 0, SEEK_SET, 0);
        fatal(axread, ctx->cmds[i].cmd_fd, ctx->tmp->s, stb.st_size);
      
        // chomp trailing whitespace
        size_t end = stb.st_size - 1;
        while(end >= 0 && (ctx->tmp->s[end] == ' ' || ctx->tmp->s[end] == '\t' || ctx->tmp->s[end] == '\r' || ctx->tmp->s[end] == '\n'))
          end--;
        end++;

        fatal(axwrite, 1, ctx->tmp->s, end);
        fatal(axwrite, 1, "\n", 1);
      }

      if(bad)
      {
        // snarf stderr
        fatal(xfstat, ctx->cmds[i].stde_fd, &stb);
        size_t lim = 500;
        size_t act = MIN(stb.st_size, lim);

        fatal(psgrow, &ctx->tmp, act);
        fatal(xlseek, ctx->cmds[i].stde_fd, 0, SEEK_SET, 0);
        fatal(axread, ctx->cmds[i].stde_fd, ctx->tmp->s, act);
        
        // chomp trailing whitespace
        size_t end = act - 1;
        while(end >= 0 && (ctx->tmp->s[end] == ' ' || ctx->tmp->s[end] == '\t' || ctx->tmp->s[end] == '\r' || ctx->tmp->s[end] == '\n'))
          end--;
        end++;

        fatal(axwrite, 1, ctx->tmp->s, end);
        fatal(axwrite, 1, "\n", 1);

        if(stb.st_size > act)
        {
          fatal(psloadf, &ctx->tmp, " ... %d more\n", stb.st_size - end);
          fatal(axwrite, 1, ctx->tmp->s, ctx->tmp->l);
        }
      }

      if(bad)
        fail++;
      if((good + fail) == wavel)
        break;
    }

    if(fail)
      break;

    // close all the files for that wave
    for(i = j; i < (j + wavel); i++)
    {
      fatal(ixclose, &ctx->cmds[i].stdo_fd);
      fatal(ixclose, &ctx->cmds[i].stde_fd);
    }
  }

  if(j >= cmdsl)
    *success = 1;

finally:
  fatal(executor_context_recycle, ctx);
  fatal(xclose, fd);
coda;
}


#if 0
  int fn(const char * fpath, const struct stat * sb, int typeflat, struct FTW * ftwbuf)
  {
    if(ftwbuf->level == 1)
    {
      int num;
      if(parseint(fpath + ftwbuf->base, SCNd32, 0, INT32_MAX, 1, 10, &num, 0))
        failf(FAB_BADTMP, "expected: int32, actual: %s", fpath + ftwbuf->base);

      if(cmdsl == ctx->cmdsa)
      {
        size_t ns = ctx->cmdsa ?: 10;
        ns = ns * 2 + ns / 2;
        fatal(xrealloc, &ctx->cmds, sizeof(*ctx->cmds), ns, ctx->cmdsa);
        ctx->cmdsa = ns;
      }

      typeof(ctx->cmds[0]) * T = &ctx->cmds[cmdsl];
      T->num = num;
      T->cmd_fd = -1;
      T->stdo_fd = -1;
      T->stde_fd = -1;
      cmdsl++;
    }
  finally : 
    XAPI_INFOS("path", fpath);
  coda;
  };

  // directory containing the buildplan stage
  fatal(psloadf, &ctx->tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d", g_params.pid, stagex);

  // traverse the buildplan stage directory
  fatal(xnftw, ctx->tmp->s, fn, 32, 0);

  // load the build products for the stage
  for(i = 0; i < cmdsl; i++)
  {
    fatal(psloadf, &ctx->tmp, XQUOTE(FABTMPDIR) "/pid/%d/bp/%d/%d/prod", g_params.pid, stagex, i);
    fatal(ixclose, &fd);
    fatal(xopen, ctx->tmp->s, O_RDONLY, &fd);

    // get the filesize
    fatal(xfstat, fd, &stb);

    // read the whole file
    fatal(psgrow, &ctx->tmp, stb.st_size);
    fatal(axread, fd, ctx->tmp->s, stb.st_size);

    // fields are delimited by null bytes and come in pairs
    int a = -1;
    int b = -1;
    while((a < stb.st_size) && (b < stb.st_size))
    {
      b = a + 1 + strlen(ctx->tmp->s + a + 1);
      
      if(b < stb.st_size)
      {
        if(ctx->cmds[i].prodl == ctx->cmds[i].proda)
        {
          size_t ns = ctx->cmds[i].proda;
          ns = ns * 2 + ns / 2;
          fatal(xrealloc, &ctx->cmds[i].prod_type, sizeof(*ctx->cmds[0].prod_type), ns, ctx->cmds[i].proda);
          fatal(xrealloc, &ctx->cmds[i].prod_id, sizeof(*ctx->cmds[0].prod_id), ns, ctx->cmds[i].proda);
          ctx->cmds[i].proda = ns;
        }

        fatal(psloads, &ctx->cmds[i].prod_type[ctx->cmds[i].prodl], ctx->tmp->s + a + 1);
        fatal(psloads, &ctx->cmds[i].prod_id[ctx->cmds[i].prodl], ctx->tmp->s + b + 1);

        ctx->cmds[i].prodl++;

        a = b + 1 + strlen(ctx->tmp->s + b + 1);
      }
    }
  }
#endif
