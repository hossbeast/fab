%{
	#include <stdio.h>

	#include "ff.h"
	#include "ff.tab.h"
	#include "ff.tokens.h"

	#include "log.h"

	#define MIN(a,b)            \
	 ({ typeof (a) _a = (a);    \
			 typeof (b) _b = (b);   \
		 _a > _b ? _b : _a; })

	#define TOPSTATE(x) (x->states_n ? x->states[x->states_n-1] : 0)
	#define STATENAME(x)	\
		  x == INITIAL			? "INITIAL"					\
		: x == quoteword		? "quoteword"				\
		: x == generatorws	? "generatorws"			\
		: x == generator		? "generator"				\
		: x == command			? "command"					\
		: x == checklw			? "checklw"					\
		: x == list					? "list" : "UNKNWN"	\

	#define PUSHSTATE(a, b)																				\
		do {																												\
			log(L_FF | L_FFSTAT, "%s:%d -> %s:%d"											\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(a), a	\
			);																												\
			b->states[b->states_n++] = a;															\
			yy_push_state(a, b->scanner);															\
		} while(0)

	#define POPSTATE(b)																							\
		do {																													\
			int x = TOPSTATE(b);																				\
			yy_pop_state(b->scanner);																		\
			b->states_n--;																							\
			log(L_FF | L_FFSTAT, "%s:%d <- %s:%d"												\
				, STATENAME(TOPSTATE(b)), TOPSTATE(b), STATENAME(x), x);	\
		} while(0)

	#define PTOKEN(s, l, t, lc, ln)																					\
			log(L_FF | L_FFTOKN, "%15s ) '%.*s'%*s @ [%3d,%3d - %3d,%3d] - %d"	\
				, ff_tokname(t)																										\
				, MIN(t == LF ? 0 : t == WS ? 0 : l, 50)													\
				, t == LF ? "" : t == WS ? "" : s																	\
				, 50 - MIN(t == LF ? 0 : t == WS ? 0 : l, 50)											\
				, ""																															\
				, lc->first_line																									\
				, lc->first_column																								\
				, lc->last_line																										\
				, lc->last_column																									\
				, ln)

	#define LEX(token)					\
		lexify(yytext, yyleng, 0, 1, yylval, yylloc, token, 0, 0, yyextra, __LINE__)

	#define LEXX(token, del)		\
		lexify(yytext, yyleng, del, 1, yylval, yylloc, token, 0, 0, yyextra, __LINE__)

	#define LEXV(token, vs, vl)	\
		lexify(yytext, yyleng, 0, 1, yylval, yylloc, token, vs, vs + vl, yyextra, __LINE__)

	void loc_reset(parse_param * xtra, YYLTYPE* lloc)
	{
		xtra->loc.f_lin++;
		xtra->loc.l_lin = xtra->loc.f_lin;

		xtra->loc.f_col = 0;
		xtra->loc.l_col = 0;
	}

	void locify(parse_param* xtra, YYLTYPE* lloc, int leng, int del)
	{
		xtra->loc.f_col = xtra->loc.l_col;
		xtra->loc.l_col = xtra->loc.f_col + leng;

		lloc->first_column = xtra->loc.f_col + del;
		lloc->last_column  = xtra->loc.l_col + del - 1;
		lloc->first_line	 = xtra->loc.f_lin;
		lloc->last_line		 = xtra->loc.l_lin;

		xtra->loc.f_col += del;
		xtra->loc.l_col += del;
	}

	int lexify(const char* text, int leng, int del, int doloc, YYSTYPE* lval, YYLTYPE* lloc, int token, char* vs, char* ve, parse_param* xtra, int ln)
	{
		if(doloc)
			locify(xtra, lloc, leng, del);
		PTOKEN(text, leng, token, lloc, ln);

		lval->str.s = text;
		lval->str.e = text + leng;
		lval->num.v = token;
		lval->var.vs = vs;
		lval->var.ve = ve;

		/* store for error reporting */
		memcpy(&xtra->last_loc, lloc, sizeof(xtra->last_loc));
		xtra->last_tok = token;
		xtra->last_s = lval->str.s;
		xtra->last_e = lval->str.e;

		return token;
	}
%}

%option nounput noyywrap
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"
%option stack

%x quoteword
%x generatorws
%x generator

%s command
%x checklw
%s list

%%

