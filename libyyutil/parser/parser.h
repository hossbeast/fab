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

#ifndef _YYUTIL_PARSER_H
#define _YYUTIL_PARSER_H

#include "xapi.h"
#include "types.h"

#include "grammar.h"

struct hashtable;     // valyria/hashtable
struct attrs;
struct yy_buffer_state;

typedef struct yyu_vtable {
  struct yy_buffer_state * (*yy_scan_bytes)(const char * bytes, int len, void * scanner);
  struct yy_buffer_state * (*yy_scan_buffer)(char * buf, size_t size, void * scanner);
  void (*yy_delete_buffer)(struct yy_buffer_state *, void *);
  int (*yylex_destroy)(void * scanner);
  int (*yylex_init)(void ** scanner);
  void (*yyset_extra)(void * parser, void * scanner);
  int (*yyparse)(void * scanner, void * parser);
  char (*yyget_hold_char)(void * scanner);
} yyu_vtable;

#define YYU_VTABLE(name, lex_prefix, tab_prefix)         \
yyu_vtable name = {                                      \
    yy_scan_bytes : lex_prefix ## _yy_scan_bytes         \
  , yy_scan_buffer : lex_prefix ## _yy_scan_buffer       \
  , yy_delete_buffer : lex_prefix ## _yy_delete_buffer   \
  , yylex_destroy : lex_prefix ## _yylex_destroy         \
  , yylex_init : lex_prefix ## _yylex_init               \
  , yyset_extra : lex_prefix ## _yyset_extra             \
  , yyget_hold_char : lex_prefix ## _yyget_hold_char     \
  , yyparse : tab_prefix ## _yyparse                     \
}

/* indexes of particular tokens */
struct yyu_tokens {
  uint16_t STR;            // sequence of printable characters
  uint16_t CREF;           // character-escape of one byte in the printable range of 7-bit ascii
  uint16_t HREF;           // hex escape of one byte
  uint16_t BOOL;           // boolean literals
  uint16_t FLOAT;          // floating point literals
  uint16_t HEX8;           // hex literals
  uint16_t HEX16;
  uint16_t HEX32;
  uint16_t HEX64;
  uint16_t UINTMAX8;       // positive integer literals
  uint16_t UINTMAX16;
  uint16_t UINTMAX32;
  uint16_t UINTMAX64;
  uint16_t INTMIN8;        // possibly negative integer literals
  uint16_t INTMIN16;
  uint16_t INTMIN32;
  uint16_t INTMIN64;
  uint16_t INTMAX8;
  uint16_t INTMAX16;
  uint16_t INTMAX32;
  uint16_t INTMAX64;
};

/// yyu_parser
//
// SUMMARY
//  structure for parsing data (yyextra)
//
typedef struct yyu_parser {
/* parse state */
  bool            scanerr;          // error raised from scanner
  bool            gramerr;          // error raised from parser
  uint16_t        attrs;            // YYU_*

  int             states[64];       // start states stack
  int             states_n;
  const char *    yybuffer;
  char *          usrbuf;
  size_t          usrbufsize;
  yyu_location    loc;              // running location track for this parse

                                    // last successfully scanned token
  yyu_location    last_loc;         //  location
  int             last_token;       //  token
  int             last_line;        //  line number in the scanner
  size_t          last_lval_aloc;   //  allocated size

                                    // upon error
  yyu_location    error_loc;        //  location
  char            error_str[256];   //  string
  char            tokenstring[256]; //  tokenstring (gramerr only)
  const char *    fname;            //  filename

/* parser configuration */
  struct hashtable * str_token_table; // tokens for parsing instead of STR in LEXA
  uint64_t logs;                      // logging categories for this parser
  const yyu_vtable * vtable;
  xapi error_syntax;
  void * scanner;                     // flex scanner
  void * last_lval;                   //  semantic value
  yyu_location * final_loc;

  /// lvalstr
  //
  // SUMMARY
  //  yyu calls this function get a textual description of a scanned token
  //
  size_t (*lvalstr)(struct yyu_parser * restrict parser, int token, void * restrict lval, char * restrict buf, size_t blen);

/* autogenerated parser configuration */
/* .tokens.h structures */
  uint16_t numtokens;
  uint16_t mintoken;
  uint16_t maxtoken;
  const uint16_t * tokenindexes;   // [mintoken, maxtoken] - indexes by token number
  const uint16_t * tokennumbers;   // [0, numtokens) - token numbers
  const char ** tokennames;        // [0, numtokens) - token names
  const char ** tokenstrings;      // [0, numtokens) - token user strings
  struct yyu_tokens tokens;        // indexes of particular tokens
/* .states.h structures */
  int numstates;
  const int * statenumbers;
  const char ** statenames;
} yyu_parser;

/// yyu_parser_init
//
// SUMMARY
//
//
xapi yyu_parser_init(
    yyu_parser * const restrict parser
  , const yyu_vtable * const restrict vtable
  , xapi error_syntax
)
  __attribute__((nonnull));

/// yyu_parser_init_tokens
//
// SUMMARY
//
//
// PARAMETERS
//  parser        -
//  numtokens     -
//  mintoken      -
//  maxtoken      -
//  tokenindexes  -
//  tokennumbers  -
//  tokennames    -
//  tokenstrings  -
//  numstates     -
//  statenumbers  -
//  statenames    -
//
xapi yyu_parser_init_tokens(
    yyu_parser * const restrict parser
  , uint16_t numtokens
  , uint16_t mintoken
  , uint16_t maxtoken
  , const uint16_t * restrict tokenindexes
  , const uint16_t * restrict tokennumbers
  , const char ** restrict tokennames
  , const char ** restrict tokenstrings
  , const uint16_t * restrict tokenstring_tokens
);

xapi yyu_parser_init_states(
    yyu_parser * const restrict parser
  , int numstates
  , const int * restrict statenumbers
  , const char ** restrict statenames
)
  __attribute__((nonnull));

/// yyu_parser_xdestroy
//
// SUMMARY
//  destroy an yyu_parser
//
xapi yyu_parser_xdestroy(yyu_parser * const restrict parser)
  __attribute__((nonnull));

#define YYU_PARTIAL 1  // stop parsing after recognizing an utterance
#define YYU_INPLACE 2  // parse the buffer without making a copy ; it ends with two null bytes

/// yyu_parse
//
// SUMMARY
//
//
// PARAMETERS
//  parser      -
//  text        -
//  text_len    -
//  fname       -
//  attrs       - 
//  [init_loc]  - 
//  [final_loc] - (returns) 
//
xapi yyu_parse(
    yyu_parser * const restrict parser
  , char * const restrict text
  , size_t text_len
  , const char * const restrict fname
  , uint16_t attrs
  , yyu_location * restrict init_loc
  , yyu_location * restrict final_loc
)
  __attribute__((nonnull(1, 2)));

#if 0
/// yyu_define_enum
//
// SUMMARY
//  define strings to be parsed with LEXA
//
// PARAMETERS
//  parser - parser
//  token  -
//  e      - attrs
//
// REMARKS
//  not useful in a parser where enum members overlap
//
xapi yyu_define_enum(yyu_parser * restrict parser, int token, const struct attrs * restrict e)
  __attribute__((nonnull));
#endif

/// yyu_define_tokenrange
//
// SUMMARY
//  define strings to be parsed with LEXA
//
// PARAMETERS
//  parser  -
//  first   -
//  last    -
//
xapi yyu_define_tokenrange(yyu_parser * restrict parser, int first, int last);

#endif
