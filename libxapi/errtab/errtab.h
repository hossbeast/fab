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

#if XAPI_STACKTRACE
#define restrict __restrict

/// xapi_errtab_stage
//
// SUMMARY
//  register an error table for xapi_error apis
//  the error table is assigned an id and etab->id is written to
//
// PARAMETERS
//  etab - error table
//
// REMARKS
//  meant to be called from __attribute__((constructor)) functions, limited
//  to a fixed number of tables that may be staged
//
void xapi_errtab_stage(errtab * const restrict etab)
  __attribute__((nonnull));

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
// REMARKS
//  able to register new error tables at runtime, not limited
//
xapi xapi_errtab_register(errtab * const restrict etab)
  __attribute__((nonnull));

/// xapi_errtab_byid
//
// SUMMARY
//  lookup an error table by id
//
const errtab * xapi_errtab_byid(const xapi_errtab_id id)
  __attribute__((nonnull));

//
// exit value api
//

/// xapi_errtab_errname
//
// SUMMARY
//  get the error name from an exit value and errtab, e.g. "ENOMEM"
//
// PARAMETERS
//  [etab] - errtab
//  exit   - exit value
//
const char * xapi_errtab_errname(const errtab * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errdesc
//
// SUMMARY
//  get the error description from an exit value and errtab, e.g. "out of memory"
//
// PARAMETERS
//  [etab] - errtab
//  exit   - exit value
//
const char * xapi_errtab_errdesc(const errtab * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errstr
//
// SUMMARY
//  get the error string from an exit value and errtab, e.g. "ENOMEM : out of memory"
//
// PARAMETERS
//  [etab] - errtab
//  exit   - exit value
//
const char * xapi_errtab_errstr(const errtab * const restrict etab, const xapi exit)
  __attribute__((nonnull));

/// xapi_errtab_errcode
//
// SUMMARY
//  get the error code from an exit value and errtab, e.g. SYS_ENOMEM
//
// PARAMETERS
//  [etab] - errtab
//  exit   - exit value
//
xapi_code xapi_errtab_errcode(const errtab * const restrict etab, const xapi exit)
  __attribute__((nonnull));

#undef restrict
#endif
#endif
