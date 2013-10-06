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

#include "listwise/operator.h"

#include "liblistwise_control.h"

#include "canon.h"
#include "parseint.h"
#include "xmem.h"
#include "color.h"

/*

stat operator - replace path entries with related filesystem information

ARGUMENTS
	[0] - printf-style expression (taken after find)
	[1] - flags

OPERATION

	1. foreach selected entry
    1.1. render expression using directives and escapes against the entry as a path
		1.1. replace the entry with the resulting string

*/

static int op_validate(operation* o);
static int op_exec(operation*, lwx*, int**, int*);

int op_exec(operation* o, lwx* ls, int** ovec, int* ovec_len)
{
	// workspace
	char space[256];
	char space2[256];
	int x;
	int i;
	size_t z;
	char * cwd = 0;
	char * s = 0;
	int sl = 0;
	int sa = 0;

	// format and flags
	char * fmt = 0;
	int isstat = 1;
	int iscolor = 1;
	int isrebase = 0;

	// process arguments and flags
	for(x = 0; x < o->argsl; x++)
	{
		i = o->args[x]->l;
		if(fmt == 0)
		{
			for(i = 0; i < o->args[x]->l; i++)
			{
				if(o->args[x]->s[i] == 'L') { }
				else if(o->args[x]->s[i] == 'C') { }
				else if(o->args[x]->s[i] == 'F') { }
				else
				{
					break;
				}
			}
		}

		if(i < o->args[x]->l)
		{
			fmt = o->args[x]->s;
		}
		else
		{
			isstat  = !strchr(o->args[x]->s, 'L');
			iscolor = !strchr(o->args[x]->s, 'C');
			isrebase = !!strchr(o->args[x]->s, 'F');
		}
	}

	// default format - perms user group size modify {path rebased to cwd}
	if(!fmt)
	{
		if(isrebase)
			fmt = "%m %u %g %s %t %F";
		else
			fmt = "%m %u %g %s %t %p";
	}

	int fmtl = strlen(fmt);

	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		struct stat st;
		int r = 1;

		char * ss;
		int ssl;
		fatal(lstack_getstring, ls, 0, x, &ss, &ssl);

		if(isstat)
			r = stat(ss, &st);
		else
			r = lstat(ss, &st);

		// copy of the starting string
		sl = ssl;
		if(sl >= sa)
		{
			fatal(xrealloc, &s, 1, sl + 1, sa);
			sa = sl + 1;
		}
		memcpy(s, ss, sl);
		s[sl] = 0;

		if(r == 0)
		{
			// clear this string on the stack
			lstack_clear(ls, 0, x);

			for(i = 0; i < fmtl; i++)
			{
				if(((i + 1) < fmtl) && fmt[i] == '%')
				{
					if(fmt[i + 1] == 'm')
					{
						fatal(lstack_appendf, ls, 0, x, "%c%c%c%c%c%c%c%c%c%c"
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
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_uid);
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

						fatal(lstack_appendf, ls, 0, x, "%s", name);
					}
					else if(fmt[i + 1] == 'G')
					{
						fatal(lstack_appendf, ls, 0, x, "%u", st.st_gid);
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

						fatal(lstack_appendf, ls, 0, x, "%s", name);
					}
					else if(fmt[i + 1] == 't')
					{
						struct tm tm;
						if(localtime_r(&st.st_mtime, &tm) == 0)
							fail("localtime failed");

						if((z = strftime(space, sizeof(space), "%b %d %T", &tm)) == 0)
							fail("strftime failed");

						fatal(lstack_append, ls, 0, x, space, z);
					}
					else if(fmt[i + 1] == 's')
					{
						fatal(lstack_appendf, ls, 0, x, "%6zu", st.st_size);
					}
					else if(fmt[i + 1] == 'p' || fmt[i + 1] == 'r' || fmt[i + 1] == 'f' || fmt[i + 1] == 'h' || fmt[i + 1] == 'F')
					{
						int wascolor = iscolor;
						if(iscolor)
						{
							if(S_ISDIR(st.st_mode))
								fatal(lstack_append, ls, 0, x, COLOR(BLUE));
							else if(S_ISCHR(st.st_mode))
								fatal(lstack_append, ls, 0, x, COLOR(YELLOW));
							else if(S_ISLNK(st.st_mode))
								fatal(lstack_append, ls, 0, x, COLOR(CYAN));
							else if(euidaccess(s, X_OK) == 0)
								fatal(lstack_append, ls, 0, x, COLOR(GREEN));
							else
								wascolor = 0;
						}

						if(fmt[i + 1] == 'p')	// path as it came in
						{
							fatal(lstack_appendf, ls, 0, x, "%.*s", sl, s);
						}
						else
						{
							if(cwd == 0 && (cwd = getcwd(0, 0)) == 0)
								fail("getcwd=[%d][%s]", errno, strerror(errno));

							fatal(canon, s, sl, space, sizeof(space), cwd, CAN_REALPATH);

							if(fmt[i + 1] == 'r')	// realpath
							{
								fatal(lstack_appendf, ls, 0, x, "%s", space);
							}
							else if(fmt[i + 1] == 'F')	// path, rebased to cwd
							{
								fatal(rebase, space, 0, cwd, 0, space2, sizeof(space2));
								fatal(lstack_appendf, ls, 0, x, "%s", space2);
							}
							else if(fmt[i + 1] == 'f' || fmt[i + 1] == 'h')
							{
								char * slash = space + strlen(space);
								while(slash[0] != '/')
									slash--;

								if(fmt[i + 1] == 'f')				// filename component
								{
									fatal(lstack_appendf, ls, 0, x, "%s", slash + 1);
								}
								else if(fmt[i + 1] == 'h')	// directory component
								{
									slash[0] = 0;
									fatal(lstack_appendf, ls, 0, x, "%s", space);
								}
							}
						}

						if(wascolor)
						{
							fatal(lstack_append, ls, 0, x, COLOR(NONE));
						}

						if(fmt[i + 1] != 'h' && S_ISLNK(st.st_mode))
						{
							if((z = readlink(s, space, sizeof(space))) != -1)
							{
								fatal(lstack_appendf, ls, 0, x, " -> %.*s", z, space);
							}
						}
					}
					else
					{
						// not a recognized escape
						fatal(lstack_append, ls, 0, x, &fmt[i], 2);
					}

					i++;
				}
				else if(((i + 1) < fmtl) && fmt[i] == '\\')
				{
					if(fmt[i + 1] == 'r')
						fatal(lstack_append, ls, 0, x, "\r", 1);
					else if(fmt[i + 1] == 'n')
						fatal(lstack_append, ls, 0, x, "\n", 1);
					else if(fmt[i + 1] == '0')
						fatal(lstack_append, ls, 0, x, "\0", 1);
					else if(fmt[i + 1] == '\\')
						fatal(lstack_append, ls, 0, x, "\\", 1);
					else if(fmt[i + 1] == 't')
						fatal(lstack_append, ls, 0, x, "\t", 1);
					else if(fmt[i + 1] == 'x')
					{
						if((i + 3) < fmtl)
						{
							int v;
							if(parseuint(fmt + i + 2, SCNx8, 0, 0xFF, 2, 2, &v, 0) == 0)
							{
								fatal(lstack_appendf, ls, 0, x, "%c", v);
							}

							i += 2;
						}
					}

					i++;
				}
				else
				{
					fatal(lstack_append, ls, 0, x, fmt + i, 1);
				}
			}

			// record this index was hit
			fatal(lstack_sel_stage, ls, x);
		}
		else
		{
			dprintf(listwise_err_fd, "%s(%.*s)=[%d][%s]\n", isstat ? "stat" : "lstat", ssl, ss, errno, strerror(errno));
		}
	}
	LSTACK_ITEREND

finally:
	free(cwd);
	free(s);
coda;
}
