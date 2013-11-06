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

/* mode is whether the scanmode supports references */
#define GENSCAN_MODE								0x00F
#define GENSCAN_MODE_DOREFS					0x000
#define GENSCAN_MODE_NOREFS					0x001

#define GS_DOREFS(x) (((x) & GENSCAN_MODE) == GENSCAN_MODE_DOREFS)
#define GS_NOREFS(x) (!(GS_DOREFS(x)))

/* closure is whether the scanmode has enclosed arguments */
#define GENSCAN_CLOSURE							0x0F0
#define GENSCAN_CLOSURE_OPEN				0x000
#define GENSCAN_CLOSURE_CLOSED			0x010

#define GS_CLOSED(x) (((x) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_CLOSED)
#define GS_OPEN(x)   (!(GS_CLOSED(x)))

/* tokens is an identifier for the token(s) used by the scanmode - which implies a closure setting */
#define GENSCAN_TOKENS								 0xFFF0
#define GENSCAN_TOKENS_SLASH					(0x0000 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_TOKENS_COMMA					(0x0100 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_TOKENS_DOT						(0x0200 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_TOKENS_COLON					(0x0400 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_TOKENS_SEMICOLON			(0x0800 | GENSCAN_CLOSURE_OPEN)
#define GENSCAN_TOKENS_BRACES					(0x1000 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_TOKENS_BRACKETS				(0x2000 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_TOKENS_PARENS					(0x4000 | GENSCAN_CLOSURE_CLOSED)
#define GENSCAN_TOKENS_ANGLES					(0x8000 | GENSCAN_CLOSURE_CLOSED)

#define GS_SLASH(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_SLASH)
#define GS_COMMA(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_COMMA)
#define GS_DOT(x) 			(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_DOT)
#define GS_COLON(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_COLON)
#define GS_SEMICOLON(x) (((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_SEMICOLON)
#define GS_BRACES(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_BRACES)
#define GS_BRACKETS(x)	(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_BRACKETS)
#define GS_PARENS(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_PARENS)		
#define GS_ANGLES(x) 		(((x) & GENSCAN_TOKENS) == GENSCAN_TOKENS_ANGLES)

// table of genscan modes
#define GENSCAN_TABLE(x) 																																																												\
	/* delimited scanmodes */																																																											\
  _GENSCAN(GENSCAN_SLASH_NOREFS			, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_SLASH			, '/'	, 0		, slash_norefs			,	slash_norefs			, x)	\
  _GENSCAN(GENSCAN_SLASH_DOREFS			, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_SLASH			, '/'	, 0		, slash_dorefs			,	slash_dorefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_NOREFS			, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_COMMA			, ','	, 0		, comma_norefs			,	comma_norefs			, x)	\
  _GENSCAN(GENSCAN_COMMA_DOREFS			, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_COMMA			, ','	, 0		, comma_norefs			,	comma_dorefs			, x)	\
  _GENSCAN(GENSCAN_DOT_NOREFS				, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_DOT				, '.'	, 0		, dot_norefs				,	dot_norefs				, x)	\
  _GENSCAN(GENSCAN_DOT_DOREFS				, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_DOT				, '.'	, 0		, dot_norefs				,	dot_dorefs				, x)	\
  _GENSCAN(GENSCAN_COLON_NOREFS			, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_COLON			, ':'	, 0		, colon_norefs			,	colon_norefs			, x)	\
  _GENSCAN(GENSCAN_COLON_DOREFS			, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_COLON			, ':'	, 0		, colon_norefs			,	colon_dorefs			, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_NOREFS	, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_SEMICOLON	, ';'	, 0		, semicolon_norefs	,	semicolon_norefs	, x)	\
  _GENSCAN(GENSCAN_SEMICOLON_DOREFS	, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_SEMICOLON	, ';'	, 0		, semicolon_norefs	,	semicolon_dorefs	, x)	\
	/* enclosed scanmodes */																																																											\
  _GENSCAN(GENSCAN_BRACES_NOREFS		, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_BRACES			, '{'	, '}'	, braces						,	braces_norefs			, x)	\
  _GENSCAN(GENSCAN_BRACES_DOREFS		, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_BRACES			, '{'	, '}'	, braces						,	braces_dorefs			, x)	\
  _GENSCAN(GENSCAN_BRACKETS_NOREFS	, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_BRACKETS		, '['	, ']'	, brackets					,	brackets_norefs		, x)	\
  _GENSCAN(GENSCAN_BRACKETS_DOREFS	, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_BRACKETS		, '['	, ']'	, brackets					,	brackets_dorefs		, x)	\
  _GENSCAN(GENSCAN_PARENS_NOREFS		, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_PARENS			, '('	, ')'	, parens						,	parens_norefs			, x)	\
  _GENSCAN(GENSCAN_PARENS_DOREFS		, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_PARENS			, '('	, ')'	, parens						,	parens_dorefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_NOREFS		, GENSCAN_MODE_NOREFS	| GENSCAN_TOKENS_ANGLES			, '<'	, '>'	, angles						,	angles_norefs			, x)	\
  _GENSCAN(GENSCAN_ANGLES_DOREFS		, GENSCAN_MODE_DOREFS	| GENSCAN_TOKENS_ANGLES			, '<'	, '>'	, angles						,	angles_dorefs			, x)	\

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
