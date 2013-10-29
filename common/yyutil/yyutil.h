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

#ifndef _YYUTIL_H
#define _YYUTIL_H

#include <stdio.h>

#include "xstring.h"

#define restrict __restrict

/// yyu_location
//
// structure for location tracking that including line numbers and start and end pointers
//
typedef struct
{
	int			f_lin;	// line where the location begins
	int			f_col;	// column where the location begins
	int			l_lin;	// line where the location ends
	int			l_col;	// column where the location ends

	char *	s;			// pointer to start of location
	char *	e;			// pointer to end of location
	int     l;			// length of location, equal to e - s
} yyu_location;

/// yyu_extra
//
// structure for parsing data (yyextra)
//
typedef struct yyu_extra
{
	int							r;							// set to 1 in yyu_error
	void *					scanner;				// flex scanner
	int							states[64];			// start states stack
	int							states_n;
	yyu_location	 	loc;						// running location track for this parse
	char						space[256];			// temp space
	char						space2[256];

	yyu_location		last_loc;				// location of last scanned token
	int							last_token;			// last token scanned
	int							last_line;			// line number in the scanner of the last scanned token
	void *					last_lval;			// semantic value of last scanned token
	size_t					last_lval_aloc;	// allocated size of last_lval

	// whether yyu should include scanner line number in output messages
	int							output_line;

	// yyu calls this function to write info messages
	void						(*info)(char * fmt, ...);

	// yyu calls this function to write error messages
	void						(*error)(char * fmt, ...);

	// yyu calls this function to get a token name from a token
	const char *		(*tokname)(int token);

	// yyu calls this function to get a state name from a state
	const char *		(*statename)(int token);

	// yyu calls this function to get a name for the input for location messages
	int							(*inputname)(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);

	// yyu calls this function get a textual description of a scanned token
	int							(*lvalstr)(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);
} yyu_extra;

void yyu_extra_destroy(yyu_extra * const restrict)
	__attribute__((nonnull));

///
/// use yyu_location
///
#define YYLTYPE yyu_location

///
/// aggregate yyu_locations in the parser
///
#define YYLLOC_DEFAULT(Cur, Rhs, N)					\
do																					\
{																						\
	if(N)																			\
	{																					\
		(Cur).f_lin = YYRHSLOC(Rhs, 1).f_lin;		\
		(Cur).f_col = YYRHSLOC(Rhs, 1).f_col;		\
		(Cur).l_lin = YYRHSLOC(Rhs, N).l_lin;		\
		(Cur).l_col = YYRHSLOC(Rhs, N).l_col;		\
																						\
		(Cur).s = YYRHSLOC(Rhs, 1).s;						\
		(Cur).e = YYRHSLOC(Rhs, N).e;						\
	}																					\
	else																			\
	{																					\
		(Cur).f_lin = YYRHSLOC(Rhs, 0).l_lin;		\
		(Cur).l_lin = YYRHSLOC(Rhs, 0).l_lin;		\
		(Cur).f_col = YYRHSLOC(Rhs, 0).l_col;		\
		(Cur).l_col = YYRHSLOC(Rhs, 0).l_col;		\
																						\
		(Cur).s = YYRHSLOC(Rhs, 0).e;						\
	}																					\
} while(0)

