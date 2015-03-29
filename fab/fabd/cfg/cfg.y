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

%code top {
	#include <stdlib.h>
	#include <string.h>

	#include "global.h"

	#include "cfg.parse.h"
	#include "cfg.protected.h"

	// defined in cfg.lex.o
	int cfg_yylex(void* yylvalp, void* yylloc, void* scanner);
}

%define api.pure
%error-verbose
%locations
%name-prefix "cfg_yy"
%parse-param { void* scanner }
%parse-param { parse_param* parm }
%lex-param { void* scanner }

/* zero based lines and columns */
%initial-action { memset(&@$, 0, sizeof(@$)); }

%union {
	struct cfg_file *	cfg;
	struct filesystem *	fs;
	struct pstring * string;
}

/* tokens */
%token ':'
%token ','
%token '{'
%token '}'

/* terminals */
%token FILESYSTEM
%token PATH
%token INVALIDATION_METHOD STAT CONTENT NOTIFY ALWAYS NEVER
%token LF WORD

/* nonterminals */
%type <cfg>					declarations
%type <filesystem> 	declaration
%type <filesystem>	properties propertylist property
%type <string>			string unquoted-string quoted-string strparts strpart
%type <uint64_t>  	invalidation_method

%destructor { ffn_free($$); } <node>

%%
utterance
	: declarations separator
	{
		parm->cfg = $1;
	}
	| declarations
	{
		parm->cfg = $1;
	}
	;

declarations
	: declarations separator declaration
	{
		YYU_FATAL(cfg_file_mk, $1, $3, &$$);
	}
	| declaration
	{
		YYU_FATAL(cfg_file_mk, 0, $1, &$$);
	}
	;

declaration
	: FILESYSTEM '{' properties '}'
	{
		YYU_FATAL(filesystem_mk, $3, &$$);
	}
	;

separator
	: separator sep
	| sep
	;

sep
	: LF
	| ','
	;

properties
	: separator propertylist separator
	{
		$$ = $2;
	}
	| separator propertylist
	{
		$$ = $2;
	}
	| propertylist separator
	{
		$$ = $1;
	}
	| propertylist
	;

propertylist
	: propertylist separator property
	| property
	;

property
	: PATH ':' string
	{
		YYU_FATAL(filesystem_mk_path, $3, &$$);
	}
	| INVALIDATION_METHOD ':' invalidation_method
	{
		YYU_FATAL(filesystem_mk_method, $3, &$$);
	}
	;

invalidation_method
	: STAT
	{
		$$ = FILESYSTEM_INVALIDATION_METHOD_STAT;
	}
	| CONTENT
	{
		$$ = FILESYSTEM_INVALIDATION_METHOD_CONTENT;
	}
	| NOTIFY
	{
		$$ = FILESYSTEM_INVALIDATION_METHOD_INOTIFY;
	}
	| ALWAYS
	{
		$$ = FILESYSTEM_INVALIDATION_METHOD_ALWAYS;
	}
	| NEVER
	{
		$$ = FILESYSTEM_INVALIDATION_METHOD_NEVER;
	}
	;

string
	: unquoted-string
	| quoted-string
	;

unquoted-string
	: strpart
	;

quoted-string
	: '"' strparts '"'
	{
		$$ = $2;
	}
	;

strparts
	: strparts strpart
	{
		YYU_FATAL(psmkw, $1, @2.s, @2.l, &$$);
	}
	| strpart
	;

strpart
	: WORD
	{
		YYU_FATAL(psmkw, 0, @1.s, @1.l, &$$);
	}
	| CREF
	{

	}
	| HREF
	{
		
	}
	;
