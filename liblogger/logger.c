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

#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "xlinux.h"

#include "internal.h"
#include "logs/logs.internal.h"
#include "category/category.internal.h"
#include "stream/stream.internal.h"
#include "filter/filter.internal.h"

#include "logger/LOGGER.errtab.h"

#define restrict __restrict

char **			g_argv;
int					g_argc;
char *			g_argvs;
int					g_argvsl;
char *			g_binary;
char *			g_interpreter;
char **			g_logv;
static int	g_logva;
int					g_logc;
char *			g_logvs;
int					g_logvsl;

//
// static
//

//
// api
//

API xapi logger_setup()
{
  enter;

  // one-time setup
  fatal(xapi_errtab_register, perrtab_LOGGER);
  fatal(category_setup);
  fatal(stream_setup);

  // register logs
  fatal(logger_category_register, logs);

  finally : coda;
}

API xapi logger_initialize(char ** restrict envp)
{
  enter;

	int	fd = -1;
	int	argsa = 0;	// g_argvs allocated size
	int	argva = 0;
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

  fatal(logger_category_activate);
  fatal(stream_activate);

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

	// read into g_argvs - single string containing entire cmdline
	int binaryx = -1;
	int interpx = -1;
	char * execfn = 0;
	do
	{
		size_t newa = argsa ?: 100;
		newa += newa * 2 + newa / 2;
		fatal(xrealloc, &g_argvs, sizeof(*g_argvs), newa, argsa);
		argsa = newa;
		g_argvsl += read(fd, &g_argvs[g_argvsl], argsa - g_argvsl);
	} while(g_argvsl == argsa);
	g_argvsl--;

	// locate binary and interpreter, if any
	for(x = -1; x < g_argvsl; x = y)
	{
		for(y = x + 1; y < g_argvsl; y++)
		{
			if(g_argvs[y] == 0)
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
		else if(interpx == -1 && execfn && strcmp(&g_argvs[x + 1], execfn) == 0)
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
	for(x = binaryx + strlen(&g_argvs[binaryx]) + 1; x < (interpx - 1); x++)
	{
		// process the optional-arg in a linux-specific way
		if(g_argvs[x] == 0x20)
			g_argvs[x] = 0;
	}
#endif

	// 1. replace nulls with spaces in g_argvs
	// 2. construct g_argv, array of arguments
	i = 0;
	for(x = 0; x <= g_argvsl; x++)
	{
		if(g_argvs[x] == 0)
		{
			int len = strlen(&g_argvs[i]);
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
				memcpy(g_argv[g_argc], &g_argvs[i], len);

				if(binaryx == i)
					g_binary = g_argv[g_argc];
				else if(interpx == i)
					g_interpreter = g_argv[g_argc];

				g_argc++;
				i = x + 1;
			}

			if(x < g_argvsl)
				g_argvs[x] = ' ';
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

#if 1
printf("liblogger program arguments\n\n");
printf("g_argvs : %s\n", g_argvs);
printf("g_argv\n");
for(x = 0; x < g_argc; x++)
{
	printf("[%2d] %s", x, g_argv[x]);
	if(g_binary == g_argv[x])
		printf(" * binary");
	else if(g_interpreter == g_argv[x])
		printf(" * interpreting");
	printf("\n");
}

printf("g_logvs : %s\n", g_logvs);
printf("g_logv\n");
for(x = 0; x < g_logc; x++)
	printf("[%2d] %s\n", x, g_logv[x]);
printf("\n");
#endif

  fatal(streams_report);

finally:
	fatal(ixclose, &fd);
  filter_free(filterp);

#if __linux__
	free(auxv);
#endif
coda;
}

API void logger_teardown()
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

  category_teardown();
  stream_teardown();
}
