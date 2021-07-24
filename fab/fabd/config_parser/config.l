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
    int r;                                                                \
    r = include_pattern_parse_partial(                                   \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r != 0) {                                                          \
      return r;                                                           \
    }                                                                     \
    yyless(subloc.l);                                                     \
    LEXLOC(config_INCLUDE_PATTERN, &subloc);                              \
  } while(0);

  #define PARSE_MATCH_PATTERN do {                                        \
    config_parser * parser = containerof(yyextra, config_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = match_pattern_parse_partial(                                     \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r != 0) {                                                          \
      return r;                                                           \
    }                                                                     \
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
<INITIAL,include,exclude,multilinecomment>[/][*]        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]+                                             { LOCWRITE; }

 /* tokens */
<INITIAL>[[\]{}:=]                                      { LEX(yytext[0]); }
<INITIAL>(?i:true|false)                                { LEXB(); }

 /* sub-parsers */
<INITIAL>"include"                                      { PUSHSTATE(include); LEX(INCLUDE); }
<include>[{:=]                                          { LEX(yytext[0]); }
<include>"}"                                            { POPSTATE; LEX(yytext[0]); }
<include>[^}:=]                                         { yyless(0); PUSHSTATE(include_pattern); }
<include_pattern>[^\n\r\t ][\x00-\xff]+                 { PARSE_INCLUDE_PATTERN; POPSTATE; }

<INITIAL>"exclude"                                      { PUSHSTATE(exclude); LEX(EXCLUDE); }
<exclude>[{:=]                                          { LEX(yytext[0]); }
<exclude>"}"                                            { POPSTATE; LEX(yytext[0]); }
<exclude>[^}:=]                                         { yyless(0); PUSHSTATE(match_pattern); }
<match_pattern>[^\n\r\t ][\x00-\xff]+                   { PARSE_MATCH_PATTERN; POPSTATE; }

 /* integers */
<INITIAL>[-][0-9]+                                      { LEXI("%"SCNdMAX); }
<INITIAL>[0-9]+                                         { LEXI("%"SCNuMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { LEXI("%"SCNxMAX); }

 /* enums and unquoted strings */
<INITIAL>[a-zA-Z0-9_][-a-zA-Z0-9_]+                     { LEXA(); }
<INITIAL>[-a-zA-Z0-9_/]+                                { LEX(STR); }

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
