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
#include "listwise/transform.h"
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
#if DEBUG || DEVEL
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
//  mem  - append contents here
//  bang - true if this file is an interpreter script
//
static int snarf(char * path, int bang, pstring ** mem)
{
	int fd = -1;
	struct stat st;

	if(strcmp(path, "-") == 0)
		path = "/dev/fd/0";

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
		.transform_token	= (uint64_t[]) { L_PARSE }
	, .transform_would	= listwise_would
	, .transform_log		= listwise_log
	, .lstack_token			= (uint64_t[]) { L_EXEC }
	, .lstack_would			= listwise_would
	, .lstack_log				= listwise_log
	, .exec_token				= (uint64_t[]) { L_EXEC }
	, .exec_would				= listwise_would
	, .exec_log					= listwise_log
	, .opinfo_token			= (uint64_t[]) { L_OPINFO }
	, .opinfo_would			= listwise_would
	, .opinfo_log				= listwise_log
#if DEBUG || DEVEL
	, .tokens_token			= (uint64_t[]) { L_TOKENS }
	, .tokens_would			= listwise_would
	, .tokens_log				= listwise_log
	, .states_token			= (uint64_t[]) { L_STATES }
	, .states_would			= listwise_would
	, .states_log				= listwise_log
#endif
#if SANITY
	, .sanity_token			= (uint64_t[]) { L_SANITY }
	, .sanity_would			= listwise_would
	, .sanity_log				= listwise_log
#endif
}};

int main(int argc, char ** argv)
{
	char space[4096];
	size_t tracesz = 0;

	pstring * temp = 0;
	pstring *	trans = 0;

	transform* g = 0;						// transform
	lwx * lx = 0;								// list stack

	// initialize logger - prepare g_argc/g_argv
	fatal(log_init);

	// parse cmdline arguments
	fatal(args_parse);

	// configure logger
#if DEBUG || DEVEL
	if(g_args.mode_logtrace == MODE_LOGTRACE_FULL)
	{
		fatal(log_config_and_describe
			, L_TOKENS | L_STATES | L_OPINFO		// prefix
#if DEVEL
			| L_LOGGER
#endif
			, L_OPINFO | L_TOKENS | L_STATES		// trace
			, 0																// describe bits
#if DEVEL
			| L_LOGGER
#endif
		);
	}
	else
	{
		fatal(log_config_and_describe
			, L_TOKENS | L_STATES | L_OPINFO
#if DEVEL
			| L_LOGGER
#endif
			, 0
			, 0
#if DEVEL
			| L_LOGGER
#endif
		);
	}
#else
	fatal(log_config, L_OPINFO);		// prefix
#endif

	// setup liblistwise logging
	listwise_logging_configure(logging);

	// allocate lstack
	fatal(lwx_alloc, &lx);

	// read transform-expr from stdin
	if(g_args.stdin_init_list_items)
	{
		fatal(psclear, &temp);
		fatal(snarf, "-", 0, &temp);

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
			fatal(psclear, &temp);
			fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, &temp);

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
				fatal(pscats, &trans, " ");

			fatal(pscats, &trans, g_args.inputs[x].s);
		}
		else if(g_args.inputs[x].type == INPUT_TYPE_TRANSFORM_FILE)
		{
			if(trans && trans->l)
				fatal(pscats, &trans, " ");
			
			fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, &trans);
		}
		else if(g_args.inputs[x].type == INPUT_TYPE_HYBRID_FILE)
		{
			fatal(psclear, &temp);
			fatal(snarf, g_args.inputs[x].s, g_args.inputs[x].bang, &temp);

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
							fatal(pscats, &trans, " ");

						fatal(pscatw, &trans, s[0], s[1] - s[0]);
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

	if(trans)
	{
#if DEBUG || DEVEL
		fatal(transform_parse2, 0, trans->s, trans->l, &g, 0);
#else
		fatal(transform_parse, 0, trans->s, trans->l, &g);
#endif
	}

	// execute
	if(g)
	{
		fatal(listwise_exec_transform2, g, 0, 0, 0, &lx, 0);
	}

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
						  { .iov_base = space	, .iov_len = spacel }
						, { .iov_base = ss		, .iov_len = ssl }
						, { .iov_base = delim	, .iov_len = 1 }
					}, 3
				);
			}
		}
		LSTACK_ITEREND

		if(g_args.mode_output != MODE_OUTPUT_STACK)
			break;
	}

finally:
	lwx_free(lx);
	transform_free(g);
	args_teardown();
	psfree(temp);
	psfree(trans);

	if(XAPI_UNWINDING)
	{
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

		logw(L_RED, space, tracesz);
	}

	log_teardown();
coda;
}
