%{
	#include <stdio.h>

	#include "ff.h"
	#include "ff.tab.h"
	#include "ff.tokens.h"

	#include "log.h"
	#include "macros.h"

	#define TOPSTATE(x) (x->states_n ? x->states[x->states_n-1] : 0)
	#define STATENAME(x)														\
		  x == INITIAL					? "INITIAL"						\
		: x == quoteword				? "quoteword"					\
		: x == generatorws			? "generatorws"				\
		: x == generator				? "generator"					\
		: x == generatorlf			? "generatorlf"				\
		: x == command					? "command"						\
		: x == commandchecklit	? "commandchecklit"		\
		: x == commandlit				? "commandlit"				\
		: x == checklw					? "checklw"						\
		: x == varref						? "varref"						\
		: x == multilinecomment	? "multilinecomment"	\
		: x == list							? "list" : "UNKNWN"

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

	#define PTOKEN(s, l, t, lc, ln)																						\
		do {																																		\
			if(log_would(L_FF | L_FFTOKN))																				\
			{																																			\
				char buf[256];																											\
				int bufl = stresc(buf, sizeof(buf), s, l);													\
				log(L_FF | L_FFTOKN, "%8s ) '%.*s'%*s @ [%3d,%3d - %3d,%3d] - %d"		\
					, ff_tokname(t)																										\
					, bufl																														\
					, buf																															\
					, MAX(50 - bufl, 0)																								\
					, ""																															\
					, lc->first_line																									\
					, lc->first_column																								\
					, lc->last_line																										\
					, lc->last_column																									\
					, ln																															\
				);																																	\
			}																																			\
		}	while(0)																															\

	#define LEX(token)					\
		lexify(yytext, yyleng, 0, 1, yylval, yylloc, token, yyextra, __LINE__)

	#define LEXX(token, del)		\
		lexify(yytext, yyleng, del, 1, yylval, yylloc, token, yyextra, __LINE__)

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

	int lexify(const char* text, int leng, int del, int doloc, YYSTYPE* lval, YYLTYPE* lloc, int token, parse_param* xtra, int ln)
	{
		if(doloc)
			locify(xtra, lloc, leng, del);
		PTOKEN(text, leng, token, lloc, ln);

		lval->str.s = text;
		lval->str.e = text + leng;
		lval->num.v = token;

		/* store for error reporting */
		memcpy(&xtra->last_loc, lloc, sizeof(xtra->last_loc));
		xtra->last_tok = token;
		xtra->last_s = lval->str.s;
		xtra->last_e = lval->str.e;

		return token;
	}
%}

%option noinput nounput noyywrap noyy_top_state
%option reentrant
%option bison-bridge bison-locations
%option prefix="ff_yy"
%option extra-type="parse_param*"
%option stack

%x quoteword
%x generatorws
%x generator
%x generatorlf

%x command
%x commandchecklit
%x commandlit

%x list
%x checklw

%x varref
%x multilinecomment

%%

<INITIAL,list,checklw>[#][^\x00-\x1F\x7F-\xFF]*				{
																												// single-line comments
																												locify(yyextra, yylloc, yyleng, 0);
																											}
