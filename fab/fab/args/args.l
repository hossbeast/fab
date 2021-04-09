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
  #include "args.tab.h"
  #include "args.tokens.h"

  #define YYSTYPE union yyu_lval
  #define YYLTYPE yyu_location
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="args_yy"
%option stack

%x adhoc
%x multilinecomment

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

 /* integers */
<INITIAL>[-][0-9]+                                      { lenter; LEXI("%"SCNdMAX); finally : lcoda; }
<INITIAL>[0-9]+                                         { lenter; LEXI("%"SCNuMAX); finally : lcoda; }
<INITIAL>(?i:0x[a-f0-9]+)                               { lenter; LEXI("%"SCNxMAX); finally : lcoda; }

 /* tokens */
<INITIAL>[-+bdghuvxtBK]                                 { lenter; LEX(yytext[0]); finally : lcoda; }

<adhoc>[\x20-\x7E]+                                     { lenter; LEX(STR); finally : lcoda; }

 /* enums and unquoted strings */
<INITIAL>"adhoc"                                        { lenter; PUSHSTATE(adhoc); LEX(ADHOC); finally : lcoda; }
<INITIAL>[-+_a-zA-Z0-9./]+                              { lenter; LEXA(); finally : lcoda; }

<*>[\x00-\x1F\x7F-\xFF]                                 {
                                                          lenter;
                                                          LOCWRITE;
                                                          failf(0, "byte", "0x%02hhx", yytext[0]);
                                                          finally : lcoda;
                                                        }
<*>[\x20-\x7E]                                          {
                                                          lenter;
                                                          LOCWRITE;
                                                          failf(0, "character", "'%1$c'(0x%1$02hhx)", yytext[0]);
                                                          finally : lcoda;
                                                        }
<<EOF>>                                                 {
                                                          lenter;
                                                          _yyu_V = 0;       /* return end-of-input */
                                                          finally : lcoda;
                                                        }
