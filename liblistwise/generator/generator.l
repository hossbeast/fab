/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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
	#include <inttypes.h>

	#include "generator/generator.def.h"
	#include "generator/generator.tab.h"
	#include "generator/generator.tokens.h"

	#include "macros.h"

	/// YYUSAY
	//
	// write informational messages to listwise_info_fd
	//
	#define YYUSAY(...) dprintf(listwise_info_fd, ##__VA_ARGS__)

	/// STATENAME
	//
	// get state name from state
	//
	#define STATENAME(x)																			\
	    x == INITIAL					? "INITIAL"											\
	  : x == next							? "next"												\
	  : x == ws 							? "ws"													\
	  : x == operator					? "operator"										\
	  : x == slash						? "slash"												\
	  : x == argument					? "argument"										\
	  : x == multilinecomment	? "multilinecomment" : "UNKNWN"

	/// YYULVAL
	//
	// write textual description of a token/lval to a buffer
	//
	#define YYULVAL(token, lval, xtra, bbuf, blen)										\
		bbuf = xtra->space2;																						\
		blen = 0;																												\
		if(token == I64)																								\
			blen = snprintf(bbuf, sizeof(bbuf), "%"PRId64, lval->i64);		\
		else if(token == BREF)																					\
			blen = snprintf(bbuf, sizeof(bbuf), "%d", lval->bref);				\
																																										
	#include "yyutil.h"

	#define LEX(token)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 0, 0, 0,   __LINE__)

	#define LEXLF	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, LF,    yyextra, 0, 0, 0, 1,   __LINE__)

	#define LEXX(token, del)	\
		lexify(yytext, yyleng, del, yylval, yylloc, token, yyextra, 0, 0, 0, 0,   __LINE__)

	#define LEXS(token, text, leng)	\
		lexify(text,   leng,     0, yylval, yylloc, token, yyextra, 0, 0, 0, 0,   __LINE__)

	#define LEXB(token, del)	\
		lexify(yytext, yyleng, del, yylval, yylloc, token, yyextra, 1, 0, 0, 0,   __LINE__)

	#define LEXI(token)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 1, 0, 0,   __LINE__)

	#define LEXO(token, op)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 0, op, 0, __LINE__)

	int lexify(char * text, int leng, int del, YYSTYPE * lval, YYLTYPE * lloc, int token, parse_param * xtra, int bref, int i64, void * op, int lf, int line)
	{
		if(lf)
			yyu_locreset(xtra, lloc);
		else
			yyu_locwrite(xtra, lloc, text, leng, del);

		if(bref)
			sscanf(text + del, "%d", &lval->bref);
		else if(i64)
			sscanf(text + del, "%"SCNd64, &lval->i64);
		else if(op)
			lval->op = op;

		/* print the token if requested */
		PTOKEN(token, lval, lloc, xtra, text, leng, line);

		/* store for error reporting */
		memcpy(&xtra->last_loc, lloc, sizeof(xtra->last_loc));
		xtra->last_tok = token;

		return token;
	}

%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="generator_yy"
%option extra-type="parse_param*"
%option stack

%x next
%x ws 
%x operator
%x slash
%x argument
%x multilinecomment

ILL		[\x00-\x08\x0B-\x1F\x7F-\xFF]

%%

<INITIAL,multilinecomment>[/][*]												{ LOCWRITE; PUSHSTATE(multilinecomment); }
<multilinecomment>[*][/]																{ LOCWRITE; POPSTATE; }
<multilinecomment>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n*/]+		{ LOCWRITE; }
<multilinecomment>[*][^\x00-\x08\x0B-\x1F\x7F-\xFF\n/]	{ LOCWRITE; }
<multilinecomment>[/]																		{ LOCWRITE; }
<INITIAL,multilinecomment>[\n]													{ LOCRESET; }

<INITIAL>[\t ]+																					{ LOCWRITE; }
<INITIAL>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /]+					{
																													PUSHSTATE(next);
																													typeof(*yylval->op) * op;

				 																									if((op = op_lookup(yytext, yyleng)))
																														PUSHSTATE(operator);
																													else
																														PUSHSTATE(argument);
																													yyless(0);
																												}
<next>[\n]																							{ PUSHSTATE(ws); LEXLF; }
<next>[\t ]+																						{ PUSHSTATE(ws); LEX(WS); }
<next>[/]																								{ PUSHSTATE(slash); LEX(yytext[0]); }
<ws>[\n]																								{ LEXLF; }
<ws>[\t ]+																							{ LEX(WS); }
<ws>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t ]+								{ POPSTATE; PUSHSTATE(operator); }
<operator>[-a-zA-Z]+																		{
																													POPSTATE;

																													typeof(*yylval->op) * op;
				 																									if((op = op_lookup(yytext, yyleng)))
																													{
				 																										return LEXO(OP, op);
																													}
																													else
																													{
																														snprintf(yyextra->space, sizeof(yyextra->space), "unknown operator : %s", yytext);
																														generator_yyerror(0, 0, yyextra, yyextra->space);
																														return 0;
																													}
																												}
<slash>[/]																							{ POPSTATE; yyless(0); LEXS(STR, "", 0); }
<slash>[^\x00-\x08\x0B-\x1F\x7F-\xFF/]+									{ POPSTATE; PUSHSTATE(argument); yyless(0); }
<argument>\\[0-9]+																			{ LEXB(BREF, 1); }
<argument>\\[/]																					{ LEXX(STR, 1); }
<argument>\\[\t ]+																			{ LEXX(STR, 1); }
<argument>-?[0-9]+																			{ LEXI(I64); }
<argument>[^\x00-\x08\x0B-\x1F\x7F-\xFF\n\t /\\]+				{ LEX(STR); }
<argument>[\n\t /]																			{ POPSTATE; yyless(0); }
<*>[\x00]																								{
																													while(yyextra->states_n)
																													{
																														POPSTATE;
																													}
																													return 0;
																												}
<*>[\x01-\x1F\x7F-\xFF]																	{
																													snprintf(yyextra->space, sizeof(yyextra->space), "unknown character : (0x%02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->space);
																													return 0;
																												}
<*>[\x20-\x7E]																					{
																													snprintf(yyextra->space, sizeof(yyextra->space), "unknown character : '%1$c'(0x%1$02hhx)", yytext[0]);
																													generator_yyerror(0, 0, yyextra, yyextra->space);
																													return 0;
																												}
