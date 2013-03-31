/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */


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

#include <listwise.h>
#include <listwise/operator.h>
#include <listwise/ops.h>
#include <listwise/generator.h>
#include <listwise/lstack.h>
#include <listwise/object.h>

// not part of the actual API
extern int lstack_exec_internal(generator* g, char** init, int* initls, int initl, lstack** ls, int dump);

#include "args.h"

#define FAIL(x, ...) { printf(x "\n", ##__VA_ARGS__); return 1; }

static int snarf(char * path, int * fd, void ** mem, size_t * sz, void ** mmem)
{
	if(((*fd) = open(strcmp(path, "-") == 0 ? "/dev/fd/0" : path, O_RDONLY)) == -1)
		FAIL("open(%s)=[%d][%s]", path, errno, strerror(errno));

	struct stat st;
	if(fstat((*fd), &st) == -1)
		FAIL("fstat(%d)=[%d][%s]", (*fd), errno, strerror(errno));

	if(S_ISREG(st.st_mode))
	{
		(*sz) = st.st_size;

		if(((*mem) = mmap(0, (*sz), PROT_READ, MAP_PRIVATE, (*fd), 0)) == MAP_FAILED)
			FAIL("mmap(%d)=[%d][%s]", (*fd), errno, strerror(errno));

		(*mmem) = (*mem);
	}
	else if(S_ISFIFO(st.st_mode))
	{
		char blk[512];
		int r = 0;
		while((r = read((*fd), blk, sizeof(blk) / sizeof(blk[0]))) > 0)
		{
			(*mem) = realloc((*mem), (*sz) + r + 1);
			memcpy(((char*)(*mem)) + (*sz), blk, r);
			(*sz) += r;
		}
		((char*)(*mem))[(*sz)] = 0;
	}
 
	return 1;
}

int main(int argc, char** argv)
{
	generator_parser* p = 0;		// generator parser
	generator* g = 0;						// generator
	lstack* ls = 0;							// list stack

	int fd = 0;				// file
	void * mem = 0;
	void * mmem = 0;
	size_t sz = 0;

	if(generator_mkparser(&p) == 0)
		FAIL("mkparser failed");

	if((ls = calloc(1, sizeof(*ls))) == 0)
		FAIL("calloc(%zu)=[%d][%s]", sizeof(*ls), errno, strerror(errno));

	// parse cmdline arguments
	int x = parse_args(argc, argv);

	// arrange for liblistwise to write errors to /dev/null
	if(!g_args.dump)
	{
		int efd = open("/dev/null", O_WRONLY);
		dup2(efd, listwise_err_fd);
	}

	if(g_args.generator_file)
	{
		// read generator-string from file

		if(snarf(g_args.generator_file, &fd, &mem, &sz, &mmem) == 0)
			return 1;

		if(generator_parse(p, mem, sz, &g) == 0)
			FAIL("parse failed");
	}
	else
	{
		if(x < argc)
		{
			if(generator_parse(p, argv[x], 0, &g) == 0)
				FAIL("parse failed");
		}

		// read generator-string from argv, read list items from stdin
		if(snarf("/dev/fd/0", &fd, &mem, &sz, &mmem) == 0)
			return 1;

		char * s[2] = { mem, 0 };
		while((s[1] = memchr(s[0], '\n', sz - (s[0] - ((char*)mem)))))
		{
			if(s[1] - s[0])
			{
				if(lstack_add(ls, s[0], s[1] - s[0]) == 0)
					FAIL("lstack_add failed");
			}
			s[0] = s[1] + 1;
		}
	}

	if(fd)
		close(fd);

	if(mmem)
		munmap(mmem, sz);

	if(g)
	{
		if(g_args.dump)
		{
			printf("GENERATOR: \n");
			generator_dump(g);
		}

		// execute 
		if(lstack_exec_internal(g, 0, 0, 0, &ls, g_args.dump) == 0)
			FAIL("lstack_exec failed");

		// OUTPUT
		int i = 0;
		int y = 0;
		for(y = 0; y < ls->l; y++)
		{
			if(y)
			{
				if(g_args.out_null)
					printf("%c", 0);
				else
					printf("\n");

				i = 0;
			}

			for(x = 0; x < ls->s[y].l; x++)
			{
				int go = 1;
				if(!g_args.out_list && !ls->sel.all)
				{
					go = 0;
					if(ls->sel.sl > (x/8))
					{
						go = ls->sel.s[x/8] & (0x01 << (x%8));
					}
				}

				if(go)
				{
					if(g_args.number)
					{
						if(g_args.out_stack)
							printf("%3d %3d ", y, i++);
						else
							printf("%3d ", i++);
					}

					char * ss = 0;
					int    ssl = 0;
					lstack_string(ls, y, x, &ss, &ssl);
					printf("%.*s", ssl, ss);

					if(g_args.out_null)
						printf("%c", 0);
					else
						printf("\n");
				}
			}

			if(!g_args.out_stack)
				break;
		}
	}

	lstack_xfree(&ls);
	generator_xfree(&g);
	generator_parser_xfree(&p);
	args_teardown();

	return 0;
}
