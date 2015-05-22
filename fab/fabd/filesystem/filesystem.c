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

#include "xlinux.h"

#include "global.h"

#include "path.h"

#define restrict __restrict

//
// private
//

//
// protected
//

void filesystem_free(filesystem * const restrict fs)
{
	if(fs)
	{
		path_free(fs->path);
	}

	free(fs);
}

void filesystem_xfree(filesystem ** const restrict fs)
{
	filesystem_free(*fs);
	*fs = 0;
}


int filesystem_mk(filesystem * const restrict e, char * const restrict path, uint64_t attrs, filesystem ** restrict fs)
{
	filesystem * lfs = 0;

	if(!fs)
		fs = &lfs;

	*fs = e;
	if(!*fs)
		fatal(xmalloc, fs, sizeof(**fs));

	if(path)
	{
		fatal(path_create, &(*fs)->path, g_args->init_fabfile_path->abs_dir, "%s", path);
	}

	if(attrs)
	{
		(*fs)->attrs = attrs;
	}

	finally : coda;
}

//
// public
//

int filesystem_lookup(char * const restrict path, filesystem ** const restrict fs)
{
	finally : coda;
}

int filesystem_init()
{
	finally : coda;
}

void filesystem_teardown()
{

}