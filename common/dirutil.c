#include <stdio.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dirutil.h"

#include "log.h"
#include "control.h"

int rmdir_recursive(const char * const dirpath, int rmself)
{
	int fn(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		if(typeflag == FTW_F)
		{
			if(unlink(fpath) != 0)
			{
				fail_log("unlink(%s)=[%d][%s]", fpath, errno, strerror(errno));
				return FTW_STOP;
			}
			return FTW_CONTINUE;
		}
		else if(typeflag == FTW_DP)
		{
			if(ftwbuf->level > 0 || rmself)
			{
				if(rmdir(fpath) != 0)
				{
					fail_log("rmdir(%s)=[%d][%s]", fpath, errno, strerror(errno));
					return FTW_STOP;
				}
			}
			return FTW_CONTINUE;
		}

printf("unknown %s\n", fpath);
		return FTW_STOP;
	};
	
	// depth-first
	return nftw(dirpath, fn, 32, FTW_DEPTH) == 0;
}

int mkdirp(const char * const path, mode_t mode)
{
	char space[256];

	const char * t = path;
	const char * e = path + strlen(path);

	while(t != e)
	{
		t++;
		while(*t != '/' && t != e)
			t++;

		memcpy(space, path, t - path);
		space[t - path] = 0;

		if(mkdir(space, mode) == -1 && errno != EEXIST)
			fail("mkdir(%s)=%s", space, strerror(errno));
	}

	return 1;
}
