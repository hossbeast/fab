/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   0This file is part of fab.

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

  #include "MODULE.errtab.h"
  #include "module_parser.internal.h"
  #include "module.tab.h"
  #include "module.tokens.h"

  #include "value.h"
  #include "value/parser.h"

  #include "pattern_parser.h"

  #define YYSTYPE MODULE_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_VALUE do {                                                \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
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
    LEXLOC(module_VALUE, &subloc);                                        \
  } while(0);

  #define PARSE_MATCH_PATTERN do {                                        \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
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
    LEXLOC(module_MATCH_PATTERN, &subloc);                                \
  } while(0);

  #define PARSE_GENERATE_PATTERN do {                                     \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    fatal(generate_pattern_parse_partial                                  \
      , parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    yyless(subloc.l);                                                     \
    LEXLOC(module_GENERATE_PATTERN, &subloc);                             \
  } while(0);

  #define PARSE_REFERENCE_PATTERN do {                                    \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    fatal(reference_pattern_parse_partial                                 \
      , parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    yyless(subloc.l);                                                     \
    LEXLOC(module_REFERENCE_PATTERN, &subloc);                            \
  } while(0);

  #define PARSE_LOOKUP_PATTERN do {                                       \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    fatal(lookup_pattern_parse_partial                                    \
      , parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    yyless(subloc.l);                                                     \
    LEXLOC(module_LOOKUP_PATTERN, &subloc);                               \
  } while(0);
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="module_yy"
%option stack

%x multilinecomment
%x quoted
%x match
%x generate
%x reference
%x parse_lookup
%x rule_bacon
%x rule_match
%x rule_colon
%x rule_arrow
%x rule_starbox
%x rule_connector
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
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]                                              { LOCWRITE; }

 /* tokens */
<INITIAL>[+[\]{}:=,]                                    { lenter; LEX(yytext[0]); finally : lcoda; }

 /* sub-parsers */
<INITIAL>"use"                                          { lenter; PUSHSTATE(parse_lookup); LEX(USE); finally : lcoda; }
<INITIAL>"require"                                      { lenter; PUSHSTATE(parse_lookup); LEX(REQUIRE); finally : lcoda; }
<INITIAL>"import"                                       { lenter; PUSHSTATE(parse_lookup); LEX(IMPORT); finally : lcoda; }
<INITIAL>"variant"                                      { lenter; PUSHSTATE(parse_lookup); LEX(VARIANT); finally : lcoda; }

 /* value sub-parser */
<INITIAL>"var"                                          { lenter; PUSHSTATE(_var_stage); LEX(VAR); finally : lcoda; }
<_var_stage>[:=]                                        { lenter; PUSHSTATE(_var); LEX(yytext[0]); finally : lcoda; }
<_var>[^\n\r\t ][\x00-\xff]+                            { lenter; PARSE_VALUE; POPSTATE; POPSTATE; finally : lcoda; }

<INITIAL>"rule"                                         { lenter; PUSHSTATE(rule_match); LEX(RULE); finally : lcoda; }
<rule_match>"{ "                                        { lenter; PUSHSTATE(rule_bacon); LEX('{'); finally : lcoda; }
<rule_match>"-- "                                       { lenter; POPSTATE; PUSHSTATE(generate); PUSHSTATE(rule_starbox); LEX(DASHES); finally : lcoda; }
<rule_match>[\x00-\xff]                                 { lenter; POPSTATE; yyless(0); PUSHSTATE(match); finally : lcoda; }
<rule_connector>"["[*]"]"                               { lenter; LEX(STARBOX); finally : lcoda; }
<rule_connector>"-- "                                   { lenter; POPSTATE; PUSHSTATE(rule_colon); LEX(DASHES); finally : lcoda; }
<rule_connector>[\x00-\xff]                             { lenter; POPSTATE; yyless(0); PUSHSTATE(generate); PUSHSTATE(rule_starbox); PUSHSTATE(rule_arrow); PUSHSTATE(rule_starbox); finally : lcoda; }
<rule_starbox>"["[*]"]"                                 { lenter; POPSTATE; LEX(STARBOX); finally : lcoda; }
<rule_starbox>[\x00-\xff]                               { lenter; POPSTATE; yyless(0); finally : lcoda; }
<rule_arrow>"<-"                                        { lenter; POPSTATE; LEX(LARROW); finally : lcoda; }
<rule_arrow>"->"                                        { lenter; POPSTATE; LEX(RARROW); finally : lcoda; }
<rule_match,rule_colon>":"                              { lenter; PUSHSTATE(reference); LEX(':'); finally : lcoda; }
<rule_match,rule_colon>[\x00-\xff]                      { lenter; POPSTATE; yyless(0); finally : lcoda; }
<rule_bacon>[:]                                         { lenter; LEX(yytext[0]); finally : lcoda; }
<rule_bacon>[}]                                         { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }

 /* sub-parsers ; consume whitespace in this parser */
<match>[^\n\r\t ][\x00-\xff]+                           { lenter; PARSE_MATCH_PATTERN;     POPSTATE; PUSHSTATE(rule_connector); finally : lcoda; }
<generate>[^\n\r\t ][\x00-\xff]+                        { lenter; PARSE_GENERATE_PATTERN;  POPSTATE; PUSHSTATE(rule_colon); finally : lcoda; }
<reference>[^\n\r\t ][\x00-\xff]+                       { lenter; PARSE_REFERENCE_PATTERN; POPSTATE; finally : lcoda; }
<parse_lookup>[^\n\r\t ][\x00-\xff]+                    { lenter; PARSE_LOOKUP_PATTERN;    POPSTATE; finally : lcoda; }

 /* enums and unquoted strings */
<INITIAL,rule_bacon>[a-zA-Z]+                           { lenter; LEXA(); finally : lcoda; }

 /* quoted strings */
<INITIAL>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); finally : lcoda; }
<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); finally : lcoda; }
<quoted>\\[\\"abtnvfrs0]                                { lenter; LEXC(1); finally : lcoda; }
<quoted>\\x[0-9a-f]{2}                                  { lenter; LEXH(2); finally : lcoda; }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { lenter; LEXH(3); finally : lcoda; }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          DROPSTATES;
                                                          LOCWRITE;
                                                          failf(MODULE_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          DROPSTATES;
                                                          LOCWRITE;
                                                          failf(MODULE_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          POPSTATES;
                                                          _yyu_V = 0;       /* return end-of-input */
                                                          finally : lcoda;
                                                        }
