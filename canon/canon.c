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
	const char *	s;
	int 					l;
	int						t;

	int						f;	// force
} item;

int breakup(const char * in, int inl, item ** i, int * ia, int * il, int at)
{
	int x = 0;
	while(x < inl)
	{
		while((*il) >= (*ia))
		{
			int ns = (*ia) ?: 10;
			ns = ns * 2 + ns / 2;
			if(xrealloc(i, sizeof(**i), ns, (*ia)) == 0)
			{
				free(*i);
				return 0;
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

			(*i)[at].s = &in[x];
			(*i)[at].l = e - &in[x];
			x += (*i)[at].l;
		}

		(*il)++;
		at++;
	}

	return 1;
}

static void add(size_t * z, int * flag, char * const resolved, size_t sz, char * fmt, ...)
{
	if(*flag)
		(*z) += snprintf(resolved + (*z), sz - (*z), "/");

	va_list va;
	va_start(va, fmt);
	(*z) += vsnprintf(resolved + (*z), sz - (*z), fmt, va);
	va_end(va);

	(*flag)++;
	resolved[(*z)] = 0;
}

//
// public
//

int canon(const char * path, int pathl, char * const resolved, size_t sz, const char * base, uint32_t opts)
{
	opts = opts ?: CAN_REALPATH;
	pathl = pathl ?: strlen(path);

	size_t	z = 0;

	item *	i = 0;
	int			ia = 0;
  int			il = 0;
	int			ix = 0;

	int			force = 0;

	// if the path starts with "." or "..", use base, first
	if(pathl >= 1 && path[0] == '.' && ((path[1] == 0 || path[1] == '/')))
	{
		if(opts & CAN_INIT_DOT)
		{
			// resolve initial DOT with base
			if(breakup(base, strlen(base), &i, &ia, &il, il) == 0)
				return 0;

			if(breakup("/", 1, &i, &ia, &il, il) == 0)
				return 0;

			// force resolution of the initial DOT from path
			force = 1;
		}
		else
		{
			// otherwise, the initial DOT is appended
		}
	}
	else if(pathl >= 2 && path[0] == '.' && ((path[1] == '.' && (path[2] == 0 || path[2] == '/'))))
	{
		if(opts & CAN_INIT_DOT)
		{
			if(breakup(base, strlen(base), &i, &ia, &il, il) == 0)
				return 0;

			if(breakup("/", 1, &i, &ia, &il, il) == 0)
				return 0;

			// force resolution of the initial DOTDOT from path
			force = 1;
		}
		else
		{
			// otherwise, initial DOTDOT is appended
		}
	}
	else if(pathl == 0 || path[0] != '/')
	{
		if(opts & CAN_FORCE_DOT)
		{
			if(breakup(base, strlen(base), &i, &ia, &il, il) == 0)
				return 0;

			if(breakup("/", 1, &i, &ia, &il, il) == 0)
				return 0;
		}
	}

	int at = il;
	if(breakup(path, pathl, &i, &ia, &il, il) == 0)
		return 0;

	if(force)
		i[at].f = 1;

	resolved[0] = 0;

	int flag = 0;
	for(ix = 0; ix < il; ix++)
	{
		if(i[ix].t == SLASH)
		{
			if(ix == 0)
				z += snprintf(resolved + z, sz - z, "/");
		}
		else if(i[ix].t == DOT)
		{
			if((opts & CAN_NEXT_DOT) == 0 && i[ix].f == 0)
			{
				add(&z, &flag, resolved, sz, ".");
			}
		}
		else if(i[ix].t == DOTDOT)
		{
			if(((opts & CAN_NEXT_DOT) == 0 && i[ix].f == 0) || z == 0 || (z == 1 && resolved[0] == '/'))
			{
				// cannot backup; append the DOTDOT
				add(&z, &flag, resolved, sz, "..");
			}
			else
			{
				z--;
				while(resolved[z] != '/')
					z--;

				// backed up all the way, reset slash flag
				if(z == 0)
					flag = 0;
			}
		}
		else
		{
			// append the link name to pass to stat
			add(&z, &flag, resolved, sz, "%.*s", i[ix].l, i[ix].s);

			struct stat stb[2] = {};
			if(lstat(resolved, &stb[0]) == 0)
			{
				if(S_ISLNK(stb[0].st_mode))
				{
					char space[512];

					int j = readlink(resolved, space, sizeof(space));
					space[j] = 0;

					if(stat(space, &stb[1]) == 0)
					{
						if(space[0] == '/')
						{
							if(stb[0].st_dev == stb[1].st_dev)
							{
								if(opts & CAN_SYMABS)
								{
									if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
										return 0;

									z = 0;
								}
							}
							else
							{
								if(opts & CAN_SYMABSMNT)
								{
									if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
										return 0;

									z = 0;
								}
							}
						}
						else
						{
							if(stb[0].st_dev == stb[1].st_dev)
							{
								if(opts & CAN_SYMREL)
								{
									if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
										return 0;
								}
							}
							else
							{
								if(opts & CAN_SYMRELMNT)
								{
									if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
										return 0;
								}
							}
						}
					}
					else
					{
						if(space[0] == '/')
						{
							if(opts & CAN_SYMABS)
							{
								if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
									return 0;

								z = 0;
							}
						}
						else
						{
							if(opts & CAN_SYMREL)
							{
								if(breakup(space, j, &i, &ia, &il, ix + 1) == 0)
									return 0;
							}
						}
					}
				}
			}
			else if(errno != ENOENT)
			{
				return 0;
			}
		}
	}

	free(i);
	resolved[z] = 0;
	return 1;
}
