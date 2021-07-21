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

#ifndef _YYUTIL_GRAMMAR_H
#define _YYUTIL_GRAMMAR_H

#include "types.h"

struct yy_buffer_state;
struct yyu_parser;
struct yyu_location;

/// yyu_location
//
// structure for location tracking that including line numbers and start and end pointers
//
// lines and columns are inclusive and zero based
//
typedef struct yyu_location {
  uint16_t f_lin;  // line where the location begins
  uint16_t f_col;  // column where the location begins
  uint16_t l_lin;  // line where the location ends
  uint16_t l_col;  // column where the location ends

  char *  s;      // pointer to start of location
  char *  e;      // pointer to end of location
  size_t  l;      // length of location, equal to e - s
} yyu_location;

typedef union yyu_lval {
  bool      b;
  char *    s;
  char      c;
  uintmax_t umax;
  intmax_t  imax;
  uint8_t   u8;
  uint16_t  u16;
  int       i;
  double    f;
  struct {        // text
    char * s;
    uint16_t l;
  } t;
} yyu_lval;

void yyu_aggregate_location(yyu_location * loc, yyu_location * A, yyu_location * B)
  __attribute__((nonnull(1, 3)));

#define YYLLOC_DEFAULT(Cur, Rhs, N) do {  \
  yyu_location *A, *B, *T;                \
  A = 0;                                  \
  B = &YYRHSLOC(Rhs, N);                  \
  int x;                                  \
  for(x = 1; x < N; x++) {                \
    T = &YYRHSLOC(Rhs, x);                \
    if(T->s) {                            \
      A = T;                              \
      break;                              \
    }                                     \
  }                                       \
  yyu_aggregate_location(&(Cur), A, B);   \
} while(0)

struct yyu_parser;
struct yyu_location;

void yyu_loc_initialize(struct yyu_parser * parser, struct yyu_location * lloc);
void yyu_loc_final(struct yyu_parser * parser, struct yyu_location * lloc);

#define YYU_LOC_INITIALIZE(_parser, _lloc) do {  \
  yyu_parser * parser = _parser;                 \
  yyu_location * lloc = _lloc;                   \
} while(0)
#undef YYU_LOC_INITIALIZE

/// YYU_YFATAL
//
// fatal for use within grammar rules - invokes YYABORT
//
// NOTE
//  requires that you have these lines in your .y file
//  %parse-param { void* scanner }
//  %parse-param { void* parm }
//
#define YFATAL(x, ...)                \
do {                                  \
  int r = x(__VA_ARGS__);           \
  if(r != 0)                          \
  {                                   \
    YYABORT;                          \
  }                                   \
} while(0)

/// yyu_grammar_error
//
// SUMMARY
//  yyerror - invoked by yyparse to report failure-to-reduce before returning 1
//
// PARAMETERS
//  lloc    -
//  scanner -
//  xtra    -
//  err     -
//
void yyu_grammar_error(struct yyu_location * const restrict lloc, void * const restrict scanner, struct yyu_parser * const restrict parser, char const * err);

#endif
