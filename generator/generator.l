/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of listwise.
   
   listwise is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   listwise is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

%{
	#include <stdio.h>

	#include "generator/generator.def.h"
	#include "generator/generator.tab.h"

	void generator_yyerror(void* scanner, parse_param* pp, char const* err);

#define PTOKEN(token)																																													\
		(token == OP ?   printf("%3d: OP   - '%.*s' (%p)\n", __LINE__, yylval->str.l, yylval->str.s, yylval->op.v)	\
	: token == STR ?  printf("%3d: STR  - '%.*s'\n"			, __LINE__, yylval->str.l, yylval->str.s)								\
	: token == I64 ?  printf("%3d: I64  - '%.*s'\n"			, __LINE__, yylval->str.l, yylval->str.s)								\
	: token == BREF ? printf("%3d: BREF - '%.*s'\n"			, __LINE__, yylval->str.l, yylval->str.s)								\
	: token == LF   ? printf("%3d: LF\n"								, __LINE__)																							\
	:                 printf("%3d: TOK  - (%c) (%d)\n"	, __LINE__, token, token))

/*
<INITIAL>\\[^0-9\\/\n][^\\/\n]*		{
				 														yylval->str.s = strdup(yytext+1);
				 														yylval->str.l = yyleng-1;
				 														return LEX(STR);
				 													}
*/

#if 0
#	define LEX(token) PTOKEN(token), token
#else
#	define LEX(token) token
#endif
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge 
%option prefix="generator_yy"
%option extra-type="parse_param*"
%option stack

%x lit
%x multilinecomment

%%

<INITIAL,multilinecomment>[\t ]*[/][*]	{ yy_push_state(multilinecomment, yyextra->scanner); }
<multilinecomment>[^*]+						{ }
<multilinecomment>[*][^/]					{ }
<multilinecomment>[*][/]					{ yy_pop_state(yyextra->scanner); }

<INITIAL>[\t ]+										{ }
<INITIAL>\\[/]										{
																		yylval->str.s = strdup(yytext+1);
																		yylval->str.l = yyleng-1;
																		return LEX(STR);
																	}
<INITIAL>\\[<]										{
																		yylval->str.s = strdup(yytext+1);
																		yylval->str.l = yyleng-1;
																		return LEX(STR);
																	}
<INITIAL>\\[\t ]									{
																		yylval->str.s = strdup(yytext+1);
																		yylval->str.l = yyleng-1;
																		return LEX(STR);
																	}
<INITIAL>\\[0-9]+									{
				 														yylval->bref.s = strdup(yytext);
				 														yylval->bref.l = yyleng;
				 														sscanf(yytext + 1, "%d", &yylval->bref.v);
				 														return LEX(BREF);
				 													}

<INITIAL>[/]											{ return LEX(yytext[0]); }
<INITIAL>[\n]											{ return LEX(LF); }
<INITIAL>-?[0-9]+									{
				 														yylval->i64.s = strdup(yytext);
				 														yylval->i64.l = yyleng;
				 														sscanf(yytext, "%ld", &yylval->i64.v);
				 														return LEX(I64);
				 													}
<INITIAL>[<]											{ yy_push_state(lit, yyextra->scanner); }

<INITIAL>[^\t \\/<\n]+						{
				 														yylval->str.s = strdup(yytext);
				 														yylval->str.l = yyleng;

				 														// determine whether this string happens to be an operator
				 														if((yylval->op.v = op_lookup(yytext, yyleng)))
				 															return LEX(OP);
				 														else
				 															return LEX(STR);
				 													}

<lit>[^>]+												{
																		yylval->str.s = strdup(yytext);
																		yylval->str.l = yyleng;
																		return LEX(STR);
																	}
<lit>[>]													{ yy_pop_state(yyextra->scanner); }
