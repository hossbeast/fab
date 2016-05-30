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

#ifndef _YYUTIL_SCANNER_H
#define _YYUTIL_SCANNER_H

#include "xapi.h"

#include "yyutil/parser.h"

#define restrict __restrict

/// yyu_extra_destroy
//
// free memory associated with an yyu_extra struct
//
void yyu_extra_destroy(yyu_extra * const restrict)
  __attribute__((nonnull));

#define lenter        \
  enter_nochecks;     \
  xapi _yyu_R = 0;    \
  int _yyu_V = 0;     \
  int _yyu_Vb = 0

#define lcoda                   \
  conclude(&_yyu_R);            \
  if(_yyu_R)                    \
  {                             \
    while(yyextra->states_n)    \
      DROPSTATE;                \
    LOCWRITE;                   \
    yyu_scanner_error(yylloc, yyextra); \
    return 0;                   \
  }                             \
  else if(_yyu_Vb)              \
  {                             \
    return _yyu_V;              \
  }

#define yield(x)       \
  do {                 \
    _yyu_V = (x);      \
    _yyu_Vb = 1;       \
    goto XAPI_LEAVE;   \
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
//
// REMARKS
//  typically you need to call this with PUSHSTATE in order to to also affect the scanner state stack
//
xapi yyu_pushstate(const int state, yyu_extra * const restrict xtra)
  __attribute__((nonnull));

/// PUSHSTATE
//
// call yyu_pushstate from a scanner rule - and push a state into the internal scanner stack
//
#define PUSHSTATE(state)                       \
  do {                                         \
    fatal(yyu_pushstate, state, yyextra);      \
    yy_push_state(state, yyextra->scanner);    \
  } while(0)

/// yyu_popstate
//
// SUMMARY
//  pop a state from the yyextra-maintained state stack and print an info message
//
// PARAMETERS
//  xtra - yyextra
//
// REMARKS
//  typically you need to call this with POPSTATE in order to to also affect the scanner state stack
//
xapi yyu_popstate(yyu_extra * const restrict xtra)
  __attribute__((nonnull));

/// POPSTATE
//
// call yyu_popstate from a scanner rule - also pop a state from the internal scanner stack
//
#define POPSTATE                          \
  do {                                    \
    yy_pop_state(yyextra->scanner);       \
    fatal(yyu_popstate, yyextra);         \
  } while(0)

void yyu_dropstate(yyu_extra * const restrict xtra)
  __attribute__((nonnull));

#define DROPSTATE                     \
  do {                                \
    yy_pop_state(yyextra->scanner);   \
    yyu_dropstate(yyextra);           \
  } while(0)

/// PTOKEN
//
// call yyu_ptoken with default parameters from a scanner rule
//
#define PTOKEN(token) fatal(yyu_ptoken, token, yylval, yylloc, yyextra, yytext, yyleng, 0)

/// yyu_scanner_error
//
// SUMMARY
//  invoked by scanner rule to report invalid input before returning 0 to indicate end-of-input
//
void yyu_scanner_error(yyu_location * const restrict lloc, yyu_extra * const restrict xtra)//, const int error, char const * fmt, ...)
  __attribute__((nonnull(1,2)));

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
//
xapi yyu_lexify(
    const int token
  , void * const restrict lval
  , const size_t lvalsz
  , yyu_location * const restrict lloc
  , yyu_extra * const restrict xtra
  , char * const restrict text
  , const int leng
  , const int del
  , const int isnl
)
  __attribute__((nonnull));

/// LEXIFY
//
// call yyu_lexify with default parameters from a scanner rule
//
#define LEXIFY(token) fatal(yyu_lexify, token, yylval, sizeof(yylval), yylloc, yyextra, yytext, yyleng, 0, 0)

#undef restrict
#endif
