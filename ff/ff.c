#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <inttypes.h>

#include "ff.h"

#include "ff.tokens.h"
#include "ff.tab.h"
#include "ff.lex.h"

#include "args.h"
#include "identity.h"
#include "gn.h"

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

// defined in ff.tab.o
int ff_yyparse(yyscan_t, parse_param*);

// defined in ff.dsc.tab.o
int ff_dsc_yyparse(yyscan_t, parse_param*);

struct ff_parser_t
{
	void *							p;
	generator_parser *	gp;
};

//
// data
//

union ff_files_t ff_files = { { .size = sizeof(ff_file) } };

//
// [[ static ]]
//

static int findvars(ff_file * const restrict ff, ff_node * const restrict root)
{
	int find(ff_node * const restrict ffn)
	{
		if(ffn)
		{
			if(ffn->type == FFN_VARREF)
			{
				// skip autovars
				if((ffn->text[0] >= 'a' && ffn->text[0] <= 'z') || (ffn->text[0] >= 'A' && ffn->text[0] <= 'Z') || ffn->text[0] == '_')
				{
					if(ff->affecting_varsl == ff->affecting_varsa)
					{
						int ns = ff->affecting_varsa ?: 10;
						ns = ns + 2 * ns / 2;

						fatal(xrealloc, &ff->affecting_vars, sizeof(*ff->affecting_vars), ns, ff->affecting_varsa);
						ff->affecting_varsa = ns;
					}

					ff->affecting_vars[ff->affecting_varsl++] = ffn;
				}
			}

			int x;
			for(x = 0; x < ffn->listl; x++)
				find(ffn->list[x]);

			for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
				find(ffn->nodes_owned[x]);
		}

		finally : coda;
	};

	// find all varrefs
	fatal(find, root);

	// sort
	int cmp(const void * A, const void * B)
	{
		return strcmp((*(ff_node**)A)->text, (*(ff_node**)B)->text);
	};
	qsort(ff->affecting_vars, ff->affecting_varsl, sizeof(*ff->affecting_vars), cmp);

	// uniq
	int j;
	for(j = ff->affecting_varsl - 1; j > 0; j--)
	{
		int i;
		for(i = j - 1; i >= 0; i--)
		{
			if(strcmp(ff->affecting_vars[i]->text, ff->affecting_vars[j]->text))
				break;
		}

		if((++i) < j)
		{
			memmove(
				  &ff->affecting_vars[i]
				, &ff->affecting_vars[j]
				, (ff->affecting_varsl - j) * sizeof(ff->affecting_vars[0])
			);

			ff->affecting_varsl -= (j - i);
			j = i;
		}
	}

	finally : coda;
}

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, struct gn * dscv_gn, ff_file ** const rff)
{
	// create state specific to this parse
	void* state = 0;
	if((state = ff_yy_scan_bytes(b, sz + 2, p->p)) == 0)
		fail("scan_bytes failed");

	// all ff_files are tracked in ff_files
	ff_file * ff = 0;
	fatal(coll_doubly_add, &ff_files.c, 0, &ff);

	// create copy of the path
	fatal(path_copy, in_path, &ff->path);

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

		// affected dir
		fatal(xsprintf, &ff->affected_dir, "%s/REGULAR/%u/affected", FF_DIR_BASE, ff->path->can_hash);
	}

	parse_param pp = {
		  .scanner = p->p
		, .ff = ff
		, .orig_base = b
		, .orig_len = sz
		, .r = 1
	};

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
		if(ffn_postprocess(ff->ffn, p->gp) == 0)
			qfail();

		if(ff->type == FFT_REGULAR)
		{
			// create flat list of VARREF's in this fabfile
			fatal(findvars, ff, ff->ffn);

			// create hashblock
			fatal(hashblock_create, &ff->hb, "%s/REGULAR/%u", FF_DIR_BASE, ff->path->can_hash);

			// load previous hashblock into [0]
			fatal(hashblock_read, ff->hb);

			// stat the file, populate [1] - now ready for hashblock_cmp
			fatal(hashblock_stat, ff->path->abs, ff->hb, 0, 0);
			ff->hb->vrshash[1] = FAB_VERSION;
		}

		(*rff) = ff;
		ff_dump(ff);
	}

	finally : coda;
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

int ff_parse_path(const ff_parser * const p, const path * const in_path, ff_file ** const ff)
{
	int			fd = 0;
	char *	b = 0;

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

finally:
	free(b);
	if(fd > 0)
		close(fd);
coda;
}

