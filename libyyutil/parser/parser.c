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
#include "types.h"

#include "xapi/exit.h"
#include "xlinux/xstdlib.h"
#include "xlinux/KERNEL.errtab.h"

#include "parser.internal.h"
#include "scanner.internal.h"
#include "logging.internal.h"

#include "common/attrs.h"
#include "common/hash.h"
#include "common/stresc.h"
#include "macros.h"
#include "zbuffer.h"

//
// static
//

static xapi reduce(yyu_parser * parser, uint16_t attrs)
{
  enter;

  int r;

  // error from the parser means failure to reduce
  if((r = parser->vtable->yyparse(parser->scanner, parser)) || parser->scanerr || parser->gramerr)
  {
    if(r == 2)
    {
      // memory exhaustion error from the parser
      fail(KERNEL_ENOMEM);
    }
    else if(XAPI_UNWINDING)
    {
      // propagate fail from within a lexer or parser rule
      fail(0);
    }
    else if(parser->gramerr)
    {
      // failure to reduce from the parser
      fails(parser->error_syntax, "message", parser->error_str);
    }
  }

finally :
  if(XAPI_UNWINDING)
  {
    if(parser->scanerr || parser->gramerr)
    {
      xapi_infof("location", "[%d,%d - %d,%d]"
        , parser->error_loc.f_lin
        , parser->error_loc.f_col
        , parser->error_loc.l_lin
        , parser->error_loc.l_col
      );

      if(parser->gramerr)
      {
        xapi_infos("token", parser->tokenstring);
      }
    }
  }
coda;
}

//
// api
//

xapi API yyu_parser_xdestroy(yyu_parser * const parser)
{
  enter;

  wfree(parser->last_lval);
  parser->vtable->yylex_destroy(parser->scanner);
  wfree(parser->token_table_bytoken);

  finally : coda;
}

xapi API yyu_parser_init(
    yyu_parser * const restrict parser
  , const yyu_vtable * const restrict vtable
  , xapi error_syntax
)
{
  enter;

  parser->vtable = vtable;
  parser->error_syntax = error_syntax;

  fatalize(KERNEL_ENOMEM, parser->vtable->yylex_init, &parser->scanner);

  finally : coda;
}

xapi API yyu_parser_init_tokens(yyu_parser * const restrict parser, const yyu_token * restrict token_table, uint16_t token_table_size)
{
  enter;

  int x;

  /* token_table is pre-sorted by token->string */
  parser->token_table = token_table;
  parser->token_table_size = token_table_size;

#if DEBUG || DEVEL || XUNIT
  logf(parser->logs | L_YYUTIL | L_PARSER, "numtokens %d", parser->token_table_size);
  for(x = 0; x < token_table_size; x++)
  {
    logf(parser->logs | L_YYUTIL | L_PARSER, "%2d %3d %-10s %s"
      , x
      , parser->token_table[x].number
      , parser->token_table[x].name
      , parser->token_table[x].string
    );
  }
#endif

  // set of indexes sorted by token
  fatal(xmalloc, &parser->token_table_bytoken, sizeof(*parser->token_table_bytoken) * parser->token_table_size);
  for(x = 0; x < token_table_size; x++)
  {
    parser->token_table_bytoken[x] = &parser->token_table[x];
  }
  qsort(parser->token_table_bytoken, token_table_size, sizeof(*parser->token_table_bytoken), token_table_bytoken_cmp_items);

  // find particular tokens
  memset(&parser->tokens, 0xff, sizeof(parser->tokens));
  for(x = 0; x < token_table_size; x++)
  {
    if(strcmp(parser->token_table[x].name, "STR") == 0)
      parser->tokens.STR = x;
    else if(strcmp(parser->token_table[x].name, "CREF") == 0)
      parser->tokens.CREF = x;
    else if(strcmp(parser->token_table[x].name, "HREF") == 0)
      parser->tokens.HREF = x;
    else if(strcmp(parser->token_table[x].name, "BOOL") == 0)
      parser->tokens.BOOL = x;
    else if(strcmp(parser->token_table[x].name, "FLOAT") == 0)
      parser->tokens.FLOAT = x;
    else if(strcmp(parser->token_table[x].name, "HEX8") == 0)
      parser->tokens.HEX8 = x;
    else if(strcmp(parser->token_table[x].name, "HEX16") == 0)
      parser->tokens.HEX16 = x;
    else if(strcmp(parser->token_table[x].name, "HEX32") == 0)
      parser->tokens.HEX32 = x;
    else if(strcmp(parser->token_table[x].name, "HEX64") == 0)
      parser->tokens.HEX64 = x;
    else if(strcmp(parser->token_table[x].name, "UINTMAX8") == 0)
      parser->tokens.UINTMAX8 = x;
    else if(strcmp(parser->token_table[x].name, "UINTMAX16") == 0)
      parser->tokens.UINTMAX16 = x;
    else if(strcmp(parser->token_table[x].name, "UINTMAX32") == 0)
      parser->tokens.UINTMAX32 = x;
    else if(strcmp(parser->token_table[x].name, "UINTMAX64") == 0)
      parser->tokens.UINTMAX64 = x;
    else if(strcmp(parser->token_table[x].name, "INTMIN8") == 0)
      parser->tokens.INTMIN8 = x;
    else if(strcmp(parser->token_table[x].name, "INTMIN16") == 0)
      parser->tokens.INTMIN16 = x;
    else if(strcmp(parser->token_table[x].name, "INTMIN32") == 0)
      parser->tokens.INTMIN32 = x;
    else if(strcmp(parser->token_table[x].name, "INTMIN64") == 0)
      parser->tokens.INTMIN64 = x;
    else if(strcmp(parser->token_table[x].name, "INTMAX8") == 0)
      parser->tokens.INTMAX8 = x;
    else if(strcmp(parser->token_table[x].name, "INTMAX16") == 0)
      parser->tokens.INTMAX16 = x;
    else if(strcmp(parser->token_table[x].name, "INTMAX32") == 0)
      parser->tokens.INTMAX32 = x;
    else if(strcmp(parser->token_table[x].name, "INTMAX64") == 0)
      parser->tokens.INTMAX64 = x;
  }

  finally : coda;
}

