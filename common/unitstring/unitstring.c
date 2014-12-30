/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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
#include <inttypes.h>
#include <time.h>

#include "unitstring.h"

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

#define restrict __restrict

//
// [[ private ]]
//

typedef struct
{
	char*	name;
	char*	fmt;
	int		quant;
} dec_config;

static void decompose(int base, dec_config* config, size_t len, int* k)
{
	int x;
	for(x = len - 1; x > 0; x--)
	{
		int prod = 1;
		int y;
		for(y = 1; y <= x; y++)
		{
			prod *= config[y].quant;
		}
		k[x] = base / prod;
		base -= k[x] * prod;
	}
	k[0] = base;
}

static int reconstruct(dec_config* conf, size_t len, int* k, char* s, int l)
{
	int c = 0;
	int n = 0;
	int x;
	for(x = len - 1; x >= 0; x--)
	{
		if(k[x] || conf[x].fmt)
		{
			if(c)
			{
				snprintf(&s[c], MAX(l - c, 0), ", %n", &n);
				c += n;
			}

			snprintf(&s[c], MAX(l - c, 0), conf[x].fmt ? conf[x].fmt : "%d%n", k[x], &n);
			c += n;

			snprintf(&s[c], MAX(l - c, 0), " %s%n", conf[x].name, &n);
			c += n;
		}
	}

	return c;
}

//
// [[ public ]]
//

static char o_space[5][100];
static unsigned char	o_x;

char* durationstring_r(int base, char* s, int l)
{
	dec_config config[] = {
		  { .name = "sec(s)",	.fmt = "%.02d%n" }
		, { .name = "min(s)",	.quant = 60 }
		, { .name = "hr(s)",	.quant = 60 }
		, { .name = "day(s)",	.quant = 24 }
		, { .name = "yr(s)",	.quant = 365 }
	};

	int components[5] = { 0 };

	decompose(base, config, sizeof(config) / sizeof(config[0]), components);
	int c = reconstruct(config, sizeof(config) / sizeof(config[0]), components, s, l);

	sprintf(s + c, " or %us", base);

	return s;
}

char* durationstring(int base)
{
	durationstring_r(base, o_space[o_x], sizeof(o_space[o_x]));
	char* r = o_space[o_x];
	o_x = (o_x + 1) % (sizeof(o_space) / sizeof(o_space[0]));
	return r;
}

char* bytestring_r(int base, char* s, int l)
{
	dec_config config[] = {
      { .name = "byte(s)" }
    , { .name = "kbyte(s)",	.quant = 1 << 10 }
    , { .name = "meg(s)",		.quant = 1 << 10 }
    , { .name = "gig(s)",		.quant = 1 << 10 }
  };

	int components[4] = { 0 };

	decompose(base, config, sizeof(config) / sizeof(config[0]), components);
	int c = reconstruct(config, sizeof(config) / sizeof(config[0]), components, s, l);

	sprintf(s + c, " or %ub", base);

	return s;
}

char* bytestring(int base)
{
	bytestring_r(base, o_space[o_x], sizeof(o_space[o_x]));
	char* r = o_space[o_x];
	o_x = (o_x + 1) % (sizeof(o_space) / sizeof(o_space[0]));
	return r;
}

typedef struct
{
	char *		name;
	char *		fmt;
	uint64_t	quant;
} config64;

static void decompose64(uint64_t base, config64 * conf, size_t len, double * k)
{
	int x;
	for(x = len - 1; x >= 0; x--)
	{
		uint64_t prod = 1;
		int y;
		for(y = 0; y <= x; y++)
		{
			prod *= conf[y].quant;
		}
		if(x == 0)
			k[x] = (double)base / prod;
		else
		{
			k[x] = (double)(base / prod);
			base -= (uint64_t)k[x] * prod;
		}
	}
}

static size_t reconstruct64(config64 * conf, size_t len, double * k, char* s, int l)
{
	int c = 0;
	int n = 0;
	int x;
	for(x = len - 1; x >= 0; x--)
	{
		if(k[x] || conf[x].fmt)
		{
			if(c)
			{
				snprintf(&s[c], MAX(l - c, 0), ", %n", &n);
				c += n;
			}

			snprintf(&s[c], MAX(l - c, 0), conf[x].fmt ? conf[x].fmt : "%3.0f%n", k[x], &n);
			c += n;

			snprintf(&s[c], MAX(l - c, 0), " %s%n", conf[x].name, &n);
			c += n;
		}
	}

	return c;
}

size_t elapsed_string_timespec(const struct timespec * const restrict start, const struct timespec * const restrict end, char * const restrict dst, size_t sz)
{
	config64 conf[] = {
		  { .name = "sec(s)"	,	.quant = 1000000000, .fmt = "%4.02lg%n" }
		, { .name = "min(s)"	,	.quant = 60 }
		, { .name = "hr(s)"		,	.quant = 60 }
		, { .name = "day(s)"	,	.quant = 24 }
		, { .name = "yr(s)"		,	.quant = 365 }
	};

	// calculate delta
	uint64_t base = end->tv_sec - start->tv_sec;
	if(end->tv_nsec < start->tv_nsec)
	{
		base -= 1;
		base *= 1000000000;
		base += (end->tv_nsec + 1000000000) - start->tv_nsec;
	}
	else
	{
		base *= 1000000000;
		base += end->tv_nsec - start->tv_nsec;
	}

	double components[sizeof(conf) / sizeof(conf[0])];
	decompose64(base, conf, sizeof(conf) / sizeof(conf[0]), components);
	return reconstruct64(conf, sizeof(conf) / sizeof(conf[0]), components, dst, sz);
}
