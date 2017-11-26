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
  #include <stdio.h>
  #include <strings.h>

  #include "ff_parser.internal.h"
  #include "yyutil/scanner.h"

  #include "ff.tab.h"
  #include "ff.tokens.h"
  #include "FF.errtab.h"

  #include "parseint.h"

  /// LEX
  //
  // SUMMARY
  //  parse a whitespace or character token with no semantic value possibly at an offset from yytext
  //
  // PARAMETERS
  //  token - token to parse
  //  ldel  - offset from start of yytext
  //
  #define LEX(token)            \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , 0                       \
      , 0                       \
    )

  #define LEXV(token, del)      \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , del                     \
      , 0                       \
    )

  static xapi lexify(int token, YYSTYPE * lval, yyu_location * lloc, ff_extra * xtra, char * text, int leng, int del, void * v)
  {
    enter;

    if(token == CREF)
    {
      uint8_t u8 = 0;
      if(leng == 1)
        u8 = ff_cref_byte(text[del]);

      if(u8)
        lval->u8 = u8;
      else
        fatal(xparseuint, text + del, SCNx8, 0, UINT8_MAX, 0, UINT8_MAX, &lval->u8, 0);
    }
    else if(token == POSINT)
    {
      fatal(xparseuint, text + del, SCNu32, 0, UINT32_MAX, 0, UINT8_MAX, &lval->u32, 0);
    }

    fatal(yyu_lexify, token, lval, sizeof(*lval), lloc, xtra, text, leng, del, 0);

    finally : coda;
  }
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="ff_extra *"
%option stack

%x multilinecomment
%x bracketed
%x quoted

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
<INITIAL>[\n]                                           { LOCRESET; }
<INITIAL>[\r\t ]+                                       { LOCWRITE; }

 /* keywords */
<INITIAL>"artifact"                                     { lenter; LEX(ARTIFACT); yield(ARTIFACT); finally : lcoda; }
<INITIAL>"rule"                                         { lenter; LEX(RULE); yield(RULE); finally : lcoda; }
<INITIAL>"use"                                          { lenter; LEX(USE); yield(USE); finally : lcoda; }
<INITIAL>"formula"                                      { lenter; LEX(FORMULA); yield(FORMULA); finally : lcoda; }
<INITIAL>"require"                                      { lenter; LEX(REQUIRE); yield(REQUIRE); finally : lcoda; }
<INITIAL>"task"                                         { lenter; LEX(TASK); yield(TASK); finally : lcoda; }
<INITIAL>"var"                                          { lenter; LEX(VAR); yield(VAR); finally : lcoda; }

 /* tokens */
<INITIAL>[{}()?:%,$/.]                                  { lenter; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<INITIAL>[0-9]+                                         { lenter; LEX(POSINT); yield(POSINT); finally : lcoda; }
<INITIAL>[a-zA-Z_][-a-zA-Z0-9_]*                        { lenter; LEX(NAME); yield(NAME); finally : lcoda; }
<INITIAL>[-a-zA-Z0-9_=]*                                { lenter; LEX(WORD); yield(WORD); finally : lcoda; }

 /* scanmodes */
<INITIAL>[[]                                            { lenter; PUSHSTATE(bracketed); LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<bracketed>[\]]                                         { lenter; POPSTATE; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }

<INITIAL>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }

 /* crefs to non ascii bytes */
<quoted>\\[0abtnvfrs\\"]                                { lenter; LEXV(CREF, 1); yield(CREF); finally : lcoda; }
<bracketed>\\[0abtnvfrs]                                { lenter; LEXV(CREF, 1); yield(CREF); finally : lcoda; }

 /* 1-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x\{[0-9a-f]\})                  { lenter; LEXV(CREF, 3); yield(CREF); finally : lcoda; }

 /* 2-char hexrefs to non-ascii bytes */
<bracketed,quoted>(?i:\\x[01][0-9a-f])                  { lenter; LEXV(CREF, 2); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[01][0-9a-f]\})              { lenter; LEXV(CREF, 3); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x[7][f])                        { lenter; LEXV(CREF, 2); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[7][f]\})                    { lenter; LEXV(CREF, 3); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x[89a-f][0-9a-f])               { lenter; LEXV(CREF, 2); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[89a-f][0-9a-f]\})           { lenter; LEXV(CREF, 3); yield(CREF); finally : lcoda; }

 /* 2-char ascii hexrefs coerced to wref */
<bracketed,quoted>(?i:\\x[0-9a-f]{2})                   { lenter; LEXV(CREF, 2); yield(CREF); finally : lcoda; }
<bracketed,quoted>(?i:\\x\{[0-9a-f]{2}\})               { lenter; LEXV(CREF, 3); yield(CREF); finally : lcoda; }

<quoted>[^"\\\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(WORD); yield(WORD); finally : lcoda; }
<bracketed>[-^]                                         { lenter; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<bracketed>[^-\\\x00-\x1F\x7F-\xFF]                     { lenter; LEX(CHAR); yield(CHAR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(FF_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(FF_ILLBYTE, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          yield(0); /* return end-of-input */
                                                          finally : lcoda;
                                                        }
