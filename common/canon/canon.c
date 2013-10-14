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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "canon.h"
#include "xmem.h"

//
// static
//

#define SLASH		1
#define	DOT			2
#define	DOTDOT	3

typedef struct
{
	char *				s;
	int 					l;
	int						t;

	int						f;	// force
} item;

int breakup(item ** i, int * ia, int * il, int at, char * fmt, ...)
{
	char in[512];

	va_list va;
	va_start(va, fmt);
	int inl = vsnprintf(in, sizeof(in), fmt, va);
	va_end(va);

//printf("breakup(%.*s)\n", inl, in);

	int x = 0;
	while(x < inl)
	{
		while((*il) >= (*ia))
		{
			int ns = (*ia) ?: 10;
			ns = ns * 2 + ns / 2;
			if(xrealloc(i, sizeof(**i), ns, (*ia)) != 0)
			{
				int k;
				for(k = 0; k < *ia; k++)
					free((*i)->s);
				free(*i);
				return 1;
			}

			(*ia) = ns;
		}

		// swap an unused entry into position at
		item T = (*i)[(*ia) - 1];

		memmove(
			  &(*i)[at + 1]
			, &(*i)[at]
			, ((*ia) - at - 1) * sizeof((*i)[0])
		);

		(*i)[at] = T;

		// reset it
		(*i)[at].s = 0;
		(*i)[at].l = 0;
		(*i)[at].t = 0;

		if((inl - x) >= 1 && in[x+0] == '/')
		{
			(*i)[at].t = SLASH;
			x++;
		}
		else if((inl - x) >= 1 && in[x+0] == '.' && (in[x+1] == 0 || in[x+1] == '/'))
		{
			(*i)[at].t = DOT;
			x++;
		}
		else if((inl - x) >= 2 && in[x+0] == '.' && in[x+1] == '.' && (in[x+2] == 0 || in[x+2] == '/'))
		{
			(*i)[at].t = DOTDOT;
			x += 2;
		}
		else
		{
			const char * e = &in[x];
			while(e[0] && e[0] != '/')
				e++;

			(*i)[at].s = strdup(&in[x]);
			(*i)[at].l = e - &in[x];
			x += (*i)[at].l;
		}

		(*il)++;
		at++;
	}

	return 0;
}

static void add(size_t * z, char * const resolved, size_t sz, char * fmt, ...)
{
	if(*z && resolved[(*z) - 1] != '/')
		(*z) += snprintf(resolved + (*z), sz - (*z), "/");

	va_list va;
	va_start(va, fmt);
	(*z) += vsnprintf(resolved + (*z), sz - (*z), fmt, va);
	va_end(va);

	resolved[(*z)] = 0;
}

//
// public
//

