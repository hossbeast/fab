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

  #include "pattern_parser.h"

  #define YYSTYPE CONFIG_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_INCLUDE_PATTERN do {                                      \
    config_parser * parser = containerof(yyextra, config_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    fatal(include_pattern_parse_partial                                   \
      , parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    yyless(subloc.l);                                                     \
    LEXLOC(config_INCLUDE_PATTERN, &subloc);                              \
  } while(0);

  #define PARSE_MATCH_PATTERN do {                                        \
    config_parser * parser = containerof(yyextra, config_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    fatal(match_pattern_parse_partial                                     \
      , parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    yyless(subloc.l);                                                     \
    LEXLOC(config_MATCH_PATTERN, &subloc);                                \
  } while(0);

%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="config_yy"
%option stack

%x multilinecomment
%x quoted
%x include
%x include_pattern
%x exclude
%x match_pattern

/* bytes that cannot appear in the input */
ILL   [\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

 /* single-line comments */
<INITIAL,include,exclude>[#][^\x00-\x08\x0B-\x1F\x7F-\xFF\n]*           { LOCWRITE; }

 /* multiline comments are nestable */
<INITIAL,include,exclude,multilinecomment>[/][*]        { lenter; LOCWRITE; PUSHSTATE(multilinecomment); finally : lcoda; }
<multilinecomment>[*][/]                                { lenter; LOCWRITE; POPSTATE; finally : lcoda; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]+                                             { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}:=]                                      { lenter; LEX(yytext[0]); finally : lcoda; }
<INITIAL>(?i:true|false)                                { lenter; LEXB(); finally : lcoda; }

 /* sub-parsers */
<INITIAL>"include"                                      { lenter; PUSHSTATE(include); LEX(INCLUDE); finally : lcoda; }
<include>[{:=]                                          { lenter; LEX(yytext[0]); finally : lcoda; }
<include>"}"                                            { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<include>[^}:=]                                         { lenter; yyless(0); PUSHSTATE(include_pattern); finally : lcoda; }
<include_pattern>[^\n\r\t ][\x00-\xff]+                 { lenter; PARSE_INCLUDE_PATTERN; POPSTATE; finally : lcoda; }

<INITIAL>"exclude"                                      { lenter; PUSHSTATE(exclude); LEX(EXCLUDE); finally : lcoda; }
<exclude>[{:=]                                          { lenter; LEX(yytext[0]); finally : lcoda; }
<exclude>"}"                                            { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<exclude>[^}:=]                                         { lenter; yyless(0); PUSHSTATE(match_pattern); finally : lcoda; }
<match_pattern>[^\n\r\t ][\x00-\xff]+                   { lenter; PARSE_MATCH_PATTERN; POPSTATE; finally : lcoda; }

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