<INITIAL,list,checklw,multilinecomment>[/][*]					{
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

<INITIAL,list,checklw,multilinecomment>[\n]						{
																												// characters with special meaning in both INITIAL and list
																												loc_reset(yyextra, yylloc);
																											}
<INITIAL,list,checklw>[\t ]+													{ locify(yyextra, yylloc, yyleng, 0); }
<INITIAL,list,command>[[]															{
																												PUSHSTATE(list, yyextra);
																												return LEX(yytext[0]);
																											}
<INITIAL,list>["]																			{
																												PUSHSTATE(quoteword, yyextra);
																												return LEX(yytext[0]);
																											}
<INITIAL,list>[$]																			{
																												PUSHSTATE(varref, yyextra);
																												return LEX(yytext[0]);
																											}

<INITIAL>[+]																					{
																												// characters with special meaning in INITIAL
																												return LEX(yytext[0]);
																											}
<INITIAL>[:]																					{ return LEX(yytext[0]); }
<INITIAL>[.]																					{ return LEX(yytext[0]); }
<INITIAL>[,]																					{ return LEX(yytext[0]); }
<INITIAL>[(]																					{ return LEX(yytext[0]); }
<INITIAL>[)]																					{ return LEX(yytext[0]); }
<INITIAL>[-]																					{ return LEX(yytext[0]); }
<INITIAL>[|]																					{ return LEX(yytext[0]); }
<INITIAL>[*]																					{ return LEX(yytext[0]); }
<INITIAL>[~]																					{ return LEX(yytext[0]); }
<INITIAL>[=]																					{ return LEX(yytext[0]); }
<INITIAL>[<]																					{ return LEX(yytext[0]); }
<INITIAL>[>]																					{ return LEX(yytext[0]); }
<INITIAL>[@]																					{ return LEX(yytext[0]); }
<INITIAL>[{]																					{
																												PUSHSTATE(command, yyextra);
																												return LEX(yytext[0]);
																											}
<list>[]]																							{
																												// list-specific
																												POPSTATE(yyextra);
																												PUSHSTATE(checklw, yyextra);
																												return LEX(yytext[0]);
																											}
<checklw>=>>																					{
																												POPSTATE(yyextra);
																												PUSHSTATE(generatorws, yyextra);
																												return LEX(LW);
																											}
<checklw>[^\x00-\x1F\x7F-\xFF]												{ POPSTATE(yyextra); yyless(0); }

<INITIAL>[^ \\#"$\x00-\x1F\x7F-\xFF/[+:.,()\-|*~=<>@{]+	{
																												// string of anything else 
																												return LEX(WORD);
																											}
<list>[^ \\#"$\x00-\x1F\x7F-\xFF\]]+ 									{
																												// string of anything else 
																												return LEX(WORD);
																											}

<command>\n																						{
																												// command
																												PUSHSTATE(commandchecklit, yyextra);
																												int x = LEX(LF);
																												loc_reset(yyextra, yylloc);
																												return x;
																											}
<command>[}]																					{
																												POPSTATE(yyextra);
																												return LEX(yytext[0]);
																											}
<command>\\\\																					{ yytext++; yyleng--; return LEXX(WORD, 1); }
<command>\\[}]																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
<command>\\[[]																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
<command>[^\\[}\x00-\x08\x0a-\x1F\x7F-\xFF]+					{ return LEX(WORD); }
<commandchecklit>\t+>[\t ]+														{
																												POPSTATE(yyextra);
																												PUSHSTATE(commandlit, yyextra);
																											}
<commandchecklit>[^\x00-\x08\x0a-\x1F\x7F-\xFF]				{ POPSTATE(yyextra); yyless(0); }
<commandlit>[^\x00-\x1F\x7F-\xFF]+										{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<varref>[-@*#<^>]																			{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<varref>[a-zA-Z_][a-zA-Z0-9_]*												{
																												POPSTATE(yyextra);
																												return LEX(WORD);
																											}
<generatorws>[\t ]+["]																{
																												POPSTATE(yyextra);
																												PUSHSTATE(quoteword, yyextra);
																												int del = yyleng - 1;
																												yytext += del;
																												yyleng -= del;
																												return LEXX('"', del);
																											}
<generatorws>[\t ]+																		{
																												POPSTATE(yyextra);
																												PUSHSTATE(generator, yyextra);
																												return LEX(WS);
																											}
<generator>\n																					{
																												POPSTATE(yyextra);
																												PUSHSTATE(generatorlf, yyextra);
																												int x = LEX(WS);
																												loc_reset(yyextra, yylloc);
																												return x;
																											}
<generator>[\t ]																			{
																												POPSTATE(yyextra);
																												return LEX(WS);
																											}
<generator>[^\n\t ]+																	{ return LEX(WORD); }
<generatorlf>[\x00-\xff]															{
																												POPSTATE(yyextra);

																												// re-scan this byte
																												yyless(0);

																												if(yyextra->states_n && yyextra->states[yyextra->states_n - 1] == command)
																												{
																													// fabricate an LF token
																													yylval->str.s = yyextra->last_s;
																													yylval->str.e = yyextra->last_e;
																													yylval->num.v = LF;
																													yylloc->first_column = yyextra->loc.f_col;
																													yylloc->last_column = yyextra->loc.l_col;
																													yylloc->first_line = yyextra->loc.f_lin;
																													yylloc->last_line = yyextra->loc.l_lin;

																													// print it
																													PTOKEN(yylval->str.s, yylval->str.e - yylval->str.s, LF, yylloc, __LINE__);

																													return LF;
																												}
																											}
<quoteword>["]																				{
																												POPSTATE(yyextra);
																												return LEX(yytext[0]);
																											}
<quoteword>\\\\																				{ yytext++; yyleng--; return LEXX(WORD, 1); }
<quoteword>\\["]																			{ yytext++; yyleng--; return LEXX(WORD, 1); }
<quoteword>[^\\"\x00-\x1F\x7F-\xFF]+									{ return LEX(WORD); }

<*>[\x00]																							{ return END; }
<*>[\x01-\x1F\x7F-\xFF]																{
																												// non-printable characters - not allowed
																												log(L_ERROR | L_FF, "unknown character : (0x%02hhx)"
																													, yytext[0]
																												);
																												yyextra->badchar = 1;
																												return 0;
																											}
<*>[\x20-\x7E]																				{
																												log(L_ERROR | L_FF, "unknown character : '%c'(0x%02hhx)"
																													, yytext[0]
																													, yytext[0]
																												);
																												yyextra->badchar = 1;
																												return 0;
																											}
