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
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>

#include "ff.h"
#include "ff.parse.h"
#include "ff/ff.tab.h"
#include "ff/ff.dsc.tab.h"
#include "ff/ff.var.tab.h"
#include "ff/ff.list.tab.h"
#include "ff/ff.lex.h"
#include "ff/ff.tokens.h"
#include "ff/ff.states.h"

#include "args.h"
#include "params.h"
#include "identity.h"
#include "gn.h"
#include "enclose.h"

#include "log.h"
#include "global.h"
#include "xlinux.h"
#include "macros.h"
#include "cksum.h"
#include "dirutil.h"
#include "parseint.h"

#define restrict __restrict

// defined in gn.c
char * gn_idstring(struct gn * const);

//
// data
//

union ff_files_t ff_files = { { .size = sizeof(ff_file) } };

//
// [[ static ]]
//

static void ff_log_token(char * fmt, ...)
{
  va_list va;
  va_start(va, fmt);
	vlog(L_FFTOKN, fmt, va);
  va_end(va);
}

static void ff_log_state(char * fmt, ...)
{
  va_list va;
  va_start(va, fmt);
	vlog(L_FFSTAT, fmt, va);
  va_end(va);
}

static const char * ff_tokenname(int token)
{
  return ff_tokennames[token];
}

static const char * ff_statename(int state)
{
  return ff_statenames[state];
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
			XAPI_INFOS("last", pp->tokenstring);
			XAPI_INFOF("loc", "[%d,%d - %d,%d]"
				, pp->last_loc.f_lin + 1
				, pp->last_loc.f_col + 1
				, pp->last_loc.l_lin + 1
				, pp->last_loc.l_col + 1
			);
		}
	}
coda;
}

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, struct gn * dscv_gn, const int * const var_id, const int * const list_id, ff_file ** const rff, char * const nofile, const int nofilel)
{
	parse_param pp = {
	    .log_token		= ff_log_token
		, .log_state		= ff_log_state
		, .tokname			= ff_tokenname
		, .statename		= ff_statename
		, .inputstr			= ff_inputstr
		, .lvalstr			= ff_lvalstr
	};

#if DEBUG
	// causes yyerror to include the scanner line number in FFTOKN output
	pp.line_numbering = 1;
#endif

	ff_file * ff = 0;
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

	// all ff_files are tracked in ff_files
	fatal(coll_doubly_add, &ff_files.c, 0, &ff);

	// create copy of the path
	fatal(path_copy, &ff->path, in_path);

	ff->type = type;

	if(dscv_gn)
	{
		ff->dscv_gn = dscv_gn;

		// idstring
		fatal(ixsprintf, &ff->idstring, "DSC:%s", gn_idstring(ff->dscv_gn));
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
		if(ff->nofile && g_args.mode_gnid != MODE_CANONICAL)
		{
			ff->idstring = strdup(ff->nofile);
		}
		else if(g_args.mode_gnid == MODE_CANONICAL)
		{
			ff->idstring = strdup(ff->path->can);
		}
		else if(g_args.mode_gnid == MODE_RELATIVE_CWD)
		{
			ff->idstring = strdup(ff->path->rel_cwd);
		}
		else if(g_args.mode_gnid == MODE_RELATIVE_FABFILE_DIR)
		{
			ff->idstring = strdup(ff->path->rel_fab);
		}

		// closure dir
		fatal(ixsprintf, &ff->closure_gns_dir
			, XQUOTE(FABCACHEDIR) "/INIT/%u/ff/%u/REGULAR/closure_gns"
			, g_params.init_fabfile_path->can_hash
			, ff->path->can_hash
		);
	}

	pp.scanner = p->p;
	pp.ff = ff;
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

		fatal(ffn_postprocess, ff->ffn, p->gp);

		if(ff->type == FFT_REGULAR)
		{
			// create VARREF closure
			fatal(enclose_vars, ff->ffn, &ff->closure_vars, &ff->closure_varsa, &ff->closure_varsl);

			// create hashblock
			fatal(hashblock_create, &ff->hb
				, XQUOTE(FABCACHEDIR) "/INIT/%u/ff/%u/REGULAR"
				, g_params.init_fabfile_path->can_hash
				, ff->path->can_hash
			);

			// load previous hashblock into [0]
			fatal(hashblock_read, ff->hb);

			// stat the file, populate [1] - now ready for hashblock_cmp
			fatal(hashblock_stat, ff->path->abs, ff->hb, 0, 0);
			ff->hb->vrshash[1] = FABVERSIONN;
			ff_dump(ff);
		}

		(*rff) = ff;
	}

