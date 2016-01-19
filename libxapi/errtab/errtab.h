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

#ifndef _XAPI_ERRTAB_NOTGENERATED_H
#define _XAPI_ERRTAB_NOTGENERATED_H

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
xapi xapi_errtab_register(etable * const restrict etab)
  __attribute__((nonnull));

/// xapi_errtab_byid
//
// SUMMARY
//  lookup an error table by id
//
const etable * xapi_errtab_byid(const xapi_etable_id id)
  __attribute__((nonnull));

/// xapi_errtab_name
//
// SUMMARY
//  get the name for an etab
//
// PARAMETERS
//  [etab] - error table
//
const char * xapi_errtab_name(const etable * const restrict etab)
  __attribute__((nonnull));

/// xapi_errtab_name
//
// SUMMARY
//  get the id for an etab
//
// PARAMETERS
//  [etab] - error table
//
xapi_etable_id xapi_errtab_id(const etable * const restrict etab)
  __attribute__((nonnull));

//
// exit value api
//

/// xapi_errtab_errname
//
// SUMMARY
//  get the error name from an exit value and etable, e.g. "ENOMEM"
//
// PARAMETERS
//  [etab] - etable
//  exit   - exit value
//
const char * xapi_errtab_errname(const etable * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errdesc
//
// SUMMARY
//  get the error description from an exit value and etable, e.g. "out of memory"
//
// PARAMETERS
//  [etab] - etable
//  exit   - exit value
//
const char * xapi_errtab_errdesc(const etable * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errstr
//
// SUMMARY
//  get the error string from an exit value and etable, e.g. "ENOMEM : out of memory"
//
// PARAMETERS
//  [etab] - etable
//  exit   - exit value
//
const char * xapi_errtab_errstr(const etable * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errcode
//
// SUMMARY
//  get the error code from an exit value and etable, e.g. SYS_ENOMEM
//
// PARAMETERS
//  [etab] - etable
//  exit   - exit value
//
xapi_code xapi_errtab_errcode(const etable * const restrict etab, const xapi exit)
  __attribute__((nonnull));

#undef restrict
#endif
