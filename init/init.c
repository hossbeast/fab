#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "control.h"

/// initialize
//
// load operators from disk
//
static int init()
{
	char space[256];
	char* dir = "/usr/lib/listwise";

	snprintf(space, sizeof(space), "%s", dir);

	DIR* dd = 0;
	if((dd = opendir(space)) == 0)
		return;

	space[strlen(dir)] = '/';

	struct dirent* tmp = alloca(sizeof(*tmp));

	fatal_os(readdir_r, dd, tmp, &tmp);

	while(tmp)
	{
		snprintf(space + strlen(dir) + 1, sizeof(space) - strlen(dir) - 1, "%s", tmp->d_name);

		void* symbols = 0;
		operator* op = 0;
		if(strcmp(tmp->d_name, ".") == 0)
		{
			// dot
		}
		else if(strcmp(tmp->d_name, "..") == 0)
		{
			// dot-dot
		}
		else
		{
			fatal(op_load, space);
		}

		fatal_os(readdir_r, dd, tmp, &tmp);
	}

	closedir(dd);

	op_sort();

	finally : coda;
}

void __attribute__((constructor)) list_initialize()
{
	init();
	identity_init();
}

void __attribute__((destructor)) list_teardown()
{
	op_teardown();
	object_teardown();
	identity_teardown();
}
