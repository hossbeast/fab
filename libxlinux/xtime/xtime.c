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

#include <errno.h>

#include "internal.h"
#include "xtime.h"

int API xlocaltime_r(const time_t * timep, struct tm * result)
{
	if(localtime_r(timep, result) == 0)
		fail(errno);

	finally : coda;
}

int API xclock_gettime(clockid_t clk_id, struct timespec * tp)
{
	fatalize(errno, clock_gettime, clk_id, tp);

	finally : coda;
}
