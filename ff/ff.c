#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <inttypes.h>

#include "ff.h"
#include "ff.parse.h"
#include "ff/ff.tab.h"
#include "ff/ff.lex.h"
#include "ff/ff.tokens.h"

#include "args.h"
#include "identity.h"
#include "gn.h"
#include "enclose.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"
#include "cksum.h"
#include "xstring.h"
#include "dirutil.h"

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

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, struct gn * dscv_gn, ff_file ** const rff)
{
	parse_param pp = {};

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
		xsprintf(&ff->idstring, "DSC:%s", gn_idstring(ff->dscv_gn));
	}
	else
	{
		ff->type = FFT_REGULAR;

		// idstring
		if(g_args.mode_gnid == MODE_GNID_CANON)
		{
			ff->idstring = strdup(ff->path->can);
		}
		else if(g_args.mode_gnid == MODE_GNID_RELATIVE)
		{
			ff->idstring = strdup(ff->path->rel);
		}

		// closure dir
		fatal(xsprintf, &ff->closure_gns_dir, "%s/REGULAR/%u/closure_gns", FF_DIR_BASE, ff->path->can_hash);
	}

	pp.scanner = p->p;
	pp.ff = ff;
	pp.orig_base = b;
	pp.orig_len = sz;
	pp.r = 1;

	fatal(xmalloc, &pp.loc, sizeof(*pp.loc));
	fatal(xmalloc, &pp.last_loc, sizeof(*pp.last_loc));

	// make available to the lexer
	ff_yyset_extra(&pp, p->p);

	if(ff->type == FFT_DDISC)
	{
		ff_dsc_yyparse(p->p, &pp);	// parse with ff/ff.dsc.y
	}
	else
	{
		ff_yyparse(p->p, &pp);			// parse with ff/ff.y
	}

	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);

	if(pp.r)
	{
		ff->ffn = pp.ffn;

		if(ffn_postprocess(ff->ffn, p->gp) == 0)
			qfail();

		if(ff->type == FFT_REGULAR)
		{
			// create VARREF closure
			fatal(enclose_vars, ff->ffn, &ff->closure_vars, &ff->closure_varsa, &ff->closure_varsl);

			// create hashblock
			fatal(hashblock_create, &ff->hb, "%s/REGULAR/%u", FF_DIR_BASE, ff->path->can_hash);

			// load previous hashblock into [0]
			fatal(hashblock_read, ff->hb);

			// stat the file, populate [1] - now ready for hashblock_cmp
			fatal(hashblock_stat, ff->path->abs, ff->hb, 0, 0);
			ff->hb->vrshash[1] = FAB_VERSION;
			ff_dump(ff);
		}

		(*rff) = ff;
	}

finally:
	free(pp.loc);
	free(pp.last_loc);
coda;
}

/// [[ api/public ]]

int ff_mkparser(ff_parser ** const p)
{
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 0;

	if(ff_yylex_init(&(*p)->p) != 0)
		return 0;

	if(generator_mkparser(&(*p)->gp) == 0)
		return 0;

	return 1;
}

int ff_reg_load(const ff_parser * const p, const path * const in_path, ff_file ** const ff)
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
			fail("open(%s) failed : [%d][%s]", in_path->abs, errno, strerror(errno));

		// snarf the file
		struct stat statbuf;
		fatal_os(fstat, fd, &statbuf);

		fatal(xmalloc, &b, statbuf.st_size + 2);
		ssize_t r = 0;
		if((r = read(fd, b, statbuf.st_size)) != statbuf.st_size)
			fail("read, expected: %d, actual: %d", (int)statbuf.st_size, (int)r);

		qfatal(parse, p, b, statbuf.st_size, in_path, 0, ff);

		if(*ff)
		{
			fatal(map_set, ff_files.by_canpath, in_path->can, in_path->canl, ff, sizeof(*ff));
		}
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
	fatal(path_create_canon, &pth, "%s", fp);
	qfatal(parse, p, b, sz, pth, dscv_gn, ff);

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

void ff_yyerror(void* loc, void * scanner, parse_param* pp, char const *err)
{
	pp->r = 0;
	log(L_ERROR | L_FF, "%s", err);

	int t						= pp->last_tok;
	const char * s	= pp->last_s;
	const char * e	= pp->last_e;
	int l						= e - s;
	ff_loc * lc			= pp->last_loc;

	// log last good token
	log(L_ERROR | L_FF, "last token - %s '%.*s' @ (%s)[%3d,%3d - %3d,%3d]"
		, ff_tokname(pp->last_tok)
		, MIN(t == ff_LF ? 0 : t == ff_WS ? 0 : l, 50)
		, t == ff_LF ? "" : t == ff_WS ? "" : s
		, ff_idstring(pp->ff)
		, lc->f_lin + 1
		, lc->f_col + 1
		, lc->l_lin + 1
		, lc->l_col + 1
	);
}

