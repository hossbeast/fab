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

#ifndef _XAPI_EXIT_H
#define _XAPI_EXIT_H

#include "xapi.h"

#define restrict __restrict

/// xapi_exit_errname
//
// SUMMARY
//  get the error name from an exit value, e.g. "ENOMEM"
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error name or null if no such etable is registered
//
const char * xapi_exit_errname(const xapi exit);

/// xapi_exit_errdesc
//
// SUMMARY
//  get the error description from an exit value, e.g. "out of memory"
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error description or null if no such etable is registered
//
const char * xapi_exit_errdesc(const xapi exit);

/// xapi_exit_errstr
//
// SUMMARY
//  get the error string from an exit value, e.g. "ENOMEM : out of memory"
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error string or null if no such etable is registered
//
const char * xapi_exit_errstr(const xapi exit);

/// xapi_exit_errtab
//
// SUMMARY
//  get the error table from an exit value, e.g. perrtab_SYS
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error table or null if no such etable is registered
//
const etable * xapi_exit_errtab(const xapi exit);

/// xapi_exit_errcode
//
// SUMMARY
//  get the error code from an exit value, e.g. SYS_ENOMEM
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error code or zero if no such etable is registered
//
xapi_code xapi_exit_errcode(const xapi exit);

/// xapi_exit_errtabname
//
// SUMMARY
//  get the name of an error table from an exit value, e.g. "SYS"
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error table name or null if no such etable is registered
//
const char * xapi_exit_errtab_name(const xapi exit);

/// xapi_exit_errtabid
//
// SUMMARY
//  get the id of an error table from an exit value
//
// PARAMETERS
//  exit - exit value
//
// RETURNS
//  error table id or zero if no such etable is registered
//
xapi_etable_id xapi_exit_errtab_id(const xapi exit);

/// xapi_exit_synth
//
// SUMMARY
//  create an exit value from an error table ane error code
//
// PARAMETERS
//  etab    - error table
//  errcode - error code
//
// RETURNS
//  exit value
//
xapi xapi_exit_synth(const etable * const restrict etab, const xapi_code errcode)
  __attribute__((nonnull));

#undef restrict
#endif
