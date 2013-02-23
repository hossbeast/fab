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

#include "gn.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"
#include "args.h"
#include "cksum.h"
#include "xstring.h"
#include "identity.h"
#include "dirutil.h"

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

ff_node* addchain(ff_node* a, ff_node* b)
{
	ff_node* i = a;
	while(a->next)
		a = a->next;

	a->next = b;
	return i;
}

static void flatten(ff_node* n)
{
	if(n)
	{
		n->list_l = 0;
		ff_node* t = n->chain[0];
		while(t)
		{
			flatten(t);

			t = t->next;
			n->list_l++;
		}

		if(n->list_l)
			n->list = calloc(n->list_l, sizeof(n->list[0]));

		n->list_l = 0;
		t = n->chain[0];
		while(t)
		{
			n->list[n->list_l++] = t;
			t = t->next;
		}

		int x;
		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
			flatten(n->nodes_owned[x]);
	}
}

static void strmeasure(ff_node* n)
{
	if(n)
	{
		n->l = n->e - n->s;

		int x;
		for(x = 0; x < n->list_l; x++)
			strmeasure(n->list[x]);

		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
			strmeasure(n->nodes_owned[x]);
	}
}

static int parse_generators(ff_node* n, generator_parser * gp)
{
	if(n)
	{
		if(n->type == FFN_GENERATOR)
		{
			if(generator_parse(gp, n->text, 0, &n->generator) == 0)
			{
				fail("failed to parse '%s' @ [%3d,%3d - %3d,%3d]"
					, n->text
					, n->loc.f_lin + 1
					, n->loc.f_col + 1
					, n->loc.l_lin + 1
					, n->loc.l_col + 1
				);
			}
		}

		int x;
		for(x = 0; x < n->list_l; x++)
		{
			if(parse_generators(n->list[x], gp) == 0)
				qfail();
		}

		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
		{
			if(parse_generators(n->nodes_owned[x], gp) == 0)
				qfail();
		}
	}

	finally : coda;
}

static int parse(const ff_parser * const p, char* b, int sz, const path * const in_path, ff_node ** const ffn, struct gn * dscv_gn)
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
		// parse
		ff_dsc_yyparse(p->p, &pp);
	}
	else
	{
		// parse
		ff_yyparse(p->p, &pp);
	}

	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);

	if(pp.r)
	{
		// convert chains to lists
		flatten(ff->ffn);

		// calculate string lengths
		strmeasure(ff->ffn);

		// parse generator strings
		if(parse_generators(ff->ffn, p->gp) == 0)
			qfail();

		*ffn = ff->ffn;

		if(ff->type == FFT_REGULAR)
		{
			// create hashblock
			fatal(hashblock_create, &ff->hb, "%s/REGULAR/%u", FF_DIR_BASE, ff->path->can_hash);

			// load previous hashblock into [0]
			fatal(hashblock_read, ff->hb);

			// stat the file, populate [1] - now ready for hashblock_cmp
			fatal(hashblock_stat, ff->path->abs, ff->hb, 0, 0);
			ff->hb->vrshash[1] = FAB_VERSION;
		}
	}

	finally : coda;
}

/// [[ public ]]

ff_node* mknode(void* loc, size_t locz, ff_file * ff, uint32_t type, ...)
{
	ff_node* n = calloc(1, sizeof(*n));
	n->type = type;
	memcpy(&n->loc, loc, locz);
	n->loc.ff = ff;

	va_list va;
	va_start(va, type);

	n->s = va_arg(va, char*);
	n->e = va_arg(va, char*);

	if(type == FFN_STMTLIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// statements
	}
	else if(type == FFN_DEPENDENCY)
	{
		n->needs					= va_arg(va, ff_node*);
		n->feeds					= va_arg(va, ff_node*);
		n->flags					= (uint8_t)va_arg(va, int);
	}
	else if(type == FFN_INVOCATION)
	{
		n->modules				= va_arg(va, ff_node*);
		n->chain[0]				= va_arg(va, ff_node*);	// designations
	}
	else if(type == FFN_WORD)
	{
		n->text 					= va_arg(va, char*);
	}
	else if(type == FFN_FORMULA)
	{
		n->targets_0			= va_arg(va, ff_node*);
		n->targets_1			= va_arg(va, ff_node*);
		n->chain[0]				= va_arg(va, ff_node*);	// commands
		n->flags					= (uint8_t)va_arg(va, int);
	}
	else if(type == FFN_VARASSIGN)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARPUSH)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARPOP)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARDESIGNATE)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_LIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// elements
		n->generator_node	= va_arg(va, ff_node*);
	}
	else if(type == FFN_LF)
	{
		// no extra params
	}
	else
	{
		fprintf(stderr, "unknown type : %s\n", FFN_STRING(type));
		exit(0);
	}

	va_end(va);
	return n;
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