int ff_parse(const ff_parser * const p, const char * const fp, const char * const base, ff_file ** const ff)
{
	int			fd = 0;
	char *	b = 0;
	path *	pth = 0;

	fatal(path_create, &pth, base, "%s", fp);

	// open the file
	if((fd = open(pth->abs, O_RDONLY)) == -1)
		fail("open(%s) failed : [%d][%s]", pth->abs, errno, strerror(errno));

	// snarf the file
	struct stat statbuf;
	fatal_os(fstat, fd, &statbuf);

	fatal(xmalloc, &b, statbuf.st_size + 2);
	ssize_t r = 0;
	if((r = read(fd, b, statbuf.st_size)) != statbuf.st_size)
		fail("read, expected: %d, actual: %d", (int)statbuf.st_size, (int)r);

	qfatal(parse, p, b, statbuf.st_size, pth, 0, ff);

finally:
	free(b);
	if(fd > 0)
		close(fd);
	path_free(pth);
coda;
}

int ff_dsc_parse(const ff_parser * const p, char* b, int sz, const char * const fp, struct gn * dscv_gn, ff_file** const ff)
{
	path * pth = 0;
	fatal(path_create_canon, &pth, fp, 0);
	fatal(parse, p, b, sz, pth, dscv_gn, ff);

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

void ff_yyerror(void* loc, yyscan_t scanner, parse_param* pp, char const *err)
{
	pp->r = 0;
	if(!pp->badchar)
		log(L_ERROR | L_FF, "%s", err);		// bison error

	int t						= pp->last_tok;
	const char * s	= pp->last_s;
	const char * e	= pp->last_e;
	int l						= e - s;
	ff_loc * lc			= &pp->last_loc;

	// log last good token
	log(L_ERROR | L_FF, "last token - %s '%.*s' @ (%s)[%3d,%3d - %3d,%3d]"
		, ff_tokname(pp->last_tok)
		, MIN(t == LF ? 0 : t == WS ? 0 : l, 50)
		, t == LF ? "" : t == WS ? "" : s
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

			free(ff->affected_dir);
			free(ff->affected_gn);
			free(ff->affecting_vars);
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
		ff_freefile(ff_files.e[x]);

	free(ff_files.e);
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
	qsort(ff->affected_gn, ff->affected_gnl, sizeof(*ff->affected_gn), cmp);

	// ensure affected directory exists
	fatal(mkdirp, ff->affected_dir, S_IRWXU | S_IRWXG | S_IRWXO);

	// write links to all nodes that are connected to this regular fabfile
	int x;
	for(x = 0; x < ff->affected_gnl; x++)
	{
		// directory for the affected node
		snprintf(to, sizeof(to), "%s/%u", ff->affected_dir, ff->affected_gn[x]->path->can_hash);
		fatal(mkdirp, to, S_IRWXU | S_IRWXG | S_IRWXO);
		
		// with a link to its PRIMARY and SECONDARY directories
		snprintf(to, sizeof(to), "%s/PRIMARY/%u", GN_DIR_BASE, ff->affected_gn[x]->path->can_hash);
		snprintf(from, sizeof(from), "%s/%u/PRIMARY", ff->affected_dir, ff->affected_gn[x]->path->can_hash);
		if(symlink(to, from) != 0 && errno != EEXIST)
			fail("symlink failed : [%d][%s]", errno, strerror(errno));

		snprintf(to, sizeof(to), "%s/SECONDARY/%u", GN_DIR_BASE, ff->affected_gn[x]->path->can_hash);
		snprintf(from, sizeof(from), "%s/%u/SECONDARY", ff->affected_dir, ff->affected_gn[x]->path->can_hash);
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

				if(bsearch(&canhash, ff->affected_gn, ff->affected_gnl, sizeof(*ff->affected_gn), kcmp) == 0)
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

	fatal_os(nftw, ff->affected_dir, fn, 32, FTW_ACTIONRETVAL | FTW_DEPTH);

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

int ff_regular_affecting_gn(struct ff_file * const ff, gn * const gn)
{
	int newa = 0;
	fatal(gn_affected_ff_reg, gn, ff, &newa);

	if(newa)
	{
		if(ff->affected_gnl >= ff->affected_gna)
		{
			int ns = ff->affected_gna ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &ff->affected_gn, sizeof(*ff->affected_gn), ns, ff->affected_gna);
			ff->affected_gna = ns;
		}

		ff->affected_gn[ff->affected_gnl++] = gn;
	}

	finally : coda;	
}

void ff_dump(ff_file * const ff)
{
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
			log(L_FF | L_FFFILE		, "%20s : %d", "var-closure", ff->affecting_varsl);
			int x;
			for(x = 0; x < ff->affecting_varsl; x++)
			{
				log(L_FF | L_FFFILE	, "  %20s : %s", "", ff->affecting_vars[x]->text);
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
