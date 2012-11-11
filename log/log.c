#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "log.h"

#include "xstring.h"

#define MAX(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

#define MIN(a,b)            \
 ({ typeof (a) _a = (a);    \
     typeof (b) _b = (b);   \
   _a > _b ? _b : _a; })

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 32])
#define COLOR(x)		(unsigned char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(unsigned char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

static uint64_t o_lgctx;
static uint64_t o_e;

//
// [[ static ]]
//

static char * o_space;
static int		o_space_l;
static int		o_space_a;

static struct {
	uint64_t		v;
	char *			s;
	int					l;
} o_logs[] = { 
	  { .v = L_ERROR	, .s = "ERROR" 	}
	, { .v = L_WARN		, .s = "WARN" 	}
	, { .v = L_INFO		, .s = "INFO" 	}
	, { .v = L_BPEXEC	, .s = "BPEXEC"	}
	, { .v = L_BPEVAL	, .s = "BPEVAL"	}
	, { .v = L_BPDUMP	, .s = "BPDUMP"	}
	, { .v = L_FFTOKN	, .s = "FFTOKN"	}
	, { .v = L_FFSTAT	, .s = "FFSTAT"	}
	, { .v = L_FFTREE	, .s = "FFTREE"	}
	, { .v = L_FF			, .s = "FF"			}
	, { .v = L_ARGS		, .s = "ARGS"		}
	, { .v = L_GN			, .s = "GN"			}
	, { .v = L_FMEXEC	, .s = "FMEXEC"	}
	, { .v = L_HASH		, .s = "HASH"		}
	, { .v = L_VAR		, .s = "VAR"		}
	, { .v = L_TAG		, .s = "TAG"		}
};

static int o_name_len;

static unsigned char o_colors[] = {
	/* NO COLOR */   0x00
	/* L_RED 		*/ , 0x31
	/* L_GREEN	*/ , 0x32
	/* L_YELLOW */ , 0x33
	/* L_CYAN 	*/ , 0x36
	/* L_BLUE 	*/ , 0x34
};

static int logwrite(const char * s, size_t l)
{
	if((o_space_l + l) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + l + 1);
		o_space_a = o_space_l + l + 1;
	}

	memcpy(o_space + o_space_l, s, l);
	o_space_l += l;
}

static int logvprintf(const char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);
	
	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if((o_space_l + size) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + size + 1);
		o_space_a = o_space_l + size + 1;
	}

	vsprintf(o_space + o_space_l, fmt, va);

	o_space_l += size;
}

static int logprintf(const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	logvprintf(fmt, va);
	va_end(va);
}

static void args_parse(char * args, int args_len)
{
	int x;
	for(x = 0; x < (args_len - 1); x++)
	{
		if(args[x] == '+' || args[x] == '-')
		{
			int y;
			for(y = 0; y < sizeof(o_logs) / sizeof(o_logs[0]); y++)
			{
				if(xstrcmp(&args[x+1], MIN(args_len - (x + 1), o_logs[y].l), o_logs[y].s, o_logs[y].l, 0) == 0)
				{
					if(args[x] == '+')
						o_lgctx |= o_logs[y].v;

					if(args[x] == '-')
						o_lgctx &= ~o_logs[y].v;

					x += o_logs[y].l + 1;
				}
			}
		}
	}
}

static int log_vstart(const uint64_t e)
{
	if(log_would(e))
	{
		o_space_l = 0;

		// colorization
		if((e & L_COLOR_VALUE) && COLORHEX(e))
		{
			logwrite(COLOR(e));
		}

		// prefix
		int x;
		for(x = 0; x < sizeof(o_logs) / sizeof(o_logs[0]); x++)
		{
			if(e & o_logs[x].v)
			{
				logprintf("%*s : ", o_name_len, o_logs[x].s);
				break;
			}
		}

		o_e = e;

		return 1;
	}

	return 0;
}

static void log_vadd(const char* fmt, va_list va)
{
	logvprintf(fmt, va);
}

static void log_vfinish(const char* fmt, va_list* va)
{
	if(fmt)
		log_vadd(fmt, *va);

	if((o_e & L_COLOR_VALUE) && COLORHEX(o_e))
	{
		logwrite(NOCOLOR);
	}

	logwrite("\n", 1);

	// flush to stderr
	write(2, o_space, o_space_l);

	o_e = 0;
}

//
// [[ public ]]
//

int log_init(char * str)
{
	// determine logtag len
	int x;
	for(x = 0; x < sizeof(o_logs) / sizeof(o_logs[0]); x++)
	{
		o_logs[x].l = strlen(o_logs[x].s);
		o_name_len = MAX(o_name_len, o_logs[x].l);
	}

	// apply initial args string
	args_parse(str, strlen(str));

	// parse cmdline
	int fd = open("/proc/self/cmdline", O_RDONLY);

	char *	args = 0;
	int			args_len = 0;
	int			nlen = 0;

	do
	{
		size_t newlen = args_len ? (args_len + (args_len / 2 ) + (args_len * 2)) : 10;

		args = realloc(args, newlen);
		nlen = newlen;

	} while((args_len += (read(fd, &args[args_len], nlen - args_len))) == nlen);

	close(fd);

	args_len--;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == 0)
			args[x] = ' ';
	}

	// parse cmdline args
	args_parse(args, args_len);

	free(args);

	return 1;
}

int log_would(const uint64_t bits)
{
	return !!(o_lgctx & bits);
}

void log_start(const uint64_t e, const char* fmt, ...)
{
	if(log_vstart(e))
	{
		va_list va;
		va_start(va, fmt);
		log_vadd(fmt, va);
	}
}

void log_add(const char* fmt, ...)
{
	if(log_would(o_e))
	{
		va_list va;
		va_start(va, fmt);
		log_vadd(fmt, va);
	}
}

void log_finish(const char* fmt, ...)
{
	if(log_would(o_e))
	{
		va_list va;
		if(fmt)
			va_start(va, fmt);

		log_vfinish(fmt, &va);
	}
}

void log(const uint64_t e, const char* fmt, ...)
{
	if(log_vstart(e))
	{
		va_list va;
		va_start(va, fmt);
		log_vadd(fmt, va);

		log_vfinish(0, 0);
	}
}

void log_teardown()
{
	free(o_space);
}
