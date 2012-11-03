#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "control.h"

#include "ff.h"
#include "ff.tab.h"
#include "ff.lex.h"

#include "xmem.h"

struct ff_parser_t
{
	void*			p;
};

// defined in the bison parser
int ff_yyparse(yyscan_t, parse_param*);

/// [[ public ]]

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

static void strmeasure(ff_node* n)
{
	if(n)
	{
		n->l = n->e - n->s;

		int x;
		for(x = 0; x < n->list_one_l; x++)
			strmeasure(n->list_one[x]);

		for(x = 0; x < n->list_two_l; x++)
			strmeasure(n->list_two[x]);
	}
}

static void flatten(ff_node* n)
{
	if(n)
	{
		n->list_one_l = 0;
		ff_node* t = n->chain[0];
		while(t)
		{
			flatten(t);

			t = t->next;
			n->list_one_l++;
		}

		if(n->list_one_l)
			n->list_one = calloc(n->list_one_l, sizeof(n->list_one[0]));

		n->list_one_l = 0;
		t = n->chain[0];
		while(t)
		{
			n->list_one[n->list_one_l++] = t;
			t = t->next;
		}

		n->list_two_l = 0;
		t = n->chain[1];
		while(t)
		{
			flatten(t);

			t = t->next;
			n->list_two_l++;
		}

		if(n->list_two_l)
			n->list_two = calloc(n->list_two_l, sizeof(n->list_two[0]));

		n->list_two_l = 0;
		t = n->chain[1];
		while(t)
		{
			n->list_two[n->list_two_l++] = t;
			t = t->next;
		}

		flatten(n->next);
	}
}

ff_node* mknode(void* loc, char* ff_dir, uint32_t type, ...)
{
	char* a;
	char* b;

	ff_node* n = calloc(1, sizeof(*n));
	n->type = type;
	memcpy(&n->loc, loc, sizeof(n->loc));

	n->ff_dir = strdup(ff_dir);

	va_list va;
	va_start(va, type);

	n->s = va_arg(va, char*);
	n->e = va_arg(va, char*);

	if(type == FFN_DEPENDENCY)
	{
		n->chain[0]				= va_arg(va, ff_node*);
		n->chain[1]				= va_arg(va, ff_node*);
	}
	else if(type == FFN_FNAME)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->name						= struse(a, b);
	}
	else if(type == FFN_FORMULA)
	{
		n->chain[0]				= va_arg(va, ff_node*);
		n->chain[1]				= va_arg(va, ff_node*);
	}
	else if(type == FFN_COMMAND_TEXT)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->text						= struse(a, b);
	}
	else if(type == FFN_COMMAND_REF)
	{
		n->ref						= va_arg(va, int);
	}

	va_end(va);
	return n;
}

void ff_yyerror(void* loc, yyscan_t scanner, parse_param* pp, char const *err)
{
	pp->r = 0;
	log(L_ERROR | L_FF, "%s", err);
}

/// [[ api/public ]]

int ff_mkparser(ff_parser ** const restrict p)
{
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 0;

	if(ff_yylex_init(&(*p)->p) != 0)
		return 0;

	return 1;
}

int ff_parse(const ff_parser * const restrict p, char* path, ff_node ** const restrict ffn)
{
	// results struct for this parse
	ff_xfreenode(ffn);

	// open the file
	int fd;
	if((fd = open(path, O_RDONLY)) == -1)
		fail("open(%s) failed : [%d][%s]", path, errno, strerror(errno));

	// snarf the file
	struct stat statbuf;
	fatal_os(fstat, fd, &statbuf);

	char* b = calloc(statbuf.st_size + 2, 1);
	ssize_t r = 0;
	if((r = read(fd, b, statbuf.st_size)) != statbuf.st_size)
		fail("read, expected: %d, actual: %d", (int)statbuf.st_size, (int)r);

	close(fd);

	// create state specific to this parse
	void* state = 0;

	if((state = ff_yy_scan_bytes(b, statbuf.st_size + 2, p->p)) == 0)
		fail("scan_bytes failed");

	parse_param pp = {
		  .scanner = p->p
		, .ffn = ffn
		, .orig_base = b
		, .orig_len = statbuf.st_size
		, .r = 1
	};

	// canonical path to the fabfile
	pp.ff_dir = realpath(path, 0);

	// terminate at the last directory specification
	char* tm = pp.ff_dir + strlen(pp.ff_dir);
	while(*tm != '/')
		tm--;

	*tm = 0;

	// make it available to the lexer
	ff_yyset_extra(&pp, p->p);

	// parse
	ff_yyparse(p->p, &pp);

	// cleanup state for this parse
	ff_yy_delete_buffer(state, p->p);

	if(pp.r)
	{
		flatten(*ffn);
		strmeasure(*ffn);
	}
	else
	{
		ff_xfreenode(ffn);
	}

	free(pp.ff_dir);

	return 1;
}

void ff_freeparser(ff_parser* const restrict p)
{
	if(p)
	{
		ff_yylex_destroy(p->p);
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
		free(ffn->ff_dir);

		int x;
		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < ffn->list_one_l; x++)
			ff_freenode(ffn->list_one[x]);

		free(ffn->list_one);

		for(x = 0; x < ffn->list_two_l; x++)
			ff_freenode(ffn->list_two[x]);

		free(ffn->list_two);
	}

	free(ffn);
}

void ff_xfreenode(ff_node ** const restrict ffn)
{
	ff_freenode(*ffn);
	*ffn = 0;
}
