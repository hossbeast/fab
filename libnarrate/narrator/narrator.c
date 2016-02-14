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

#include "internal.h"
#include "dynamic/dynamic.internal.h"
#include "file/file.internal.h"
#include "fixed/fixed.internal.h"

// global narration context
narrator * _narrator = (typeof(*_narrator)[]){{ .fd = 1 }};

//
// api
//
API xapi narrate_mark(narrator * const restrict n, size_t * const restrict mark)
{
  enter;

  // route to implementation
	if(n->ps)
		fatal(dynamic_mark, n, mark);
	else if(n->bb)
		fatal(fixed_mark, n, mark);
	else if(n->fd)
    fatal(file_mark, n, mark);

  finally : coda;
}
