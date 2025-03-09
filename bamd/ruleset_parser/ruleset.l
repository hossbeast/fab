/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%{
  #include "yyutil/scanner.h"

  #include "ruleset_parser.h"
  #include "ruleset.tab.h"
  #include "ruleset.tokens.h"

  #include "bacon/value.h"
  #include "bacon/parser.h"

  #include "pattern_parser.h"

  #define YYSTYPE RULESET_YYSTYPE
  #define YYLTYPE yyu_location

  #define PARSE_SEARCH_PATTERN ({                                         \
    ruleset_parser * parser = containerof(yyextra, ruleset_parser, yyu);  \
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
      , &yylval->search_pattern                                           \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(ruleset_SEARCH_PATTERN, &subloc);                        \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_GENERATE_PATTERN ({                                       \
    ruleset_parser * parser = containerof(yyextra, ruleset_parser, yyu);  \
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
      , &yylval->generate_pattern                                         \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(ruleset_GENERATE_PATTERN, &subloc);                      \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_LOOKUP_PATTERN ({                                         \
    ruleset_parser * parser = containerof(yyextra, ruleset_parser, yyu);  \
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
      , &yylval->lookup_pattern                                           \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(ruleset_LOOKUP_PATTERN, &subloc);                        \
    }                                                                     \
    r;                                                                    \
  })

  #define PARSE_ATTACHMENT_PATTERN ({                                     \
    ruleset_parser * parser = containerof(yyextra, ruleset_parser, yyu);  \
    size_t off = yytext - parser->yyu.yybuffer;                           \
    char * usr = parser->yyu.usrbuf + off;                                \
    size_t bufsz = parser->yyu.usrbufsize - off;                          \
    yyu_location subloc;                                                  \
    int r;                                                                \
    r = attachment_pattern_parse_partial(                                 \
        parser->pattern_parser                                            \
      , usr                                                               \
      , bufsz                                                             \
      , parser->yyu.fname                                                 \
      , &parser->yyu.loc                                                  \
      , &subloc                                                           \
      , &yylval->attachment_pattern                                       \
    );                                                                    \
    if(r == 0) {                                                          \
      yyless(subloc.l);                                                   \
      r = LEXLOC(ruleset_ATTACHMENT_PATTERN, &subloc);                    \
    }                                                                     \
    r;                                                                    \
  })
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="ruleset_yy"
%option stack

%x multilinecomment
%x quoted
%x search
%x generate
%x lookup
%x attachment
%x match_pattern
%x rule_pre
%x rule_bacon
%x rule_lhs
%x rule_rhs
%x rule_rhs_bind
%x rule_search
%x rule_colon
%x rule_arrow
%x rule_starbox
%x rule_mid
%x rule_mid_bind
%x bind_bacon
%x bind_lookup
%x bind_arrow

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
<INITIAL>[%.+[\]{}():=,$!]                              { return LEX(yytext[0]); }

 /* keywords */
<INITIAL>"use"                                          { PUSHSTATE(lookup); return LEX(USE); }
<INITIAL>"anchor"                                       { PUSHSTATE(lookup); return LEX(ANCHOR); }

<INITIAL>"bind"                                         { PUSHSTATE(bind_lookup); return LEX(BIND); }
<bind_lookup>"{ "                                       { PUSHSTATE(bind_bacon); return LEX('{'); }
<bind_lookup>[\x00-\xff]                                { POPSTATE; yyless(0); PUSHSTATE(attachment); PUSHSTATE(rule_starbox); PUSHSTATE(bind_arrow); PUSHSTATE(lookup); }
<bind_arrow>[<][-]                                      { return LEX(LARROW); }
<bind_arrow>[-][>]                                      { return LEX(RARROW); }
<bind_arrow>[=][>]                                      { return LEX(BINDARROW); }
<bind_arrow>[\x00-\xff]                                 { POPSTATE; yyless(0); }
<bind_bacon>[:]                                         { return LEX(yytext[0]); }
<bind_bacon>[}]                                         { POPSTATE; return LEX(yytext[0]); }
<rule_starbox>"["[*]"]"                                 { POPSTATE; return LEX(STARBOX); }
<rule_starbox>[\x00-\xff]                               { POPSTATE; yyless(0); }

