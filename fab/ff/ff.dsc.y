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

%code top {
	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>

	#include "xlinux.h"

	#include "ff.parse.h"
	#include "ffn.h"

	static int __attribute__((weakref, alias("ff_yylex"))) ff_dsc_yylex(); 
	static int __attribute__((weakref, alias("ff_yyerror"))) ff_dsc_yyerror(); 
}

/* terminals with a semantic value */
%token <str> WORD		"WORD"				/* string token */
%token <str> WS			"WS"					/* single tab/space */
%token <str> LF			"LF"					/* single newline */
%token <num> '$'									/* variable reference */
%token <num> '@'									/* nofile reference */
%token <num> '.'									/* nofile scoping */
%token <num> ':'									/* dependency */
%token <num> '*'									/* weak dependency */
%token <num> '^'									/* bridge dependency */
%token <num> '%'									/* dependency discovery */
%token <num> '['									/* list start */
%token <num> ']'									/* list end */
%token <num> ','									/* list element separator */
%token <num> '~'									/* list transformation via listwise */
%token <num> '{'									/* formula start */
%token <num> '}'									/* formula end */
%token <num> '='									/* variable assignment */
%token <num> '-'									/* variable link */
%token <num> ';'									/* varref separator */
%token <num> '+'									/* invocation */
%token <num> '('									/* invocation context */
%token <num> ')'									/* invocation context */

%token <num> ONCE		"ONCE"				/* once */

/* terminals */
%right LF ONCE WORD

/* nonterminals */
%type  <node> statement
%type  <node> statements
%type  <node> dependency
%type  <node> list
%type  <node> listparts
%type  <node> listpart
%type  <node> word
%type  <node> varref
%type  <node> nofile
%type  <node> nofileparts
%type  <node> generator

%destructor { ffn_free($$); } <node>

%%
ff
	: statements
	{
		YYU_FATAL(ffn_mknode, &parm->ffn, &@$, FFN_STMTLIST, $1);
	}
	;

statements
	: statements statement
	{
		$$ = ffn_addchain($1, $2);
	}
	| statement
	;
statement
	: dependency
	;
dependency
	: list ':' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $3, FFN_SINGLE);
	}
	| list ':' '*' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $4, FFN_SINGLE | FFN_WEAK);
	}
	| list ':' '^' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $4, FFN_SINGLE | FFN_BRIDGE);
	}
	| list ':' ':' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $4, FFN_MULTI);
	}
	| list ':' ':' '*' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $5, FFN_MULTI | FFN_WEAK);
	}
	| list ':' ':' '^' list
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_DEPENDENCY, $1, $5, FFN_MULTI | FFN_BRIDGE);
	}
	;
list
	: '[' listparts ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , (void*)0, (void*)0);
	}
	| '[' ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, (void*)0, (void*)0);
	}
	| '[' listparts '~' generator ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , $4      , (void*)0);
	}
	| '[' '~' generator ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, $3, (void*)0);
	}
	| '[' listparts '~' list ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, $2      , (void*)0, $4);
	}
	| '[' '~' list ']'
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_LIST, (void*)0, (void*)0, $3);
	}
	;

listparts
	: listparts listpart
	{
		$$ = ffn_addchain($1, $2);
	}
	| listpart
	;

listpart
	: varref
	| word %prec WORD
	| nofile
	| list
	;

varref
	: '$' word %prec WORD
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_VARREF, $2);
	}
	;

generator
	: word
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_TRANSFORM, $1);
	}
	;

nofile
	: '@' nofileparts
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_NOFILE, $2);
	}

nofileparts
	: nofileparts '.' word %prec WORD
	{
		$$ = ffn_addchain($1, $3);
	}
	| word %prec WORD
	;

word
	: word WORD
	{
		$$ = $1;

		/* contiguous words coalesce */
		YYU_FATAL(pscatw, &$$->text, @2.s, @2.e - @2.s);
	}
	| WORD
	{
		YYU_FATAL(ffn_mknode, &$$, &@$, FFN_WORD);
	}
	;


