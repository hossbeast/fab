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

#ifndef _GENSCAN_H
#define _GENSCAN_H

#define GENSCAN_MODE								0x00F
#define GENSCAN_MODE_WITHREFS				0x000
#define GENSCAN_MODE_NOREFS					0x001

#define GS_REFS(x) (((x) & GENSCAN_MODE) == GENSCAN_MODE_REFS)

#define GENSCAN_CLOSURE							0x0F0
#define GENSCAN_CLOSURE_OPEN				0x000
#define GENSCAN_CLOSURE_CLOSED			0x010

#define GS_CLOSED(x) (((x) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_CLOSED)

#define GENSCAN_CHAR								0xFF0
#define GENSCAN_CHAR_SLASH					(0x000 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_COMMA					(0x100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_DOT						(0x100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_COLON					(0x100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_SEMICOLON			(0x100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_CHAR_BRACES					(0x200 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_CHAR_BRACKETS				(0x200 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_CHAR_PARENS					(0x200 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_CHAR_ANGLES					(0x200 | GENSCAN_CLOSURE_CLOSED)

// table of genscan modes
#define GENSCAN_TABLE(x) 																																																													\
	/* delimited scanmodes */																																																												\
  _GENSCAN(GENSCAN_SLASH_NOREFS				, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_SLASH			, '/'	, 0		, slash_norefs			,	slash_norefs			, x)	\
  _GENSCAN(GENSCAN_SLASH_WITHREFS			, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_SLASH			, '/'	, 0		, slash_dorefs			,	slash_dorefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_NOREFS				, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_COMMA			, ','	, 0		, comma_norefs			,	comma_norefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_WITHREFS			, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_COMMA			, ','	, 0		, comma_norefs			,	comma_dorefs			, x)	\
  _GENSCAN(GENSCAN_DOT_NOREFS					, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_DOT				, '.'	, 0		, dot_norefs				,	dot_norefs				, x)	\
  _GENSCAN(GENSCAN_DOT_WITHREFS				, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_DOT				, '.'	, 0		, dot_norefs				,	dot_dorefs				, x)	\
  _GENSCAN(GENSCAN_COLON_NOREFS				, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_COLON			, ':'	, 0		, colon_norefs			,	colon_norefs			, x)	\
  _GENSCAN(GENSCAN_COLON_WITHREFS			, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_COLON			, ':'	, 0		, colon_norefs			,	colon_dorefs			, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_NOREFS		, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_SEMICOLON	, ';'	, 0		, semicolon_norefs	,	semicolon_norefs	, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_WITHREFS	, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_SEMICOLON	, ';'	, 0		, semicolon_norefs	,	semicolon_dorefs	, x)	\
	/* enclosed scanmodes */																																																												\
  _GENSCAN(GENSCAN_BRACES_NOREFS			, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_BRACES			, '{'	, '}'	, braces						,	braces_norefs			, x)	\
  _GENSCAN(GENSCAN_BRACES_WITHREFS		, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_BRACES			, '{'	, '}'	, braces						,	braces_dorefs			, x)	\
  _GENSCAN(GENSCAN_BRACKETS_NOREFS		, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_BRACKETS		, '['	, ']'	, brackets					,	brackets_norefs		, x)	\
  _GENSCAN(GENSCAN_BRACKETS_WITHREFS	, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_BRACKETS		, '['	, ']'	, brackets					,	brackets_dorefs		, x)	\
  _GENSCAN(GENSCAN_PARENS_NOREFS			, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_PARENS			, '('	, ')'	, parens						,	parens_norefs			, x)	\
  _GENSCAN(GENSCAN_PARENS_WITHREFS		, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_PARENS			, '('	, ')'	, parens						,	parens_dorefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_NOREFS			, GENSCAN_MODE_NOREFS		| GENSCAN_CHAR_ANGLES			, '<'	, '>'	, angles						,	angles_norefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_WITHREFS		, GENSCAN_MODE_WITHREFS	| GENSCAN_CHAR_ANGLES			, '<'	, '>'	, angles						,	angles_dorefs			, x)	\

// declare genscan modes
enum {
#define _GENSCAN(a, b, c, d, e, f, g) a = b,
GENSCAN_TABLE(0)
#undef _GENSCAN
};

// number of genscan modes
int genscan_num;

// zero-based indexed up to genscan_num, contains genscan_mode values
uint32_t * genscan_modes;

// indexed by genscan mode, yields opening char
char * genscan_opener;

// indexed by genscan mode, yields closing char
char * genscan_closer;

// indexed by genscan mode, yields initial scan state for that scanmode
int * genscan_basestate;

// indexed by genscan mode, yields arg scanning state for that scanmode
int * genscan_argstate;

// parse a genscan mode from a !scanmode directive
uint32_t genscan_parse(char * s, int l)
	__attribute__((nonnull));

#define _GENSCAN(a, b, c, d, e, f, g) g == b ? #a :
#define GENSCAN_STR(x) GENSCAN_TABLE(x) "GENSCAN_UNKNWN"

#endif
