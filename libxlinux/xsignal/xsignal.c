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

#include "internal.h"

int xkill(pid_t pid, int sig)
{
	fatalize(errno, kill, pid, sig);

	finally : coda;
}

int uxkill(pid_t pid, int sig, int * r)
{
	fatalize(errno, kill, pid, sig);

	if(r && ((*r) = kill(pid, sig)) == -1 && errno != ESRCH)
	{
		fail(errno);
	}
	else if(!r && kill(pid, sig) == -1 && errno != ESRCH)
	{
		fail(errno);
	}

	finally : coda;
}
