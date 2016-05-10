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
#include "xlinux.h"

#include "internal.h"
#include "arguments.internal.h"
#include "filter.internal.h"

#if 0
#include <stdio.h>
#include <string.h>

#include "xapi/errtab.h"

#include "internal.h"
#include "logs/logs.internal.h"
#include "category/category.internal.h"
#include "stream/stream.internal.h"
#include "filter/filter.internal.h"

#include "logger/LOGGER.errtab.h"
#endif

#define restrict __restrict

APIDATA char **			  g_argv;
APIDATA int					  g_argc;
APIDATA char *			  g_argvs;
APIDATA size_t			  g_argvsl;
APIDATA char *			  g_binary;
APIDATA char *			  g_interpreter;
APIDATA char **			  g_logv;
APIDATA static size_t	g_logva;
APIDATA int					  g_logc;
APIDATA char *			  g_logvs;
APIDATA size_t			  g_logvsl;

//
// public
//

xapi arguments_report()
{
  enter;

  int token = 0;

  logf(L_LOGGER, "logger cmdline arguments");
  logf(L_LOGGER, " args : %s", g_argvs);
  int x;
  for(x = 0; x < g_argc; x++)
  {
    fatal(log_start, L_LOGGER, &token);
    logf(L_LOGGER, "  [%2d] %s", x, g_argv[x]);
    if(g_binary == g_argv[x])
      logf(L_LOGGER, "   <-- binary");
    else if(g_interpreter == g_argv[x])
      logf(L_LOGGER, "   <-- interpreting");
    fatal(log_finish, &token);
  }

  logf(L_LOGGER, " logs : %s", g_logvs);
  for(x = 0; x < g_logc; x++)
    logf(L_LOGGER, "  [%2d] %s", x, g_logv[x]);

finally:
  fatal(log_finish, &token);
coda;
}

xapi arguments_initialize(char ** restrict envp)
{
  enter;

	int	fd = -1;
  char * argvs = 0;
  size_t argvsl = 0; // argvs length
	size_t argvsa = 0;	// argvs allocated size
	size_t argva = 0;
#if __linux__
	char * auxv = 0;
	size_t auxvl = 0;
	size_t auxva = 0;
#endif
	int x;
	int y;
	int i;

  filter * filterp = 0;

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

	// snarf the cmdline
	fatal(xopen, "/proc/self/cmdline", O_RDONLY, &fd);

	// read into argvs - single string containing entire cmdline
	int binaryx = -1;
	int interpx = -1;
	char * execfn = 0;
	do
	{
		size_t newa = argvsa ?: 100;
		newa += newa * 2 + newa / 2;
		fatal(xrealloc, &argvs, sizeof(*argvs), newa, argvsa);
		argvsa = newa;
		argvsl += read(fd, &argvs[argvsl], argvsa - argvsl);
	} while(argvsl == argvsa);
	argvsl--;

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

				fatal(ixclose, &fd);
				fatal(xopen, "/proc/self/auxv", O_RDONLY, &fd);
				do
				{
					size_t newa = auxva ?: 100;
					newa += newa * 2 + newa / 2;
					fatal(xrealloc, &auxv, sizeof(*auxv), newa, auxva);
					auxva = newa;
					auxvl += read(fd, &auxv[auxvl], auxva - auxvl);
				} while(auxvl == auxva);
				auxvl--;

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

	// construct g_argv, array of arguments
	i = 0;
	for(x = 0; x <= argvsl; x++)
	{
		if(argvs[x] == 0)
		{
			int len = strlen(&argvs[i]);
			if(len)
			{
				if(g_argc == argva)
				{
					size_t newa = argva ?: 10;
					newa += newa * 2 + newa / 2;
					fatal(xrealloc, &g_argv, sizeof(*g_argv), newa, argva);
					argva = newa;
				}

				fatal(xmalloc, &g_argv[g_argc], len + 1);
				memcpy(g_argv[g_argc], &argvs[i], len);

				if(binaryx == i)
					g_binary = g_argv[g_argc];
				else if(interpx == i)
					g_interpreter = g_argv[g_argc];

				g_argc++;
				i = x + 1;
			}
		}
	}

	// process g_argv, splicing out recognized logger-options
	for(x = 0; x < g_argc; x++)
	{
    fatal(filter_parse, g_argv[x], 0, &filterp);
    if(filterp)
    {
      fatal(filter_push, 0, filterp);
      filterp = 0;

			if(g_logc == g_logva)
			{
				size_t ns = g_logva ?: 10;
				ns = ns * 2 + ns / 2;
				fatal(xrealloc, &g_logv, sizeof(*g_logv), ns, g_logva);
				g_logva = ns;
			}

			g_logv[g_logc++] = g_argv[x];
			g_argv[x] = 0;
		}
	}

	// shrink g_argv to size
	for(x = g_argc - 1; x >= 0; x--)
	{
		if(g_argv[x] == 0)
		{
			memmove(
			    &g_argv[x]
				, &g_argv[x+1]
				, (g_argc - x - 1) * sizeof(g_argv[0])
			);
			g_argc--;
		}
	}

  // render to g_argvs
	g_argvsl = 0;
	for(x = 0; x < g_argc; x++)
	{
		if(x)
			g_argvsl++;
		g_argvsl += strlen(g_argv[x]);
	}

	fatal(xmalloc, &g_argvs, g_argvsl + 1);

	for(x = 0; x < g_argc; x++)
	{
		if(x)
			strcat(g_argvs, " ");
		strcat(g_argvs, g_argv[x]);
	}

  // render to g_logvs
	g_logvsl = 0;
	for(x = 0; x < g_logc; x++)
	{
		if(x)
			g_logvsl++;
		g_logvsl += strlen(g_logv[x]);
	}

	fatal(xmalloc, &g_logvs, g_logvsl + 1);

	for(x = 0; x < g_logc; x++)
	{
		if(x)
			strcat(g_logvs, " ");
		strcat(g_logvs, g_logv[x]);
	}

finally:
	fatal(ixclose, &fd);
  filter_free(filterp);

#if __linux__
	free(auxv);
#endif
  free(argvs);
coda;
}

void arguments_teardown()
{
  free(g_argvs);
  int x;
  for(x = 0; x < g_argc; x++)
    free(g_argv[x]);
  free(g_argv);
  for(x = 0; x < g_logc; x++)
    free(g_logv[x]);
  free(g_logv);
  free(g_logvs);
}
