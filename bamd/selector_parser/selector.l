/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"

  #include "selector_parser.h"
  #include "selector.tab.h"
  #include "selector.tokens.h"

  #include "pattern_parser.h"

  #define YYSTYPE SELECTOR_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_LOOKUP_PATTERN ({                                          \
    selector_parser *parser = containerof(yyextra, selector_parser, yyu);  \
    size_t off = yytext - parser->yyu.yybuffer;                            \
    char * usr = parser->yyu.usrbuf + off;                                 \
    size_t bufsz = parser->yyu.usrbufsize - off;                           \
    yyu_location subloc;                                                   \
    int r;                                                                 \
    r = lookup_pattern_parse_partial(                                      \
        parser->pattern_parser                                             \
      , usr                                                                \
      , bufsz                                                              \
      , "-selector-"                                                       \
      , &parser->yyu.loc                                                   \
      , &subloc                                                            \
      , &yylval->lookup_pattern                                            \
    );                                                                     \
    if(r == 0) {                                                           \
      yyless(subloc.l);                                                    \
      r = LEXLOC(selector_LOOKUP_PATTERN, &subloc);                        \
    }                                                                      \
    r;                                                                     \
  })

  #define PARSE_PATH_PATTERN ({                                            \
    selector_parser *parser = containerof(yyextra, selector_parser, yyu);  \
    size_t off = yytext - parser->yyu.yybuffer;                            \
    char * usr = parser->yyu.usrbuf + off;                                 \
    size_t bufsz = parser->yyu.usrbufsize - off;                           \
    yyu_location subloc;                                                   \
    int r;                                                                 \
    r = path_pattern_parse_partial(                                        \
        parser->pattern_parser                                             \
      , usr                                                                \
      , bufsz                                                              \
      , "-selector-"                                                       \
      , &parser->yyu.loc                                                   \
      , &subloc                                                            \
      , &yylval->path_pattern                                              \
    );                                                                     \
    if(r == 0) {                                                           \
      yyless(subloc.l);                                                    \
      r = LEXLOC(selector_PATH_PATTERN, &subloc);                          \
    }                                                                      \
    r;                                                                     \
  })

  #define PARSE_SEARCH_PATTERN ({                                          \
    selector_parser *parser = containerof(yyextra, selector_parser, yyu);  \
    size_t off = yytext - parser->yyu.yybuffer;                            \
    char * usr = parser->yyu.usrbuf + off;                                 \
    size_t bufsz = parser->yyu.usrbufsize - off;                           \
    yyu_location subloc;                                                   \
    int r;                                                                 \
    r = search_pattern_parse_partial(                                      \
        parser->pattern_parser                                             \
      , usr                                                                \
      , bufsz                                                              \
      , "-selector-"                                                       \
      , &parser->yyu.loc                                                   \
      , &subloc                                                            \
      , &yylval->search_pattern                                            \
    );                                                                     \
    if(r == 0) {                                                           \
      yyless(subloc.l);                                                    \
      r = LEXLOC(selector_SEARCH_PATTERN, &subloc);                        \
    }                                                                      \
    r;                                                                     \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="selector_yy"
%option stack

%x multilinecomment
%x quoted
%x lookup_selector
%x pattern_lookup lookup
%x path_selector
%x pattern_path path
%x search_selector
%x pattern_search search

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
<*>[\r\t ]+                                             { LOCWRITE; }

 /* tokens */
<INITIAL,path_selector,lookup_selector,search_selector>[[\]{:] { return LEX(yytext[0]); }
<INITIAL>"}"                                            { return LEX(yytext[0]); }
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* invoke sub-parsers */
<INITIAL>"lookup"                                       { PUSHSTATE(lookup_selector); return LEX(LOOKUP); }
<INITIAL>"path"                                         { PUSHSTATE(path_selector); return LEX(PATH); }
<INITIAL>"search"                                       { PUSHSTATE(search_selector); return LEX(SEARCH); }

<path_selector>"pattern"                                { PUSHSTATE(pattern_path); return LEX(PATTERN); }
<path_selector>"}"                                      { POPSTATE; return LEX(yytext[0]); }
<pattern_path>[:]                                       { PUSHSTATE(path); return LEX(yytext[0]); }
<path>[^\n\r\t ][\x00-\xff]+                            { int r = PARSE_PATH_PATTERN; POPSTATE; POPSTATE; POPSTATE; return r; }

<search_selector>"pattern"                              { PUSHSTATE(pattern_search); return LEX(PATTERN); }
<search_selector>"}"                                    { POPSTATE; return LEX(yytext[0]); }
<pattern_search>[:]                                     { PUSHSTATE(search); return LEX(yytext[0]); }
<search>[^\n\r\t ][\x00-\xff]+                          { int r = PARSE_SEARCH_PATTERN; POPSTATE; POPSTATE; POPSTATE; return r; }

<lookup_selector>"pattern"                              { PUSHSTATE(pattern_lookup); return LEX(PATTERN); }
<lookup_selector>"}"                                    { POPSTATE; return LEX(yytext[0]); }
<pattern_lookup>[:]                                     { PUSHSTATE(lookup); return LEX(yytext[0]); }
<lookup>[^\n\r\t ][\x00-\xff]+                          { int r = PARSE_LOOKUP_PATTERN; POPSTATE; POPSTATE; POPSTATE; return r; }

 /* integers */
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { return LEXI("%"SCNxMAX); }

 /* enums, unquoted strings */
<INITIAL>[a-zA-Z0-9_/.+][-a-zA-Z0-9_/.+]*               { return LEXA(); }
<INITIAL>[-a-zA-Z0-9_/.}]+                              { return LEX(STR); }

 /* quoted strings */
<INITIAL>["]                                            { PUSHSTATE(quoted); return LEX(yytext[0]); }
<quoted>["]                                             { POPSTATE; return LEX(yytext[0]); }
<quoted>\\[\\"abtnvfrs0]                                { return LEXC(1); }
<quoted>\\x[0-9a-f]{2}                                  { return LEXH(2); }
<quoted>\\x\{[0-9a-f]{1,2}\}                            { return LEXH(3); }
<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { return LEX(STR); }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "invalid byte 0x%02hhx", yytext[0]);
                                                          return 0;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          LOCWRITE;
                                                          yyu_errorf(yyuparser, yylloc, "unexpected character '%1$c'(0x%1$02hhx)", yytext[0]);
                                                          return 0;
                                                        }
<<EOF>>                                                 {
                                                          return 0;
                                                        }
