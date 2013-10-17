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

	#define TOPSTATE(x) (x->states_n ? x->states[x->states_n-1] : 0)
	#define STATENAME(x)																			\
	    x == INITIAL					? "INITIAL"											\
	  : x == next							? "next"												\
	  : x == ws 							? "ws"													\
	  : x == operator					? "operator"										\
	  : x == slash						? "slash"												\
	  : x == argument					? "argument"										\
	  : x == multilinecomment	? "multilinecomment" : "UNKNWN"

	#define PUSHSTATE(a)																											\
		do {																																		\
			int al = snprintf(yyextra->space, sizeof(yyextra->space), "%s -> %s"	\
				, STATENAME(TOPSTATE(yyextra))																			\
				, STATENAME(a)																											\
			);																																		\
																																						\
			dprintf(listwise_info_fd, "(%2d) %.*s %*s @ %*s %d\n"									\
				, yyextra->states_n																									\
				, al																																\
				, yyextra->space																										\
				, MAX(58 - al, 0)																										\
				, ""																																\
				, 21																																\
				, ""																																\
				, __LINE__																													\
			);																																		\
																																						\
			yyextra->states[yyextra->states_n++] = a;															\
			yy_push_state(a, yyextra->scanner);																		\
		} while(0)

	#define POPSTATE																													\
		do {																																		\
			int x = TOPSTATE(yyextra);																						\
			yy_pop_state(yyextra->scanner);																				\
			yyextra->states_n--;																									\
																																						\
			int al = snprintf(yyextra->space, sizeof(yyextra->space), "%s <- %s"	\
				, STATENAME(TOPSTATE(yyextra))																			\
				, STATENAME(x)																											\
			);																																		\
																																						\
			dprintf(listwise_info_fd, "(%2d) %.*s %*s @ %*s %d\n"									\
				, yyextra->states_n																									\
				, al																																\
				, yyextra->space																										\
				, MAX(58 - al, 0)																										\
				, ""																																\
				, 21																																\
				, ""																																\
				, __LINE__																													\
			);																																		\
		} while(0)

	#define PTOKEN(text, leng, lval, lloc, token, xtra, line)														\
		do {																																							\
			int al = stresc(xtra->space, sizeof(xtra->space), text, leng);									\
																																											\
			int bl = 0;																																			\
			if(token == I64)																																\
				bl = snprintf(xtra->space2, sizeof(xtra->space2), "%"PRId64, lval->i64);			\
			else if(token == BREF)																													\
				bl = snprintf(xtra->space2, sizeof(xtra->space2), " %d", lval->bref);					\
																																											\
			dprintf(listwise_info_fd, "%8s ) '%.*s'%.*s %*s @ [%3d,%3d - %3d,%3d] - %d\n"		\
				, generator_tokname(token)																										\
				, al																																					\
				, xtra->space																																	\
				, bl																																					\
				, xtra->space2 																																\
				, MAX(50 - al - bl, 0)																												\
				, ""																																					\
				, lloc->f_lin																																	\
				, lloc->f_col																																	\
				, lloc->l_lin																																	\
				, lloc->l_col																																	\
				, line																																				\
			);																																							\
		}	while(0)																																				\

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

	#define LOCWRITE locwrite(yyextra, yylloc, yytext, yyleng, 0)
	#define LOCRESET locreset(yyextra, yylloc)

	int stresc(char * const buf, size_t sz, const char * const s, int l)
	{
		int o = 0;
		int x;
		for(x = 0; x < l; x++)
		{
			int r = 0;
			if(s[x] == 0x09)
				r = snprintf(buf + o, sz - o, "\\t");
			else if(s[x] == 0x0a)
				r = snprintf(buf + o, sz - o, "\\n");
			else if(s[x] == 0x0d)
				r = snprintf(buf + o, sz - o, "\\r");
			else if(s[x] >= 0x20 && s[x] <= 0x7e)
				r = snprintf(buf + o, sz - o, "%c", s[x]);
			else
				r = snprintf(buf + o, sz - o, "\\x%02hhx", s[x]);

			if(r >= (sz - o))
				break;
			else
				o += r;
		}

		return o;
	}

	void locreset(parse_param * xtra, YYLTYPE * lloc)
	{
		xtra->loc.f_lin++;
		xtra->loc.l_lin = xtra->loc.f_lin;

		xtra->loc.f_col = 0;
		xtra->loc.l_col = 0;
	}

	void locwrite(parse_param* xtra, YYLTYPE* lloc, char * text, int leng, int del)
	{
		xtra->loc.f_col = xtra->loc.l_col;
		xtra->loc.l_col = xtra->loc.f_col + leng;
		xtra->loc.s     = text;
		xtra->loc.e     = text + leng;

		lloc->f_col = xtra->loc.f_col + del;
		lloc->l_col = xtra->loc.l_col + del - 1;
		lloc->f_lin = xtra->loc.f_lin;
		lloc->l_lin = xtra->loc.l_lin;
		lloc->s = xtra->loc.s + del;
		lloc->e = xtra->loc.s - del - 1;

		xtra->loc.f_col += del;
		xtra->loc.l_col += del;
		xtra->loc.s += del;
		xtra->loc.e -= del;
	}

	int lexify(char * text, int leng, int del, YYSTYPE * lval, YYLTYPE * lloc, int token, parse_param * xtra, int bref, int i64, void * op, int lf, int line)
	{
		if(lf)
			locreset(xtra, lloc);
		else
			locwrite(xtra, lloc, text, leng, del);

		if(bref)
			sscanf(text + del, "%d", &lval->bref);
		else if(i64)
			sscanf(text + del, "%"SCNd64, &lval->i64);
		else if(op)
			lval->op = op;

		/* print the token if requested */
		PTOKEN(text, leng, lval, lloc, token, xtra, line);

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
																														char space[64];
																														snprintf(space, sizeof(space), "unknown operator : %s", yytext);
																														generator_yyerror(0, 0, yyextra, space);
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
