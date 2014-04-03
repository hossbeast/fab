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

#include <errno.h>

#include "internal.h"

#include "xdlfcn.h"

#undef perrtab
#define perrtab perrtab_XLINUX

int API xdlopen(const char * filename, int flag, void ** dl)
{
	if(((*dl) = dlopen(filename, flag)) == 0)
		fails(XLINUX_DLERROR, dlerror());

finally :
	XAPI_INFOF("path", "%s", filename);
coda;
}

int API xdlsym(void * dl, const char * sym, void ** psym)
{
	dlerror();
	(*psym) = dlsym(dl, sym);
	char * e = dlerror();
	if(e)
		fails(XLINUX_DLERROR, e);

finally :
	XAPI_INFOF("sym", "%s", sym);
coda;
}