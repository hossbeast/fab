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

#ifndef _YYUTIL_H
#define _YYUTIL_H

/*
** yyutil contains common implementation details for scanner/parser systems, including :
**
**  uniform logging output : tokens scanned, start condition changes, grammar and scanner errors
**
**  location tracking - provides a generic location struct including starting/ending line/column/string and a macro
**   for combining locations in parser productions
**
**  start condition management - provides a mechanism for storing a stack of start conditions in tandem with but outside of flex
**   which 1) does not make them accessible for logging purposes, and 2) intermittently crashes when calling topstate
**   
**  fatal macros for use within the parser - for things such as allocating memory
**
**  yyerror implementation - 
*/

#include <stdio.h>

#include "xapi.h"

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
	int							scanerr;					// error from scanner
	int							gramerr;					// error from parser

	void *					scanner;					// flex scanner
	int							states[64];				// start states stack
	int							states_n;
	yyu_location	 	loc;							// running location track for this parse
	char						space[256];				// temp space
	char						space2[256];

																		// last successfully scanned token
	yyu_location		last_loc;					//  location
	int							last_token;				//  token
	int							last_line;				//  line number in the scanner
	void *					last_lval;				//  semantic value
	size_t					last_lval_aloc;		//  allocated size

																		// upon error
	yyu_location		error_loc;				//  location
	char						error_str[256];		//  string
	char						tokenstring[256];	//  tokenstring (gramerr only)

	// logging configuration
	void *					udata;						// passed to logging callbacks

	// yyu calls this function to log scanner state changes
	void *					state_token;			// passed to state logging callback
	int							(*state_would)(void * token, void * udata);
	void						(*state_log)(void * token, void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// yyu calls this function to log parsed tokens
	void *					token_token;			// passed to token logging callback
	int							(*token_would)(void * token, void * udata);
	void						(*token_log)(void * token, void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// yyu calls this function to get a token name from a token
	const char *		(*tokname)(int token);

	// yyu calls this function to get a state name from a state
	const char *		(*statename)(int token);

	// yyu calls this function to get a description of the input for location messages
	int							(*inputstr)(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);

	// yyu calls this function get a textual description of a scanned token
	int							(*lvalstr)(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);
} yyu_extra;

/// yyu_extra_destroy
//
// free memory associated with an yyu_extra struct
//
void yyu_extra_destroy(yyu_extra * const restrict)
	__attribute__((nonnull));

/// YYULTYPE
//
// use yyu_location within bison
//
#define YYLTYPE yyu_location

/// YYULLOC_DEFAULT
//
// aggregate yyu_locations in the parser
//
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
// fatal for use within grammar rules - invokes YYABORT
//
// NOTE
//  requires that you have these lines in your .y file
//  %parse-param { void * scanner }
//  %parse-param { parse_param * parm }
//
#define YYU_FATAL(x, ...)							\
do {																	\
	prologue;														\
	fatal(x, ##__VA_ARGS__);						\
	int R;															\
	finally : conclude(&R);							\
	if(R)																\
	{																		\
		YYABORT;													\
	}																		\
} while(0)

/// yyu_locwrite
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
//  reset the location track (i.e. exactly one newline was just parsed possibly preceeded by {del} non-newline characters)
//
// PARAMETERS
//  lloc - yylloc
//  xtra - yyextra
//  del  - offset from start of this token to the newline
//
void yyu_locreset(yyu_location * const restrict lloc, yyu_extra * const restrict xtra, const int del)
	__attribute__((nonnull));

/// LOCRESET
//
// call yyu_locreset with default parameters from a scanner rule
//
#define LOCRESET yyu_locreset(yylloc, yyextra, 0)

/// yyu_nstate
//
// SUMMARY
//  retrieve the nth state from the top of the yyextra-maintained state stack
//   0 - current state
//   1 - next state following POP
//
int yyu_nstate(yyu_extra * const restrict xtra, const int n)
	__attribute__((nonnull));

/// NSTATE
//
// call yyu_nstate with default parameters from a scanner rule
//
#define NSTATE(n) yyu_nstate(yyextra, n)

/// TOPSTATE
//
// call yyu_nstate with default parameters and the 0th state from a scanner rule
//
#define TOPSTATE NSTATE(0)

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
void yyu_pushstate(
	  const int state
	, yyu_extra * const restrict xtra
	, const char * func
	, const char * file
	, const int line
)
	__attribute__((nonnull));

/// PUSHSTATE
//
// call yyu_pushstate from a scanner rule - and push a state into the internal scanner stack
//
#define PUSHSTATE(state)																							\
	do {																																\
		yyu_pushstate(state, yyextra, __FUNCTION__, __FILE__, __LINE__);	\
		yy_push_state(state, yyextra->scanner);														\
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
void yyu_popstate(
	  yyu_extra * const restrict xtra
	, const char * func
	, const char * file
	, const int line
)
	__attribute__((nonnull));

/// POPSTATE
//
// call yyu_popstate from a scanner rule - also pop a state from the internal scanner stack
//
#define POPSTATE																							\
	do {																												\
		yy_pop_state(yyextra->scanner);														\
		yyu_popstate(yyextra, __FUNCTION__, __FILE__, __LINE__);	\
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
//  line  - line number where the token was scanned
//
void yyu_ptoken(
	  const int token
	, void * const restrict lval
	, yyu_location * const restrict lloc
	, yyu_extra * const restrict xtra
	, char * restrict text
	, const int leng
	, const char * func
	, const char * file
	, const int line
)
	__attribute__((nonnull));

/// PTOKEN
//
// call yyu_ptoken with default parameters from a scanner rule
//
#define PTOKEN(token) yyu_ptoken(token, yylval, yylloc, yyextra, yytext, yyleng, 0, __FUNCTION__, __FILE__, __LINE__)

/// yyu_scanner_error
//
// SUMMARY
//  invoked by scanner rule to report invalid input before returning 0 to indicate end-of-input
//
void yyu_scanner_error(yyu_location * const restrict lloc, yyu_extra * const restrict xtra, const int error, char const * fmt, ...)
	__attribute__((nonnull(1,2,4)));

/// yyu_grammar_error
//
// SUMMARY
//  yyerror - invoked by yyparse to report failure-to-reduce before returning 1
//
void yyu_grammar_error(yyu_location * const restrict lloc, void * const restrict scanner, yyu_extra * const restrict xtra, char const * err)
	__attribute__((nonnull(1,2,3)));

/// yyu_lexify
//
// SUMMARY
//  process a just scanned token : print the token, assign its location and lexical value
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
	, const char * func
	, const char * file
	, const int line
)
	__attribute__((nonnull));

/// LEXIFY
//
// call yyu_lexify with default parameters from a scanner rule
//
#define LEXIFY(token) yyu_lexify(token, yylval, sizeof(yylval), yylloc, yyextra, yytext, yyleng, 0, 0, __FUNCTION__, __FILE__, __LINE__)

#undef restrict
#endif
