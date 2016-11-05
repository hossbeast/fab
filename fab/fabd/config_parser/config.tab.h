/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_CONFIG_YY_FAB_FABD_CONFIG_PARSER_CONFIG_TAB_H_INCLUDED
# define YY_CONFIG_YY_FAB_FABD_CONFIG_PARSER_CONFIG_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int config_yydebug;
#endif
/* "%code requires" blocks.  */
#line 18 "fab/fabd/config_parser/config.y" /* yacc.c:1909  */

  #include <stdint.h>
  #include <string.h>

  #include "value.h"
  #include "value/make.h"
  #include "valyria/list.h"
  #include "valyria/pstring.h"

  #include "config_parser.internal.h"

#line 56 "fab/fabd/config_parser/config.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STR = 258,
    BOOL = 259,
    INT = 260,
    HEX = 261,
    FLOAT = 262,
    CREF = 263,
    HREF = 264
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 45 "fab/fabd/config_parser/config.y" /* yacc.c:1909  */

  uint8_t     u8;
  int64_t     i64;
  double      fp;

  value * key;

  struct {
    value * key;
    value * val;
    uint16_t attr;
  } mapping;

  struct value * val;

#line 94 "fab/fabd/config_parser/config.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int config_yyparse (void* scanner, config_xtra * xtra);

#endif /* !YY_CONFIG_YY_FAB_FABD_CONFIG_PARSER_CONFIG_TAB_H_INCLUDED  */