[#].*																				{ locify(yyextra, yylloc, yyleng, 0); }

<INITIAL>>>																	{ return LEX(INC); }
<INITIAL>=>>																{
																							PUSHSTATE(generatorws, yyextra);
																							return LEX(LW);
																						}
<checklw>=>>																{
																							POPSTATE(yyextra);
																							PUSHSTATE(generatorws, yyextra);
																							return LEX(LW);
																						}
<checklw>[^ \t=]														{ POPSTATE(yyextra); yyless(0); }
<checklw>[ \t]+															{ locify(yyextra, yylloc, yyleng, 0); }

<INITIAL>[^\n\t\\ :"$[{=\x00]+							{ return LEX(WORD); }
<list>[^\n\t\\ "$[\]\x00]+									{ return LEX(WORD); }
<command>[^\n\\}[\x00]+											{ return LEX(WORD); }
\\\\																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
<INITIAL,list>\\[ ]													{ yytext++; yyleng--; return LEXX(WORD, 1); }
<list>\\[$]																	{ yytext++; yyleng--; return LEXX(WORD, 1); }
\\[[]																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
<list>\\["]																	{ yytext++; yyleng--; return LEXX(WORD, 1); }
<list>\\[]]																	{ yytext++; yyleng--; return LEXX(WORD, 1); }
<INITIAL>\\[:]															{ yytext++; yyleng--; return LEXX(WORD, 1); }
<INITIAL>\\[{]															{ yytext++; yyleng--; return LEXX(WORD, 1); }
<INITIAL>\\[=]															{ yytext++; yyleng--; return LEXX(WORD, 1); }

<INITIAL,list>\n														{ loc_reset(yyextra, yylloc); }
<command>\n																	{
																							int x = LEX(LF);
																							loc_reset(yyextra, yylloc);
																							return x;
																						}
<INITIAL,list>[\t ]+												{ locify(yyextra, yylloc, yyleng, 0); }
<list>\$[a-zA-Z_@*#][a-zA-Z0-9_]*						{ return LEXV(VARNAME, yytext + 1, yyleng - 1); }
<list>\$\{[a-zA-Z_@*#][a-zA-Z0-9_]*\}				{ return LEXV(VARNAME, yytext + 2, yyleng - 3); }
<INITIAL>[:]																{ return LEX(yytext[0]); }
<INITIAL>[=]																{ return LEX(yytext[0]); }
<command>[}]																{
																							POPSTATE(yyextra);
																							return LEX(yytext[0]);
																						}
<list>[]]																		{
																							POPSTATE(yyextra);
																							PUSHSTATE(checklw, yyextra);
																							return LEX(yytext[0]);
																						}
[[]																					{
																							PUSHSTATE(list, yyextra);
																							return LEX(yytext[0]);
																						}
<INITIAL,list>["]														{
																							PUSHSTATE(quoteword, yyextra);
																							return LEX(yytext[0]);
																						}
<INITIAL>[{]																{
																							PUSHSTATE(command, yyextra);
																							return LEX(yytext[0]);
																						}

<generatorws>{
[\t ]+["]																		{
																							POPSTATE(yyextra);
																							PUSHSTATE(quoteword, yyextra);
																							int del = yyleng - 1;
																							yytext += del;
																							yyleng -= del;
																							return LEXX('"', del);
																						}
[\t ]+																			{
																							POPSTATE(yyextra);
																							PUSHSTATE(generator, yyextra);
																							return LEX(WS);
																						}
}
<generator>{
\n																					{
																							POPSTATE(yyextra);
																							int x = LEX(WS);
																							loc_reset(yyextra, yylloc);
																							return x;
																						}
[\t ]																				{
																							POPSTATE(yyextra);
																							return LEX(WS);
																						}
\\\\																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
\\[ ]+																			{ yytext++; yyleng--; return LEXX(WORD, 1); }
[^\\ \n\t]+																	{ return LEX(WORD); }
}
<quoteword>{
["]																					{
																							POPSTATE(yyextra);
																							return LEX(yytext[0]);
																						}
\\\\																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
\\["]																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
[^\\"]+																			{ return LEX(WORD); }
}

\x00																				{ return END; }
<*>[\x00-\xff]															{
																							locify(yyextra, yylloc, yyleng, 0);
																							if(yytext[0] >= 0x20 && yytext[0] <= 0x7E)
																							{
																								log(L_ERROR | L_FF, "unknown character : '%c'(0x%02hhx)"
																									, yytext[0]
																									, yytext[0]
																								);
																							}
																							else
																							{
																								log(L_ERROR | L_FF, "unknown character : (0x%02hhx)"
																									, yytext[0]
																								);
																							}
																							REJECT;
																						}
