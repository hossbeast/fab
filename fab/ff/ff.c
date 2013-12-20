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
#include "xmem.h"
#include "macros.h"
#include "cksum.h"
#include "xstring.h"
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

static void ff_log_error(char * fmt, ...)
{
  va_list va;
  va_start(va, fmt);
	vlog(L_ERROR, fmt, va);
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

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, struct gn * dscv_gn, const int * const var_id, const int * const list_id, ff_file ** const rff, char * const nofile, const int nofilel)
{
	parse_param pp = {
	    .log_token		= ff_log_token
		, .log_state		= ff_log_state
		, .log_error		= ff_log_error
		, .tokname			= ff_tokenname
		, .statename		= ff_statename
		, .inputstr			= ff_inputstr
		, .lvalstr			= ff_lvalstr
	};

#if DEBUG
	// causes yyerror to include the scanner line number where the last token was lexed
	pp.output_line = 1;
#endif

	// create state specific to this parse
	void* state = 0;
	if((state = ff_yy_scan_bytes(b, sz + 2, p->p)) == 0)
		fail("scan_bytes failed");

	// all ff_files are tracked in ff_files
	ff_file * ff = 0;
	fatal(coll_doubly_add, &ff_files.c, 0, &ff);

	// create copy of the path
	fatal(path_copy, &ff->path, in_path);

	if(dscv_gn)
	{
		ff->type = FFT_DDISC;
		ff->dscv_gn = dscv_gn;

		// idstring
		fatal(xsprintf, &ff->idstring, "DSC:%s", gn_idstring(ff->dscv_gn));
	}
	else if(var_id)
	{
		ff->type = FFT_VAREXPR;
		ff->id = *var_id;

		fatal(xsprintf, &ff->idstring, "VAR:%s", ff->path->can);
	}
	else if(list_id)
	{
		ff->type = FFT_LISTEXPR;
		ff->id = *list_id;

		fatal(xsprintf, &ff->idstring, "LIST:%s", ff->path->can);
	}
	else
	{
		ff->type = FFT_REGULAR;
		if(nofile)
			fatal(xsprintf, &ff->nofile, "%.*s", nofilel, nofile);

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
		fatal(xsprintf, &ff->closure_gns_dir
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

	// return value from yyparse - whether the input was reduced according to the grammar
	int r = 0;
	if(ff->type == FFT_DDISC)
	{
		r = ff_dsc_yyparse(p->p, &pp);	// parse with ff/ff.dsc.y
	}
	else if(ff->type == FFT_VAREXPR)
	{
		r = ff_var_yyparse(p->p, &pp);	// parse with ff/ff.var.y
	}
	else if(ff->type == FFT_LISTEXPR)
	{
		r = ff_list_yyparse(p->p, &pp);	// parse with ff/ff.list.y
	}
	else
	{
		r = ff_yyparse(p->p, &pp);			// parse with ff/ff.y
	}

	// in addition, pp.r is nonzero whenever yyerror has been called, which covers a few more
	// cases than failure-to-reduce, such as when the scanner encounters invalid byte(s)
	r |= pp.r;

	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);

	if(r != 0)
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
	yyu_extra_destroy(&pp);
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
		if(symlink(tmpa, tmpb) != 0 && errno != EEXIST)
			fail("symlink=[%d][%s]", errno, strerror(errno));
	}

	if((dh = opendir(ff->closure_gns_dir)) == 0)
		fail("opendir(%s)=[%d][%s]", ff->closure_gns_dir, errno, strerror(errno));

	struct dirent ent;
	struct dirent * entp = 0;
	while(1)
	{
		fatal_os(readdir_r, dh, &ent, &entp);

		if(!entp)
			break;

		if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
		{
			// get the canhash for this gn
			uint32_t canhash = 0;
			if(parseuint(entp->d_name, SCNu32, 1, 0xFFFFFFFF, 1, UINT8_MAX, &canhash, 0) != 0)
				fail("unexpected file %s/%s", ff->closure_gns_dir, entp->d_name);

			// delete
			snprintf(tmpa, sizeof(tmpa), "%s/%u/PRIMARY/dscv", ff->closure_gns_dir, canhash);
			if(unlink(tmpa) != 0 && errno != ENOENT)
				fail("unlink(%s)=[%d][%s]", tmpa, errno, strerror(errno));

			snprintf(tmpa, sizeof(tmpa), "%s/%u/SECONDARY/fab/noforce_ff", ff->closure_gns_dir, canhash);
			if(unlink(tmpa) != 0 && errno != ENOENT)
				fail("unlink(%s)=[%d][%s]", tmpa, errno, strerror(errno));

			// If it is no longer in the closure, also delete the symlink
			int kcmp(const void * K, const void * A)
			{
				return *(uint32_t*)K - (*(struct gn **)A)->path->can_hash;
			};

			if(bsearch(&canhash, ff->closure_gns, ff->closure_gnsl, sizeof(*ff->closure_gns), kcmp) == 0)
			{
				snprintf(tmpa, sizeof(tmpa), "%s/%u", ff->closure_gns_dir, canhash);
				fatal_os(unlink, tmpa);
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
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 1;

	if(ff_yylex_init(&(*p)->p) != 0)
		return 1;

	if(generator_mkparser(&(*p)->gp) != 0)
		return 1;

	return 0;
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
		if((fd = open(in_path->abs, O_RDONLY)) == -1)
			fail("open(%s)=[%d][%s]", in_path->abs, errno, strerror(errno));

		// snarf the file
		struct stat statbuf;
		fatal_os(fstat, fd, &statbuf);

		fatal(xmalloc, &b, statbuf.st_size + 2);
		ssize_t r = 0;
		if((r = read(fd, b, statbuf.st_size)) != statbuf.st_size)
			fail("read(%s,%d)=%d [%d][%s]", in_path->abs, (int)statbuf.st_size, (int)r, errno, strerror(errno));

		fatal(parse, p, b, statbuf.st_size, in_path, 0, 0, 0, ff, nofile, nofilel);

		if(ff == 0)
		{
			fail("");
		}

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

	if(*ff == 0)
	{
		fail("");
	}

finally:
	path_free(pth);
coda;
}

int ff_list_parse(const ff_parser * const p, char* b, int sz, int id, ff_file ** const ff)
{
	path * pth = 0;
	fatal(path_create, &pth, "/../FABSYS/cmdline/l", "%d", id);
	fatal(parse, p, b, sz, pth, 0, 0, &id, ff, 0, 0);

	if(*ff == 0)
	{
		fail("");
	}

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
