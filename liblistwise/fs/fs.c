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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#include "listwise/internal.h"
#include "liblistwise_control.h"

#include "canon.h"
#include "parseint.h"
#include "xmem.h"
#include "color.h"
#include "macros.h"

#define restrict __restrict

/// snwrite
//
// SUMMARY
//  like snprintf
//
// PARAMETERS
//  dst - destination
//  siz - maximum bytes to write to destination
//  src - source
//  amt - bytes to copy from source
//
// RETURNS
//  number of bytes copied
//
static size_t snwrite(char * const restrict dst, const size_t siz, const char * const restrict src, const size_t amt)
{
	size_t z = MIN(siz, amt);
	memcpy(dst, src, z);
	return z;
}

int API fs_statfmt(
	  const char * const restrict s
	, int sl
	, const char * restrict fmt
	, const char * const restrict flags
	, char * const restrict dst
	, size_t sz
	, size_t * const z
)
{
	// workspace
	char space[256];
	char space2[256];
	char * cwd = 0;

	// process flags
	int isstat = 1;
	int iscolor = 1;
	int isrebase = 0;
	if(flags)
	{
		isstat  = !strchr(flags, 'L');
		iscolor = !strchr(flags, 'C');
		isrebase = !!strchr(flags, 'F');
	}

	// default format - perms user group size modify {path rebased to cwd}
	if(!fmt && isrebase)
		fmt = "%m %u %g %s %t %F";
	else if(!fmt)
		fmt = "%m %u %g %s %t %p";

	int fmtl = strlen(fmt);
	sl = sl ?: strlen(s);

	struct stat st;
	int r = 1;

	// stat the file
	if(isstat)
		r = stat(s, &st);
	else
		r = lstat(s, &st);

	if(r)
	{
		dprintf(listwise_info_fd, "%s(%.*s)=[%d][%s]\n", isstat ? "stat" : "lstat", sl, s, errno, strerror(errno));
	}
	else
	{
		(*z) = 0;
		int i;
		for(i = 0; i < fmtl; i++)
		{
			if(((i + 1) < fmtl) && fmt[i] == '%')
			{
				if(fmt[i + 1] == 'm')
				{
					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%c%c%c%c%c%c%c%c%c%c"
						,   S_ISDIR(st.st_mode)  ? 'd'
							: S_ISCHR(st.st_mode)  ? 'c'
							: S_ISBLK(st.st_mode)  ? 'b'
							: S_ISFIFO(st.st_mode) ? 'f'
							: S_ISLNK(st.st_mode)  ? 'l'
							: S_ISSOCK(st.st_mode) ? 's'
							:                        '-'
						, st.st_mode & S_IRUSR ? 'r' : '-'
						, st.st_mode & S_IWUSR ? 'w' : '-'
						, st.st_mode & S_IXUSR ? 'x' : '-'
						, st.st_mode & S_IRGRP ? 'r' : '-'
						, st.st_mode & S_IWGRP ? 'w' : '-'
						, st.st_mode & S_IXGRP ? 'x' : '-'
						, st.st_mode & S_IROTH ? 'r' : '-'
						, st.st_mode & S_IWOTH ? 'w' : '-'
						, st.st_mode & S_IXOTH ? 'x' : '-'
					);
				}
				else if(fmt[i + 1] == 'U')
				{
					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%u", st.st_uid);
				}
				else if(fmt[i + 1] == 'u')
				{
					char * name = "(none)";
					struct passwd stor;
					struct passwd * pwd;
					if(getpwuid_r(st.st_uid, &stor, space, sizeof(space), &pwd) == 0)
					{
						if(pwd)
						{
							name = pwd->pw_name;
						}
						else
						{
							// name not found
						}
					}
					else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM)
					{
						// name not found
					}
					else
					{
						// other error
						fail("getpwd(%d)=[%d][%s]", st.st_uid, errno, strerror(errno));
					}

					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%s", name);
				}
				else if(fmt[i + 1] == 'G')
				{
					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%u", st.st_gid);
				}
				else if(fmt[i + 1] == 'g')
				{
					char * name = "(none)";
					struct group stor;
					struct group * grp;
					if(getgrgid_r(st.st_gid, &stor, space, sizeof(space), &grp) == 0)
					{
						if(grp)
						{
							name = grp->gr_name;
						}
					}
					else if(errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM) { }
					else
					{
						// other error
						fail("getgrp(%d)=[%d][%s]", st.st_uid, errno, strerror(errno));
					}

					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%s", name);
				}
				else if(fmt[i + 1] == 't')
				{
					struct tm tm;
					if(localtime_r(&st.st_mtime, &tm) == 0)
						fail("localtime failed");

					size_t zz;
					if((zz = strftime(space, sizeof(space), "%b %d %T", &tm)) == 0)
						fail("strftime failed");

					(*z) += snwrite(dst + (*z), sz - (*z) - 1, space, zz);
				}
				else if(fmt[i + 1] == 's')
				{
					(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%6zu", st.st_size);
				}
				else if(fmt[i + 1] == 'p' || fmt[i + 1] == 'r' || fmt[i + 1] == 'f' || fmt[i + 1] == 'h' || fmt[i + 1] == 'F')
				{
					int wascolor = iscolor;
					if(iscolor)
					{
						if(S_ISDIR(st.st_mode))
							(*z) += snwrite(dst + (*z), sz - (*z) - 1, COLOR(BLUE));
						else if(S_ISCHR(st.st_mode))
							(*z) += snwrite(dst + (*z), sz - (*z) - 1, COLOR(YELLOW));
						else if(S_ISLNK(st.st_mode))
							(*z) += snwrite(dst + (*z), sz - (*z) - 1, COLOR(CYAN));
						else if(euidaccess(s, X_OK) == 0)
							(*z) += snwrite(dst + (*z), sz - (*z) - 1, COLOR(GREEN));
						else
							wascolor = 0;
					}

					if(fmt[i + 1] == 'p')	// path as it came in
					{
						(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%.*s", sl, s);
					}
					else
					{
						if(cwd == 0 && (cwd = getcwd(0, 0)) == 0)
							fail("getcwd=[%d][%s]", errno, strerror(errno));

						fatal(canon, s, sl, cwd, 0, space, sizeof(space), 0, CAN_REALPATH);

						if(fmt[i + 1] == 'r')	// realpath
						{
							(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%s", space);
						}
						else if(fmt[i + 1] == 'F')	// path, rebased to cwd
						{
							size_t zl;
							fatal(rebase, space, 0, cwd, 0, space2, sizeof(space2), &zl);
							(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%.*s", (int)zl, space2);
						}
						else if(fmt[i + 1] == 'f' || fmt[i + 1] == 'h')
						{
							char * slash = space + strlen(space);
							while(slash[0] != '/')
								slash--;

							if(fmt[i + 1] == 'f')				// filename component
							{
								(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%s", slash + 1);
							}
							else if(fmt[i + 1] == 'h')	// directory component
							{
								slash[0] = 0;
								(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%s", space);
							}
						}
					}

					if(wascolor)
					{
						(*z) += snwrite(dst + (*z), sz - (*z) - 1, COLOR(NONE));
					}

					if(fmt[i + 1] != 'h' && S_ISLNK(st.st_mode))
					{
						size_t zz;
						if((zz = readlink(s, space, sizeof(space))) != -1)
						{
							(*z) += snprintf(dst + (*z), sz - (*z) - 1, " -> %.*s", (int)zz, space);
						}
					}
				}
				else if(fmt[i + 1] == 'x')
				{
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "%x   ", 5);
				}
				else
				{
					// not a recognized escape
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, &fmt[i], 2);
				}

				i++;
			}
			else if(((i + 1) < fmtl) && fmt[i] == '\\')
			{
				if(fmt[i + 1] == 'r')
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "\r", 1);
				else if(fmt[i + 1] == 'n')
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "\n", 1);
				else if(fmt[i + 1] == '0')
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "\0", 1);
				else if(fmt[i + 1] == '\\')
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "\\", 1);
				else if(fmt[i + 1] == 't')
					(*z) += snwrite(dst + (*z), sz - (*z) - 1, "\t", 1);
				else if(fmt[i + 1] == 'x')
				{
					if((i + 3) < fmtl)
					{
						int v;
						if(parseuint(fmt + i + 2, SCNx8, 0, 0xFF, 2, 2, &v, 0) == 0)
						{
							(*z) += snprintf(dst + (*z), sz - (*z) - 1, "%c", v);
						}

						i += 2;
					}
				}

				i++;
			}
			else
			{
				(*z) += snwrite(dst + (*z), sz - (*z) - 1, fmt + i, 1);
			}
		}

		snwrite(dst + (*z), sz - (*z), "\0", 1);
	}

finally:
	free(cwd);
coda;
}
