#include <stdio.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tmp.h"
#include "args.h"

#include "control.h"

//
// static
//

static int fn(const char * fp, const struct stat * sb, int tf, struct FTW * ftw)
{
	if(tf == FTW_F && ftw->level > 0)
	{
		if(unlink(fp) != 0)
			return FTW_STOP;
		return FTW_CONTINUE;
	}
	else if(tf == FTW_DP && ftw->level > 0)
	{
		if(rmdir(fp) != 0)
			return FTW_STOP;
		return FTW_CONTINUE;
	}
	else if(ftw->level == 0)
	{
		return FTW_CONTINUE;
	}

	return FTW_STOP;
}

static int mkdirp(char * path, mode_t mode)
{
	char space[256];

	char * t = path;
	char * e = path + strlen(path);

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

//
// public
//

int tmp_setup()
{
	// temporarily assume fabsys identity
	fatal(identity_assume_fabsys);

	// ensure execdir exists
	fatal(mkdirp, g_args.execdir_base, S_IRWXU | S_IRWXG | S_IRWXO);

	// delete everything in it
	fatal_os(nftw, g_args.execdir_base, fn, 32, FTW_DEPTH);

	// create execdir specific to this pid
	fatal(mkdirp, g_args.execdir, S_IRWXU | S_IRWXG | S_IRWXO);

	// ensure hashdir exists
	fatal(mkdirp, g_args.hashdir, S_IRWXU | S_IRWXG | S_IRWXO);

	// resume user identity
	fatal(identity_assume_user);

	return 1;
}
