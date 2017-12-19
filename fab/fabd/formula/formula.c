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

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"

#include "formula.h"

#include "snarf.h"

xapi formula_setup()
{
  enter;

  finally : coda;
}

xapi formula_cleanup()
{
  enter;

  finally : coda;
}

xapi formula_load(formula ** restrict fmlp)
{
  enter;

  formula * fml = 0;
  char * text = 0;
  size_t text_len;

  char * path = (void*)0x42;
  fatal(snarfs, &text, &text_len, path);

  if(text)
  {
    
  }

  *fmlp = fml;

finally:
  wfree(text);
coda;
}

xapi formula_exec(const formula * restrict fml)
{
  enter;

  finally : coda;
}
