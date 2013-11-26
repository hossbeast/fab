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

#ifndef _XAPI_H
#define _XAPI_H

/*
** xapi is an interface for propagating detailed and specific error information
**
** xapi specifies a calling convention and provides macros to facilitate its application
**
** xapi-enabled code operates in one of three modes specifying what is provided when an error occurs :
**  1. unwind    - a complete backtrace
**  2. immediate - an error message
**  3. errcode   - a nonzero error code
**
** which of these modes is possible is specified at compile-time, and selected at runtime during
** program initialization
*/

#if XAPI_UNWIND
# include "xapi/unwind.h"
#elif XAPI_ERRCODE
# include "xapi/errcode.h"
#else
# error "either XAPI_UNWIND or XAPI_ERRCODE must be defined"
#endif

#endif
