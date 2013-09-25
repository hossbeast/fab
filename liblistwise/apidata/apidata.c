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

#include "listwise/internal.h"

#include "xmem.h"

int APIDATA listwise_err_fd = 2;
int APIDATA listwise_sanity = 0;
int APIDATA listwise_allocation_seed = 10;

lwx * APIDATA listwise_identity = 0;

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
