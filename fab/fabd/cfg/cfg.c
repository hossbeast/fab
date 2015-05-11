/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>

#include "xlinux.h"

#include "global.h"
#include "cfg/cfg.parse.h"
#include "cfg/cfg.tab.h"
#include "cfg/cfg.lex.h"
#include "cfg/cfg.tokens.h"
#include "cfg/cfg.states.h"

#include "macros.h"
#include "cksum.h"
#include "parseint.h"

#define restrict __restrict

//
// data
//

struct cfg_parser
{
	void * p;
};

//
// [[ static ]]
//

static void cfg_freefile(cfg_file * cfg)
{
	if(cfg)
	{
		for(x = 0; x < cfg->fsl; x++)
			filesystem_free(cfg->fs[x]);

		free(cfg->fs);
	}

	free(cfg);
}

static const char * cfg_tokenname(int token)
{
  return cfg_tokennames[token];
}

static const char * cfg_statename(int state)
{
  return state >= 0 ? cfg_statenames[state] : "";
}

static int cfg_lvalstr(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
	return 0;
}

static int reduce(int (*parser)(void *, parse_param*), parse_param * pp)
{
	// error from the parser means failure to reduce
	int r;
	if((r = parser(pp->scanner, pp)) || pp->scanerr)
	{
		if(r == 2)
		{
			// memory exhaustion error from the parser
			tfail(perrtab_SYS, ENOMEM);
		}
		else if(pp->scanerr)
		{
			// scanner error
			fails(pp->scanerr, pp->error_str);
		}
		else if(pp->gramerr)
		{
			// failure to reduce
			fails(FAB_SYNTAX, pp->error_str);
		}
		else
		{
			// error thrown from a grammar rule (such as ENOMEM)
			fail(0);
		}
	}

finally :
	if(XAPI_UNWINDING)
	{
		if(pp->scanerr || pp->gramerr)
		{
			XAPI_INFOF("location", "[%d,%d - %d,%d]"
				, pp->error_loc.f_lin + 1
				, pp->error_loc.f_col + 1
				, pp->error_loc.l_lin + 1
				, pp->error_loc.l_col + 1
			);

			if(pp->gramerr)
			{
				XAPI_INFOS("token", pp->tokenstring);
			}
		}
	}
coda;
}

static int parse(const cfg_parser * const p, char* b, int sz, const char * const path, cfg_file ** const cfg)
{
#if DEVEL
	int logwould(void * token, void * udata)
	{
		return log_would(*(uint64_t*)token);
	}

	void loglog(void * token, void * udata, const char * func, const char * file, int line, char * fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		log_vlogf(func, file, line, *(uint64_t*)token, fmt, va);
		va_end(va);
	}
#endif

	parse_param pp = {
		  .tokname			= cfg_tokenname
		, .statename		= cfg_statename
		, .lvalstr			= cfg_lvalstr

#if DEVEL
	  , .token_token	= (uint64_t[]) { L_CFGTOKEN }
	  , .token_would	= logwould
	  , .token_log		= loglog
	  , .state_token	= (uint64_t[]) { L_CFGSTATE }
	  , .state_would	= logwould
	  , .state_log		= loglog
#endif
	};

	// create state specific to this parse
	void* state = 0;
	if((state = cfg_yy_scan_bytes(b, sz, p->p)) == 0)
		tfail(perrtab_SYS, ENOMEM);

	pp.scanner = p->p;
	pp.orig_base = b;
	pp.orig_len = sz;

	// make available to the lexer
	cfg_yyset_extra(&pp, p->p);

	// invoke the appropriate parser, raise errors as needed
	fatal(reduce, cfg_yyparse, &pp);

	if(pp.cfg)
	{
		// create copy of the path
		fatal(ixstrdup, &pp.cfg->canpath, path);

		// return the cfg
		if(cfg)
			*cfg = pp.cfg;

		// cfg_files are tracked in cfg_files
		fatal(coll_doubly_add, &cfg_files.c, &pp.cfg, 0);

		// ownership transfer
		pp.cfg = 0;
	}

finally :
	// cleanup state for this parse
	cfg_yy_delete_buffer(state, p->p);
	yyu_extra_destroy(&pp);

	cfg_freefile(pp.cfg);
coda;
}

//
// protected
//
int cfg_mk_fs(struct cfg_file * const restrict e, struct filesystem * const restrict fs, struct cfg_file ** const restrict cfg)
{
	// get or create cfg_file instance
	*cfg = e;

	if(!*cfg)
		fatal(xmalloc, cfg, sizeof(**cfg));

	// append filesystem declaration
	if((*cfg)->fsa == (*cfg)->fsl)
	{
		size_t ns = (*cfg)->fsa ?: 3;
		ns = ns * 2 + ns / 2;
		fatal(xrealloc, &(*cfg)->fs, sizeof(*(*cfg)->fs), ns, (*cfg)->fsa);
		(*cfg)->fsa = ns;
	}

	(*cfg)->fs[(*cfg)->fsl++] = fs;

	finally : coda;
}

/// [[ api/public ]]

int cfg_mkparser(cfg_parser ** const p)
{
	fatal(xmalloc, p, sizeof(**p));
	tfatalize(perrtab_SYS, ENOMEM, cfg_yylex_init, &(*p)->p);

	finally : coda;
}

int cfg_load(const cfg_parser * const restrict p, const char * const restrict path, cfg_file ** restrict cfg)
{
	int fd = -1;
	char * b = 0;
	cfg_file * lcfg = 0;

	if(cfg == 0)
		cfg = &lcfg;

	cfg_file ** c = 0;
	if((c = map_get(cfg_files.by_canpath, MMS(path))))
	{
		*cfg = *c;
	}
	else
	{
		// open the file
		fatal(xopen, path, O_RDONLY, &fd);

		// snarf the file
		struct stat statbuf;
		fatal(xfstat, fd, &statbuf);
		fatal(xmalloc, &b, statbuf.st_size + 2);
		fatal(axread, fd, b, statbuf.st_size);

		// parse the file
		fatal(parse, p, b, statbuf.st_size, path, cfg);

		// save it
		fatal(map_set, cfg_files.by_canpath, MMS(path), cfg, sizeof(*cfg), 0);
	}

finally:
	free(b);
	if(fd != -1)
		close(fd);

	XAPI_INFOS("path", path);
coda;
}

void cfg_freeparser(cfg_parser* const p)
{
	if(p)
	{
		cfg_yylex_destroy(p->p);
	}

	free(p);
}

void cfg_xfreeparser(cfg_parser ** const p)
{
	cfg_freeparser(*p);
	*p = 0;
}

int cfg_init()
{
	fatal(map_create, &cfg_files.by_canpath, 0);

	finally : coda;
}

void cfg_teardown()
{
	int x;
	for(x = 0; x < cfg_files.l; x++)
		cfg_freefile(cfg_files.e[x]);

	free(cfg_files.e);
	map_free(cfg_files.by_canpath);
}

int cfg_finalize()
{
	finally : coda;
}
