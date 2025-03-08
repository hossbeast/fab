/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "types.h"

#include "xdlfcn.h"

void * xdlopen(const char * restrict filename, int flag)
{
  void *r;

  dlerror();
  r = dlopen(filename, flag);
  RUNTIME_ASSERT(r != 0);

  return r;
}

void xdlclose(void * dl)
{
  const char *e;

  dlerror();
  dlclose(dl);
  e = dlerror();
  RUNTIME_ASSERT(e == 0);
}

void ixdlclose(void ** dl)
{
  xdlclose(*dl);
  *dl = 0;
}

void * xdlsym(void * dl, const char * sym)
{
  void *r;
  const char *e;

  dlerror();
  r = dlsym(dl, sym);
  e = dlerror();
  RUNTIME_ASSERT(e == 0);

  return r;
}

void * uxdlsym(void * dl, const char * sym)
{
  void *r;
  const char *e;

  dlerror();
  r = dlsym(dl, sym);
  e = dlerror();
  RUNTIME_ASSERT(e == 0 || strstr(e, "undefined symbol") != 0);

  return r;
}