/// YYU_FATAL
//
//
//
#define YYU_FATAL(x, ...)					\
do {															\
	int __r = x(__VA_ARGS__);				\
	if(__r != 0)										\
	{																\
		if(__r > 0)										\
		{															\
			YYU_ERROR(#x "failed");			\
			YYABORT;										\
		}															\
	}																\
} while(0)

///  yyu_locwrite
//
// SUMMARY
//  update the location tracking for a token just parsed (that does not contain newline(s))
//  
// PARAMETERS
//  lloc - yylloc
//  xtra - yyextra
//  text - yytext
//  leng - yyleng
//  del  - offset from yytext to start of location for this token
//
void yyu_locwrite(yyu_location * const restrict lloc, yyu_extra * const restrict xtra, char * const restrict text, const int leng, const int del)
	__attribute__((nonnull));

/// LOCWRITE
//
// call yyu_locwrite with default parameters from a scanner rule
//
#define LOCWRITE yyu_locwrite(yylloc, yyextra, yytext, yyleng, 0)

/// yyu_locreset
//
// SUMMARY
//  reset the location track (i.e. a single newline was just parsed)
//
// PARAMETERS
//  lloc - yylloc
//  xtra - yyextra
//
void yyu_locreset(yyu_location * const restrict lloc, yyu_extra * const restrict xtra)
	__attribute__((nonnull));

/// LOCRESET
//
// call yyu_locreset with default parameters from a scanner rule
//
#define LOCRESET yyu_locreset(yylloc, yyextra)

/// yyu_topstate
//
// SUMMARY
//  retrieve the top state from the yyextra-maintained state stack
//
int yyu_topstate(yyu_extra * const restrict xtra)
	__attribute__((nonnull));

/// TOPSTATE
//
// call yyu_topstate with default parameters from a scanner rule
//
#define TOPSTATE yyu_topstate(yyextra)

/// yyu_pushstate
//
// SUMMARY
//  push a state onto the yyextra-maintained state stack and print an info message
//
// PARAMETERS
//  state - state
//  xtra  - yyextra
//  line  - line number in the scanner
//
// REMARKS
//  typically you need to call this with PUSHSTATE in order to to also affect the scanner state stack
//
void yyu_pushstate(const int state, yyu_extra * const restrict xtra, const int line)
	__attribute__((nonnull));

/// PUSHSTATE
//
// call yyu_pushstate from a scanner rule - and push a state into the internal scanner stack
//
#define PUSHSTATE(state)											\
	do {																				\
		yyu_pushstate(state, yyextra, __LINE__);	\
		yy_push_state(state, yyextra->scanner);		\
	} while(0)

/// yyu_popstate
//
// SUMMARY
//  pop a state from the yyextra-maintained state stack and print an info message
//
// PARAMETERS
//  xtra - yyextra
//  line - line number in the scanner
//
// REMARKS
//  typically you need to call this with POPSTATE in order to to also affect the scanner state stack
//
void yyu_popstate(yyu_extra * const restrict xtra, const int line)
	__attribute__((nonnull));

/// POPSTATE
//
// call yyu_popstate from a scanner rule - also pop a state from the internal scanner stack
//
#define POPSTATE											\
	do {																\
		yy_pop_state(yyextra->scanner);		\
		yyu_popstate(yyextra, __LINE__);	\
	} while(0)

/// yyu_ptoken
//
// SUMMARY
//  print an info message about a scanned token
//
// PARAMETERS
//  token - token
//  lval  - yylval
//  lloc  - yylloc
//  xtra  - yyextra
//  text  - yytext (text the token was parsed from)
//  leng  - yyleng 
//  del   - offset from text
//  line  - line number where the token was scanned
//
void yyu_ptoken(const int token, void * const restrict lval, yyu_location * const restrict lloc, yyu_extra * const restrict xtra, char * restrict text, const int leng, const int del, const int line)
	__attribute__((nonnull));

/// PTOKEN
//
// call yyu_ptoken with default parameters from a scanner rule
//
#define PTOKEN(token) yyu_ptoken(token, yylval, yylloc, yyextra, yytext, yyleng, 0, __LINE__)

/// yyu_error
//
// SUMMARY
//  yyerror - write error messages, set extra->r = 1
//
void yyu_error(yyu_location * const restrict lloc, void * const restrict scanner, yyu_extra * const restrict xtra, char const * err)
	__attribute__((nonnull));

/// yyu_lexify
//
// SUMMARY
//  process a just scanned token
//
// PARAMETERS
//  token - token
//  lval  - yylval
//  lvalsz - sizeof(yylval)
//  lloc  - yylloc
//  xtra  - yyextra
//  text  - yytext
//  leng  - yyleng
//  del   - offset from start of yytext
//  isnl  - whether this token is a single newline
//  line  - line number in the scanner
//
int yyu_lexify(
	  const int token
	, void * const restrict lval
	, const size_t lvalsz
	, yyu_location * const restrict lloc
	, yyu_extra * const restrict xtra
	, char * const restrict text
	, const int leng
	, const int del
	, const int isnl
	, const int line
)
	__attribute__((nonnull));

/// LEXIFY
//
// call yyu_lexify with default parameters from a scanner rule
//
#define LEXIFY(token) yyu_lexify(token, yylval, sizeof(yylval), yylloc, yyextra, yytext, yyleng, 0, 0, __LINE__)

#undef restrict
#endif
