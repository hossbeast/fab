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

#ifndef _XAPI_ERROR_H
#define _XAPI_ERROR_H

#include "xapi.h"

#define restrict __restrict

/// xapi_errtab_errname
//
// SUMMARY
//  get the error name from an exit value, e.g. "ENOMEM"
//
// PARAMETERS
//  exit - exit value
//
const char * xapi_errname(const xapi exit);

/// xapi_errtab_errdesc
//
// SUMMARY
//  get the error description string from an exit value, e.g. "out of memory"
//
// PARAMETERS
//  exit - exit value
//
const char * xapi_errdesc(const xapi exit);

/// xapi_errtab_errstr
//
// SUMMARY
//  get the error string from an exit value, e.g. "ENOMEM : out of memory"
//
// PARAMETERS
//  exit - exit value
//
const char * xapi_errstr(const xapi exit);

/// xapi_errtab_errtab
//
// SUMMARY
//  get the error table from an exit value, e.g. perrtab_SYS
//
// PARAMETERS
//  exit - exit value
//
const struct etable * xapi_errtab(const xapi exit);

/// xapi_errtab_errcode
//
// SUMMARY
//  get the error code from an exit value, e.g. SYS_ENOMEM
//
// PARAMETERS
//  exit - exit value
//
int xapi_errcode(const xapi exit);

#undef restrict
#endif
