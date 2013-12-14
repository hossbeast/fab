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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "xapi.h"

#include "xdlfcn.h"

int xdlopen(const char * filename, int flag, void ** dl)
{
	if(((*dl) = dlopen(filename, flag)) == 0)
	{
		fail(0, "%s", dlerror());
	}

finally :
	XAPI_INFO(1, "path", "%s", filename);
coda;
}

int xdlsym(void * dl, const char * sym, void ** psym)
{
	dlerror();
	(*sym) = dlsym(dl, sym);
	char * e = dlerror();
	if(e)
	{
		fail(0, "%s", dlerror());
	}

finally :
	XAPI_INFO(1, "sym", "%s", sym);
coda;
}
