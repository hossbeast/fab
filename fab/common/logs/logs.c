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

#include "logs.h"

// logtag definitions
struct g_logs_t logs[] = {
	  { .v = L_ERROR		, .s = "ERROR"		, .d = "fatal errors" }
	, { .v = L_WARN			, .s = "WARN"			, .d = "nonfatal warnings" }
	, { .v = L_INFO			, .s = "INFO"			, .d = "program flow" }
	, { .v = L_ARGS			, .s = "ARGS"			, .d = "program arguments" }
	, { .v = L_PARAMS		, .s = "PARAMS"		, .d = "program execution parameters" }
#if DEBUG || DEVEL
	, { .v = L_USAGE		, .s = "USAGE"		, .d = "fabd resource usage" }
	, { .v = L_FFTOKEN	, .s = "FFTOKEN"	, .d = "fabfile parsing - token stream" }
	, { .v = L_FFSTATE	, .s = "FFSTATE"	, .d = "fabfile parsing - lexer states" }
	, { .v = L_FFTREE		, .s = "FFTREE"		, .d = "fabfile parsing - parsed tree" }
	, { .v = L_FFFILE		, .s = "FFFILE"		, .d = "fabfile parsing - parsed file" }
#endif
	, { .v = L_BPINFO		, .s = "BPINFO"		, .d = "buildplan - flow" }
	, { .v = L_BPEXEC		, .s = "BPEXEC"		, .d = "buildplan - execution" }
	, { .v = L_BPEVAL		, .s = "BPEVAL"		, .d = "buildplan - pruning/evaluating" }
	, { .v = L_BPDUMP		, .s = "BPDUMP"		, .d = "buildplan - log final buildplan" }
	, { .v = L_BPCMD		, .s = "BPCMD"		, .d = "buildplan - commands" }
	, { .v = L_DSCINFO	, .s = "DSCINFO"	, .d = "dependency discovery - flow" }
	, { .v = L_DSCEXEC	, .s = "DSCEXEC"	, .d = "dependency discovery - execution" }
	, { .v = L_DSCCMD		, .s = "DSCCMD"		, .d = "dependency discovery - commands" }
	, { .v = L_DSCRES		, .s = "DSCRES"		, .d = "dependency discovery - results" }
	, { .v = L_NODE			, .s = "NODE"			, .d = "dependency graph node details (+i)" }
	, { .v = L_INVALID	, .s = "INVALID"	, .d = "dependency graph node invalidations" }
	, { .v = L_VARAUTO	, .s = "VARAUTO"	, .d = "variable definitions - automatic" }
	, { .v = L_VARUSER	, .s = "VARUSER"	, .d = "variable definitions - user-defined" }
	, { .v = L_VAR			, .s = "VAR"			, .d = "variable definitions" }
	, { .v = L_INVOKE		, .s = "INVOKE"		, .d = "fabfile invocations" }
	, { .v = L_SELECT		, .s = "SELECT"		, .d = "selector execution" }
	, { .v = L_LISTS		, .s = "LISTS"		, .d = "node lists" }
#if DEBUG || DEVEL
	, { .v = L_LWEXEC		, .s = "LWEXEC"		, .d = "liblistwise - execution" }
	, { .v = L_LWOPINFO	, .s = "LWOPINFO"	, .d = "liblistwise - operator info messages" }
	, { .v = L_LWPARSE	, .s = "LWPARSE"	, .d = "liblistwise - transform parsing" }
	, { .v = L_LWTOKEN	, .s = "LWTOKEN"	, .d = "liblistwise - transform parsing - token stream" }
	, { .v = L_LWSTATE	, .s = "LWSTATE"	, .d = "liblistwise - transform parsing - lexer states" }
#endif
#if SANITY
	, { .v = L_LWSANITY	, .s = "LWSANITY"	, .d = "liblistwise - sanity checks" }
#endif
};

struct g_logs_t * g_logs = logs;
int g_logs_l = sizeof(logs) / sizeof(logs[0]);