finally :
	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);
	yyu_extra_destroy(&pp);

XAPI_INFOS("type", FFT_STRING(type));
coda;
}

static int regular_rewrite(ff_file * ff)
{
	char tmpa[512];
	char tmpb[512];

	DIR * dh = 0;

	fatal(identity_assume_fabsys);

	// sort the affected gn list by canonical path hash
	int cmp(const void * _A, const void * _B)
	{
		uint32_t A = (*(struct gn **)_A)->path->can_hash;
		uint32_t B = (*(struct gn **)_B)->path->can_hash;

		if(A > B) return 1;
		else if(B > A) return -1;

		return 0;
	};
	qsort(ff->closure_gns, ff->closure_gnsl, sizeof(*ff->closure_gns), cmp);

	// ensure closure directory exists
	fatal(mkdirp, ff->closure_gns_dir, S_IRWXU | S_IRWXG | S_IRWXO);

	// create links to all nodes that are connected to this regular fabfile
	int x;
	for(x = 0; x < ff->closure_gnsl; x++)
	{
		// symlink to the gn
		snprintf(tmpa, sizeof(tmpa), XQUOTE(FABCACHEDIR) "/INIT/%u/gn/%u", g_params.init_fabfile_path->can_hash, ff->closure_gns[x]->path->can_hash);
		snprintf(tmpb, sizeof(tmpb), "%s/%u", ff->closure_gns_dir, ff->closure_gns[x]->path->can_hash);

		fatal(uxsymlink, tmpa, tmpb);
	}

	fatal(xopendir, ff->closure_gns_dir, &dh);

	struct dirent ent;
	struct dirent * entp = 0;
	while(1)
	{
		fatal(xreaddir_r, dh, &ent, &entp);

		if(!entp)
			break;

		if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
		{
			// get the canhash for this gn
			uint32_t canhash = 0;
			if(parseuint(entp->d_name, SCNu32, 1, 0xFFFFFFFF, 1, UINT8_MAX, &canhash, 0) != 0)
				failf(FAB_BADCACHE, "unexpected file %s/%s", ff->closure_gns_dir, entp->d_name);

			// delete
			snprintf(tmpa, sizeof(tmpa), "%s/%u/PRIMARY/dscv", ff->closure_gns_dir, canhash);
			fatal(uxunlink, tmpa, 0);

			snprintf(tmpa, sizeof(tmpa), "%s/%u/SECONDARY/fab/noforce_ff", ff->closure_gns_dir, canhash);
			fatal(uxunlink, tmpa, 0);

			// If it is no longer in the closure, also delete the symlink
			int kcmp(const void * K, const void * A)
			{
				return *(uint32_t*)K - (*(struct gn **)A)->path->can_hash;
			};

			if(bsearch(&canhash, ff->closure_gns, ff->closure_gnsl, sizeof(*ff->closure_gns), kcmp) == 0)
			{
				snprintf(tmpa, sizeof(tmpa), "%s/%u", ff->closure_gns_dir, canhash);
				fatal(xunlink, tmpa, 0);
			}
		}
	}

	// rewrite the hashblock (reverts to user identity)
	fatal(hashblock_write, ff->hb);

finally:
	if(dh)
		closedir(dh);
coda;
}

/// [[ api/public ]]

int ff_mkparser(ff_parser ** const p)
{
	fatal(xmalloc, p, sizeof(**p));
	tfatalize(perrtab_SYS, ENOMEM, ff_yylex_init, &(*p)->p);
	fatal(generator_mkparser, &(*p)->gp);

	finally : coda;
}

