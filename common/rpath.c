#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int rpath(const char * s, char ** d)
{
	char src[256];
	char dst[256] = {};

	if(realpath(s, dst))
	{
		*d = strdup(dst);
		return 1;
	}

	if(errno != ENOENT)
		return 0;

	const char * t = s + strlen(s);

	while(t != s)
	{
		t--;
		while(*t != '/' && t != s)
			t--;

		if(t != s)
		{
			memcpy(src, s, t - s);
			src[t - s] = 0;
			if(realpath(src, dst))
			{
				*d = calloc(strlen(dst) + strlen(s) - (t - s) + 1, 1);

				memcpy((*d) + strlen(*d), dst, strlen(dst));
				memcpy((*d) + strlen(*d), t, strlen(s) - (t - s) + 1);

				return 1;
			}
			if(errno != ENOENT)
				return 0;
		}
	}

	return 0;
}
