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

#include "types.h"

struct yyu_location;
struct yyu_parser;

#define LEXTOKEN(r)                     \
  if(r < 0)                             \
  {                                     \
    POPSTATES;                          \
    LOCWRITE;                           \
    yyu_save_error_loc(yyextra, yylloc); \
    return 0; /* end-of-input */        \
  }                                     \
  else if(r > -1)                       \
  {                                     \
    return r;                           \
  }

#define YYUXTRA ((yyu_parser*)yyextra)

/// yyu_locwrite
//
// SUMMARY
//  update the location tracking for a token just parsed (that does not contain newline(s))
//
// PARAMETERS
//  lloc     - yylloc
//  xtra     - yyextra
//  text     - yytext
//  end_line - extent of the current token
//  end_col  - extent of the current token
//  bytes    - extent of the current token
//
void yyu_locwrite(
    struct yyu_location * const restrict lloc
  , struct yyu_parser * const restrict xtra
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
)
  __attribute__((nonnull));

/// LOCWRITE
//
// call yyu_locwrite with default parameters from a scanner rule - for a token that doesnt span lines
//
#define LOCWRITE yyu_locwrite(yylloc, yyextra, yytext, YYUXTRA->loc.l_lin, YYUXTRA->loc.l_col + yyleng, yyleng)

/// yyu_locreset
//
// SUMMARY
//  reset the location track (i.e. exactly one newline was just parsed)
//
// PARAMETERS
//  lloc - yylloc
//  xtra - yyextra
//
void yyu_locreset(struct yyu_location * const restrict lloc, struct yyu_parser * const restrict xtra, char * const restrict text)
  __attribute__((nonnull));

/// LOCRESET
//
// call yyu_locreset with default parameters from a scanner rule
//
#define LOCRESET yyu_locreset(yylloc, yyextra, yytext)

/// yyu_nstate
//
// SUMMARY
//  retrieve the nth state from the top of the yyextra-maintained state stack
//   0 - current state
//   1 - next state following POP
//
int yyu_nstate(struct yyu_parser * const restrict xtra, int n)
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
void yyu_pushstate(int state, struct yyu_parser * const restrict xtra)
  __attribute__((nonnull));

