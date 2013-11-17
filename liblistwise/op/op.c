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

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include <pcre.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "liblistwise_control.h"
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

static void __attribute__((constructor)) init()
{
	listwise_register_opdir(XQUOTE(LWOPDIR));	/* /usr/lib/listwise */
}

static void __attribute__((destructor)) teardown()
{
	int x;
	for(x = 0; x < g_dls_l; x++)
	{
		dlclose(g_dls[x]);
	}

	free(g_dls);
	free(g_ops);
}

static int op_load(char* path)
{
	operator* op = 0;

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
	if((dl = dlopen(path, RTLD_NOW | RTLD_GLOBAL)) == 0)
	{
		dprintf(listwise_warn_fd, "FAILED TO LOAD: %s [%s]\n", path, dlerror());
	}
	else if((op = dlsym(dl, "op_desc")) == 0)
	{
		dprintf(listwise_warn_fd, "FAILED TO LOAD: %s [%s]\n", path, dlerror());
	}
	else
	{
		g_dls[g_dls_l++] = dl;

		while(op->desc)
		{
			if(g_ops_a == g_ops_l)
			{
				int n = g_ops_a ? g_ops_a * 2 + g_ops_a / 2 : 10;
				fatal(xrealloc, &g_ops, sizeof(g_ops[0]), n, g_ops_a);
				g_ops_a = n;
			}

			op->sl = strlen(op->s);
			g_ops[g_ops_l++] = op++;
		}
	}

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

//
// public
//

static int read_opdir(char * s)
{
	char space[256];
	snprintf(space, sizeof(space) - 1, "%s", s);

  DIR * dd = 0;
  if((dd = opendir(space)))
  {
		space[strlen(space)] = '/';

    struct dirent ent;
    struct dirent * entp = 0;
    int r = 0;
    while(1)
    {
      if((r = readdir_r(dd, &ent, &entp)) == 0)
      {
        if(entp)
        {
          if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
          {
						if(entp->d_type == DT_DIR)
						{
							snprintf(space + strlen(s) + 1, sizeof(space) - strlen(s) - 1, "%s", entp->d_name);
							fatal(read_opdir, space);
						}
						else if(strlen(entp->d_name) > 3 && strcmp(entp->d_name + strlen(entp->d_name) - 3, ".so") == 0)
						{
							snprintf(space + strlen(s) + 1, sizeof(space) - strlen(s) - 1, "%s", entp->d_name);
							fatal(op_load, space);
						}
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        fail("readdir(%s)=[%d][%s]", s, r, strerror(r));
      }
    }
  }
  else
  {
    fail("opendir(%s)=[%d][%s]\n", s, errno, strerror(errno));
  }

finally:
	if(dd)
	  closedir(dd);
coda;
}

int API listwise_register_opdir(char * dir)
{
	fatal(read_opdir, dir);

	op_sort();

	finally : coda;
}

operator* op_lookup(char* s, int l)
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
