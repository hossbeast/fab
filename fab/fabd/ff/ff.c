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
#include "ff/ff.parse.h"
#include "ff/ff.tab.h"
#include "ff/ff.dsc.tab.h"
#include "ff/ff.var.tab.h"
#include "ff/ff.list.tab.h"
#include "ff/ff.lex.h"
#include "ff/ff.tokens.h"
#include "ff/ff.states.h"

#include "macros.h"
#include "cksum.h"
#include "parseint.h"

#define restrict __restrict

//
// data
//

union ff_files_t ff_files = { { .size = sizeof(ff_file) } };

//
// [[ static ]]
//

static void ff_freefile(ff_file * ff)
{
	if(ff)
	{
		free(ff->nofile);

		if(ff->type == FFT_DDISC)
		{
			// no-op
		}
		if(ff->type == FFT_REGULAR)
		{
			hashblock_free(ff->hb);
		}

		path_free(ff->path);
		free(ff->idstring);
		ffn_free(ff->ffn);
	}

	free(ff);
}

static const char * ff_tokenname(int token)
{
  return ff_tokennames[token];
}

static const char * ff_statename(int state)
{
  return state >= 0 ? ff_statenames[state] : "";
}

static int ff_inputstr(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
  parse_param * pp = (parse_param*)xtra;
	
	if(pp->ff)
	{
		*buf = pp->ff->idstring;
		*bufl = strlen(*buf);
	}
	
	return 0;
}

static int ff_lvalstr(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
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

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, struct gn * dscv_gn, const int * const var_id, const int * const list_id, ff_file ** const rff, char * const nofile, const int nofilel)
{
	ff_file * ff = 0;

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
		  .tokname			= ff_tokenname
		, .statename		= ff_statename
		, .inputstr			= ff_inputstr
		, .lvalstr			= ff_lvalstr

#if DEVEL
	  , .token_token	= (uint64_t[]) { L_FFTOKEN }
	  , .token_would	= logwould
	  , .token_log		= loglog
	  , .state_token	= (uint64_t[]) { L_FFSTATE }
	  , .state_would	= logwould
	  , .state_log		= loglog
#endif
	};

	uint32_t type = FFT_REGULAR;
	if(dscv_gn)
		type = FFT_DDISC;
	else if(var_id)
		type = FFT_VAREXPR;
	else if(list_id)
		type = FFT_LISTEXPR;

	// create state specific to this parse
	void* state = 0;
	if((state = ff_yy_scan_bytes(b, sz, p->p)) == 0)
		tfail(perrtab_SYS, ENOMEM);

	// allocate
	fatal(xmalloc, &ff, sizeof(*ff));

	// create copy of the path
	fatal(path_copy, &ff->path, in_path);

	ff->type = type;
	if(dscv_gn)
	{
		ff->dscv_gn = dscv_gn;

		// idstring
		fatal(ixsprintf, &ff->idstring, "%s", ff->dscv_gn->idstring);
	}
	else if(var_id)
	{
		ff->id = *var_id;

		fatal(ixsprintf, &ff->idstring, "VAR:%s", ff->path->can);
	}
	else if(list_id)
	{
		ff->id = *list_id;

		fatal(ixsprintf, &ff->idstring, "LIST:%s", ff->path->can);
	}
	else
	{
		if(nofile)
			fatal(ixsprintf, &ff->nofile, "%.*s", nofilel, nofile);

		// idstring
		if(ff->nofile && g_args->mode_gnid != MODE_CANONICAL)
		{
			ff->idstring = strdup(ff->nofile);
		}
		else if(g_args->mode_gnid == MODE_CANONICAL)
		{
			ff->idstring = strdup(ff->path->can);
		}
		else if(g_args->mode_gnid == MODE_RELATIVE_CWD)
		{
			ff->idstring = strdup(ff->path->rel_cwd);
		}
		else if(g_args->mode_gnid == MODE_RELATIVE_FABFILE_DIR)
		{
			ff->idstring = strdup(ff->path->rel_fab);
		}
	}

	pp.scanner = p->p;
	pp.orig_base = b;
	pp.orig_len = sz;

	// make available to the lexer
	ff_yyset_extra(&pp, p->p);

	// invoke the appropriate parser, raise errors as needed
	if(ff->type == FFT_DDISC)
		fatal(reduce, ff_dsc_yyparse, &pp);
	else if(ff->type == FFT_VAREXPR)
		fatal(reduce, ff_var_yyparse, &pp);
	else if(ff->type == FFT_LISTEXPR)
		fatal(reduce, ff_list_yyparse, &pp);
	else if(ff->type == FFT_REGULAR)
		fatal(reduce, ff_yyparse, &pp);

	if(ff)
	{
		ff->ffn = pp.ffn;

		fatal(ffn_postprocess, ff->ffn, ff, p->gp);

		if(ff->type == FFT_REGULAR)
		{
			// create hashblock
			fatal(hashblock_create, &ff->hb);

			ff_dump(ff);
		}
		else if(ff->type == FFT_VAREXPR)
		{
			ff_dump(ff);
		}

		// regular ff_files are tracked in ff_files
		if(dscv_gn == 0 && var_id == 0 && list_id == 0)
			fatal(coll_doubly_add, &ff_files.c, &ff, 0);

		(*rff) = ff;
		ff = 0;
	}

