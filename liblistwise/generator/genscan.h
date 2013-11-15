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

/* escape mode is whether the scanmode supports backslash references */
#define GENSCAN_ESCAPE_MODE									0x00F
#define GENSCAN_ESCAPE_MODE_DOREFS					0x000
#define GENSCAN_ESCAPE_MODE_NOREFS					0x001

// interrogate a genscan mode to determine whether it supports backslash references
#define GS_DOREFS(x) (((x) & GENSCAN_ESCAPE_MODE) == GENSCAN_ESCAPE_MODE_DOREFS)
#define GS_NOREFS(x) (!(GS_DOREFS(x)))

/* separation mode is whether the scanmode separates strings by a delimiter or has enclosed arguments */
#define GENSCAN_SEPARATION_MODE							0x0F0
#define GENSCAN_SEPARATION_MODE_DELIMITED		0x000
#define GENSCAN_SEPARATION_MODE_ENCLOSED		0x010

// interrogate a genscan mode to determine whether it is delimited or enclosed
#define GS_ENCLOSED(x)		(((x) & GENSCAN_SEPARATION_MODE) == GENSCAN_SEPARATION_MODE_ENCLOSED)
#define GS_DELIMITED(x)   (!(GS_ENCLOSED(x)))

/* tokens is an identifier for the token(s) used by the scanmode - which implies a separation mode */
#define GENSCAN_TOKENS								 0xFFF0
#define GENSCAN_TOKENS_SLASH					(0x0000 | GENSCAN_SEPARATION_MODE_DELIMITED)
#define GENSCAN_TOKENS_COMMA					(0x0100 | GENSCAN_SEPARATION_MODE_DELIMITED)
#define GENSCAN_TOKENS_DOT						(0x0200 | GENSCAN_SEPARATION_MODE_DELIMITED)
#define GENSCAN_TOKENS_COLON					(0x0400 | GENSCAN_SEPARATION_MODE_DELIMITED)
#define GENSCAN_TOKENS_SEMICOLON			(0x0800 | GENSCAN_SEPARATION_MODE_DELIMITED)
#define GENSCAN_TOKENS_BRACES					(0x1000 | GENSCAN_SEPARATION_MODE_ENCLOSED)
#define GENSCAN_TOKENS_BRACKETS				(0x2000 | GENSCAN_SEPARATION_MODE_ENCLOSED)
#define GENSCAN_TOKENS_PARENS					(0x4000 | GENSCAN_SEPARATION_MODE_ENCLOSED)
#define GENSCAN_TOKENS_ANGLES					(0x8000 | GENSCAN_SEPARATION_MODE_ENCLOSED)

// table of genscan modes with DOREFS
#define GENSCAN_TABLE_DOREFS(x) 																																																												\
	/* delimited scanmodes */																																																															\
  _GENSCAN(GENSCAN_SLASH_DOREFS			, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_SLASH			, '/'	, 0		, slash_dorefs			,	slash_dorefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_DOREFS			, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_COMMA			, ','	, 0		, comma_dorefs			,	comma_dorefs			, x)	\
  _GENSCAN(GENSCAN_DOT_DOREFS				, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_DOT				, '.'	, 0		, dot_dorefs				,	dot_dorefs				, x)	\
  _GENSCAN(GENSCAN_COLON_DOREFS			, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_COLON			, ':'	, 0		, colon_dorefs			,	colon_dorefs			, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_DOREFS	, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_SEMICOLON	, ';'	, 0		, semicolon_dorefs	,	semicolon_dorefs	, x)	\
	/* enclosed scanmodes */																																																															\
  _GENSCAN(GENSCAN_BRACES_DOREFS		, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_BRACES			, '{'	, '}'	, braces						,	braces_dorefs			, x)	\
  _GENSCAN(GENSCAN_BRACKETS_DOREFS	, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_BRACKETS		, '['	, ']'	, brackets					,	brackets_dorefs		, x)	\
  _GENSCAN(GENSCAN_PARENS_DOREFS		, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_PARENS			, '('	, ')'	, parens						,	parens_dorefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_DOREFS		, GENSCAN_ESCAPE_MODE_DOREFS	| GENSCAN_TOKENS_ANGLES			, '<'	, '>'	, angles						,	angles_dorefs			, x)	\