int canon(
	  const char * path
	, int pathl
	, const char * const base
	, int basel
	, char * const dst
	, const size_t siz
	, size_t * z
	, uint32_t opts
)
{
	opts = opts ?: CAN_REALPATH;
	pathl = pathl ?: strlen(path);

	if(base)
	{
		basel = basel ?: strlen(base);
	}

	size_t local_z;
	if(z == 0)
	{
		z = &local_z;
	}

	(*z) = 0;

	item *	i = 0;
	int			ia = 0;
  int			il = 0;
	int			ix = 0;

	// if the path begins with a DOT or a DOTDOT
	if(pathl >= 1 && path[0] == '.')
	{
		if(pathl == 1 || (path[1] == '.' || path[1] == '/' || path[1] == 0))
		{
			if(pathl == 2 || path[1] == '/' || path[1] == 0 || (path[2] == '/' || path[2] == 0))
			{
				if(opts & CAN_INIT_DOT)
				{
					if(breakup(&i, &ia, &il, il, "%.*s/", basel, base) != 0)
						return 1;
				}
			}
		}
	}

	// else if the path is not an absolute path
	else if(pathl == 0 || path[0] != '/')
	{
		if(opts & CAN_FORCE_DOT)
		{
			if(breakup(&i, &ia, &il, il, "%.*s/", basel, base) != 0)
				return 1;
		}
	}

	int init = il;
	if(breakup(&i, &ia, &il, il, "%.*s", pathl, path) != 0)
		return 1;

	dst[0] = 0;

/*
for(ix = 0; ix < il; ix++)
{
	if(i[ix].t == SLASH)
		printf("[%d] SLASH\n", ix);
	else if(i[ix].t == DOT)
		printf("[%d] DOT\n", ix);
	else if(i[ix].t == DOTDOT)
		printf("[%d] DOTDOT\n", ix);
	else
		printf("[%d] %.*s\n", ix, i[ix].l, i[ix].s);
}

printf("init=%d\n", init);
*/
	for(ix = 0; ix < il; ix++)
	{
		if(i[ix].t == SLASH)
		{
			if((*z) == 0)
				(*z) += snprintf(dst + (*z), siz - (*z), "/");
		}
		else if(i[ix].t == DOT)
		{
			if(init != -1)
			{
				if((opts & CAN_INIT_DOT) == 0)
					add(z, dst, siz, ".");
			}
			else
			{
				if((opts & CAN_NEXT_DOT) == 0)
					add(z, dst, siz, ".");
			}
		}
		else if(i[ix].t == DOTDOT)
		{
			int no = 0;
			if(init != -1)
			{
				if((opts & CAN_INIT_DOT) == 0)
					no = 1;
			}
			else
			{
				if((opts & CAN_NEXT_DOT) == 0)
					no = 1;
			}

			if(no || (*z) == 0 || ((*z) == 1 && dst[0] == '/'))
			{
				// cannot backup; append the DOTDOT
				add(z, dst, siz, "..");
			}
			else
			{
				if(*z)
					(*z)--;
				while((*z) && dst[(*z)] != '/')
					(*z)--;
			}
		}
		else
		{
			if(ix > init)
				init = -1;

			int isfinal = 1;
			int fix;
			for(fix = ix + 1; fix < il; fix++)
			{
				if(i[fix].t != SLASH)
				{
					isfinal = 0;
				}
			}

			// append the link name to pass to stat
			size_t oldz = (*z);
			add(z, dst, siz, "%.*s", i[ix].l, i[ix].s);

			struct stat stb[2] = {};
			if(lstat(dst, &stb[0]) == 0)
			{
				if(S_ISLNK(stb[0].st_mode))
				{
					char space[512];

					int j = readlink(dst, space, sizeof(space));
					space[j] = 0;

					char space2[512];
					snprintf(space2, sizeof(space2), "%.*s/%s", (int)oldz, dst, space);

					int r = lstat(space2, &stb[1]);

					if(r || stb[0].st_dev == stb[1].st_dev)
					{
						// dangling links, and fulfilled links which do not cross mount points
						if((isfinal && (opts & CAN_FINL_SYM)) || (!isfinal && (opts & CAN_NEXT_SYM)))
						{
							if(breakup(&i, &ia, &il, ix + 1, "%.*s", j, space) != 0)
								return 1;

							if(space[0] == '/')
								(*z) = 0;
							else
								(*z) = oldz;
						}
					}
					else
					{
						if((isfinal && (opts & CAN_FINL_SYMMNT)) || (!isfinal && (opts & CAN_NEXT_SYMMNT)))
						{
							if(breakup(&i, &ia, &il, ix + 1, "%.*s", j, space) != 0)
								return 1;

							if(space[0] == '/')
								(*z) = 0;
							else
								(*z) = oldz;
						}
					}
				}
			}
			else if(errno != ENOENT && errno != EACCES && errno != ENOTDIR)
			{
				return 1;
			}
		}
	}

	int x;
	for(x = 0; x < ia; x++)
		free(i[x].s);
	free(i);
	dst[(*z)] = 0;
	return 0;
}

int rebase(
	  const char * const path
	, int pathl
	, const char * const base
	, int basel
	, char * const dst
	, const size_t siz
	, size_t * z
)
{
	pathl = pathl ?: strlen(path);
	basel = basel ?: strlen(base);

	size_t local_z;
	if(z == 0)
	{
		z = &local_z;
	}

	// match up as many segments between path and base as possible
	int x;
	for(x = 0; x < pathl && x < basel; x++)
	{
		int y = 0;
		while((x + y) < pathl && (x + y) < basel && path[x + y] == base[x + y] && path[x + y] != '/')
			y++;

		if((path[x + y] == '/' || ((x + y) == pathl)) &&  (base[x + y] == '/' || ((x + y) == basel)))
		{
			x += y;
		}
		else
		{
			break;
		}
	}

	// for each segment in base beyond path, append ".."
	(*z) = 0;
	int i;
	for(i = x; i < basel; i++)
	{
		int y = 0;
		while((i + y) < basel && base[i + y] != '/')
			y++;

		if(y)
		{
			if(i != x)
				(*z) += snprintf(dst + (*z), siz - (*z), "/");
			(*z) += snprintf(dst + (*z), siz - (*z), "..");
		}

		i += y;
	}

	// append the remainder of the path
	if((pathl - x) > 0)
	{
		if(*z)
			(*z) += snprintf(dst + (*z), siz - (*z), "/");

		(*z) += snprintf(dst + (*z), siz - (*z), "%.*s", pathl - x, path + x);
	}

	return 0;
}
