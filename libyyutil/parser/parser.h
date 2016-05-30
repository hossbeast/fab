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

#ifndef _YYUTIL_PARSER_H
#define _YYUTIL_PARSER_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

#define restrict __restrict

/// yyu_location
//
// structure for location tracking that including line numbers and start and end pointers
//
typedef struct yyu_location
{
  int     f_lin;  // line where the location begins
  int     f_col;  // column where the location begins
  int     l_lin;  // line where the location ends
  int     l_col;  // column where the location ends

  char *  s;      // pointer to start of location
  char *  e;      // pointer to end of location
  int     l;      // length of location, equal to e - s
} yyu_location;

/// yyu_extra
//
// structure for parsing data (yyextra)
//
typedef struct yyu_extra
{
  int             scanerr;          // error from scanner
  int             gramerr;          // error from parser

  void *          scanner;          // flex scanner
  int             states[64];       // start states stack
  int             states_n;
  yyu_location    loc;              // running location track for this parse
  char            space[256];       // temp space
  char            space2[256];

                                    // last successfully scanned token
  yyu_location    last_loc;         //  location
  int             last_token;       //  token
  int             last_line;        //  line number in the scanner
  void *          last_lval;        //  semantic value
  size_t          last_lval_aloc;   //  allocated size

                                    // upon error
  yyu_location    error_loc;        //  location
  char            error_str[256];   //  string
  char            tokenstring[256]; //  tokenstring (gramerr only)

  uint64_t        state_logs;       // logging category ids for state transition logging
  uint64_t        token_logs;       // logging category ids for token logging

  // yyu calls this function to get a token name from a token
  const char *    (*tokname)(int token);

  // yyu calls this function to get a state name from a state
  const char *    (*statename)(int token);

  // yyu calls this function to get a description of the input for location messages
  int             (*inputstr)(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);

  // yyu calls this function get a textual description of a scanned token
  int             (*lvalstr)(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict blen);
} yyu_extra;

/// YYULTYPE
//
// use yyu_location within bison
//
#define YYLTYPE yyu_location

/// YYULLOC_DEFAULT
//
// aggregate yyu_locations in the parser
//
#define YYLLOC_DEFAULT(Cur, Rhs, N)         \
do                                          \
{                                           \
  if(N)                                     \
  {                                         \
    (Cur).f_lin = YYRHSLOC(Rhs, 1).f_lin;   \
    (Cur).f_col = YYRHSLOC(Rhs, 1).f_col;   \
    (Cur).l_lin = YYRHSLOC(Rhs, N).l_lin;   \
    (Cur).l_col = YYRHSLOC(Rhs, N).l_col;   \
                                            \
    (Cur).s = YYRHSLOC(Rhs, 1).s;           \
    (Cur).e = YYRHSLOC(Rhs, N).e;           \
  }                                         \
  else                                      \
  {                                         \
    (Cur).f_lin = YYRHSLOC(Rhs, 0).l_lin;   \
    (Cur).l_lin = YYRHSLOC(Rhs, 0).l_lin;   \
    (Cur).f_col = YYRHSLOC(Rhs, 0).l_col;   \
    (Cur).l_col = YYRHSLOC(Rhs, 0).l_col;   \
                                            \
    (Cur).s = YYRHSLOC(Rhs, 0).e;           \
  }                                         \
} while(0)

/// YYU_YFATAL
//
// fatal for use within grammar rules - invokes YYABORT
//
// NOTE
//  requires that you have these lines in your .y file
//  %parse-param { void * scanner }
//  %parse-param { parse_param * parm }
//
#define YFATAL(x, ...)                \
do {                                  \
  enter_nochecks;                     \
  xapi _yyu_R;                        \
  fatal(x, ##__VA_ARGS__);            \
  finally : conclude(&_yyu_R);        \
  if(_yyu_R)                          \
  {                                   \
    YYABORT;                          \
  }                                   \
} while(0)

#endif