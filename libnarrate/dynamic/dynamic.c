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

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "internal.h"

#define restrict __restrict

//
// public
//

int dynamic_vsayf(pstring ** ps, const char * const restrict fmt, va_list va)
{
	xproxy(psvcatf, ps, fmt, va);
}

int dynamic_sayw(pstring ** ps, char * const restrict b, size_t l)
{
	xproxy(pscatw, ps, b, l);
}