/// PUSHSTATE
//
// call yyu_pushstate from a scanner rule - and push a state into the internal scanner stack
//
#define PUSHSTATE(state)                       \
  do {                                         \
    yyu_pushstate(state, yyextra);             \
    yy_push_state(state, YYUXTRA->scanner);    \
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
void yyu_popstate(struct yyu_parser * const restrict xtra)
  __attribute__((nonnull));

/// POPSTATE
//
// call yyu_popstate from a scanner rule - also pop a state from the internal scanner stack
//
#define POPSTATE                          \
  do {                                    \
    yy_pop_state(YYUXTRA->scanner);       \
    yyu_popstate(yyextra);                \
  } while(0)

#define POPSTATES                 \
  do {                            \
    while(YYUXTRA->states_n) {    \
      POPSTATE;                   \
    }                             \
  } while(0)                      \

/* invoked by scanner rule to report invalid input before returning 0 to indicate end-of-input */
void yyu_save_error_loc(struct yyu_parser * const restrict parser, struct yyu_location * const restrict lloc)
  __attribute__((nonnull));

void yyu_errorf(struct yyu_parser * restrict parser, const char * restrict fmt, ...)
  __attribute__((nonnull))
  __attribute__((format(printf, 2, 3)));

/// yyu_lexify
//
// SUMMARY
//  process a just scanned token : print the token, assign its location and lexical value
//
// PARAMETERS
//  lval  - yylval
//  lvalsz - sizeof(yylval)
//  lloc  - yylloc
//  xtra  - yyextra
//  text  - yytext
//  token - token
//
int yyu_lexify(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
  __attribute__((nonnull));

#define LEXIFY(f, ...) do { \
    int r = f(__VA_ARGS__); \
    LEXTOKEN(r);            \
  } while(0)

/* call yyu_lexify with default parameters from a scanner rule */
#define LEX(token)                  \
    LEXIFY(yyu_lexify               \
      , yyextra                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , YYUXTRA->loc.l_lin          \
      , YYUXTRA->loc.l_col + yyleng \
      , yyleng                      \
      , token                       \
    )

int yyu_lexify_valtoken(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
  __attribute__((nonnull));

/*
 * lexify a token whose semantic value is its token number.
 * such a token must be declared as having type yyu.16 like so,
 *
 *  > %token <yyu.u16> footoken
 */
#define LEXVTOK(token)              \
  LEXIFY(yyu_lexify_valtoken        \
      , yyextra                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , YYUXTRA->loc.l_lin          \
      , YYUXTRA->loc.l_col + yyleng \
      , yyleng                      \
      , token                       \
    )

#define LEXLOC(token, locp)         \
    LEXIFY(yyu_lexify               \
      , yyextra                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , (locp)->l_lin               \
      , (locp)->l_col               \
      , (locp)->l                   \
      , token                       \
    )

/* scan an attrs token, or a STR token */
#define LEXA()                      \
    LEXIFY(yyu_lexify_attrs         \
      , yyextra                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , yyleng                      \
    )

/// yyu_lexify_attrs
//
// SUMMARY
//  process a just scanned STR token which might be interpreted as an attrs token
//
// PARAMETERS
//  parser - parser
//  lval   - yylval
//  lvalsz - sizeof(yylval)
//  lloc   - yylloc
//  text   - yytext
//  leng   - yyleng
//
int yyu_lexify_attrs(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
  __attribute__((nonnull));

/// LEXE
//
// SUMMARY
//  scan an enumerated token
//
#define LEXE()                  \
    LEXIFY(yyu_lexify_enum      \
      , yyextra                 \
      , yylval                  \
      , sizeof(yylval)          \
      , yylloc                  \
      , yytext                  \
      , yyleng                  \
    )

/// yyu_lexify_enum
//
// SUMMARY
//  process a just scanned STR token which must be interpreted as an attrs token
//
// PARAMETERS
//  parser - parser
//  lval   - yylval
//  lvalsz - sizeof(yylval)
//  lloc   - yylloc
//  text   - yytext
//  leng   - yyleng
//
int yyu_lexify_enum(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
  __attribute__((nonnull));


#define LEXI(fmt)               \
    LEXIFY(yyu_lexify_int       \
      , yyextra                 \
      , yylval                  \
      , sizeof(yylval)          \
      , yylloc                  \
      , yytext                  \
      , yyleng                  \
      , fmt                     \
    )

int yyu_lexify_int(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const text
  , int leng
  , char * restrict fmt
)
  __attribute__((nonnull));

/// LEXC
//
// SUMMARY
//  scan a character reference
//
// PARAMETERS
//  token - character token
//  off   - offset from start of yytext
//
#define LEXC(off)               \
    LEXIFY(yyu_lexify_cref      \
      , yyextra                 \
      , yylval                  \
      , sizeof(yylval)          \
      , yylloc                  \
      , yytext                  \
      , yyleng                  \
      , off                     \
    )

int yyu_lexify_cref(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
  __attribute__((nonnull));

/// LEXH
//
// SUMMARY
//  scan a hex reference
//
// PARAMETERS
//  token - character token
//  off   - offset from start of yytext
//
#define LEXH(off)             \
  LEXIFY(yyu_lexify_href      \
    , yyextra                 \
    , yylval                  \
    , sizeof(yylval)          \
    , yylloc                  \
    , yytext                  \
    , yyleng                  \
    , off                     \
  )

int yyu_lexify_href(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
  __attribute__((nonnull));

/// LEXB
//
// SUMMARY
//  scan a hex reference
//
// PARAMETERS
//  token - character token
//  off   - offset from start of yytext
//
#define LEXB()                \
  LEXIFY(yyu_lexify_bool      \
    , yyextra                 \
    , yylval                  \
    , sizeof(yylval)          \
    , yylloc                  \
    , yytext                  \
    , yyleng                  \
  )

int yyu_lexify_bool(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const text
  , int leng
)
  __attribute__((nonnull));

/// LEXF
//
// SUMMARY
//  scan a hex reference
//
// PARAMETERS
//  token - character token
//  off   - offset from start of yytext
//
#define LEXF()                \
  LEXIFY(yyu_lexify_float     \
    , yyextra                 \
    , yylval                  \
    , sizeof(yylval)          \
    , yylloc                  \
    , yytext                  \
    , yyleng                  \
  )

int yyu_lexify_float(
    struct yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , struct yyu_location * const restrict lloc
  , char * const text
  , int leng
)
  __attribute__((nonnull));

#endif
