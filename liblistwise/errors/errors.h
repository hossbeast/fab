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

#ifndef _ERRORS_H
#define _ERRORS_H

#include "xapi.h"

#define EPREFIX LW_ERROR

#define ETABLE																																	\
	_E(GENERATOR_SYNTAX		, 0x01	, "generator string could not be parsed")				\
	_E(GENERATOR_INVALID	, 0x02	, "generator contains invalid elements")				\
	_E(GENERATOR_ILLBYTE	, 0x03	, "generator string contains illegal byte(s)")	\
	_E(GENERATOR_BACKREF	, 0x04	, "generator contains illegal backreference")

enum
{
#define _E(a, b, c) EPREFIX _ a = b,
ETABLE
#undef _E
};

extern etable errtab_LIBLW;

#endif
