#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ff.h"
#include "ff.tokens.h"
#include "ff.tab.h"
#include "ff.lex.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"
#include "args.h"
#include "cksum.h"
#include "xstring.h"

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

static char* struse(char* s, char* e)
{
	char* v = 0;

	if(s)
	{
		v = malloc((e - s) + 1);
		memcpy(v, s, e - s);
		v[e - s] = 0;
	}

	return v;
}

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

		if(n->nodes_freeguard == 0)
		{
			int x;
			for(x = 0; x < sizeof(n->nodes) / sizeof(n->nodes[0]); x++)
				flatten(n->nodes[x]);
		}
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

		for(x = 0; x < sizeof(n->nodes) / sizeof(n->nodes[0]); x++)
			strmeasure(n->nodes[x]);
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

		if(n->nodes_freeguard == 0)
		{
			for(x = 0; x < sizeof(n->nodes) / sizeof(n->nodes[0]); x++)
			{
				if(parse_generators(n->nodes[x], gp) == 0)
					qfail();
			}
		}
	}

	finally : coda;
}

static int parse(const ff_parser * const p, char* b, int sz, char* path, ff_node ** const ffn, struct gn * dscv_gn)
{
	// create state specific to this parse
	void* state = 0;
	if((state = ff_yy_scan_bytes(b, sz + 2, p->p)) == 0)
		fail("scan_bytes failed");

	// all ff_files are tracked in ff_files
	ff_file * ff = 0;
	fatal(coll_doubly_add, &ff_files.c, 0, &ff);

	if(dscv_gn)
	{
		ff->type = FFT_DDISC;
		ff->dscv_gn = dscv_gn;
	}
	else
	{
		ff->type = FFT_REGULAR;
	}

	// canonical path to the fabfile and its directory
	ff->path = realpath(path, 0);
	ff->pathhash = cksum(path, strlen(path));
	ff->dir = strdup(ff->path);

	// terminate at the last directory specification
	char * tm = ff->dir + strlen(ff->dir);
	while(*tm != '/')
		tm--;
	*tm = 0;

	// name of fabfile
	ff->name = strdup(++tm);

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

		// create hashblock
		fatal(hashblock_create, &ff->hb, "%s/REGULAR/%u", FF_DIR_BASE, ff->pathhash);

		// load previous hashblock into [0]
		fatal(hashblock_read, ff->hb);

		// stat the file, populate [1] - now ready for hashblock_cmp
		fatal(hashblock_stat, ff->hb, ff->path);
		ff->hb->vrshash[1] = FAB_VERSION;
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
	}

	finally : coda;
}

/// [[ public ]]

ff_node* mknode(void* loc, size_t locz, ff_file * ff, uint32_t type, ...)
{
	char* a;
	char* b;

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
		n->chain[0]				= va_arg(va, ff_node*);
	}
	else if(type == FFN_DEPENDENCY)
	{
		n->flags					= (uint8_t)va_arg(va, int);
		n->needs					= va_arg(va, ff_node*);
		n->feeds					= va_arg(va, ff_node*);
	}
	else if(type == FFN_WORD)
	{
		a = va_arg(va, char*);

		n->text 					= a;
	}
	else if(type == FFN_VARNAME)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->name						= struse(a, b);
	}
	else if(type == FFN_FORMULA)
	{
		n->chain[0]				= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARDECL)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->name						= struse(a, b);
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_LIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);
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

int ff_parse(const ff_parser * const p, char* path, ff_node ** const ffn)
{
	int fd = 0;
	char * b = 0;

	// open the file
	if((fd = open(path, O_RDONLY)) == -1)
		fail("open(%s) failed : [%d][%s]", path, errno, strerror(errno));

	// snarf the file
	struct stat statbuf;
	fatal_os(fstat, fd, &statbuf);

	fatal(xmalloc, &b, statbuf.st_size + 2);
	ssize_t r = 0;
	if((r = read(fd, b, statbuf.st_size)) != statbuf.st_size)
		fail("read, expected: %d, actual: %d", (int)statbuf.st_size, (int)r);

	qfatal(parse, p, b, statbuf.st_size, path, ffn, 0);

finally:
	free(b);
	if(fd)
		close(fd);
coda;
}

