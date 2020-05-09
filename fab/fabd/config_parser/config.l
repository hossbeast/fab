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

  #include "CONFIG.errtab.h"
  #include "config_parser.internal.h"
  #include "config.tab.h"
  #include "config.tokens.h"

  #include "value.h"
  #include "value/parser.h"

  #define YYSTYPE CONFIG_YYSTYPE
  #define YYLTYPE yyu_location

#define PARSE_VALUE do {                                                \
  config_parser * parser = containerof(yyextra, config_parser, yyu);    \
  size_t off = yytext - parser->yyu.yybuffer;                           \
  char * usr = parser->yyu.usrbuf + off;                                \
  size_t bufsz = parser->yyu.usrbufsize - off;                          \
  yyu_location subloc;                                                  \
  fatal(value_parser_parse_partial                                      \
    , parser->value_parser                                              \
    , usr                                                               \
    , bufsz                                                             \
    , "var"                                                             \
    , &parser->yyu.loc                                                  \
    , &subloc                                                           \
    , 0                                                                 \
    , &yylval->value                                                    \
  );                                                                    \
  yyless(subloc.l);                                                     \
  LEXLOC(config_VALUE, &subloc);                                        \
} while(0);
  
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="config_yy"
%option stack

%x multilinecomment
%x quoted
%x _var_stage _var

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
<_var,_var_stage>[\n]                                   |
<INITIAL>[\n]                                           { LOCRESET; }
<_var,_var_stage>[\r\t ]+                               |
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}:=]                                      { lenter; LEX(yytext[0]); finally : lcoda; }
<INITIAL>(?i:true|false)                                { lenter; LEXB(); finally : lcoda; }

 /* invoke sub-parsers */
<INITIAL>"var"                                          { lenter; PUSHSTATE(_var_stage); LEX(VAR); finally : lcoda; }
<_var_stage>[:=]                                        { lenter; PUSHSTATE(_var); LEX(yytext[0]); finally : lcoda; }
<_var>[^\n\r\t ][\x00-\xff]+                            { lenter; PARSE_VALUE; POPSTATE; POPSTATE; finally : lcoda; }

 /* integers */
<INITIAL>[-][0-9]+                                      { lenter; LEXI("%"SCNdMAX); finally : lcoda; }
<INITIAL>[0-9]+                                         { lenter; LEXI("%"SCNuMAX); finally : lcoda; }
<INITIAL>(?i:0x[a-f0-9]+)                               { lenter; LEXI("%"SCNxMAX); finally : lcoda; }

 /* enums and unquoted strings */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]+                     { lenter; LEXA(); finally : lcoda; }
<INITIAL>[-a-zA-Z0-9_/]+                                { lenter; LEX(STR); finally : lcoda; }

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
                                                          failf(CONFIG_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          POPSTATES;
                                                          LOCWRITE;
                                                          failf(CONFIG_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0;       /* return end-of-input */
                                                          finally : lcoda;
                                                        }