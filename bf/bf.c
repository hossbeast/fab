#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "control.h"

#include "bf.h"
#include "bf.tab.h"
#include "bf.lex.h"

#include "xmem.h"

struct bf_parser_t
{
	void*			p;
};

// defined in the bison parser
int bf_yyparse(yyscan_t, parse_param*);

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

static void strmeasure(bf_node* n)
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

static void flatten(bf_node* n)
{
	if(n)
	{
		n->list_one_l = 0;
		bf_node* t = n->chain[0];
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

bf_node* mknode(void* loc, char* bf_dir, uint32_t type, ...)
{
	char* a;
	char* b;

	bf_node* n = calloc(1, sizeof(*n));
	n->type = type;
	memcpy(&n->loc, loc, sizeof(n->loc));

	n->bf_dir = strdup(bf_dir);

	va_list va;
	va_start(va, type);

	n->s = va_arg(va, char*);
	n->e = va_arg(va, char*);

	if(type == BFN_DEPENDENCY)
	{
		n->chain[0]				= va_arg(va, bf_node*);
		n->chain[1]				= va_arg(va, bf_node*);
	}
	else if(type == BFN_FNAME)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->name						= struse(a, b);
	}
	else if(type == BFN_FORMULA)
	{
		n->chain[0]				= va_arg(va, bf_node*);
		n->chain[1]				= va_arg(va, bf_node*);
	}
	else if(type == BFN_COMMAND_TEXT)
	{
		a = va_arg(va, char*);
		b = va_arg(va, char*);

		n->text						= struse(a, b);
	}
	else if(type == BFN_COMMAND_REF)
	{
		n->ref						= va_arg(va, int);
	}

	va_end(va);
	return n;
}

void bf_yyerror(void* loc, yyscan_t scanner, parse_param* pp, char const *err)
{
	pp->r = 0;
	log(L_ERROR | L_BF, "%s", err);
}

/// [[ api/public ]]

int bf_mkparser(bf_parser ** const restrict p)
{
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 0;

	if(bf_yylex_init(&(*p)->p) != 0)
		return 0;

	return 1;
}

int bf_parse(const bf_parser * const restrict p, char* path, bf_node ** const restrict bfn)
{
	// results struct for this parse
	bf_xfreenode(bfn);

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

	if((state = bf_yy_scan_bytes(b, statbuf.st_size + 2, p->p)) == 0)
		fail("scan_bytes failed");

	parse_param pp = {
		  .scanner = p->p
		, .bfn = bfn
		, .orig_base = b
		, .orig_len = statbuf.st_size
		, .r = 1
	};

	// canonical path to the buildfile
	pp.bf_dir = realpath(path, 0);

	// terminate at the last directory specification
	char* tm = pp.bf_dir + strlen(pp.bf_dir);
	while(*tm != '/')
		tm--;

	*tm = 0;

	// make it available to the lexer
	bf_yyset_extra(&pp, p->p);

	// parse
	bf_yyparse(p->p, &pp);

	// cleanup state for this parse
	bf_yy_delete_buffer(state, p->p);

	if(pp.r)
	{
		flatten(*bfn);
		strmeasure(*bfn);
	}
	else
	{
		bf_xfreenode(bfn);
	}

	free(pp.bf_dir);

	return 1;
}

void bf_freeparser(bf_parser* const restrict p)
{
	if(p)
	{
		bf_yylex_destroy(p->p);
	}

	free(p);
}

void bf_xfreeparser(bf_parser ** const restrict p)
{
	bf_freeparser(*p);
	*p = 0;
}

void bf_freenode(bf_node * const restrict bfn)
{
	if(bfn)
	{
		free(bfn->bf_dir);

		int x;
		for(x = 0; x < sizeof(bfn->strings) / sizeof(bfn->strings[0]); x++)
			free(bfn->strings[x]);

		for(x = 0; x < bfn->list_one_l; x++)
			bf_freenode(bfn->list_one[x]);

		free(bfn->list_one);

		for(x = 0; x < bfn->list_two_l; x++)
			bf_freenode(bfn->list_two[x]);

		free(bfn->list_two);
	}

	free(bfn);
}

void bf_xfreenode(bf_node ** const restrict bfn)
{
	bf_freenode(*bfn);
	*bfn = 0;
}
