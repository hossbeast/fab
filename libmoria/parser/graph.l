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

%{
  #include <stdio.h>
  #include <strings.h>

  struct graph_xtra;
  #define YYU_EXTRA_TYPE struct graph_xtra
  #include "yyutil/scanner.h"

  #include "MORIA.errtab.h"
  #include "parser.internal.h"
  #include "graph.tab.h"
  #include "graph.tokens.h"

  #include "parseint.h"

  /// LEX
  //
  // SUMMARY
  //  parse a whitespace or character token with no semantic value possibly at an offset from yytext
  //
  // PARAMETERS
  //  token - token to parse
  //  ldel  - offset from start of yytext
  //
  #define LEX(token)            \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , 0                       \
      , 0                       \
      , 0                       \
    )

  #define LEXV(token, ldel)     \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , ldel                    \
      , 0                       \
      , 0                       \
    )

  static xapi lexify(const int token, YYSTYPE * restrict lval, yyu_location * restrict lloc, graph_xtra * restrict xtra, char * restrict text, const int leng, const int ldel, const int vdel, void * restrict v)
  {
    enter;

    if(token == HEX)
      fatal(xparseuint, text + vdel, SCNx64, 0, UINT32_MAX, 0, UINT8_MAX, &lval->u32, 0);
    else if(token == POSINT)
      fatal(xparseuint, text + vdel, SCNu64, 0, UINT64_MAX, 0, UINT8_MAX, &lval->u64, 0);

    fatal(yyu_lexify, token, lval, sizeof(*lval), lloc, xtra, text, leng, ldel, 0);

    finally : coda;
  }
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="graph_yy"
%option extra-type="graph_xtra *"
%option stack

%x multilinecomment

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*           { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,multilinecomment>[/][*]                        { lenter; LOCWRITE; PUSHSTATE(multilinecomment); finally : lcoda; }
<multilinecomment>[*][/]                                { lenter; LOCWRITE; POPSTATE; finally : lcoda; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<INITIAL>[\n]                                           { LOCRESET; }
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[-:!]                                          { lenter; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<INITIAL>(?i:0x[a-f0-9]+)                               { lenter; LEX(HEX); yield(HEX); finally : lcoda; }
<INITIAL>[0-9]+                                         { lenter; LEX(POSINT); yield(POSINT); finally : lcoda; }
<INITIAL>[a-zA-Z_][-a-zA-Z0-9_]*                        { lenter; LEX(STR); yield(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(MORIA_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(MORIA_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          yield(0); /* return end-of-input */
                                                          finally : lcoda;
                                                        }
