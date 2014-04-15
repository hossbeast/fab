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
	  { .v = L_ERROR		, .s = "ERROR"		, .d = "errors leading to shutdown" }
	, { .v = L_WARN			, .s = "WARN"			, .d = "nonfatal warnings" }
	, { .v = L_INFO			, .s = "INFO"			, .d = "program flow" }
	, { .v = L_ARGS			, .s = "ARGS"			, .d = "program arguments" }
	, { .v = L_PARAMS		, .s = "PARAMS"		, .d = "program execution parameters" }
#if DEVEL
	, { .v = L_FFTOKEN	, .s = "FFTOKEN"	, .d = "fabfile parsing - token stream" }
	, { .v = L_FFSTATE	, .s = "FFSTATE"	, .d = "fabfile parsing - lexer states" }
	, { .v = L_FFTREE		, .s = "FFTREE"		, .d = "fabfile parsing - parsed tree" }
	, { .v = L_FFFILE		, .s = "FFFILE"		, .d = "fabfile parsing - parsed file" }
	, { .v = L_FF				, .s = "FF"				, .d = "fabfile parsing" }
#endif
	, { .v = L_BPINFO		, .s = "BPINFO"		, .d = "buildplan - flow" }
	, { .v = L_BPEXEC		, .s = "BPEXEC"		, .d = "buildplan - execution" }
	, { .v = L_BPEVAL		, .s = "BPEVAL"		, .d = "buildplan - pruning/evaluating" }
	, { .v = L_BPDUMP		, .s = "BPDUMP"		, .d = "buildplan - dump the final buildplan" }
	, { .v = L_BP				, .s = "BP"				, .d = "buildplan" }
	, { .v = L_FMLEXEC	, .s = "FMLEXEC"	, .d = "formulas - execution results/details" }
	, { .v = L_FML			, .s = "FML"			, .d = "formulas" }
	, { .v = L_FAB			, .s = "FAB"			, .d = "fabrication formulas" }
	, { .v = L_DSCINFO	, .s = "DSCINFO"	, .d = "dependency discovery - flow" }
	, { .v = L_DSCEXEC	, .s = "DSCEXEC"	, .d = "dependency discovery - execution" }
	, { .v = L_DSCNEW		, .s = "DSCNEW"		, .d = "dependency discovery - new nodes/edges" }
	, { .v = L_DSC			, .s = "DSC"			, .d = "dependency discovery" }
	, { .v = L_DGRAPH		, .s = "DGRAPH"		, .d = "dependency graph - dump/details" }
	, { .v = L_DG				, .s = "DG"				, .d = "dependency graph" }
	, { .v = L_VARAUTO	, .s = "VARAUTO"	, .d = "variable definitions - automatic" }
	, { .v = L_VARUSER	, .s = "VARUSER"	, .d = "variable definitions - user-defined" }
	, { .v = L_VAR			, .s = "VAR"			, .d = "variable definitions" }
	, { .v = L_INVOKE		, .s = "INVOKE"		, .d = "fabfile invocations" }
	, { .v = L_SELECT		, .s = "SELECT"		, .d = "node selectors" }
	, { .v = L_LISTS		, .s = "LISTS"		, .d = "node lists" }
	, { .v = L_INVALID	, .s = "INVALID"	, .d = "node invalidation" }
#if DEVEL
	, { .v = L_LWEXEC		, .s = "LWEXEC"		, .d = "liblistwise - execution" }
	, { .v = L_LWOPINFO	, .s = "LWOPINFO"	, .d = "liblistwise - operator info messages" }
	, { .v = L_LWPARSE	, .s = "LWPARSE"	, .d = "liblistwise - generator parsing" }
	, { .v = L_LWTOKEN	, .s = "LWTOKEN"	, .d = "liblistwise - generator parsing - token stream" }
	, { .v = L_LWSTATE	, .s = "LWSTATE"	, .d = "liblistwise - generator parsing - lexer states" }
#endif
#if SANITY
	, { .v = L_LWSANITY	, .s = "LWSANITY"	, .d = "liblistwise - sanity checks" }
#endif
};

struct g_logs_t * o_logs = logs;
int o_logs_l = sizeof(logs) / sizeof(logs[0]);