char * ff_idstring(ff_file * const ff)
{
	return ff->idstring;
}

static void ff_freefile(ff_file * ff)
{
	if(ff)
	{
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

int ff_regular_rewrite(ff_file * ff)
{
	char to[512];
	char from[512];

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

	// ensure affected directory exists
	fatal(mkdirp, ff->closure_gns_dir, S_IRWXU | S_IRWXG | S_IRWXO);

	// write links to all nodes that are connected to this regular fabfile
	int x;
	for(x = 0; x < ff->closure_gnsl; x++)
	{
		// directory for the affected node
		snprintf(to, sizeof(to), "%s/%u", ff->closure_gns_dir, ff->closure_gns[x]->path->can_hash);
		fatal(mkdirp, to, S_IRWXU | S_IRWXG | S_IRWXO);
		
		// with a link to its PRIMARY and SECONDARY directories
		snprintf(to, sizeof(to), "%s/PRIMARY/%u", GN_DIR_BASE, ff->closure_gns[x]->path->can_hash);
		snprintf(from, sizeof(from), "%s/%u/PRIMARY", ff->closure_gns_dir, ff->closure_gns[x]->path->can_hash);
		if(symlink(to, from) != 0 && errno != EEXIST)
			fail("symlink failed : [%d][%s]", errno, strerror(errno));

		snprintf(to, sizeof(to), "%s/SECONDARY/%u", GN_DIR_BASE, ff->closure_gns[x]->path->can_hash);
		snprintf(from, sizeof(from), "%s/%u/SECONDARY", ff->closure_gns_dir, ff->closure_gns[x]->path->can_hash);
		if(symlink(to, from) != 0 && errno != EEXIST)
			fail("symlink failed : [%d][%s]", errno, strerror(errno));
	}

	// follow ALL links (pre-existing and newly-created)
	int fn(const char * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
	{
		if(ftwbuf->level == 3)
		{
			// force fab and dscv
			int fpl = strlen(fpath);

			if(fpl >= 3 && strcmp(fpath + fpl - 3, "fab") == 0)
			{
				if(rmdir_recursive(fpath, 0) == 0)
					return FTW_STOP;

				return FTW_SKIP_SUBTREE;
			}
			else if(fpl > 4 && strcmp(fpath + fpl - 4, "dscv") == 0)
			{
				if(rmdir_recursive(fpath, 0) == 0)
					return FTW_STOP;

				return FTW_SKIP_SUBTREE;
			}
		}
		else if(ftwbuf->level == 1)
		{
			// This is the directory for a gn. If it is no longer in the affected
			// list, delete the directory as well

			uint32_t canhash = 0;
			int n = 0;

			if(sscanf(fpath + ftwbuf->base, "%u%n", &canhash, &n) == 1 && canhash && n > 0)
			{
				int kcmp(const void * K, const void * A)
				{
					return *(uint32_t*)K - (*(struct gn **)A)->path->can_hash;
				};

				if(bsearch(&canhash, ff->closure_gns, ff->closure_gnsl, sizeof(*ff->closure_gns), kcmp) == 0)
				{
					if(rmdir_recursive(fpath, 1) == 0)
					{
						return FTW_STOP;
					}
				}
			}
			else
			{
				// unexpected file
				log(L_WARN, "unexpected file %s - %u, %d", fpath, canhash, n);
			}
		}
		
		return FTW_CONTINUE;
	};

	fatal_os(nftw, ff->closure_gns_dir, fn, 32, FTW_ACTIONRETVAL | FTW_DEPTH);

	// rewrite the hashblock (reverts to user identity)
	fatal(hashblock_write, ff->hb);

	log(L_HASHBLK
		, "%s (fab) ==> 0x%08x%08x%08x"
		, ff->idstring
		, ff->hb->stathash[1]
		, ff->hb->contenthash[1]
		, ff->hb->vrshash[1]
	);

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
		log(L_FF | L_FFFILE			, "%20s : %s", "idstring", ff->idstring);
		log(L_FF | L_FFFILE			, "%20s : %s", "type", FFT_STRING(ff->type));
		log(L_FF | L_FFFILE			, "%20s : %s", "can-path", ff->path->can);
		log(L_FF | L_FFFILE			, "%20s : %s", "abs-path", ff->path->abs);
		log(L_FF | L_FFFILE			, "%20s : %s", "rel-path", ff->path->rel);
		log(L_FF | L_FFFILE			, "%20s : %s", "path-in", ff->path->in);
		log(L_FF | L_FFFILE			, "%20s : %s", "path-base", ff->path->base);
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
}
