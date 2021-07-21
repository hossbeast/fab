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

  #include "parser.internal.h"
  #include "value.tab.h"
  #include "value.tokens.h"

  #include "common/parseint.h"
  #include "common/attrs.h"

  #define YYSTYPE struct value_yystype
  #define YYLTYPE yyu_location

  static int lexify_variable(
      struct yyu_parser * const restrict parser
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
    yyu_lval* lval = (typeof(lval))_lval;

    lval->t.s = text + 1;
    lval->t.l = bytes - 1;

    return yyu_lexify(parser, lval, lvalsz, lloc, text, end_line, end_col, bytes, token);
  }

  #define LEXVAR()                  \
    LEXIFY(lexify_variable          \
      , yyextra                     \
      , yylval                      \
      , sizeof(yylval)              \
      , yylloc                      \
      , yytext                      \
      , yyleng                      \
      , YYUXTRA->loc.l_lin          \
      , YYUXTRA->loc.l_col + yyleng \
      , VARIABLE                    \
    )
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
<INITIAL,multilinecomment>[/][*]                        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<INITIAL>[\n]                                           { LOCRESET; }
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}()+=:]                                   { LEX(yytext[0]); }

 /* variables */
<INITIAL>[$][a-zA-Z_][a-zA-Z0-9_]{0,62}                 { LEXVAR(); }

 /* bools */
<INITIAL>(?i:true|false)                                { LEXB(); }

 /* integers */
<INITIAL>[0-9]+                                         { LEXI("%"SCNuMAX); }
<INITIAL>[-][0-9]+                                      { LEXI("%"SCNdMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { LEXI("%"SCNxMAX); }

 /* floats */
<INITIAL>[+-]?[0-9]+[.][0-9]+                           { LEXF(); }

 /* strings */
<INITIAL>[-a-zA-Z0-9_/.][-a-zA-Z0-9_/.]*                { LEX(STR); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { LEXH(2); }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { LEXH(3); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { LEX(STR); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(YYUXTRA, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          yyu_errorf(YYUXTRA, "character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          POPSTATES;
                                                          return 0;
                                                        }
