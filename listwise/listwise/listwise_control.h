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

#ifndef _LISTWISE_CONTROL_H
#define _LISTWISE_CONTROL_H

#include "control_core.h"

#if UNWIND
# define UNWIND_ERRORS listwise_errors_unwind
#else
# define UNWIND_ERRORS 0
#endif

#define CODA_BAD_ACTION                             \
	_coda_r = FAILURE_CODE

#define CODA_GOOD_ACTION                            \
  _coda_r = 0;                                      \

#define LOG_ERROR(fmt, ...)													\
	printf(fmt "\n"																		\
		, ##__VA_ARGS__																	\
	);																								\

#endif
