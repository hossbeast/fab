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
  #include "yyutil/scanner.h"

  #include "VALUE.errtab.h"
  #include "parser.internal.h"
  #include "value.tab.h"
  #include "value.tokens.h"

  #include "parseint.h"
  #include "attrs.h"

  #define YYSTYPE struct value_yystype
  #define YYLTYPE yyu_location

  static xapi lexify_variable(
      struct yyu_parser * const restrict parser
    , int * const restrict ytoken
    , void * const restrict _lval
    , size_t lvalsz
    , struct yyu_location * const restrict lloc
    , char * const restrict text
    , size_t bytes
    , uint16_t end_line
    , uint16_t end_col
    , int token
  )
  {
    enter;

    yyu_lval* lval = (typeof(lval))_lval;

    lval->t.s = text + 1;
    lval->t.l = bytes - 1;

    fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, end_line, end_col, bytes, token);

    finally : coda;
  }

  #define LEXVAR() do {             \
    fatal(lexify_variable           \
      , yyextra                     \
      , &_yyu_V                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , yyleng                      \
      , YYUXTRA->loc.l_lin          \
      , YYUXTRA->loc.l_col + yyleng \
      , VARIABLE                    \
    );                              \
  } while(0)
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="value_yy"
%option stack

%x multilinecomment
%x quoted

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
<INITIAL>[[\]{}()+=:]                                   { lenter; LEX(yytext[0]); finally : lcoda; }

 /* variables */
<INITIAL>[$][a-zA-Z_][a-zA-Z0-9_]{0,62}                 { lenter; LEXVAR(); finally : lcoda; }

 /* bools */
<INITIAL>(?i:true|false)                                { lenter; LEXB(); finally : lcoda; }

 /* integers */
<INITIAL>[0-9]+                                         { lenter; LEXI("%"SCNuMAX); finally : lcoda; }
<INITIAL>[-][0-9]+                                      { lenter; LEXI("%"SCNdMAX); finally : lcoda; }
<INITIAL>(?i:0x[a-f0-9]+)                               { lenter; LEXI("%"SCNxMAX); finally : lcoda; }

 /* floats */
<INITIAL>[+-]?[0-9]+[.][0-9]+                           { lenter; LEXF(); finally : lcoda; }

 /* strings */
<INITIAL>[-a-zA-Z0-9_/.][-a-zA-Z0-9_/.]*                { lenter; LEX(STR); finally : lcoda; }

 /* quoted strings */
<INITIAL>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); finally : lcoda; }
<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<quoted>\\[\\"abtnvfrs0]                                { lenter; LEXC(1); finally : lcoda; }
<quoted>\\x[0-9a-f]{2}                                  { lenter; LEXH(2); finally : lcoda; }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { lenter; LEXH(3); finally : lcoda; }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(VALUE_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(VALUE_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0; /* return end-of-input */
                                                          finally : lcoda;
                                                        }