finally :
	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);
	yyu_extra_destroy(&pp);

	ff_freefile(ff);

if(type != FFT_REGULAR)
	XAPI_INFOS("type", FFT_STRING(type));
coda;
}

/// [[ api/public ]]

int ff_mkparser(ff_parser ** const p)
{
	fatal(xmalloc, p, sizeof(**p));
	tfatalize(perrtab_SYS, ENOMEM, ff_yylex_init, &(*p)->p);
	fatal(transform_mkparser, &(*p)->gp);

	finally : coda;
}

int ff_reg_load(const ff_parser * const p, const path * const in_path, char * const nofile, const int nofilel, ff_file ** const ff)
{
	int			fd = -1;
	char *	b = 0;

	if(!ff_files.by_canpath)
		fatal(map_create, &ff_files.by_canpath, 0);

	ff_file ** c = 0;
	c = map_get(ff_files.by_canpath, in_path->can, in_path->canl);
	if(c)
	{
		*ff = *c;
	}
	else
	{
		// open the file
		fatal(xopen, in_path->abs, O_RDONLY, &fd);

		// snarf the file
		struct stat statbuf;
		fatal(xfstat, fd, &statbuf);
		fatal(xmalloc, &b, statbuf.st_size + 2);
		fatal(axread, fd, b, statbuf.st_size);

		// parse the file
		fatal(parse, p, b, statbuf.st_size, in_path, 0, 0, 0, ff, nofile, nofilel);

		// save it
		fatal(map_set, ff_files.by_canpath, in_path->can, in_path->canl, ff, sizeof(*ff), 0);
	}

finally:
	free(b);
	if(fd != -1)
		close(fd);

	XAPI_INFOS("path", in_path->abs);
coda;
}

int ff_dsc_parse(const ff_parser * const p, char* b, int sz, const char * const fp, struct gn * dscv_gn, ff_file** const ff)
{
	path * pth = 0;
	fatal(path_create, &pth, "/../FABSYS/dscv", "%s", fp);
	fatal(parse, p, b, sz, pth, dscv_gn, 0, 0, ff, 0, 0);

finally:
	path_free(pth);
XAPI_INFOS("gn", dscv_gn->idstring);
coda;
}

int ff_var_parse(const ff_parser * const p, char* b, int sz, int id, ff_file** const ff)
{
	path * pth = 0;
	fatal(path_create, &pth, "/../FABSYS/cmdline/v", "%d", id);
	fatal(parse, p, b, sz, pth, 0, &id, 0, ff, 0, 0);

finally:
	path_free(pth);
coda;
}

int ff_list_parse(const ff_parser * const p, char* b, int sz, int id, ff_file ** const ff)
{
	path * pth = 0;
	fatal(path_create, &pth, "/../FABSYS/cmdline/l", "%d", id);
	fatal(parse, p, b, sz, pth, 0, 0, &id, ff, 0, 0);

finally:
	path_free(pth);
coda;
}

void ff_freeparser(ff_parser* const p)
{
	if(p)
	{
		ff_yylex_destroy(p->p);
		transform_parser_free(p->gp);
	}

	free(p);
}

void ff_xfreeparser(ff_parser ** const p)
{
	ff_freeparser(*p);
	*p = 0;
}

char * ff_idstring(ff_file * const ff)
{
	return ff->idstring;
}

void ff_teardown()
{
	int x;
	for(x = 0; x < ff_files.l; x++)
	{
		ff_freefile(ff_files.e[x]);
	}

	free(ff_files.e);
	map_free(ff_files.by_canpath);
}

void ff_dump(ff_file * const ff)
{
#if DEBUG || DEVEL
	if(log_would(L_FFFILE))
	{
		logf(L_FFFILE			, "%20s : %s", "idstring"						, ff->idstring);
		logf(L_FFFILE			, "%20s : %s", "type"								, FFT_STRING(ff->type));
		logf(L_FFFILE			, "%20s : %s", "can-path"						, ff->path->can);
		logf(L_FFFILE			, "%20s : %s", "in-path"						, ff->path->in_path);
		logf(L_FFFILE			, "%20s : %s", "in-base"						, ff->path->in_base);
		logf(L_FFFILE			, "%20s : %s", "abs-path"						, ff->path->abs);
		logf(L_FFFILE			, "%20s : %s", "rel-cwd-path"				, ff->path->rel_cwd);
		logf(L_FFFILE			, "%20s : %s", "rel-fab-path"				, ff->path->rel_fab);
		logf(L_FFFILE			, "%20s : %s", "rel-nofile-path"		, ff->path->rel_nofile);
		if(ff->type == FFT_DDISC)
		{
			logf(L_FFFILE		, "%20s : %s", "dscv-gn", ff->dscv_gn);
		}
	}

	logf(L_FFFILE			, "%20s :", "tree");
	ffn_dump(ff->ffn);
	logs(L_FFFILE, "");
#endif
}
