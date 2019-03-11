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

#ifndef VALUE_PARSER_INTERNAL_H
#define VALUE_PARSER_INTERNAL_H

#include "yyutil/parser.h"
#include "parser.h"

struct value_store;
struct YYLTYPE;

struct value_parser {
  yyu_parser value_yyu;
  yyu_parser value_set_yyu;
  yyu_parser value_list_yyu;

  struct value_store * store; // value storage
  struct value *       root;  // (returns) root of the parsed tree
};

static void value_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int value_set_yylex(void *, void *, void *)
  __attribute__((weakref("value_yylex")));

static void value_set_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int value_list_yylex(void *, void *, void *)
  __attribute__((weakref("value_yylex")));

static void value_list_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));


#endif
