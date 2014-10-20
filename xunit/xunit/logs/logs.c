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

#include "logs.h"

// logtag definitions
struct g_logs_t logs[] = {
#if DEVEL
	  { .v = L_LOGGER		, .s = "LOGGER"		, .d = "logger - operation" }
	,
#endif
	  { .v = L_XUNIT		, .s = "XUNIT"		, .d = "unit tests" }
};

struct g_logs_t * g_logs = logs;
int g_logs_l = sizeof(logs) / sizeof(logs[0]);
