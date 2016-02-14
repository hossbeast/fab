/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include <stdlib.h>
#include <stdio.h>

#include "xapi.h"
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "list.h"

typedef struct item
{
  int a;
  int x;
} item;

xapi validate(list * listp)
{
  enter;

  int x;
  for(x = 1; x < list_size(listp); x++)
  {
    item * A = list_get(listp, x - 1);
    item * B = list_get(listp, x);

    if(B->x <= A->x)
      tfail(perrtab_SYS, SYS_ENOMEM);
  }

  finally : coda;
}

xapi test_primary()
{
  enter;

  list * listp = 0;
  fatal(list_create, &listp, sizeof(item), 0, LIST_PRIMARY);

  item * itemp = 0;
  fatal(list_unshift, listp, &itemp);
  itemp->x = 6;

  fatal(list_unshift, listp, &itemp);
  itemp->x = 5;
  fatal(list_unshift, listp, &itemp);
  itemp->x = 4;

  fatal(validate, listp);

  item * itemps[2];
  fatal(list_insert_range, listp, 3, itemps, 2);
  itemps[0]->x = 7;
  itemps[1]->x = 8;

  fatal(validate, listp);

  fatal(list_insert, listp, 0, itemp);
  itemp->x = 1;
  fatal(list_insert, listp, 1, itemp);
  itemp->x = 2;
  fatal(list_insert, listp, 2, itemp);
  itemp->x = 3;

finally:
  list_free(listp);
coda;
}

xapi test_secondary()
{
  enter;

  list * listp = 0;
  item * itemp = 0;
  item * itemps[2];

  fatal(list_create, &listp, 0, free, LIST_SECONDARY);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 3;
  fatal(list_push, listp, &itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 4;
  fatal(list_push, listp, &itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 5;
  fatal(list_push, listp, &itemp);
  itemp = 0;

  fatal(validate, listp);

  fatal(xmalloc, &itemps[0], sizeof(**itemps));
  itemps[0]->x = 1;
  fatal(xmalloc, &itemps[1], sizeof(**itemps));
  itemps[1]->x = 2;
  fatal(list_unshift_range, listp, itemps, 2); 

  fatal(validate, listp);

finally:
  list_free(listp);
  free(itemp);
coda;
}

int main()
{
  enter;

  fatal(test_primary);
  fatal(test_secondary);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}