int ff_dsc_parse(const ff_parser * const p, char* b, int sz, char* path, struct gn * dscv_gn, ff_node ** const ffn)
{
	return parse(p, b, sz, path, ffn, dscv_gn);
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

		if(ffn->nodes_freeguard == 0)
		{
			for(x = 0; x < sizeof(ffn->nodes) / sizeof(ffn->nodes[0]); x++)
				ff_freenode(ffn->nodes[x]);
		}

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
				log(L_FF | L_FFTREE, "%*s  %10s : %d"
					, lvl * 2, ""
					, "statements", ffn->statements_l
				);
				for(x = 0; x < ffn->statements_l; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			if(ffn->type == FFN_DEPENDENCY)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_MULTI ? "multi" : "single"
				);
				log(L_FF | L_FFTREE, "%*s  %10s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_WEAK ? "weak" : "strong"
				);
				log(L_FF | L_FFTREE, "%*s  %10s :"
					, lvl * 2, ""
					, "needs"
				);
				dump(ffn->needs, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %10s :"
					, lvl * 2, ""
					, "feeds"
				);
				dump(ffn->feeds, lvl + 1);
			}
			else if(ffn->type == FFN_FORMULA)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : %10s, %10s : %10s"
					, lvl * 2, ""
					, "type"				, ffn->flags & FFN_DISCOVERY ? "discovery" : "fabrication"
					, "cardinality"	, ffn->flags & FFN_SINGLE ? "single " : ffn->flags & FFN_MULTI ? "multi" : "UNKNOWN"
				);
				log(L_FF | L_FFTREE, "%*s  %10s :"
					, lvl * 2, ""
					, "targets"
				);
				dump(ffn->targets, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %10s : %d"
					, lvl * 2, ""
					, "command", ffn->commands_l
				);
				for(x = 0; x < ffn->commands_l; x++)
					dump(ffn->commands[x], lvl + 1);
			}
			else if(ffn->type == FFN_INCLUDE)
			{

			}
			else if(ffn->type == FFN_VARDECL)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : '%s'"
					, lvl * 2, ""
					, "name", ffn->name
				);

				log(L_FF | L_FFTREE, "%*s  %10s :"
					, lvl * 2, ""
					, "definition"
				);
				dump(ffn->definition, lvl + 1);
			}
			else if(ffn->type == FFN_VARNAME)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : '%s'"
					, lvl * 2, ""
					, "name", ffn->name
				);
			}
			else if(ffn->type == FFN_LIST)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : %d"
					, lvl * 2, ""
					, "elements", ffn->elements_l
				);
				for(x = 0; x < ffn->elements_l; x++)
					dump(ffn->elements[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %10s :"
					, lvl * 2, ""
					, "generator"
				);
				dump(ffn->generator_node, lvl + 1);
			}
			else if(ffn->type == FFN_WORD)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : '%s'"
					, lvl * 2, ""
					, "text", ffn->text
				);
			}
			else if(ffn->type == FFN_GENERATOR)
			{
				log(L_FF | L_FFTREE, "%*s  %10s : '%s'"
					, lvl * 2, ""
					, "generator-string", ffn->text
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
	if(ff->type == FFT_DDISC)
	{
		if(ff->idstring == 0)
			xsprintf(&ff->idstring, "DSC:%s", gn_idstring(ff->dscv_gn));

		return ff->idstring;
	}
	else if(g_args.mode_gnid == MODE_GNID_CANON)
	{
		return ff->path;
	}
	else if(g_args.mode_gnid == MODE_GNID_RELATIVE)
	{
		if(ff->idstring == 0)
		{
			if(strcmp(g_args.fabfile_canon, ff->path) == 0)
			{
				ff->idstring = strdup(ff->name);
			}
			else
			{
				int x;
				for(x = 0; x < strlen(g_args.fabfile_canon) && x < strlen(ff->path); x++)
				{
					if(g_args.fabfile_canon[x] != ff->path[x])
						break;
				}

				ff->idstring = strdup(&ff->path[x]);
			}
		}

		return ff->idstring;
	}

	return 0;
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
		}

		free(ff->name);
		free(ff->path);
		free(ff->dir);
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
