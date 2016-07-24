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

  struct config_xtra;
  #define YYU_EXTRA_TYPE struct config_xtra
  #include "yyutil/scanner.h"

  #include "config_parser.internal.h"
  #include "config_parser/config.tab.h"
  #include "config_parser/config.tokens.h"
  #include "CONFIG.errtab.h"

  #undef perrtab
  #define perrtab perrtab_CONFIG

  /// LEX
  //
  // SUMMARY
  //  parse a whitespace or character token with no semantic value possibly at an offset from yytext
  // 
  // PARAMETERS
  //  token - token to parse
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
      , 0                       \
    )

  #define LEXV(token, ldel)     \
    fatal(lexify                \
      , token                   \
      , yylval                  \
      , yylloc                  \
      , yyextra                 \
      , yytext                  \
      , yyleng                  \
      , ldel                    \
      , 0                       \
      , 0                       \
    )

  xapi lexify(const int token, YYSTYPE * const lval, yyu_location * const lloc, config_xtra * const xtra, char * const text, const int leng, const int ldel, const int vdel, void * const v)
  {
    enter;

    if(token == CREF)
    {
      if(text[vdel] == '0')
        lval->u8 = 0;
      else if(text[vdel] == 'a')
        lval->u8 = 0x7;
      else if(text[vdel] == 'b')
        lval->u8 = 0x8;
      else if(text[vdel] == 't')
        lval->u8 = 0x9;
      else if(text[vdel] == 'n')
        lval->u8 = 0xa;
      else if(text[vdel] == 'v')
        lval->u8 = 0xb;
      else if(text[vdel] == 'f')
        lval->u8 = 0xc;
      else if(text[vdel] == 'r')
        lval->u8 = 0xd;
      else if(text[vdel] == 's')
        lval->u8 = 0x20;
      else if(text[vdel] == '\\')
        lval->u8 = '\\';
      else if(text[vdel] == '"')
        lval->u8 = '"';
    }
    else if(token == HREF)
      sscanf(text + vdel, "%hhx", &lval->u8);
    else if(token == INT)
      sscanf(text + vdel, "%"SCNd64, &lval->i64);
    else if(token == HEX)
      sscanf(text + vdel, "%"SCNx64, &lval->i64);
    else if(token == FLOAT)
      sscanf(text + vdel, "%Lf", &lval->fp);

    fatal(yyu_lexify, token, lval, sizeof(*lval), lloc, xtra, text, leng, ldel, 0);

    finally : coda;
  }
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="config_yy"
%option extra-type="config_xtra *"
%option stack

%x initial
%x multilinecomment
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
<initial>[\n]                                           { LOCRESET; }
<initial>[\r\t ]+                                       { LOCWRITE; }

 /* tokens */
<initial>[[\]{}]                                        { lenter; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<initial>(?i:true|false)                                { lenter; LEX(BOOL); yield(BOOL); finally : lcoda; }
<initial>[0-9]+                                         { lenter; LEX(INT); yield(INT); finally : lcoda; }
<initial>[-][0-9]+                                      { lenter; LEX(INT); yield(INT); finally : lcoda; }
<initial>(?i:0x[a-f0-9]+)                               { lenter; LEX(HEX); yield(HEX); finally : lcoda; }
<initial>[+-]?[0-9]+[.][0-9]+                           { lenter; LEX(FLOAT); yield(FLOAT); finally : lcoda; }
<initial>["]                                            { lenter; PUSHSTATE(quoted); LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }

<quoted>["]                                             { lenter; POPSTATE; LEX(yytext[0]); yield(yytext[0]); finally : lcoda; }
<quoted>\\[\\"abtnvfrs0]                                { lenter; LEXV(CREF, 1); yield(CREF); finally : lcoda; }
<quoted>(?i:\\x[0-9a-f]{2})                             { lenter; LEXV(HREF, 2); yield(HREF); finally : lcoda; }
<quoted>(?i:\\x\{[0-9a-f]{1,2}\})                       { lenter; LEXV(HREF, 3); yield(HREF); finally : lcoda; }
<quoted>[^\n"\x00-\x1F\x7F-\xFF]+                       { lenter; LEX(STR); yield(STR); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(CONFIG_ILLBYTE, "byte", "0x%02hhx", yytext[0]);
                                                          yield(0); /* return end-of-input */
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          LOCWRITE;
                                                          failf(CONFIG_ILLBYTE, "character", "'%1c'(0x%$102hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          while(yyextra->states_n)
                                                            { POPSTATE; }
                                                          yield(0); /* return end-of-input */
                                                          finally : lcoda;
                                                        }
