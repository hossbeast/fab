/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"

  #include "glue.h"
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

  #define LEXVAR()                     \
    LEXIFY(lexify_variable             \
      , yyextra                        \
      , yylval                         \
      , sizeof(yylval)                 \
      , yylloc                         \
      , yytext                         \
      , yyleng                         \
      , yyuparser->loc.l_lin           \
      , yyuparser->loc.l_col + yyleng  \
      , VARIABLE                       \
    )
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="value_yy"
%option stack

%x multilinecomment
%x quoted

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
<INITIAL>"&&"                                           { return LEX(LOGAND); }
<INITIAL>"||"                                           { return LEX(LOGOR); }
<INITIAL>"<<"                                           { return LEX(SHL); }
<INITIAL>">>"                                           { return LEX(SHR); }
<INITIAL>"=="                                           { return LEX(EQ); }
<INITIAL>[-![\]{}()+:=<>&|/%*^~]                        { return LEX(yytext[0]); }

 /* variables */
<INITIAL>[$][a-zA-Z_][a-zA-Z0-9_]{0,62}                 { return LEXVAR(); }

 /* bools */
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* integers */
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { return LEXI("%"SCNxMAX); }

 /* floats */
<INITIAL>[+-]?[0-9]+[.][0-9]+                           { return LEXF(); }

 /* strings */
<INITIAL>[-a-zA-Z0-9_./+=]+                             { return LEX(STR); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { return LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { return LEXH(2); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          return 0;
                                                        }