// table of genscan modes with NOREFS
#define GENSCAN_TABLE_NOREFS(x) 																																																												\
	/* delimited scanmodes */																																																															\
  _GENSCAN(GENSCAN_SLASH_NOREFS			, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_SLASH			, '/'	, 0		, slash_norefs			,	slash_norefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_NOREFS			, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_COMMA			, ','	, 0		, comma_norefs			,	comma_norefs			, x)	\
  _GENSCAN(GENSCAN_DOT_NOREFS				, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_DOT				, '.'	, 0		, dot_norefs				,	dot_norefs				, x)	\
  _GENSCAN(GENSCAN_COLON_NOREFS			, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_COLON			, ':'	, 0		, colon_norefs			,	colon_norefs			, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_NOREFS	, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_SEMICOLON	, ';'	, 0		, semicolon_norefs	,	semicolon_norefs	, x)	\
	/* enclosed scanmodes */																																																															\
  _GENSCAN(GENSCAN_BRACES_NOREFS		, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_BRACES			, '{'	, '}'	, braces						,	braces_norefs			, x)	\
  _GENSCAN(GENSCAN_BRACKETS_NOREFS	, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_BRACKETS		, '['	, ']'	, brackets					,	brackets_norefs		, x)	\
  _GENSCAN(GENSCAN_PARENS_NOREFS		, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_PARENS			, '('	, ')'	, parens						,	parens_norefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_NOREFS		, GENSCAN_ESCAPE_MODE_NOREFS	| GENSCAN_TOKENS_ANGLES			, '<'	, '>'	, angles						,	angles_norefs			, x)	\

// declare genscan modes
enum {
#define _GENSCAN(a, b, c, d, e, f, g) a = b,
GENSCAN_TABLE_DOREFS(0)
GENSCAN_TABLE_NOREFS(0)
#undef _GENSCAN
};

/// genscan_num
//
// number of genscan modes
//
extern int genscan_num;

/// genscan_by_number
//
// zero-based indexed up to genscan_num-1, contains genscan_mode values
//
extern uint32_t * genscan_by_number;

/// genscan_by_opening_char_dorefs
//
// indexed by int8_t, yields the DOREFS genscan_mode with that opening char
//
extern uint32_t * genscan_by_opening_char_dorefs;

/// genscan_by_opening_char_norefs
//
// indexed by int8_t, yields the NOREFS genscan_mode with that opening char
//
extern uint32_t * genscan_by_opening_char_norefs;

/// genscan_opening_char
//
// indexed by scanmode, yields opening char
//
extern char * genscan_opening_char;

/// genscan_closing_char
//
// indexed by scanmode, yields closing char
//
extern char * genscan_closing_char;

/// genscan_startcondition_initial
//
// indexed by scanmode, yields initial start condition (scanner state) for that scanmode
//
extern int * genscan_startcondition_initial;

/// genscan_startcondition_argscan
//
// indexed by scanmode, yields arg scanning start condition (scanner state) for that scanmode
//
extern int * genscan_startcondition_argscan;

/// genscan_parse
//
// parse a genscan mode from a ?scanmode directive
//
uint32_t genscan_parse(char * s, int l)
	__attribute__((nonnull));

/// GENSCAN_STR
//
// get string name from genscan mode
//
#define _GENSCAN(a, b, c, d, e, f, g) (g) == (b) ? #a :
#define GENSCAN_STR(x) GENSCAN_TABLE_DOREFS(x) GENSCAN_TABLE_NOREFS(x) "GENSCAN_UNKNWN"

#endif
