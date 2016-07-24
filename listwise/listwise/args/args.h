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

#ifndef _ARGS_H
#define _ARGS_H

#include <stdint.h>

#define restrict __restrict

#define DEFAULT_MODE_OUTPUT             MODE_OUTPUT_SELECTED
#define DEFAULT_MODE_OUTPUT_NUMBERING   MODE_OUTPUT_NUMBERING_NONE
#define DEFAULT_MODE_OUTPUT_SEPARATOR   MODE_OUTPUT_SEPARATOR_NEWLINE

#define MODE_TABLE(x)                                                                                   \
/* output mode */                                                                                       \
  _MODE(MODE_OUTPUT_SELECTED            , 0x01  , x)  /* s : output only selected entries */            \
  _MODE(MODE_OUTPUT_LIST                , 0x02  , x)  /* a : output entire list */                      \
  _MODE(MODE_OUTPUT_STACK               , 0x03  , x)  /* k : output entire stack */                     \
/* output separation mode */                                                                            \
  _MODE(MODE_OUTPUT_SEPARATOR_NEWLINE   , 0x04  , x)  /*   : separate output rows by newline */         \
  _MODE(MODE_OUTPUT_SEPARATOR_NULL      , 0x05  , x)  /* Z : separate output rows by nullbyte */        \
/* output numbering mode */                                                                             \
  _MODE(MODE_OUTPUT_NUMBERING_NONE      , 0x06  , x)  /*   : do not number output rows */               \
  _MODE(MODE_OUTPUT_NUMBERING_ASC       , 0x07  , x)  /* n : number output rows 0 .. n */               \
  _MODE(MODE_OUTPUT_NUMBERING_INDEX     , 0x08  , x)  /* N : number output rows by list index */

#if DEBUG || DEVEL
# define DEFAULT_MODE_BACKTRACE   MODE_BACKTRACE_FULL

# define MODE_TABLE_DEBUG(x)                                                                            \
/* error reporting modes */                                                                             \
  _MODE(MODE_BACKTRACE_FULL             , 0x09  , x)  /* report on immediate error condition only */    \
  _MODE(MODE_BACKTRACE_PITHY            , 0x0a  , x)  /* unwind stack when reporting errors */
#endif

#define _MODE(a, b, c) a = b,
enum {
MODE_TABLE(0)
#if DEBUG || DEVEL
MODE_TABLE_DEBUG(0)
#endif
};
#undef _MODE

#define _MODE(a, b, c) (c) == b ? #a :
#if DEBUG || DEVEL
# define MODE_STR(x) MODE_TABLE_DEBUG(x) "UNKNWN"
#else
# define MODE_STR(x) MODE_TABLE(x) "UNKNWN"
#endif

#define INPUT_TYPE_TABLE(x)                           \
  _INPUT_TYPE(TRANSFORM_ITEM  , 0x01  , x)  /* -x */  \
  _INPUT_TYPE(TRANSFORM_FILE  , 0x02  , x)  /* -X */  \
  _INPUT_TYPE(INIT_LIST_ITEM  , 0x03  , x)  /* -l */  \
  _INPUT_TYPE(INIT_LIST_FILE  , 0x04  , x)  /* -L */  \
  _INPUT_TYPE(HYBRID_FILE     , 0x05  , x)  /* -H */

#define _INPUT_TYPE(a, b, c) INPUT_TYPE_ ## a = b,
enum {
INPUT_TYPE_TABLE(0)
};
#undef _INPUT_TYPE

#define _INPUT_TYPE(a, b, x) (x) == b ? #a :
#define INPUT_TYPE_STR(x) INPUT_TYPE_TABLE(x) "UNKNOWN"

struct g_args_t
{
#if DEBUG || DEVEL
  int     mode_backtrace;       // backtrace reporting mode
#endif

  int     mode_output;          // MODE_OUTPUT_*
  int     mode_numbering;       // MODE_OUTPUT_NUMBERING_*
  int     mode_separator;       // MODE_OUTPUT_SEPARATOR_*

  int     stdin_init_list_items;
  int     stdin_linewise;

  struct
  {
    char *  s;
    int     linewise;
    int     type;       // INPUT_TYPE_*
    int8_t  rank;
    int     order;
    int     bang;       // true if interpreting
  } *     inputs;
  size_t  inputsl;
  size_t  inputsa;
} g_args;

xapi args_parse(void);
xapi args_report(void);
void args_teardown(void);

#endif