int ff_parse_path(const ff_parser * const p, const path * const in_path, ff_node ** const ffn)
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

	qfatal(parse, p, b, statbuf.st_size, in_path, ffn, 0);

finally:
	free(b);
	if(fd)
		close(fd);
coda;
}

int ff_parse(const ff_parser * const p, const char * const fp, const char * const base, ff_node ** const ffn)
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

	qfatal(parse, p, b, statbuf.st_size, pth, ffn, 0);

finally:
	free(b);
	if(fd)
		close(fd);
	path_free(pth);
coda;
}

int ff_dsc_parse(const ff_parser * const p, char* b, int sz, const char * const fp, struct gn * dscv_gn, ff_node ** const ffn)
{
	path * pth = 0;
	fatal(path_create_canon, &pth, fp, 0);
	fatal(parse, p, b, sz, pth, ffn, dscv_gn);

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

void ff_freenode(ff_node * const ffn)
{
	if(ffn)
	{
		int x;
		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
			ff_freenode(ffn->nodes_owned[x]);

		for(x = 0; x < ffn->list_l; x++)
			ff_freenode(ffn->list[x]);

		free(ffn->list);

		if(ffn->type == FFN_GENERATOR)
			generator_free(ffn->generator);
	}

	free(ffn);
}

void ff_xfreenode(ff_node ** const ffn)
{
	ff_freenode(*ffn);
	*ffn = 0;
}

void ff_dump(ff_node * const root)
{
	void dump(ff_node * const ffn, int lvl)
	{
		int x;
		if(ffn)
		{
			log(L_FF | L_FFTREE, "%*s%-20s%*s @ [%3d,%3d - %3d,%3d]"
				, lvl * 2, ""
				, FFN_STRING(ffn->type)
				, 70 - MIN(((lvl * 2) + 20), 70)
				, ""
				, ffn->loc.f_lin, ffn->loc.f_col, ffn->loc.l_lin, ffn->loc.l_col
			);

			if(ffn->type == FFN_STMTLIST)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "statements", ffn->statements_l
				);
				for(x = 0; x < ffn->statements_l; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			if(ffn->type == FFN_DEPENDENCY)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_WEAK ? "weak" : "strong"
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"
					, ffn->flags & FFN_MULTI ? "multi" : "single"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "needs"
				);
				dump(ffn->needs, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "feeds"
				);
				dump(ffn->feeds, lvl + 1);
			}
			else if(ffn->type == FFN_FORMULA)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_DISCOVERY ? "discovery" : "fabrication"
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"	, ffn->flags & FFN_SINGLE ? "single " : ffn->flags & FFN_MULTI ? "multi" : "UNKNOWN"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "targets"
				);
				dump(ffn->targets_0, lvl + 1);
				dump(ffn->targets_1, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "command", ffn->commands_l
				);
				for(x = 0; x < ffn->commands_l; x++)
					dump(ffn->commands[x], lvl + 1);
			}
			else if(ffn->type == FFN_INVOCATION)
			{

			}
			else if(    ffn->type == FFN_VARASSIGN
			         || ffn->type == FFN_VARPUSH
			         || ffn->type == FFN_VARPOP
			         || ffn->type == FFN_VARDESIGNATE)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "vars", ffn->vars_l
				);
				for(x = 0; x < ffn->vars_l; x++)
					dump(ffn->vars[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "definition"
				);
				dump(ffn->definition, lvl + 1);
			}
			else if(ffn->type == FFN_LIST)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "elements", ffn->elements_l
				);
				for(x = 0; x < ffn->elements_l; x++)
					dump(ffn->elements[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "generator"
				);
				dump(ffn->generator_node, lvl + 1);
			}
			else if(ffn->type == FFN_WORD)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "text", ffn->text
				);
			}
			else if(ffn->type == FFN_GENERATOR)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "generator-string", ffn->text
				);
			}
			else if(ffn->type == FFN_VARREF)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "name", ffn->text
				);
			}
		}
	};

	dump(root, 0);
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
		}

		path_free(ff->path);
		free(ff->idstring);
		ff_freenode(ff->ffn);
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
