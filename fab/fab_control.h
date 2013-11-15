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

#ifndef _FAB_CONTROL_H
#define _FAB_CONTROL_H

#include "control_core.h"

#include "log.h"
#include "args.h"

#define CODA_BAD_ACTION															\
	do {																							\
		if(g_args.mode_errors == MODE_ERRORS_IMMEDIATE)	\
			_coda_r = -1;																	\
		else																						\
			_coda_r = 1;																	\
	} while(0)

#define CODA_GOOD_ACTION														\
	_coda_r = 0

#define HANDLE_ERROR(__fmt, ...)										\
	log_error(L_ERROR, __fmt													\
		, __FUNCTION__																	\
		, __FILE__																			\
		, __LINE__																			\
		, ##__VA_ARGS__																	\
	)																									\

#define HANDLE_INFO(fmt, ...)												\
	log(L_INFO, fmt 																	\
		, ##__VA_ARGS__																	\
	)																									\

#define HANDLE_WARN(fmt, ...)												\
	log(L_WARN, fmt 																	\
		, ##__VA_ARGS__																	\
	)																									\

#endif