<INITIAL>"rule"                                         { PUSHSTATE(rule_lhs); PUSHSTATE(rule_pre); return LEX(RULE); }
<INITIAL>"search"                                       { PUSHSTATE(rule_mid); PUSHSTATE(search); return LEX(SEARCH); }
<rule_pre>"{ "                                          { POPSTATE; PUSHSTATE(rule_bacon); return LEX('{'); }
<rule_pre>[\x00-\xff]                                   { POPSTATE; yyless(0); }
<rule_bacon>[:]                                         { return LEX(yytext[0]); }
<rule_bacon>[}]                                         { POPSTATE; return LEX(yytext[0]); }
<rule_lhs>"search"                                      { POPSTATE; PUSHSTATE(rule_mid); PUSHSTATE(search); return LEX(SEARCH); }
<rule_lhs>"-- "                                         { POPSTATE; PUSHSTATE(rule_mid); return LEX(DASHES); }
<rule_lhs>[\x00-\xff]                                   { POPSTATE; PUSHSTATE(rule_mid); PUSHSTATE(search); yyless(0); }
<rule_mid,rule_mid_bind>"["[*]"]"                       { return LEX(STARBOX); }
<rule_mid>[<][-]                                        { return LEX(LARROW); }
<rule_mid>[-][>]                                        { return LEX(RARROW); }
<rule_mid>[=][>]                                        { POPSTATE; PUSHSTATE(rule_mid_bind); return LEX(BINDARROW); }
<rule_mid,rule_mid_bind>"-- "                           { POPSTATE; PUSHSTATE(rule_colon); return LEX(DASHES); }
<rule_mid>[\x00-\xff]                                   { POPSTATE; PUSHSTATE(rule_rhs); yyless(0); }
<rule_mid_bind>[\x00-\xff]                              { POPSTATE; PUSHSTATE(rule_rhs_bind); yyless(0); }
<rule_search,rule_colon>":"                             { PUSHSTATE(lookup); return LEX(':'); }
<rule_search,rule_colon>[\x00-\xff]                     { POPSTATE; yyless(0); }
<rule_rhs,rule_rhs_bind>"search"                        { POPSTATE; PUSHSTATE(search); return LEX(SEARCH); }
<rule_rhs,rule_rhs_bind>"generate"                      { POPSTATE; PUSHSTATE(rule_colon); PUSHSTATE(generate); return LEX(GENERATE); }
<rule_rhs>[\x00-\xff]                                   { POPSTATE; PUSHSTATE(rule_colon); PUSHSTATE(generate); yyless(0); }
 /* rhs is attachment pattern if arrow was bindarrow */
 /* otherwise the rhs is a generate pattern */
<rule_rhs_bind>[\x00-\xff]                              { POPSTATE; PUSHSTATE(rule_colon); PUSHSTATE(attachment); yyless(0); }

 /* sub-parsers ; consume whitespace in this parser */
<search>[^\n\r\t ][\x00-\xff]+                          { int r = PARSE_SEARCH_PATTERN;     POPSTATE; return r; }
<generate>[^\n\r\t ][\x00-\xff]+                        { int r = PARSE_GENERATE_PATTERN;   POPSTATE; return r; }
<lookup>[^\n\r\t ][\x00-\xff]+                          { int r = PARSE_LOOKUP_PATTERN;     POPSTATE; return r; }
<attachment>[^\n\r\t ][\x00-\xff]+                      { int r = PARSE_ATTACHMENT_PATTERN; POPSTATE; return r; }

 /* integers */
<INITIAL>[-][0-9]+                                      { return LEXI("%"SCNdMAX); }
<INITIAL>[0-9]+                                         { return LEXI("%"SCNuMAX); }
<INITIAL>(?i:0x[a-f0-9]+)                               { return LEXI("%"SCNxMAX); }
<INITIAL>(?i:true|false)                                { return LEXB(); }

 /* enums and unquoted strings */
<INITIAL,rule_bacon,bind_bacon>[-_a-zA-Z0-9]+           { return LEXA(); }

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
