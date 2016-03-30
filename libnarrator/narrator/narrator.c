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
#include "growing/growing.internal.h"
#include "file/file.internal.h"
#include "fixed/fixed.internal.h"

//
// api
//

API void narrator_free(narrator * restrict n)
{
  // route to implementation
  if(n->type == NARRATOR_GROWING)
		growing_free(n);
	else if(n->type == NARRATOR_FIXED)
		fixed_free(n);
	else if(n->type == NARRATOR_FILE)
    file_free(n);
}

API void narrator_xfree(narrator ** const restrict n)
{
  narrator_free(*n);
  *n = 0;
}

API xapi narrator_mark(narrator * const restrict n, size_t * const restrict mark)
{
  enter;

  // route to implementation
  if(n->type == NARRATOR_GROWING)
		fatal(growing_mark, n, mark);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_mark, n, mark);
	else if(n->type == NARRATOR_FILE)
    fatal(file_mark, n, mark);

  finally : coda;
}

API const char * narrator_first(narrator * const restrict n)
{
  // route to implementation
  if(n->type == NARRATOR_GROWING)
		return growing_first(n);
	else if(n->type == NARRATOR_FIXED)
		return fixed_first(n);
	else if(n->type == NARRATOR_FILE)
    return file_first(n);

  return 0;
}

API xapi narrator_sayf(narrator * n, const char * const restrict fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);

	fatal(narrator_vsayf, n, fmt, va);

finally:
	va_end(va);
coda;
}

API xapi narrator_sayc(narrator * const restrict n, int c)
{
	xproxy(narrator_sayw, n, (char*)&c, 1);
}

API xapi narrator_vsayf(narrator * const restrict n, const char * const restrict fmt, va_list va)
{
  enter;

	if(n->type == NARRATOR_GROWING)
		fatal(growing_vsayf, n, fmt, va);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_vsayf, n, fmt, va);
	else if(n->type == NARRATOR_FILE)
		fatal(file_vsayf, n, fmt, va);

	finally : coda;
}

API xapi narrator_sayw(narrator * const restrict n, char * const restrict b, size_t l)
{
  enter;

	if(n->type == NARRATOR_GROWING)
		fatal(growing_sayw, n, b, l);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_sayw, n, b, l);
	else if(n->type == NARRATOR_FILE)
		fatal(file_sayw, n, b, l);

	finally : coda;
}
