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

#ifndef _XAPI_MM_H
#define _XAPI_MM_H

#define XAPI_BYTES_DEFAULT (1024 * 25)

/// xapi_allocate
//
// SUMMARY
//  cause libxapi to allocate memory for backtrace tracking
//
// PARAMETERS
//  bytes - 
//
API void xapi_allocate(size_t bytes);

#endif
