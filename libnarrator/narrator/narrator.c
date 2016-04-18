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

#include "xapi.h"
#include "xlinux.h"

#include "internal.h"
#include "growing/growing.internal.h"
#include "file/file.internal.h"
#include "fixed/fixed.internal.h"
#include "multi/multi.internal.h"

#include "nullity/nullity.internal.h"
#include "record/record.internal.h"

//
// api
//

API xapi narrator_setup()
{
  enter;

  fatal(nullity_setup);

  finally : coda;
}

API void narrator_teardown()
{
  nullity_teardown();
}

API void narrator_free(narrator * restrict n)
{
  if(n)
  {
    // dispatch
    if(n->type == NARRATOR_GROWING)
      growing_destroy(&n->growing);
    else if(n->type == NARRATOR_FIXED)
      fixed_destroy(&n->fixed);
    else if(n->type == NARRATOR_FILE)
      file_destroy(&n->file);
    else if(n->type == NARRATOR_MULTI)
      multi_destroy(&n->multi);
    else if(n->type == NARRATOR_NULLITY)
      nullity_destroy(&n->nullity);
    else if(n->type == NARRATOR_RECORD)
      record_destroy(&n->record);
  }

  xfree(n);
}

API void narrator_xfree(narrator ** const restrict n)
{
  narrator_free(*n);
  *n = 0;
}

API xapi narrator_seek(narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  enter;

  // dispatch
  if(n->type == NARRATOR_GROWING)
		fatal(growing_seek, &n->growing, offset, whence, res);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_seek, &n->fixed, offset, whence, res);
	else if(n->type == NARRATOR_FILE)
    fatal(file_seek, &n->file, offset, whence, res);
	else if(n->type == NARRATOR_MULTI)
    fatal(multi_seek, &n->multi, offset, whence, res);
	else if(n->type == NARRATOR_NULLITY)
    fatal(nullity_seek, &n->nullity, offset, whence, res);
	else if(n->type == NARRATOR_RECORD)
    fatal(record_seek, &n->record, offset, whence, res);

  finally : coda;
}

API xapi narrator_reset(narrator * const restrict n)
{
  xproxy(narrator_seek, n, 0, SEEK_SET, 0);
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
		fatal(growing_vsayf, &n->growing, fmt, va);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_vsayf, &n->fixed, fmt, va);
	else if(n->type == NARRATOR_FILE)
		fatal(file_vsayf, &n->file, fmt, va);
	else if(n->type == NARRATOR_MULTI)
		fatal(multi_vsayf, &n->multi, fmt, va);
	else if(n->type == NARRATOR_NULLITY)
		fatal(nullity_vsayf, &n->nullity, fmt, va);
	else if(n->type == NARRATOR_RECORD)
		fatal(record_vsayf, &n->record, fmt, va);

	finally : coda;
}

API xapi narrator_sayw(narrator * const restrict n, const char * const restrict b, size_t l)
{
  enter;

	if(n->type == NARRATOR_GROWING)
		fatal(growing_sayw, &n->growing, b, l);
	else if(n->type == NARRATOR_FIXED)
		fatal(fixed_sayw, &n->fixed, b, l);
	else if(n->type == NARRATOR_FILE)
		fatal(file_sayw, &n->file, b, l);
	else if(n->type == NARRATOR_MULTI)
		fatal(multi_sayw, &n->multi, b, l);
	else if(n->type == NARRATOR_NULLITY)
		fatal(nullity_sayw, &n->nullity, b, l);
	else if(n->type == NARRATOR_RECORD)
		fatal(record_sayw, &n->record, b, l);

	finally : coda;
}
