#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tmp.h"
#include "args.h"
#include "identity.h"

#include "log.h"
#include "control.h"
#include "dirutil.h"
#include "macros.h"

/// minmodify
//
// get the minimum modify time of all files and directories in the specified directory
//
// *minmod should have been initialized to the modify time of the directory itself
//
// symbolic links are not followed
//
static int minmodify(const char* dirpath, time_t * minmod)
{
	int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		(*minmod) = MIN((*minmod), sb->st_mtime);
		return FTW_CONTINUE;
	};

	return nftw(dirpath, fn, 32, FTW_PHYS) == 0;
}

//
// public
//

int tmp_setup()
{
	char space[512];

	// temporarily assume fabsys identity
	fatal(identity_assume_fabsys);

	//
	// SID_GN_DIR - delete directories where the session has terminated
	//
	fatal(mkdirp, SID_DIR_BASE, S_IRWXU | S_IRWXG | S_IRWXO);
	{
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(typeflag != FTW_D)
			{
				// something other than a directory
				log(L_WARN, "unexpected file %s", fpath);
				return FTW_CONTINUE;
			}
			else if(ftwbuf->level == 0)
			{
				return FTW_CONTINUE;
			}

			pid_t sid = 0;
			int n = 0;
			if(sscanf(fpath + ftwbuf->base, "%d%n", &sid, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
			{
				// dirname consists of something other than <sid>, or the sid leader is unkillable
				log(L_WARN, "unexpected file %s", fpath);
				return FTW_CONTINUE;
			}

			// sid leader is unkillable
			if(kill(sid, 0))
			{
				if(rmdir_recursive(fpath, 1) == 0)
					return FTW_STOP;
			}

			// process only toplevel files
			return FTW_SKIP_SUBTREE;
		};

		fatal_os(nftw, SID_DIR_BASE, fn, 32, FTW_ACTIONRETVAL);
	}

	// ensure directories for my own sid exists
	snprintf(space, sizeof(space), "%s/%d/gn", SID_DIR_BASE, g_args.sid);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO);

	//
	// PID_FML_DIR
	//
	fatal(mkdirp, PID_DIR_BASE, S_IRWXU | S_IRWXG | S_IRWXO); 
	{
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(typeflag != FTW_D)
			{
				// something other than a directory
				log(L_WARN, "unexepected file %s", fpath);
				return FTW_CONTINUE;
			}
			else if(ftwbuf->level == 0)
			{
				return FTW_CONTINUE;
			}

			pid_t pid = 0;
			int n = 0;
			if(sscanf(fpath + ftwbuf->base, "%d%n", &pid, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
			{
				// dirname consists of something other than <pid>
				log(L_WARN, "unexepected file %s", fpath);
				return FTW_CONTINUE;
			}

			// pid is myself, or it is unkillable
			if(pid == g_args.pid || kill(pid, 0))
			{
				if(rmdir_recursive(fpath, 1) == 0)
					return FTW_STOP;
			}

			// process only toplevel files
			return FTW_SKIP_SUBTREE;
		};

		fatal_os(nftw, PID_DIR_BASE, fn, 32, FTW_ACTIONRETVAL);
	}

	// ensure directories for my own pid exist
	snprintf(space, sizeof(space), "%s/%d/fml", PID_DIR_BASE, g_args.pid);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO);

	//
	// FF_DIR/REGULAR
	//
	snprintf(space, sizeof(space), "%s/REGULAR", FF_DIR_BASE);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO); 
	{
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(typeflag != FTW_D)
			{
				// something other than a directory
				log(L_WARN, "unexepected file %s", fpath);
				return FTW_CONTINUE;
			}
			else if(ftwbuf->level == 0)
			{
				return FTW_CONTINUE;
			}

			uint32_t ffhash = 0;
			int n = 0;
			if(sscanf(fpath + ftwbuf->base, "%u%n", &ffhash, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
			{
				// dirname consists of something other than <ffhash>
				log(L_WARN, "unexpected file %s", fpath);
				return FTW_CONTINUE;
			}

			// get the min modify time of everything in the directory
			time_t minmod = sb->st_mtime;
			if(minmodify(fpath, &minmod) == 0)
				return FTW_STOP;

			// minimum modify time older than expiration
			if((time(0) - minmod) > EXPIRATION_POLICY)
			{
				if(rmdir_recursive(fpath, 1) == 0)
					return FTW_STOP;
			}

			// process only toplevel files
			return FTW_SKIP_SUBTREE;
		};

		fatal_os(nftw, space, fn, 32, FTW_ACTIONRETVAL);
	}

	//
	// GN_DIR/PRIMARY
	//
	snprintf(space, sizeof(space), "%s/PRIMARY", GN_DIR_BASE);
	fatal(mkdirp, space, S_IRWXU | S_IRWXG | S_IRWXO);
	{
		int fn(const char* fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
		{
			if(typeflag != FTW_D)
			{
				// something other than a directory
				log(L_WARN, "unexepected file %s", fpath);
				return FTW_CONTINUE;
			}
			else if(ftwbuf->level == 0)
			{
				return FTW_CONTINUE;
			}

			uint32_t gnhash = 0;
			int n = 0;
			if(sscanf(fpath + ftwbuf->base, "%u%n", &gnhash, &n) != 1 || (ftwbuf->base + n) != strlen(fpath))
			{
				log(L_WARN, "unexepected file %s", fpath);
				return FTW_CONTINUE;
			}

			// get the min modify time of everything in the directory
			time_t minmod = sb->st_mtime;
			if(minmodify(fpath, &minmod) == 0)
				return FTW_STOP;

			// minimum modify time older than expiration
			if((time(0) - minmod) > EXPIRATION_POLICY)
			{
				if(rmdir_recursive(fpath, 1) == 0)
					return FTW_STOP;
			}

			// process only toplevel files
			return FTW_SKIP_SUBTREE;
		};

		fatal_os(nftw, space, fn, 32, FTW_ACTIONRETVAL);
	}

	// resume user identity
	fatal(identity_assume_user);

	finally : coda;
}
