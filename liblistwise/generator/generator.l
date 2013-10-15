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

	#include "generator/generator.def.h"
	#include "generator/generator.tab.h"
	#include "generator/generator.tokens.h"

	#include "macros.h"

	#define PUSHSTATE(a, b)																				\
		do {																												\
			dprintf(listwise_info_fd, "(%2d) %s:%d -> %s:%d @ %d"			\
				, b->states_n																						\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(a), a	\
				, __LINE__																							\
			);																												\
			b->states[b->states_n++] = a;															\
			yy_push_state(a, b->scanner);															\
		} while(0)

	#define POPSTATE(b)																						\
		do {																												\
			int x = TOPSTATE(b);																			\
			yy_pop_state(b->scanner);																	\
			b->states_n--;																						\
			dprintf(listwise_info_fd, "(%2d) %s:%d <- %s:%d @ %d"			\
				, b->states_n																						\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(x), x	\
				, __LINE__																							\
			);																												\
		} while(0)

	#define PTOKEN(text, leng, lval, lloc, token, xtra, line)														\
		do {																																							\
			int al = stresc(xtra->space, sizeof(xtra->space), text, leng);									\
																																											\
			int bl = 0;																																			\
			if(t == OP)																																			\
				bl = snprintf(xtra->space2, sizeof(xtra->space2), "%s", lval->op->s);					\
			else if(t == I64)																																\
				bl = snprintf(xtra->space2, sizeof(xtra->space2), "%"PRId64, lval->i64);			\
			else if(t == BREF)																															\
				bl = snprintf(xtra->space2, sizeof(xtra->space2), " %d", lval->bref);					\
																																											\
			dprintf(listwise_info_fd, "%8s ) '%.*s'%.*s %*s @ [%3d,%3d - %3d,%3d] - %d"		  \
				, generator_tokname(t)																												\
				, al																																					\
				, xtra->space																																	\
				, bl																																					\
				, xtra->space2 																																\
				, MAX(50 - al - bl, 0)																												\
				, ""																																					\
				, lloc->first_line																														\
				, lloc->first_column																													\
				, lloc->last_line																															\
				, lloc->last_column																														\
				, line																																				\
			);																																							\
		}	while(0)																																				\


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

	#define LEX(token)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 0, 0,  __LINE__)

	#define LEXX(token, del)	\
		lexify(yytext, yyleng, del, yylval, yylloc, token, yyextra, 0, 0, 0,  __LINE__)

	#define LEXB(token, del)	\
		lexify(yytext, yyleng, del, yylval, yylloc, token, yyextra, 1, 0, 0,  __LINE__)

	#define LEXI(token)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 1, 0,  __LINE__)

	#define LEXO(token, op)	\
		lexify(yytext, yyleng, 0,   yylval, yylloc, token, yyextra, 0, 0, op, __LINE__)

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

	int lexify(const char * text, int leng, int del, YYSTYPE * lval, YYLTYPE * lloc, int token, parse_param * xtra, int bref, int i64, void * op, int line)
	{
		locify(xtra, lloc, leng, del);

		if(bref)
			sscanf(text + del, "%d", &lval->bref);
		else if(i64)
			sscanf(text + del, "%"SCNd64, &lval->i64);
		else if(op)
			lval->op = op;

		/* print the token if requested */
		PTOKEN(text, leng, lval, lloc, token, xtra, line);

		/* store for error reporting */
		memcpy(xtra->last_loc, lloc, sizeof(*xtra->last_loc));
		xtra->last_tok = token;

		return token;
	}

	void loc_reset(parse_param * xtra, YYLTYPE * lloc)
	{
		xtra->loc->f_lin++;
		xtra->loc->l_lin = xtra->loc->f_lin;

		xtra->loc->f_col = 0;
		xtra->loc->l_col = 0;
	}

	void locify(parse_param* xtra, YYLTYPE* lloc, int leng, int del)
	{
		locp->s = text;
		locp->e = text + leng;

		xtra->loc->f_col = xtra->loc->l_col;
		xtra->loc->l_col = xtra->loc->f_col + leng;

		lloc->first_column = xtra->loc->f_col + del;
		lloc->last_column  = xtra->loc->l_col + del - 1;
		lloc->first_line	 = xtra->loc->f_lin;
		lloc->last_line		 = xtra->loc->l_lin;

		xtra->loc->f_col += del;
		xtra->loc->l_col += del;
	}
	

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

<INITIAL,multilinecomment>[/][*]											{
																												// multi-line comments
																												PUSHSTATE(multilinecomment, yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<multilinecomment>[^*\x00-\x08\x0a-\x1F\x7F-\xFF]+		{ locify(yyextra, yylloc, yyleng, 0); }
<multilinecomment>[*][^/\x00-\x08\x0a-\x1F\x7F-\xFF]	{ locify(yyextra, yylloc, yyleng, 0); }
<multilinecomment>[*][/]															{
																												POPSTATE(yyextra);
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<INITIAL,multilinecomment>[\n]												{
																												loc_reset(yyextra, yylloc);
																											}

<INITIAL>[^\n\t \\/{[]+																{ yy_push_state(operator, yyextra->scanner); }
<INITIAL>[\t \n]+																			{ }
<operator>[-a-zA-Z]+																	{
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
