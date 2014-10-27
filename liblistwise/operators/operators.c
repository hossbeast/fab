/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "internal.h"

#include "xlinux.h"
#include "macros.h"

operator **	APIDATA g_ops;
int									g_ops_a;
int					APIDATA g_ops_l;

void **	g_dls;
int			g_dls_a;
int			g_dls_l;

//
// static
//

#define DEBUG_LWOP_LOAD 0

static int op_load(char* path)
{
	operator* op = 0;

#if DEBUG_LWOP_LOAD
dprintf(501, " +%s :", path);
#endif

	char* name = path + strlen(path) - 1;
	int namel = 0;
	while(name[0] != '/')
		name--;
	name++;

	while(name[namel] != '.')
		namel++;

	if(g_dls_a == g_dls_l)
	{
		int n = g_dls_a ? g_dls_a * 2 + g_dls_a / 2 : 10;
		fatal(xrealloc, &g_dls, sizeof(*g_dls), n, g_dls_a);
		g_dls_a = n;
	}

	void * dl = 0;
	fatal(xdlopen, path, RTLD_NOW | RTLD_GLOBAL, &dl);
	fatal(xdlsym, dl, "op_desc", (void*)&op);
	g_dls[g_dls_l++] = dl;

	while(op->desc)
	{
		if(g_ops_a == g_ops_l)
		{
			int ns = g_ops_a ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &g_ops, sizeof(g_ops[0]), ns, g_ops_a);
			g_ops_a = ns;
		}

#if DEBUG_LWOP_LOAD
dprintf(501, " %s", op->s);
#endif
		op->sl = strlen(op->s);
		g_ops[g_ops_l++] = op++;
	}

#if DEBUG_LWOP_LOAD
dprintf(501, "\n");
#endif

	finally : coda;
}

static void op_sort()
{
	int op_compare(const operator** A, const operator** B)
	{
		return strcmp((*A)->s, (*B)->s);
	};

	qsort(g_ops, g_ops_l, sizeof(g_ops[0]), (void*)op_compare);
}

static int read_opdir(char * s)
{
	char space[256];
	snprintf(space, sizeof(space) - 1, "%s", s);

  DIR * dd = 0;
	fatal(xopendir, space, &dd);

	space[strlen(space)] = '/';

	struct dirent ent;
	struct dirent * entp = 0;
	while(1)
	{
		fatal(xreaddir_r, dd, &ent, &entp);

		if(entp)
		{
			if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
			{
				if(entp->d_type == DT_DIR)
				{
					snprintf(space + strlen(s) + 1, sizeof(space) - strlen(s) - 1, "%s", entp->d_name);
					fatal(read_opdir, space);
				}
//			else if(strlen(entp->d_name) > 3 && strcmp(entp->d_name + strlen(entp->d_name) - 3, ".so") == 0)
				else if(strlen(entp->d_name) > 2)
				{
					// locate the filename portion
					char * f = entp->d_name + strlen(entp->d_name) - 1;
					while(f != entp->d_name && *f != '/')
						f--;

					if(*f == '/')
						f++;

					// locate the extension portion
					while(*f != '.')
						f++;

					if(*f == '.')
						f++;

#ifndef LWOPEXT
#define LWOPEXT so
#endif
					if(strcmp(f, XQUOTE(LWOPEXT)) == 0)
					{
						snprintf(space + strlen(s) + 1, sizeof(space) - strlen(s) - 1, "%s", entp->d_name);
						fatal(op_load, space);
					}
					else
					{
#if DEBUG_LWOP_LOAD
dprintf(501, " -%s : %s cmp %s\n", entp->d_name, f, QUOTE(LWOPEXT));
#endif
					}
				}
			}
		}
		else
		{
			break;
		}
	}

finally:
	if(dd)
	  closedir(dd);
coda;
}

//
// API
//

typedef operator * opstar;
opstar API op_lookup(char* s, int l)
{
	int op_compare(const void* __attribute__((unused)) K, const operator** B)
	{
		int x;
		for(x = 0; x < l && x < (*B)->sl; x++)
		{
			if(s[x] - (*B)->s[x])
				return s[x] - (*B)->s[x];
		}

		if(l > (*B)->sl)
			return 1;
		if(l < (*B)->sl)
			return -1;

		return 0;
	};

	operator** r = bsearch((void*)1, g_ops, g_ops_l, sizeof(g_ops[0]), (void*)op_compare);

	if(r)
		return *r;

	return 0;
}

int API listwise_register_opdir(char * dir)
{
	fatal(read_opdir, dir);

	op_sort();

finally:
	XAPI_INFOF("dir", "'%s'", dir);
coda;
}

int API __attribute__((constructor)) listwise_operators_setup()
{
	prologue;

	fatal(listwise_register_opdir, XQUOTE(LWOPDIR));	/* /usr/lib/listwise */

	finally : coda;
}

void API __attribute__((destructor)) listwise_operators_teardown()
{
	int x;
	for(x = 0; x < g_dls_l; x++)
		dlclose(g_dls[x]);

	free(g_dls);
	free(g_ops);
}