int ff_reg_load(const ff_parser * const p, const path * const in_path, char * const nofile, const int nofilel, ff_file ** const ff)
{
	int			fd = 0;
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
		fatal(axread, fd, b, statbuf.st_size, 0);

		// parse the file
		fatal(parse, p, b, statbuf.st_size, in_path, 0, 0, 0, ff, nofile, nofilel);

		// save it
		fatal(map_set, ff_files.by_canpath, in_path->can, in_path->canl, ff, sizeof(*ff), 0);
	}

finally:
	free(b);
	if(fd > 0)
		close(fd);
coda;
}

int ff_dsc_parse(const ff_parser * const p, char* b, int sz, const char * const fp, struct gn * dscv_gn, ff_file** const ff)
{
	path * pth = 0;
	fatal(path_create, &pth, "/../FABSYS/dscv", "%s", fp);
	fatal(parse, p, b, sz, pth, dscv_gn, 0, 0, ff, 0, 0);

finally:
	path_free(pth);
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
		generator_parser_free(p->gp);
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

			free(ff->closure_gns_dir);
			free(ff->closure_gns);
			free(ff->closure_vars);
		}

		path_free(ff->path);
		free(ff->idstring);
		ffn_free(ff->ffn);
	}

	free(ff);
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

int ff_regular_reconcile()
{
	int x;
	for(x = 0; x < ff_files.l; x++)
	{
		if(ff_files.e[x]->type == FFT_REGULAR)
		{
			if(hashblock_cmp(ff_files.e[x]->hb))
			{
				fatal(regular_rewrite, ff_files.e[x]);
			}
		}
	}

	finally : coda;
}

int ff_regular_enclose_gn(struct ff_file * const ff, gn * const gn)
{
	int newa = 0;
	fatal(gn_enclose_ff, gn, ff, &newa);

	if(newa)
	{
		if(ff->closure_gnsl >= ff->closure_gnsa)
		{
			int ns = ff->closure_gnsa ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &ff->closure_gns, sizeof(*ff->closure_gns), ns, ff->closure_gnsa);
			ff->closure_gnsa = ns;
		}

		ff->closure_gns[ff->closure_gnsl++] = gn;
	}

	finally : coda;	
}

void ff_dump(ff_file * const ff)
{
	int x;
	if(log_would(L_FF | L_FFFILE))
	{
		log(L_FF | L_FFFILE			, "%20s : %s", "idstring"						, ff->idstring);
		log(L_FF | L_FFFILE			, "%20s : %s", "type"								, FFT_STRING(ff->type));
		log(L_FF | L_FFFILE			, "%20s : %s", "can-path"						, ff->path->can);
		log(L_FF | L_FFFILE			, "%20s : %s", "in-path"						, ff->path->in_path);
		log(L_FF | L_FFFILE			, "%20s : %s", "in-base"						, ff->path->in_base);
		log(L_FF | L_FFFILE			, "%20s : %s", "abs-path"						, ff->path->abs);
		log(L_FF | L_FFFILE			, "%20s : %s", "rel-cwd-path"				, ff->path->rel_cwd);
		log(L_FF | L_FFFILE			, "%20s : %s", "rel-fab-path"				, ff->path->rel_fab);
		log(L_FF | L_FFFILE			, "%20s : %s", "rel-nofile-path"		, ff->path->rel_nofile);
		if(ff->type == FFT_REGULAR)
		{
			log(L_FF | L_FFFILE		, "%20s : %d", "closure-gns", ff->closure_gnsl);
			for(x = 0; x < ff->closure_gnsl; x++)
			{
				log(L_FF | L_FFFILE , "  %20s : %s", "", ff->closure_gns[x]->idstring);
			}

			log(L_FF | L_FFFILE		, "%20s : %d", "closure-vars", ff->closure_varsl);
			for(x = 0; x < ff->closure_varsl; x++)
			{
				log(L_FF | L_FFFILE	, "  %20s : %s", "", ff->closure_vars[x]->text);
			}
		}
		else if(ff->type == FFT_DDISC)
		{
			log(L_FF | L_FFFILE		, "%20s : %s", "dscv-gn", ff->dscv_gn);
		}
	}

	log(L_FF | L_FFFILE			, "%20s :", "tree");
	ffn_dump(ff->ffn);

	log(L_FF | L_FFFILE, "");
}
