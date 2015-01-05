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
#if DEBUG || DEVEL
	  { .v = L_LOGGER		, .s = "LOGGER"		, .d = "logger - operation" }
	,
#endif
	  { .v = L_ERROR		, .s = "ERROR"		, .d = "errors leading to shutdown" }
	, { .v = L_WARN			, .s = "WARN"			, .d = "nonfatal warnings" }
	, { .v = L_INFO			, .s = "INFO"			, .d = "program flow" }
	, { .v = L_ARGS			, .s = "ARGS"			, .d = "program arguments" }
	, { .v = L_PARAMS		, .s = "PARAMS"		, .d = "program execution parameters" }
	, { .v = L_INSET		, .s = "INSET"		, .d = "unit test input set" }
	, { .v = L_OUTSET		, .s = "OUTSET"		, .d = "unit test output set" }
	, { .v = L_DLOAD		, .s = "DLOAD"		, .d = "dload details" }
	, { .v = L_XUNIT		, .s = "XUNIT"		, .d = "unit test results" }
};

struct g_logs_t * g_logs = logs;
int g_logs_l = sizeof(logs) / sizeof(logs[0]);
