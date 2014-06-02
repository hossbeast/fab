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

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/uio.h>

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/ops.h"
#include "listwise/generator.h"
#include "listwise/object.h"
#include "listwise/logging.h"
#include "listwise/describe.h"
#include "listwise/lstack.h"
#include "listwise/exec.h"

#include "xapi.h"
#include "LISTWISE.errtab.h"
#undef perrtab
#define perrtab perrtab_LISTWISE

#include "xlinux.h"

#include "pstring.h"

#include "args.h"
#include "logs.h"

int listwise_would(void * token, void * udata)
{
	return log_would(*(uint64_t*)token);
}

void listwise_log(void * token, void * udata, const char * func, const char * file, int line, char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
#if DEVEL
	log_vlogf(func, file, line, *(uint64_t*)token, fmt, va);
#else
	log_vlogf(*(uint64_t*)token, fmt, va);
#endif
	va_end(va);
}

/// snarf
//
// SUMMARY
//  read a file into memory
//
// PARAMETERS
//  path - path to file to load
//  mem  - contents returned here
//
static int snarf(char * path, pstring ** mem)
{
	int fd = -1;
	struct stat st;

	if(strcmp(path, "-") == 0)
		path = "/dev/fd/0";

	fatal(psclear, mem);
	fatal(xopen, path, O_RDONLY | O_NONBLOCK, &fd);
	fatal(xfstat, fd, &st);

	if(S_ISREG(st.st_mode) || S_ISFIFO(st.st_mode))
	{
		char blk[512];
		ssize_t r = 1;
		while(r != 0)
		{
			fatal(uxread, fd, blk, sizeof(blk) / sizeof(*blk), &r);

			if(r > 0)
				fatal(pscatw, mem, blk, r);
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
		failf(LISTWISE_BADFILE, "type : %s (%d)"
			,   S_ISREG(st.st_mode)			? "REG"
				: S_ISDIR(st.st_mode)			? "DIR"
				: S_ISCHR(st.st_mode)			? "CHR"
				: S_ISBLK(st.st_mode)			? "BLK"
				: S_ISFIFO(st.st_mode)		? "FIFO"
				: S_ISLNK(st.st_mode)			? "LNK"
				: S_ISSOCK(st.st_mode)		? "SOCK"
				: 												  "UNKNOWN"
			, st.st_mode & S_IFMT
		);
	}
 
finally:
	fatal(ixclose, &fd);

	XAPI_INFOF("path", "%s", path);
coda;
}


// setup liblistwise logging
static struct listwise_logging * logging = (struct listwise_logging[]) {{
		.generator_token	= (uint64_t[]) { L_LWPARSE }
	, .generator_would	= listwise_would
	, .generator_log		= listwise_log
	, .lstack_token			= (uint64_t[]) { L_LWEXEC }
	, .lstack_would			= listwise_would
	, .lstack_log				= listwise_log
	, .exec_token				= (uint64_t[]) { L_LWEXEC }
	, .exec_would				= listwise_would
	, .exec_log					= listwise_log
	, .opinfo_token			= (uint64_t[]) { L_LWOPINFO }
	, .opinfo_would			= listwise_would
	, .opinfo_log				= listwise_log
#if DEVEL
	, .tokens_token			= (uint64_t[]) { L_LWTOKEN }
	, .tokens_would			= listwise_would
	, .tokens_log				= listwise_log
	, .states_token			= (uint64_t[]) { L_LWSTATE }
	, .states_would			= listwise_would
	, .states_log				= listwise_log
	, .sanity_token			= (uint64_t[]) { L_LWSANITY }
	, .sanity_would			= listwise_would
	, .sanity_log				= listwise_log
#endif
}};

int main(int g_argc, char** g_argv)
{
	char space[4096];
	size_t tracesz = 0;

	pstring * args_remnant = 0;	// concatenated unprocessed arguments
	pstring * temp = 0;

	generator* g = 0;						// generator
	lwx * lx = 0;								// list stack

	// initialize logger - prepare g_argc/g_argv
	fatal(log_init);

	// parse cmdline arguments
	fatal(args_parse, &args_remnant);

	// configure logger
#if DEVEL
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
	{
		fatal(log_config_and_describe
			, L_LOGGER | L_LWTOKEN | L_LWSTATE | L_LWOPINFO		// prefix
			, L_LWOPINFO | L_LWTOKEN | L_LWSTATE							// trace
			, L_LOGGER																				// describe bits
		);
	}
	else
	{
		fatal(log_config_and_describe
			, L_LOGGER | L_LWTOKEN | L_LWSTATE | L_LWOPINFO
			, 0
			, L_LOGGER
		);
	}
#else
	fatal(log_config, L_LWOPINFO);		// prefix
#endif

	// setup liblistwise logging
	listwise_logging_configure(logging);

	if(g_args.generator_file)
	{
		// read transform-file
		fatal(snarf, g_args.generator_file, &temp);
	}

	if(args_remnant && args_remnant->l)
	{
		// transform-string from argv
		fatal(pscatw, &temp, args_remnant->s, args_remnant->l);
	}

	if(temp)
	{
#if DEVEL
		fatal(generator_parse2, 0, temp->s, temp->l, &g, 0);
#else
		fatal(generator_parse, 0, temp->s, temp->l, &g);
#endif
	}

	// attempt to read initial list items from stdin and a specified file
	int x;
	for(x = 0; x < 2; x++)
	{
		char * p = 0;
		if(x == 0 && (g_args.generator_file == 0 || strcmp(g_args.generator_file, "-") == 0 || strcmp(g_args.generator_file, "/dev/fd/0") == 0))
			p = "-";
		if(x == 1)
			p = g_args.init_file;

		if(p)
		{
			// read in the specified file
			fatal(snarf, p, &temp);

			char * s[2] = { temp->s, 0 };
			while((s[1] = memchr(s[0], '\n', temp->l - (s[0] - ((char*)temp->s)))))
			{
				if(s[1] - s[0])
				{
					if(g_args.init_listl == g_args.init_lista)
					{
						int ns = g_args.init_lista ?: 10;
						ns = ns * 2 + ns / 2;

						fatal(xrealloc, &g_args.init_list, sizeof(*g_args.init_list), ns, g_args.init_lista);
						fatal(xrealloc, &g_args.init_list_lens, sizeof(*g_args.init_list_lens), ns, g_args.init_lista);
						g_args.init_lista = ns;
					}
					fatal(ixstrndup, &g_args.init_list[g_args.init_listl], s[0], s[1] - s[0]);
					g_args.init_list_lens[g_args.init_listl] = s[1] - s[0];
					g_args.init_listl++;
				}
				s[0] = s[1] + 1;
			}
		}
	}

	fatal(lwx_alloc, &lx);

	for(x = 0; x < g_args.init_listl; x++)
	{
		fatal(lstack_writew, lx, 0, x, g_args.init_list[x], g_args.init_list_lens[x]);
	}

	if(g)
	{
		fatal(listwise_exec_generator2, g, 0, 0, 0, &lx, 0);
	}

	// OUTPUT
	int i = 0;
	int y = 0;
	for(y = 0; lx && y < LSTACK_LISTS(lx); y++)
	{
		if(y)
		{
			if(g_args.out_null)
				printf("%c", 0);
			else
				printf("\n");

			i = 0;
		}

		int x;
		LSTACK_ITERATE_LIST(lx, y, x, go)
		{
			if(go)
			{
				int spacel = 0;
				space[0] = 0;

				// numbering
				if(g_args.numbering)
				{
					int j = i++;
					if(g_args.numbering == 2)
						j = x;

					if(g_args.out_stack)
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
				if(g_args.out_null)
					delim[0] = '\0';
				else
					delim[0] = '\n';

				int __attribute__((unused)) rr = writev(1
					, (struct iovec[]) {
						  { .iov_base = space	, .iov_len = spacel }
						, { .iov_base = ss		, .iov_len = ssl }
						, { .iov_base = delim	, .iov_len = 1 }
					}, 3
				);
			}
		}
		LSTACK_ITEREND

		if(!g_args.out_stack)
			break;
	}

finally:
	lwx_free(lx);
	generator_free(g);
	args_teardown();
	psfree(temp);
	psfree(args_remnant);

	if(XAPI_UNWINDING)
	{
#if DEVEL
		if(g_args.mode_backtrace == MODE_BACKTRACE_PITHY)
		{
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEVEL
		}
		else
		{
			tracesz = xapi_trace_full(space, sizeof(space));
		}
#endif

		logw(L_RED, space, tracesz);
	}

	log_teardown();
coda;
}
