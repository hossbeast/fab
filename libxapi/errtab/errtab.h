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

#ifndef _XAPI_ERRTAB_INTERNAL_H
#define _XAPI_ERRTAB_INTERNAL_H

extern etable **  tab;
extern size_t     tabl;

#define restrict __restrict

/// xapi_errtab_byid
//
// SUMMARY
//  lookup an error table by id
//
const struct etable * xapi_errtab_byid(const int id);

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
xapi xapi_errtab_register(struct etable * const restrict etab)
  __attribute__((nonnull));

/// xapi_errtab_tag
//
// SUMMARY
//  get the tag for an etab
//
// PARAMETERS
//  [etab] - error table
//
char * xapi_errtab_tag(const etable * const restrict etab);

/// errtab_teardown
//
// SUMMARY
//  release memory
//
void errtab_teardown();

#undef restrict
#endif
