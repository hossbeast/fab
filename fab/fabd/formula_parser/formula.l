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
  #include "valyria/hashtable.h"
  #include "yyutil/scanner.h"

  #include "FORMULA.errtab.h"
  #include "formula_parser.internal.h"
  #include "formula.tab.h"
  #include "formula.tokens.h"
  #include "selector_parser.h"

  #define YYSTYPE FORMULA_YYSTYPE
  #define YYLTYPE yyu_location

#define PARSE_SELECTOR do {                                             \
  formula_parser * parser = containerof(yyextra, formula_parser, yyu);  \
  size_t off = yytext - parser->yyu.yybuffer;                           \
  char * usr = parser->yyu.usrbuf + off;                                \
  size_t bufsz = parser->yyu.usrbufsize - off;                          \
  yyu_location subloc;                                                  \
  fatal(selector_parser_parse_partial                                   \
    , parser->selector_parser                                           \
    , usr                                                               \
    , bufsz                                                             \
    , "selector"                                                        \
    , &parser->yyu.loc                                                  \
    , &subloc                                                           \
    , &yylval->selector                                                 \
  );                                                                    \
  yyless(subloc.l);                                                     \
  LEXLOC(formula_SELECTOR, &subloc);                                    \
} while(0);

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
%option prefix="formula_yy"
%option stack

%x multilinecomment
%x quoted
%x _select _selector

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<_select,_selector>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]* |
<INITIAL>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*           { LOCWRITE; }

 /* multiline comments are nestable */
<_select,_selector>[/][*]                               |
<INITIAL,multilinecomment>[/][*]                        { lenter; LOCWRITE; PUSHSTATE(multilinecomment); finally : lcoda; }
<multilinecomment>[*][/]                                { lenter; LOCWRITE; POPSTATE; finally : lcoda; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<_select,_selector>[\n]                                 |
<INITIAL>[\n]                                           { LOCRESET; }
<_select,_selector>[\r\t ]+                             |
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}=:]                                      { lenter; LEX(yytext[0]); finally : lcoda; }

 /* variables */
<INITIAL>[$]bm_source                                   { lenter; LEXVTOK(BM_SOURCE); finally : lcoda; }
<INITIAL>[$]bm_sources                                  { lenter; LEXVTOK(BM_SOURCES); finally : lcoda; }
<INITIAL>[$]bm_target                                   { lenter; LEXVTOK(BM_TARGET); finally : lcoda; }
<INITIAL>[$]bm_targets                                  { lenter; LEXVTOK(BM_TARGETS); finally : lcoda; }
<INITIAL>[$]bm_variant                                  { lenter; LEXVTOK(BM_VARIANT); finally : lcoda; }
<INITIAL>[$][a-zA-Z_][a-zA-Z0-9_]{0,62}                 { lenter; LEXVAR(); finally : lcoda; }

 /* integers */
<INITIAL>[0-9]+                                         { lenter; LEXI("%"SCNuMAX); finally : lcoda; }
<INITIAL>[-][0-9]+                                      { lenter; LEXI("%"SCNdMAX); finally : lcoda; }
<INITIAL>(?i:0x[a-f0-9]+)                               { lenter; LEXI("%"SCNxMAX); finally : lcoda; }

 /* invoke selector sub-parser */
<INITIAL>"select"                                       { lenter; PUSHSTATE(_select); LEX(SELECT); finally : lcoda; }
<_select>":"                                            { lenter; PUSHSTATE(_selector); LEX(':'); finally : lcoda; }
 /* consume whitespace in this parser */
<_selector>[^\n\r\t ][\x00-\xff]+                       { lenter; PARSE_SELECTOR; POPSTATE; POPSTATE; finally : lcoda; }

 /* tokenrange, unquoted strings */
<INITIAL>[a-zA-Z0-9_/.][-a-zA-Z0-9_/.]*                 { lenter; LEXA(); finally : lcoda; }
<INITIAL>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); finally : lcoda; }

 /* quoted strings */
<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<quoted>\\[\\"abtnvfrs0]                                { lenter; LEXC(1); finally : lcoda; }
<quoted>\\x[0-9a-f]{2}                                  { lenter; LEXH(2); finally : lcoda; }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { lenter; LEXH(3); finally : lcoda; }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(FORMULA_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(FORMULA_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0; /* return end-of-input */
                                                          finally : lcoda;
                                                        }
