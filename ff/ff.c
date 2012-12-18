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

char * gn_idstring(struct gn * const);

struct ff_parser_t
{
	void *							p;
	generator_parser *	gp;
};

// defined in ff.tab.o
int ff_yyparse(yyscan_t, parse_param*);

// defined in ff.dsc.tab.o
int ff_dsc_yyparse(yyscan_t, parse_param*);

/// [[ static ]]

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

		n->list = calloc(n->list_l, sizeof(n->list[0]));

		n->list_l = 0;
		t = n->chain[0];
		while(t)
		{
			n->list[n->list_l++] = t;
			t = t->next;
		}

		int x;
		for(x = 0; x < sizeof(n->nodes) / sizeof(n->nodes[0]); x++)
			flatten(n->nodes[x]);
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
				return 0;
		}

		for(x = 0; x < sizeof(n->nodes) / sizeof(n->nodes[0]); x++)
		{
			if(parse_generators(n->nodes[x], gp) == 0)
				return 0;
		}
	}

	return 1;
}

static int parse(const ff_parser * const restrict p, char* b, int sz, char* path, ff_node ** const restrict ffn, struct gn * dscv_gn)
{
	// create state specific to this parse
	void* state = 0;
	if((state = ff_yy_scan_bytes(b, sz + 2, p->p)) == 0)
		fail("scan_bytes failed");

	ff_file * ff = 0;
	fatal(xmalloc, &ff, sizeof(*ff));

	// canonical path to the fabfile and its directory
	ff->path = realpath(path, 0);
	ff->dir = realpath(path, 0);

	// terminate at the last directory specification
	char * tm = ff->dir + strlen(ff->dir);
	while(*tm != '/')
		tm--;
	*tm = 0;

	// name of fabfile
	ff->name = strdup(++tm);

	if(dscv_gn)
		ff->dscv_gn = dscv_gn;

	parse_param pp = {
		  .scanner = p->p
		, .ffn = ffn
		, .ff = ff
		, .orig_base = b
		, .orig_len = sz
		, .r = 1
	};

	// make available to the lexer
	ff_yyset_extra(&pp, p->p);

	if(dscv_gn)
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
		flatten(*ffn);

		// calculate string lengths
		strmeasure(*ffn);

		// parse generator strings
		if(parse_generators(*ffn, p->gp) == 0)
			return 0;
	}
	else
	{
		ff_xfreenode(ffn);
	}

	return 1;
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
	else if(type == FFN_GENERATOR)
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

	va_end(va);
	return n;
}

/// [[ api/public ]]

int ff_mkparser(ff_parser ** const restrict p)
{
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 0;

	if(ff_yylex_init(&(*p)->p) != 0)
		return 0;

	if(generator_mkparser(&(*p)->gp) == 0)
		return 0;

	return 1;
}

int ff_parse(const ff_parser * const restrict p, char* path, ff_node ** const restrict ffn)
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

	close(fd);

	int R = parse(p, b, statbuf.st_size, path, ffn, (void*)0);
	free(b);
	return R;
}

int ff_dsc_parse(const ff_parser * const restrict p, char* b, int sz, char* path, struct gn * dscv_gn, ff_node ** const restrict ffn)
{
	return parse(p, b, sz, path, ffn, dscv_gn);
}

void ff_freeparser(ff_parser* const restrict p)
{
	if(p)
	{
		ff_yylex_destroy(p->p);
		generator_parser_free(p->gp);
	}

	free(p);
}

void ff_xfreeparser(ff_parser ** const restrict p)
{
	ff_freeparser(*p);
	*p = 0;
}

void ff_freenode(ff_node * const restrict ffn)
{
	if(ffn)
	{
		int x;
		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < sizeof(ffn->nodes) / sizeof(ffn->nodes[0]); x++)
			ff_freenode(ffn->nodes[x]);

		for(x = 0; x < ffn->list_l; x++)
			ff_freenode(ffn->list[x]);

		free(ffn->list);

		if(ffn->type == FFN_GENERATOR)
			generator_free(ffn->generator);

		memset(ffn, 0, sizeof(*ffn));
	}

	free(ffn);
}

void ff_xfreenode(ff_node ** const restrict ffn)
{
	ff_freenode(*ffn);
	*ffn = 0;
}

void ff_dump(ff_node * const restrict root)
{
	void dump(ff_node * const restrict ffn, int lvl)
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
	log(L_ERROR | L_FF, "last token - %s '%.*s' @ [%3d,%3d - %3d,%3d]"
		, ff_tokname(pp->last_tok)
		, MIN(t == LF ? 0 : t == WS ? 0 : l, 50)
		, t == LF ? "" : t == WS ? "" : s
		, lc->f_lin + 1
		, lc->f_col + 1
		, lc->l_lin + 1
		, lc->l_col + 1
	);
}

char * ff_idstring(ff_file * const restrict ff)
{
	if(ff->dscv_gn)
	{
		if(ff->idstring == 0)
		{
			size_t sz = snprintf(0, 0, "DSC:%s", gn_idstring(ff->dscv_gn));
			ff->idstring = calloc(sz + 1, 1);
			sprintf(ff->idstring, "DSC:%s", gn_idstring(ff->dscv_gn));
		}

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
