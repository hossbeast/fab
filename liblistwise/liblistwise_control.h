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

#ifndef _LIBLISTWISE_CONTROL_H
#define _LIBLISTWISE_CONTROL_H

#include "control_core.h"
#include "listwise/xtra.h"

#define CODA_BAD_ACTION                             	\
	if(listwise_errors_unwind)													\
		_coda_r = 1;																			\
	else																								\
		_coda_r = -1;																			\

#define CODA_GOOD_ACTION                            	\
  _coda_r = 0;                                      	\

#define HANDLE_ERROR(fmt, ...)												\
	dprintf(listwise_error_fd, fmt " at [%s:%d (%s)]\n"	\
		, ##__VA_ARGS__																		\
		, __FILE__																				\
		, __LINE__																				\
		, __FUNCTION__																		\
	);																									\

#define HANDLE_INFO(fmt, ...)													\
	dprintf(listwise_info_fd, fmt " at [%s:%d (%s)]\n"	\
		, ##__VA_ARGS__																		\
		, __FILE__																				\
		, __LINE__																				\
		, __FUNCTION__																		\
	);																									\

#define HANDLE_WARN(fmt, ...)													\
	dprintf(listwise_warn_fd, fmt " at [%s:%d (%s)]\n"	\
		, ##__VA_ARGS__																		\
		, __FILE__																				\
		, __LINE__																				\
		, __FUNCTION__																		\
	);																									\

#endif
