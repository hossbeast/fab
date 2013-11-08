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

#include "listwise.h"
#include "listwise/xtra.h"
#include "listwise/operator.h"
#include "listwise/ops.h"
#include "listwise/generator.h"
#include "listwise/object.h"

#include "listwise_control.h"

#include "args.h"

#include "xmem.h"

/// snarf
//
// SUMMARY
//  read a file into memory
//
// PARAMETERS
//  path - path to file to load, or "-" for stdin
//  mem  - memory containing the loaded file
//  sz   - size of mem
//
static int snarf(char * path, void ** mem, size_t * sz)
{
	int fd = -1;

	xfree(mem);

	if((fd = open(strcmp(path, "-") == 0 ? "/dev/fd/0" : path, O_RDONLY)) == -1)
		fail("open(%s)=[%d][%s]", path, errno, strerror(errno));

	struct stat st;
	if(fstat(fd, &st) == -1)
		fail("fstat(%d)=[%d][%s]", fd, errno, strerror(errno));

	if(S_ISFIFO(st.st_mode) || S_ISREG(st.st_mode))
	{
		char blk[512];
		int r = 1;
		while(r > 0)
		{
			if((r = read(fd, blk, sizeof(blk) / sizeof(blk[0]))) == -1)
				fail("read(%d)=[%d][%s]", fd, errno, strerror(errno));

			else if(r > 0)
			{
				fatal(xrealloc, mem, 1, (*sz) + r + 1, (*sz));
				memcpy(((char*)(*mem)) + (*sz), blk, r);
				(*sz) += r;
			}
		}
	}
	else if(S_ISCHR(st.st_mode))
	{
		// dev/null
	}
	else
	{
		fail("unable to process file of type %s (%d)"
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
	if(fd != -1)
		fatal_os(close, fd);
coda;
}

int main(int argc, char** argv)
{
	generator_parser* p = 0;		// generator parser
	generator* g = 0;						// generator
	lwx * lx = 0;								// list stack
	int nullfd = -1;						// fd to /dev/null

	void * mem = 0;
	size_t sz = 0;

	// parse cmdline arguments
	int genx = 0;
	fatal(parse_args, argc, argv, &genx);

	int x;

	if(g_args.dump)
	{
		listwise_errors_unwind = 1;
	}
	else
	{
		// arrange for liblistwise to write to /dev/null
		if((nullfd = open("/dev/null", O_WRONLY)) == -1)
			fail("open(/dev/null)=[%d][%s]", errno, strerror(errno));

		listwise_info_fd = nullfd;
		listwise_warn_fd = nullfd;
	}

	// create generator parser
	fatal(generator_mkparser, &p);

	if(g_args.generator_file)
	{
		// read generator-string from file
		fatal(snarf, g_args.generator_file, &mem, &sz);

		// parse generator-string from file
		fatal(generator_parse, p, mem, sz, &g);
	}
	else if(genx < argc)
	{
		// concatenate remaining arguments with spaces
		for(x = genx; x < (argc - 1); x++)
			argv[x][strlen(argv[x])] = ' ';

		// parse generator-string from argv
		fatal(generator_parse, p, argv[genx], 0, &g);
	}

	// attempt to read initial list items from stdin and a specified file
	for(x = 0; x < 2; x++)
	{
		char * p = 0;
		if(x == 0 && (g_args.generator_file == 0 || (strcmp(g_args.generator_file, "-") && strcmp(g_args.generator_file, "/dev/fd/0"))))
			p = "-";
		if(x == 1)
			p = g_args.init_file;

		if(p)
		{
			// read list items from stdin
			fatal(snarf, p, &mem, &sz);

			char * s[2] = { mem, 0 };
			while((s[1] = memchr(s[0], '\n', sz - (s[0] - ((char*)mem)))))
			{
				if(s[1] - s[0])
				{
					if(g_args.init_listl == g_args.init_lista)
					{
						int ns = g_args.init_lista ?: 10;
						ns = ns * 2 + ns / 2;

						g_args.init_list = realloc(g_args.init_list, sizeof(*g_args.init_list) * ns);
						g_args.init_list_lens = realloc(g_args.init_list_lens, sizeof(*g_args.init_list_lens) * ns);
						g_args.init_lista = ns;
					}
					g_args.init_list[g_args.init_listl] = strndup(s[0], s[1] - s[0]);
					g_args.init_list_lens[g_args.init_listl] = s[1] - s[0];
					g_args.init_listl++;
				}
				s[0] = s[1] + 1;
			}
		}
	}

	if(g_args.dump)
	{
		if(g)
		{
			printf("GENERATOR: \n");
			generator_dump(g);
		}
	}

	if(g)
	{
		// execute 
		fatal(listwise_exec_generator, g, g_args.init_list, g_args.init_list_lens, g_args.init_listl, &lx, g_args.dump);
	}
	else
	{
		// listwise_exec with some kind of identity generator
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
				if(g_args.number)
				{
					int j = i++;
					if(g_args.number == 2)
						j = x;

					if(g_args.out_stack)
						printf("%3d %3d ", y, j);
					else
						printf("%3d ", j);
				}

				char * ss = 0;
				int    ssl = 0;
				fatal(lstack_getstring, lx, y, x, &ss, &ssl);

				if(fwrite(ss, 1, ssl, stdout) == -1)
					fail("write()=[%d][%s]", errno, strerror(errno));

				if(g_args.out_null)
					printf("%c", 0);
				else
					printf("\n");
			}
		}
		LSTACK_ITEREND

		if(!g_args.out_stack)
			break;
	}

finally:
	free(mem);

	if(nullfd != -1)
		close(nullfd);

	lwx_free(lx);
	generator_free(g);
	generator_parser_free(p);
	args_teardown();
coda;
}
