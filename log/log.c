#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "log.h"

#include "xstring.h"
#include "macros.h"

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 60])
#define COLOR(x)		(char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

static struct filter
{
	uint64_t	v;		// tag
	char			m;		// mode, (, [, {, or <
	char			o;		// operation, + or -
} * 							o_filter;
static int				o_filter_a;
static int				o_filter_l;

static uint64_t o_e;						// when a log is being constructed, effective bits

// logtag definitions
struct g_logs_t o_logs[] = {
	  { .v = L_ERROR		, .s = "ERROR"		, .d = "errors leading to shutdown" }
	, { .v = L_WARN			, .s = "WARN"			, .d = "nonfatal warnings" }
	, { .v = L_INFO			, .s = "INFO"			, .d = "program flow" }
	, { .v = L_ARGS			, .s = "ARGS"			, .d = "program arguments" }
	, { .v = L_PARAMS		, .s = "PARAMS"		, .d = "program execution parameters" }
	, { .v = L_FFTOKN		, .s = "FFTOKN"		, .d = "fabfile parsing - token stream" }
	, { .v = L_FFSTAT		, .s = "FFSTAT"		, .d = "fabfile parsing - lexer start conditions" }
	, { .v = L_FFTREE		, .s = "FFTREE"		, .d = "fabfile parsing - parsed tree" }
	, { .v = L_FFFILE		, .s = "FFFILE"		, .d = "fabfile parsing - parsed file" }
	, { .v = L_FF				, .s = "FF"				, .d = "fabfile parsing" }
	, { .v = L_BPINFO		, .s = "BPINFO"		, .d = "buildplan - flow" }
	, { .v = L_BPEXEC		, .s = "BPEXEC"		, .d = "buildplan - execution" }
	, { .v = L_BPEVAL		, .s = "BPEVAL"		, .d = "buildplan - pruning/evaluating" }
	, { .v = L_BPDUMP		, .s = "BPDUMP"		, .d = "buildplan - dump the final buildplan" }
	, { .v = L_BP				, .s = "BP"				, .d = "buildplan" }
	, { .v = L_FMLEXEC	, .s = "FMLEXEC"	, .d = "formulas - execution results/details" }
	, { .v = L_FMLTARG	, .s = "FMLTARG"	, .d = "formulas - target resolution/assignment" }
	, { .v = L_FML			, .s = "FML"			, .d = "formulas" }
	, { .v = L_FAB			, .s = "FAB"			, .d = "fabrication formulas" }
	, { .v = L_DSCINFO	, .s = "DSCINFO"	, .d = "dependency discovery - flow" }
	, { .v = L_DSCEXEC	, .s = "DSCEXEC"	, .d = "dependency discovery - execution details" }
	, { .v = L_DSCNEW		, .s = "DSCNEW"		, .d = "dependency discovery - new nodes/edges" }
	, { .v = L_DSC			, .s = "DSC"			, .d = "dependency discovery" }
	, { .v = L_DGDEPS		, .s = "DGDEPS"		, .d = "dependency graph - dependencies" }
	, { .v = L_DGRAPH		, .s = "DGRAPH"		, .d = "dependency graph - dump/details" }
	, { .v = L_DG				, .s = "DG"				, .d = "dependency graph" }
	, { .v = L_HASHBLK	, .s = "HASHBLK"	, .d = "hashblock - read/write" }
	, { .v = L_VARAUTO	, .s = "VARAUTO"	, .d = "variable definitions - automatic" }
	, { .v = L_VARUSER	, .s = "VARUSER"	, .d = "variable definitions - user-defined" }
	, { .v = L_VAR			, .s = "VAR"			, .d = "variable definitions" }
	, { .v = L_INVOKE		, .s = "INVOKE"		, .d = "fabfile invocations" }
	, { .v = L_LWDEBUG	, .s = "LWDEBUG"	, .d = "debug liblistwise invocations ** VERBOSE" }
};

struct g_logs_t * g_logs = o_logs;

int g_logs_l = sizeof(o_logs) / sizeof(o_logs[0]);

//
// [[ static ]]
//

static char * o_space;
static int		o_space_l;
static int		o_space_a;

static int o_name_len;

static unsigned char o_colors[] = {
	/* NO COLOR */   0x00
	/* L_RED 		*/ , 0x31
	/* L_GREEN	*/ , 0x32
	/* L_YELLOW */ , 0x33
	/* L_CYAN 	*/ , 0x36
	/* L_BLUE 	*/ , 0x34
};

static void __attribute__((constructor)) init()
{
	// determine logtag len
	int x;
	for(x = 0; x < g_logs_l; x++)
	{
		g_logs[x].l = strlen(g_logs[x].s);
		o_name_len = MAX(o_name_len, g_logs[x].l);
	}
}

static int logwrite(const char * s, size_t l)
{
	if((o_space_l + l) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + l + 1);
		o_space_a = o_space_l + l + 1;
	}

	memcpy(o_space + o_space_l, s, l);
	o_space_l += l;

	return l;
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

	return size;
}

static int logprintf(const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	int R = logvprintf(fmt, va);
	va_end(va);

	return R;
}

static int log_vstart(const uint64_t e)
{
	int R = 0;
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
		for(x = 0; x < g_logs_l; x++)
		{
			if((e & g_logs[x].v) == g_logs[x].v)
			{
				R = logprintf("%*s : ", o_name_len, g_logs[x].s);
				break;
			}
		}

		o_e = e;
	}

	return R;
}

static int log_vadd(const char* fmt, va_list va)
{
	return logvprintf(fmt, va);
}

