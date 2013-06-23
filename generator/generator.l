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

	#include "macros.h"

	typedef unsigned short yytype_uint16;
	static const char *const yytname[] =
	{
		"$end", "error", "$undefined", "STR", "BREF", "I64", "OP", "$accept",
		"items", "item", "arg", "string", "strpart", 0
	};
	static const yytype_uint16 yytoknum[] =
	{
		0,   256,   257,   258,   259,   260,   261
	};
	#define YYNTOKENS  51
	const char * generator_tokname(int tok)
	{
		int x;
		for(x = 0; x < YYNTOKENS; x++)
		{
			if(yytoknum[x] == tok)
			{
				return yytname[x];
			}
		}
		return 0;
	}

	void generator_yyerror(void* loc, void* scanner, parse_param* pp, char const* err);

	int generator_lexify(char * text, int leng, YYLTYPE * locp, YYSTYPE * valp, int token, int line)
	{
		locp->s = text;
		locp->e = text + leng;

#if TOKENSTREAM
	char tmp[32] = { [0] = 0 };
	if(token == I64)
		snprintf(tmp, sizeof(tmp), "%ld", valp->i64);
	if(token == BREF)
		snprintf(tmp, sizeof(tmp), "%d", valp->bref);

	printf("%14s ) '%.*s'%s%*s @ %d\n"
		, generator_tokname(token)
		, (int)(locp->e - locp->s), locp->s
		, tmp
		, MAX(0, 50 - (int)(locp->e - locp->s)- (int)strlen(tmp)), ""
		, line
	);
#endif

		return token;
	}

#define LEX(token)              return generator_lexify(yytext, yyleng, yylloc, yylval, token, __LINE__)
#define LEXX(token, text, leng) return generator_lexify(text, leng, yylloc, yylval, token, __LINE__)
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="generator_yy"
%option extra-type="parse_param*"
%option stack

%x lit
%x multilinecomment
%x slash

%%

<INITIAL,multilinecomment>[\t ]*[/][*]	{ yy_push_state(multilinecomment, yyextra->scanner); }
<multilinecomment>[^*]+						{ }
<multilinecomment>[*][^/]					{ }
<multilinecomment>[*][/]					{ yy_pop_state(yyextra->scanner); }

<INITIAL>[\t ]+										{ }
<INITIAL>\\[/]										{ LEXX(STR, yytext+1, yyleng-1); }
<INITIAL>\\[<]										{ LEXX(STR, yytext+1, yyleng-1); }
<INITIAL>\\[\t ]									{ LEXX(STR, yytext+1, yyleng-1); }
<INITIAL>\\[0-9]+									{
				 														sscanf(yytext + 1, "%d", &yylval->bref);
																		LEX(BREF);
				 													}
<INITIAL>[/]											{ yy_push_state(slash, yyextra->scanner); LEX(yytext[0]); }
<slash>[/]												{ yy_pop_state(yyextra->scanner); yyless(0); LEXX(STR, "", 0); }
<slash>[^/]												{ yy_pop_state(yyextra->scanner); yyless(0); }

<INITIAL>[\n]											{ LEX(LF); }
<INITIAL>-?[0-9]+									{
				 														sscanf(yytext, "%ld", &yylval->i64);
				 														LEX(I64);
				 													}
<INITIAL>[<]											{ yy_push_state(lit, yyextra->scanner); }
<INITIAL>[^\t \\/<\n]+						{
				 														// determine whether this string happens to be an operator
				 														if((yylval->op = op_lookup(yytext, yyleng)))
																		{
				 															LEX(OP);
																		}
				 														else
																		{
				 															LEX(STR);
																		}
				 													}
<lit>[^>]+												{	LEX(STR); }
<lit>[>]													{ yy_pop_state(yyextra->scanner); }
