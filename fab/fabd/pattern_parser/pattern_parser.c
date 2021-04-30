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

#include "xapi.h"
#include "xapi/exit.h"

#include "common/attrs.h"
#include "narrator.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct pattern_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other - only include one set */
#define SEARCH_PATTERN_YYTOKENTYPE
#define REFERENCE_PATTERN_YYTOKENTYPE
#define LOOKUP_PATTERN_YYTOKENTYPE
#define INCLUDE_PATTERN_YYTOKENTYPE
#define MATCH_PATTERN_YYTOKENTYPE

#include "pattern_parser.internal.h"
#include "PATTERN.errtab.h"
#include "pattern.lex.h"
#include "pattern.states.h"
#include "search_pattern.tab.h"
#include "search_pattern.tokens.h"
#include "generate_pattern.tab.h"
#include "generate_pattern.tokens.h"
#include "reference_pattern.tab.h"
#include "reference_pattern.tokens.h"
#include "lookup_pattern.tab.h"
#include "lookup_pattern.tokens.h"
#include "include_pattern.tab.h"
#include "include_pattern.tokens.h"
#include "match_pattern.tab.h"
#include "match_pattern.tokens.h"

#include "logging.h"

static YYU_VTABLE(search_vtable, pattern, search_pattern);
static YYU_VTABLE(generate_vtable, pattern, generate_pattern);
static YYU_VTABLE(reference_vtable, pattern, reference_pattern);
static YYU_VTABLE(lookup_vtable, pattern, lookup_pattern);
static YYU_VTABLE(include_vtable, pattern, include_pattern);
static YYU_VTABLE(match_vtable, pattern, match_pattern);

//
// public
//

xapi pattern_parser_create(pattern_parser ** rv)
{
  enter;

  pattern_parser * p = 0;

  fatal(xmalloc, &p, sizeof(*p));

  // search
  fatal(yyu_parser_init, &p->search_yyu, &search_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->search_yyu, search_pattern_token_table, search_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->search_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // generate
  fatal(yyu_parser_init, &p->generate_yyu, &generate_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->generate_yyu, generate_pattern_token_table, generate_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->generate_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // reference (formula)
  fatal(yyu_parser_init, &p->reference_yyu, &reference_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->reference_yyu, reference_pattern_token_table, reference_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->reference_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // lookup
  fatal(yyu_parser_init, &p->lookup_yyu, &lookup_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->lookup_yyu, lookup_pattern_token_table, lookup_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->lookup_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // include
  fatal(yyu_parser_init, &p->include_yyu, &include_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->include_yyu, include_pattern_token_table, include_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->include_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

  // match
  fatal(yyu_parser_init, &p->match_yyu, &match_vtable, PATTERN_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->match_yyu, match_pattern_token_table, match_pattern_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->match_yyu
    , pattern_numstates
    , pattern_statenumbers
    , pattern_statenames
  );

#if DEBUG || DEVEL || XUNIT
  p->search_yyu.logs = L_PATTERN;
  p->generate_yyu.logs = L_PATTERN;
  p->reference_yyu.logs = L_PATTERN;
  p->lookup_yyu.logs = L_PATTERN;
  p->include_yyu.logs = L_PATTERN;
  p->match_yyu.logs = L_PATTERN;
#endif

  *rv = p;
  p = 0;

finally:
  fatal(pattern_parser_xfree, p);
coda;
}

xapi pattern_parser_xfree(pattern_parser* const p)
{
  enter;

  if(p)
  {
    fatal(yyu_parser_xdestroy, &p->search_yyu);
    fatal(yyu_parser_xdestroy, &p->generate_yyu);
    fatal(yyu_parser_xdestroy, &p->reference_yyu);
    fatal(yyu_parser_xdestroy, &p->lookup_yyu);
    fatal(yyu_parser_xdestroy, &p->include_yyu);
    fatal(yyu_parser_xdestroy, &p->match_yyu);
  }

  wfree(p);

  finally : coda;
}

xapi pattern_parser_ixfree(pattern_parser ** const p)
{
  enter;

  fatal(pattern_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi search_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  parser->group_counter = 0;
  fatal(yyu_parse, &parser->search_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}

xapi generate_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->generate_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}

xapi reference_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->reference_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}

xapi lookup_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->lookup_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}

xapi include_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->include_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}

xapi match_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , pattern ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->match_yyu, buf, size, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  if(rv)
  {
    if(fname) {
      fatal(ixstrdup, &parser->pattern->fname, fname);
    }
    *rv = parser->pattern;
    parser->pattern = 0;
  }

finally:
  pattern_free(parser->pattern);
coda;
}
