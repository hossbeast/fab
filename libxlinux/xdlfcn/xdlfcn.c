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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "types.h"

#include "xdlfcn/xdlfcn.h"

void * API xdlopen(const char * restrict filename, int flag)
{
  void *r;

  dlerror();
  r = dlopen(filename, flag);
  RUNTIME_ASSERT(r != 0);

  return r;
}

void API xdlclose(void * dl)
{
  const char *e;

  dlerror();
  dlclose(dl);
  e = dlerror();
  RUNTIME_ASSERT(e == 0);
}

void API ixdlclose(void ** dl)
{
  xdlclose(*dl);
  *dl = 0;
}

void * API xdlsym(void * dl, const char * sym)
{
  void *r;
  const char *e;

  dlerror();
  r = dlsym(dl, sym);
  e = dlerror();
  RUNTIME_ASSERT(e == 0);

  return r;
}

void * API uxdlsym(void * dl, const char * sym)
{
  void *r;
  const char *e;

  dlerror();
  r = dlsym(dl, sym);
  e = dlerror();
  RUNTIME_ASSERT(e == 0 || strstr(e, "undefined symbol") != 0);

  return r;
}
