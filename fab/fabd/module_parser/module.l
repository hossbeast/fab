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

  #include "module_parser.internal.h"
  #include "module.tab.h"
  #include "module.tokens.h"

  #include "value.h"
  #include "value/parser.h"

  #include "pattern_parser.h"

  #define YYSTYPE MODULE_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_VALUE ({                                                  \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = value_parser_parse_partial(                                       \
        parser->value_parser                                              \
      , usr                                                               \
      , bufsz                                                             \
      , "var"                                                             \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , 0                                                                 \
      , &yylval->value                                                    \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(module_VALUE, &subloc);                                  \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_SEARCH_PATTERN ({                                         \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = search_pattern_parse_partial(                                     \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(module_MATCH_PATTERN, &subloc);                          \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_GENERATE_PATTERN ({                                       \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = generate_pattern_parse_partial(                                   \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(module_GENERATE_PATTERN, &subloc);                       \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_REFERENCE_PATTERN ({                                      \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = reference_pattern_parse_partial(                                  \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(module_REFERENCE_PATTERN, &subloc);                      \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_LOOKUP_PATTERN ({                                         \
    module_parser * parser = containerof(yyextra, module_parser, yyu);    \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = lookup_pattern_parse_partial(                                     \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->pattern                                                  \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(module_LOOKUP_PATTERN, &subloc);                         \
    }                                                                     \
    r;                                                                    \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="module_yy"
%option stack

%x multilinecomment
%x quoted
%x search
%x generate
%x reference
%x parse_lookup
%x rule_bacon
%x rule_search
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
<INITIAL,multilinecomment>[/][*]                        { LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]                                { LOCWRITE; POPSTATE; }
<multilinecomment>[^*\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+  { LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]  { LOCWRITE; }
<multilinecomment>[/]                                   { LOCWRITE; }
<multilinecomment>[\n]                                  { LOCRESET; }

 /* whitespace */
<*>[\n]                                                 { LOCRESET; }
<*>[\r\t ]                                              { LOCWRITE; }

 /* tokens */
<INITIAL>[+[\]{}:=,]                                    { return LEX(yytext[0]); }

 /* sub-parsers */
<INITIAL>"use"                                          { PUSHSTATE(parse_lookup); return LEX(USE); }
<INITIAL>"require"                                      { PUSHSTATE(parse_lookup); return LEX(REQUIRE); }
<INITIAL>"import"                                       { PUSHSTATE(parse_lookup); return LEX(IMPORT); }
<INITIAL>"variant"                                      { PUSHSTATE(parse_lookup); return LEX(VARIANT); }

 /* value sub-parser */
<INITIAL>"var"                                          { PUSHSTATE(_var_stage); return LEX(VAR); }
<_var_stage>[:=]                                        { PUSHSTATE(_var); return LEX(yytext[0]); }
<_var>[^\n\r\t ][\x00-\xff]+                            { int r = PARSE_VALUE; POPSTATE; POPSTATE; return r; }

<INITIAL>"rule"                                         { PUSHSTATE(rule_search); return LEX(RULE); }
<rule_search>"{ "                                       { PUSHSTATE(rule_bacon); return LEX('{'); }
<rule_search>"-- "                                      { POPSTATE; PUSHSTATE(generate); PUSHSTATE(rule_starbox); return LEX(DASHES); }
<rule_search>[\x00-\xff]                                { POPSTATE; yyless(0); PUSHSTATE(search); }
<rule_connector>"["[*]"]"                               { return LEX(STARBOX); }
<rule_connector>"-- "                                   { POPSTATE; PUSHSTATE(rule_colon); return LEX(DASHES); }
<rule_connector>[\x00-\xff]                             { POPSTATE; yyless(0); PUSHSTATE(generate); PUSHSTATE(rule_starbox); PUSHSTATE(rule_arrow); PUSHSTATE(rule_starbox); }
<rule_starbox>"["[*]"]"                                 { POPSTATE; return LEX(STARBOX); }
<rule_starbox>[\x00-\xff]                               { POPSTATE; yyless(0); }
<rule_arrow>"<-"                                        { POPSTATE; return LEX(LARROW); }
<rule_arrow>"->"                                        { POPSTATE; return LEX(RARROW); }
<rule_search,rule_colon>":"                             { PUSHSTATE(reference); return LEX(':'); }
<rule_search,rule_colon>[\x00-\xff]                     { POPSTATE; yyless(0); }
<rule_bacon>[:]                                         { return LEX(yytext[0]); }
<rule_bacon>[}]                                         { POPSTATE; return LEX(yytext[0]); }

 /* sub-parsers ; consume whitespace in this parser */
<search>[^\n\r\t ][\x00-\xff]+                          { int r = PARSE_SEARCH_PATTERN;    POPSTATE; PUSHSTATE(rule_connector); return r; }
<generate>[^\n\r\t ][\x00-\xff]+                        { int r = PARSE_GENERATE_PATTERN;  POPSTATE; PUSHSTATE(rule_colon); return r; }
<reference>[^\n\r\t ][\x00-\xff]+                       { int r = PARSE_REFERENCE_PATTERN; POPSTATE; return r; }
<parse_lookup>[^\n\r\t ][\x00-\xff]+                    { int r = PARSE_LOOKUP_PATTERN;    POPSTATE; return r; }

 /* enums and unquoted strings */
<INITIAL,rule_bacon>[a-zA-Z]+                           { return LEXA(); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { return LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { return LEXH(2); }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { return LEXH(3); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }

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
