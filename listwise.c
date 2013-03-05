#include "listwise/internal.h"
#include "control.h"

int APIDATA listwise_err_fd = 2;
int APIDATA listwise_sanity = 0;
int APIDATA listwise_allocation_seed = 10;

lstack * APIDATA listwise_identity = 0;

int identity_init()
{
	fatal(xmalloc, &listwise_identity, sizeof(*listwise_identity));
	fatal(xmalloc, &listwise_identity->s, sizeof(listwise_identity->s[0]) * 1);
	listwise_identity->l = 1;
	listwise_identity->a = 1;

	finally : coda;
}

void identity_teardown()
{
	if(listwise_identity)
		free(listwise_identity->s);
	free(listwise_identity);
}

int listwise_exec(char* s, int l, char** init, int* initls, int initl, lstack** ls)
{
	// generator parser
	generator_parser* p = 0;

	// generator
	generator* g = 0;

	if(generator_mkparser(&p) == 0)
		fail("mkparser failed\n");

	if(generator_parse(p, s, l, &g) == 0)
		fail("parse failed\n");

	if(lstack_exec(g, init, initls, initl, ls) == 0)
		fail("lstack_exec failed");

	generator_free(g);
	generator_parser_free(p);

	finally : coda;
}