xapi API yyu_parser_init_states(
    yyu_parser * const restrict parser
  , int numstates
  , const int * restrict statenumbers
  , const char ** restrict statenames
)
{
  enter;

  parser->numstates = numstates;
  parser->statenumbers = statenumbers;
  parser->statenames = statenames;

  finally : coda;
}

xapi API yyu_parse(
    yyu_parser * const restrict parser
  , char * const restrict text
  , size_t text_len
  , const char * const restrict fname
  , uint16_t attrs
  , yyu_location * restrict init_loc
  , yyu_location * restrict final_loc
)
{
  enter;

  void * state = 0;
  yyu_location lfinal_loc;
  char hold_char;
  size_t plen;

  // reset state for this parse
  memset(&parser->scanerr, 0, offsetof(typeof(*parser), end_of_per_parse_state));

  // create state specific to this parse
  if(attrs & YYU_INPLACE)
  {
    if((state = parser->vtable->yy_scan_buffer(text, text_len, parser->scanner)) == 0)
      fail(KERNEL_ENOMEM);
  }
  else
  {
    if((state = parser->vtable->yy_scan_bytes(text, text_len, parser->scanner)) == 0)
      fail(KERNEL_ENOMEM);
  }

  parser->usrbuf = text;
  parser->usrbufsize = text_len;
  parser->fname = fname;
  parser->attrs = attrs;
  parser->final_loc = final_loc ?: &lfinal_loc;

  if(init_loc)
    parser->loc = *init_loc;

  // make parser available to the lexer
  parser->vtable->yyset_extra(parser, parser->scanner);

  // invoke the appropriate parser, raise errors as needed
  fatal(reduce, parser, attrs);

  if(attrs & YYU_INPLACE)
  {
    // restore the buffer by replacing the holdchar
    if(parser->yybuffer && parser->loc.e)
    {
      hold_char = parser->vtable->yyget_hold_char(parser->scanner);
      plen = parser->loc.e - parser->yybuffer;
      text[plen] = hold_char;
    }
  }

  if((attrs & YYU_INPLACE) && (attrs & YYU_PARTIAL) == 0)
  {
    /* skip trailing whitespace */
    plen = parser->loc.e - parser->yybuffer;
    while(plen < (text_len - 2)) {
      if(text[plen] != ' ' && text[plen] != '\n' && text[plen] != '\r' && text[plen] != '\t' && text[plen] != 0) {
        break;
      }
      plen++;
    }

    /* for non-partial, assert the entire buffer was consumed */
    if((plen + 2) < text_len)
    {
      fail(KERNEL_EINVAL);
    }
  }

finally:
  parser->vtable->yy_delete_buffer(state, parser->scanner);
coda;
}
