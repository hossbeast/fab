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

#ifndef _XAPI_ERRTAB_CODE_H
#define _XAPI_ERRTAB_CODE_H

#include "xapi.h"

#define restrict __restrict

/// xapi_errtab_register
//
// SUMMARY
//  register an error table for xapi/error apis
//  the error table is assigned an id and etab->id is written to
//
// PARAMETERS
//  etab - error table
//
// NOTES
//  not threadsafe
//
xapi xapi_errtab_register(struct etable * const restrict etab);

/// xapi_errtab_tag
//
// SUMMARY
//  get the tag for an etab
//
// PARAMETERS
//  [etab] - error table
//
char * xapi_errtab_tag(const etable * const restrict etab);

/// xapi_errtab_byid
//
// SUMMARY
//  lookup an error table by id
//
const struct etable * xapi_errtab_byid(const xapi_etable_id id);

/// xapi_errtab_errname
//
// SUMMARY
//  get the error name from an exit value and etable, e.g. "ENOMEM"
//
// PARAMETERS
//  exit   - exit value
//  [etab] - etable
//
const char * xapi_errtab_errname(const xapi exit, const etable * const restrict etab);

/// xapi_errtab_errdesc
//
// SUMMARY
//  get the error description string from an exit value and etable, e.g. "out of memory"
//
// PARAMETERS
//  exit   - exit value
//  [etab] - etable
//
const char * xapi_errtab_errdesc(const xapi exit, const etable * const restrict etab);

/// xapi_errtab_errstr
//
// SUMMARY
//  get the error string from an exit value and etable, e.g. "ENOMEM : out of memory"
//
// PARAMETERS
//  exit   - exit value
//  [etab] - etable
//
const char * xapi_errtab_errstr(const xapi exit, const etable * const restrict etab);

/// xapi_errtab_errcode
//
// SUMMARY
//  get the error code from an exit value and etable, e.g. SYS_ENOMEM
//
// PARAMETERS
//  exit   - exit value
//  [etab] - etable
//
xapi_code xapi_errtab_errcode(const xapi exit, const etable * const restrict etab);

#undef restrict
#endif
