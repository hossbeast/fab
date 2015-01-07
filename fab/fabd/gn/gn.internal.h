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

#ifndef _GN_INTERNAL_H
#define _GN_INTERNAL_H

struct gn;

/// gn_idstring
//
// returns gn->idstring
//
char * gn_idstring(struct gn * gn)
	__attribute__((nonnull));

/// gn_typestring
//
// returns GN_TYPE_STR(gn->type) (after gn_finalize)
//
char * gn_typestring(struct gn * gn)
	__attribute__((nonnull));

#endif
