#include "listwise/internal.h"

#include "xmem.h"

int APIDATA listwise_err_fd = 2;
int APIDATA listwise_sanity = 0;
int APIDATA listwise_allocation_seed = 10;

lstack * APIDATA listwise_identity = 0;

/// initialize
//
// load operators from disk
//
static void __attribute__((constructor)) init()
{
	xmalloc(&listwise_identity, sizeof(*listwise_identity));
	xmalloc(&listwise_identity->s, sizeof(listwise_identity->s[0]) * 1);
	listwise_identity->l = 1;
	listwise_identity->a = 1;
}

void __attribute__((destructor)) list_teardown()
{
	if(listwise_identity)
		free(listwise_identity->s);
	free(listwise_identity);
}
