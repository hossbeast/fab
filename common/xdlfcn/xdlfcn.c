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

#include "xapi.h"

#include "xdlfcn.h"

int xdlopen(const char * filename, int flag, void ** dl)
{
	if(((*dl) = dlopen(filename, flag)) == 0)
	{
		fatality("dlopen", 0, 0, "%s", dlerror());
	}

finally :
	XAPI_INFO("path", "%s", filename);
coda;
}

int xdlsym(void * dl, const char * sym, void ** psym)
{
	dlerror();
	(*psym) = dlsym(dl, sym);
	char * e = dlerror();
	if(e)
	{
		fatality("dlsym", 0, 0, "%s", dlerror());
	}

finally :
	XAPI_INFO("sym", "%s", sym);
coda;
}
