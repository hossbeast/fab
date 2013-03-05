#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include <pcre.h>

#include "listwise/internal.h"
#include "re.h"

#include "xmem.h"
#include "control.h"

operator **	APIDATA g_ops;
int									g_ops_a;
int					APIDATA g_ops_l;

int op_load(char* path)
{
	void* sym = 0;
	operator* op = 0;

	char* name = path + strlen(path) - 1;
	int namel = 0;
	while(name[0] != '/')
		name--;
	name++;

	while(name[namel] != '.')
		namel++;

	if((sym = dlopen(path, RTLD_NOW | RTLD_GLOBAL)) == 0)
	{
		dprintf(listwise_err_fd, "FAILED TO LOAD: %s [%s]\n", path, dlerror());
/* I guess this segfaults ...
		dlclose(sym);
*/
	}
	else if((op = dlsym(sym, "op_desc")) == 0)
	{
		dprintf(listwise_err_fd, "FAILED TO LOAD: %s\n", path);
		dlclose(sym);
	}
	else
	{
#if 0
		dprintf(listwise_err_fd, "-> LOADED : %s\n", path);
#endif

		if(g_ops_a == g_ops_l)
		{
			int n = g_ops_a ? g_ops_a * 2 + g_ops_a / 2 : 10;
			fatal(xrealloc, &g_ops, sizeof(g_ops[0]), n, g_ops_a);
			g_ops_a = n;
		}

		op->sl = namel;
		memcpy(op->s, name, op->sl);

		op->handle = sym;

		g_ops[g_ops_l++] = op;
	}

	finally : coda;
}

void op_sort()
{
	int op_compare(const operator** A, const operator** B)
	{
		return strcmp((*A)->s, (*B)->s);
	};

	qsort(g_ops, g_ops_l, sizeof(g_ops[0]), (void*)op_compare);
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

	operator** r = bsearch(0, g_ops, g_ops_l, sizeof(g_ops[0]), (void*)op_compare);

	if(r)
		return *r;

	return 0;
}

void op_teardown()
{
	int x;
	for(x = 0; x < g_ops_l; x++)
		dlclose(g_ops[x]->handle);

	free(g_ops);
}