static int log_vfinish(const char* fmt, va_list* va)
{
	int R = 0;
	if(fmt)
		R = log_vadd(fmt, *va);

	if((o_e & L_COLOR_VALUE) && COLORHEX(o_e))
	{
		logwrite(NOCOLOR);
	}

	logwrite("\n", 1);

	// flush to stderr
	int __attribute__((unused)) r = write(2, o_space, o_space_l);

	o_e = 0;

	return R;
}

//
// [[ public ]]
//

void log_active(char * s, size_t z)
{
	int l = 0;
	int x;
	for(x = 0; x < o_filter_l; x++)
	{
		if(x)
			l += snprintf(s + l, z - l, " ");

		l += snprintf(s + l, z - l, "%c%c", o_filter[x].o, o_filter[x].m);

		int y;
		int i = 0;
		for(y = 0; y < g_logs_l; y++)
		{
			if(g_logs[y].v & o_filter[x].v)
			{
				if(i++)
					l += snprintf(s + l, z - l, "|");
				l += snprintf(s + l, z - l, "%s", g_logs[y].s);
			}
		}

		l += snprintf(s + l, z - l, "%c"
			, o_filter[x].m == '(' ? ')'
			: o_filter[x].m == '[' ? ']'
			: o_filter[x].m == '{' ? '}'
			: o_filter[x].m == '<' ? '>' : ' '
		);
	}
}

void log_parse(char * args, int args_len)
{
	args_len = args_len ?: strlen(args);

	int x;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == '+' || args[x] == '-')
		{
			char lhs = ' ';
			int off = 1;

			if((x + off) < args_len)
			{
				if(args[x+off] == '(')
					lhs = '(';
				else if(args[x+off] == '{')
					lhs = '{';
				else if(args[x+off] == '[')
					lhs = '[';
				else if(args[x+off] == '<')
					lhs = '<';

				if(lhs != ' ')
					off++;

				if((x + off) < args_len)
				{
					int y;
					for(y = off; (x+y) < args_len; y++)
					{
						if(lhs == ' ' && args[x+y] == ' ')
							break;
						else if(lhs == '(' && args[x+y] == ')')
							break;
						else if(lhs == '{' && args[x+y] == '}')
							break;
						else if(lhs == '[' && args[x+y] == ']')
							break;
						else if(lhs == '<' && args[x+y] == '>')
							break;
					}

					uint64_t tag = 0;

					int k = 0;
					while(k != y)
					{
						if(args[x+off] == ',' || args[x+off] == '|')
							off++;

						for(k = off; k < y; k++)
						{
							if(args[x+k] == ',' || args[x+k] == '|')
								break;
						}

						if(xstrcmp(&args[x+off], k-off, "TAG", 3, 0) == 0)
						{
							// special case for TAG to exclude LWDEBUG
							tag |= L_TAG & ~L_LWDEBUG;
							off += 3;
						}
						else
						{
							int i;
							for(i = 0; i < g_logs_l; i++)
							{
								if(xstrcmp(&args[x+off], k-off, g_logs[i].s, g_logs[i].l, 0) == 0)
									break;
							}

							if(i == g_logs_l)
							{
								break; // nothing
							}
							else
							{
								tag |= g_logs[i].v;
								off += g_logs[i].l;
							}
						}
					}

					if(tag && (off == y))
					{
						if(o_filter_a == o_filter_l)
						{
							int ns = 3;
							if(o_filter_a)
								ns = o_filter_a * 2 + o_filter_a / 2;

							o_filter = realloc(o_filter, sizeof(*o_filter) * ns);
							o_filter_a = ns;
						}

						struct filter * f = &o_filter[o_filter_l++];

						f->v = tag & L_TAG;
						f->o = args[x];
						f->m = lhs == ' ' ? '(' : lhs;
					}
				}
			}
		}
	}
}

int log_init(char * str)
{
	// apply initial args string
	log_parse(str, 0);

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
	int x;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == 0)
			args[x] = ' ';
	}

	// parse cmdline args
	log_parse(args, args_len);

	free(args);

	return 1;
}

int log_would(const uint64_t bits)
{
	if(o_filter_l == 0)
		return 1;

	int r = 0;
	int x;
	for(x = 0; x < o_filter_l; x++)
	{
		int rr = 0;
		if(o_filter[x].m == '(')
			rr = (bits & o_filter[x].v);
		if(o_filter[x].m == '{')
			rr = ((bits & o_filter[x].v) == bits);
		if(o_filter[x].m == '[')
			rr = ((bits & o_filter[x].v) == o_filter[x].v);
		if(o_filter[x].m == '<')
			rr = (bits == o_filter[x].v);

		if(rr)
		{
			if(o_filter[x].o == '+')
				r = 1;
			if(o_filter[x].o == '-')
				r = 0;
		}
	}

	return r;
}

int log_start(const uint64_t e, const char* fmt, ...)
{
	int R = 0;
	if((R = log_vstart(e)))
	{
		va_list va;
		va_start(va, fmt);
		return R + log_vadd(fmt, va);
	}

	return 0;
}

int log_add(const char* fmt, ...)
{
	if(log_would(o_e))
	{
		va_list va;
		va_start(va, fmt);
		return log_vadd(fmt, va);
	}

	return 0;
}

int log_finish(const char* fmt, ...)
{
	if(log_would(o_e))
	{
		va_list va;
		if(fmt)
			va_start(va, fmt);

		return log_vfinish(fmt, &va);
	}

	return 0;
}

int log_written()
{
	return o_space_l;
}

int log(const uint64_t e, const char* fmt, ...)
{
	if(log_vstart(e))
	{
		va_list va;
		va_start(va, fmt);
		log_vadd(fmt, va);

		return log_vfinish(0, 0);
	}

	return 0;
}

void log_teardown()
{
	free(o_space);
	free(o_filter);
}
