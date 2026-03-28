/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

%code requires {
  #include <stdlib.h>
  #include <inttypes.h>

  #include "yyutil/parser.h"
  #include "yyutil/grammar.h"
  #include "common/attrs.h"

  #include "event_parser/parser.h"
  #include "event_parser/glue.h"
  #include "zcat.h"
}

%code top {
  #include "event_parser/parser.h"
  #include <string.h>
  #include <stddef.h>

  extern int events_yylex(void *lval, void *loc, void *scanner);
  #define PARSER containerof(parser, events_parser, yyu)
  #define EVENTS_YYLTYPE yyu_location

  static void str_setup(events_parser * restrict parser, char *s, size_t sz, uint16_t *lenp)
  {
    parser->strp = s;
    parser->str_lenp = lenp;
    parser->str_sz = sz;
    parser->str_z = 0;
  }

  static void parser_event_reset(events_parser * restrict parser)
  {
    memset(&parser->_per_event_state, 0, sizeof(*parser) - offsetof(typeof(*parser), _per_event_state));
  }

  #define STRSETUP(x) str_setup(PARSER, PARSER-> x, sizeof(PARSER-> x), &PARSER-> x ## _len)
  #define EVENT_RESET parser_event_reset(PARSER)
}

%define api.pure
%define parse.error verbose
%locations
%define api.prefix {events_yy}
%parse-param { void *scanner }
%parse-param { void *parser }
%lex-param { void *scanner }

/* zero based lines and columns */
%initial-action { yyu_loc_initialize(parser, &@$); (void)yynerrs; }

%union {
  yyu_lval yyu;
}

/* types */
%type <yyu.umax> uint
 /*
%type <yyu.t> string quoted-string words
 */

/* terminals */
%token             STR          "string"
%token             BECH32       "bech32"
%token <yyu.umax>  UINTMAX8
%token <yyu.umax>  UINTMAX16
%token <yyu.umax>  UINTMAX32
%token <yyu.umax>  UINTMAX64
%token <yyu.imax>  INTMAX8
%token <yyu.imax>  INTMAX16
%token <yyu.imax>  INTMAX32
%token <yyu.imax>  INTMAX64

/* words */
%token
 ID                             "id"
 PATH                           "path"
 RUNTIME                        "runtime"
 LEVEL                          "level"
 ATTACHMENT                     "attachment"
 TARGET                         "target"
 BINDING                        "binding"
 HIT                            "hit"
 MISS                           "miss"

/* events */
 RMA_START                      "rma-start"
 RMA_SEARCH                     "rma-search"
 RMA_GENERATE                   "rma-generate"
 RMA_END                        "rma-end"
 RMA_RESOLVE                    "rma-resolve"
 RMA_BIND                       "rma-bind"

%%
utterance
  : event
  ;

event
  : event-type ':' { EVENT_RESET; } '{' event-mappings '}'
  {
    PARSER->type_len = zcatw(PARSER->type, sizeof(PARSER->type), @1.s, @1.l);
  }
  ;

event-type
  : RMA_START
  | RMA_SEARCH
  | RMA_GENERATE
  | RMA_END
  | RMA_RESOLVE
  | RMA_BIND
  ;

event-mappings
  : event-mapping-list
  | %empty
  ;

event-mapping-list
  : event-mapping-list event-mapping
  | event-mapping
  ;

event-mapping
  : LEVEL ':' uint
  {
    PARSER->level = $3;
  }
  | ID ':' { STRSETUP(id); } bech32
  | PATH ':' { STRSETUP(path); } string
  | RUNTIME ':' { STRSETUP(runtime); } string
  | ATTACHMENT ':' '{' attachment-mapping-list '}'
  | TARGET ':' '{' target-mapping-list '}'
  | BINDING ':' '{' binding-mapping-list '}'
  | HIT ':' '{' hit-mapping-list '}'
  | MISS ':' '{' miss-mapping-list '}'
  ;

attachment-mapping-list
  : attachment-mapping-list attachment-mapping
  | attachment-mapping
  ;

attachment-mapping
  : ID ':' { STRSETUP(attachment.id); } bech32
  | PATH ':' { STRSETUP(attachment.path); } string
  ;

target-mapping-list
  : target-mapping-list target-mapping
  | target-mapping
  ;

target-mapping
  : ID ':' { STRSETUP(target.id); } bech32
  | PATH ':' { STRSETUP(target.path); } string
  ;

hit-mapping-list
  : hit-mapping-list hit-mapping
  | hit-mapping
  ;

hit-mapping
  : ID ':' { STRSETUP(hit.id); } bech32
  | PATH ':' { STRSETUP(hit.path); } string
  ;

miss-mapping-list
  : miss-mapping-list miss-mapping
  | miss-mapping
  ;

miss-mapping
  : ID ':' { STRSETUP(miss.id); } bech32
  | PATH ':' { STRSETUP(miss.path); } string
  ;

binding-mapping-list
  : binding-mapping-list binding-mapping
  | binding-mapping
  ;

binding-mapping
  : ID ':' { STRSETUP(binding.id); } bech32
  | PATH ':' { STRSETUP(binding.path); } string
  ;

bech32
  : BECH32
  {
    PARSER->str_z += zcatw(PARSER->strp + PARSER->str_z, PARSER->str_sz - PARSER->str_z, @1.s, @1.l);
    *PARSER->str_lenp = PARSER->str_z;
  }
  ;

string
  : literal
  {
    *PARSER->str_lenp = PARSER->str_z;
  }
  ;

literal
  : quoted-string
  | unquoted-string
  ;

quoted-string
  : '"' words '"'
  ;

unquoted-string
  : words
  ;

words
  : words word
  {
    PARSER->str_z += zcatw(PARSER->strp + PARSER->str_z, PARSER->str_sz - PARSER->str_z, @2.s, @2.l);
  }
  | word
  {
    PARSER->str_z += zcatw(PARSER->strp + PARSER->str_z, PARSER->str_sz - PARSER->str_z, @1.s, @1.l);
  }
  ;

word
  : STR
  ;

uint
  : INTMAX8   { $$ = $1; }
  | INTMAX16  { $$ = $1; }
  | INTMAX32  { $$ = $1; }
  | INTMAX64  { $$ = $1; }
  | UINTMAX8
  | UINTMAX16
  | UINTMAX32
  | UINTMAX64
  ;
