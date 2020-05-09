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
#include "valyria/hashtable.h"

#include "internal.h"
#include "parser.internal.h"
#include "scanner.internal.h"
#include "logging.internal.h"

#include "attrs.h"
#include "hash.h"
#include "macros.h"
#include "stresc.h"
#include "zbuffer.h"

//
// static
//

static uint32_t str_token_hash(uint32_t h, const void * _ent, size_t entsz)
{
  const str_token_entry * ent = _ent;
  return hash32(h, ent->name, ent->namel);
}

static int str_token_cmp(const void * _A, size_t Asz, const void * _B, size_t Bsz)
{
  const str_token_entry * A = _A;
  const str_token_entry * B = _B;
  return memncmp(A->name, A->namel, B->name, B->namel);
}

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

API xapi yyu_parser_xdestroy(yyu_parser * const parser)
{
  enter;

  wfree(parser->last_lval);
  fatal(hashtable_xfree, parser->str_token_table);
  parser->vtable->yylex_destroy(parser->scanner);

  finally : coda;
}

API xapi yyu_parser_init(
    yyu_parser * const restrict parser
  , const yyu_vtable * const restrict vtable
  , xapi error_syntax
)
{
  enter;

  parser->vtable = vtable;
  parser->error_syntax = error_syntax;

  fatal(hashtable_createx, &parser->str_token_table, sizeof(str_token_entry), 16, str_token_hash, str_token_cmp, 0, 0);
  fatalize(KERNEL_ENOMEM, parser->vtable->yylex_init, &parser->scanner);

  finally : coda;
}

API xapi yyu_parser_init_tokens(
    yyu_parser * const restrict parser
  , uint16_t numtokens
  , uint16_t mintoken
  , uint16_t maxtoken
  , const uint16_t * tokenindexes
  , const uint16_t * tokennumbers
  , const char ** tokennames
  , const char ** tokenstrings
  , const uint16_t * tokenstring_tokens
)
{
  enter;

  int x;

  parser->numtokens = numtokens;
  parser->mintoken = mintoken;
  parser->maxtoken = maxtoken;
  parser->tokenindexes = tokenindexes;
  parser->tokennumbers = tokennumbers;
  parser->tokennames = tokennames;

  // find the tokenstring base
  for(x = 0 ;; x++)
  {
    if(tokenstrings[x] == 0)
      break;

    if(tokenstring_tokens[x] == mintoken)
    {
      parser->tokenstrings = &tokenstrings[x];
      break;
    }
  }

#if DEBUG || DEVEL || XUNIT

  logf(parser->logs | L_YYUTIL | L_PARSER, "numtokens %d", parser->numtokens);
  logf(parser->logs | L_YYUTIL | L_PARSER, "mintoken %d", parser->mintoken);
  logf(parser->logs | L_YYUTIL | L_PARSER, "maxtoken %d", parser->maxtoken);
  for(x = 0; x < numtokens; x++)
  {
    logf(parser->logs | L_YYUTIL | L_PARSER, "%2d %3d %-10s %s"
      , x
      , parser->tokennumbers[x]
      , parser->tokennames[x]
      , parser->tokenstrings[x]
    );
  }
#endif

  memset(&parser->tokens, 0xff, sizeof(parser->tokens));

  // find particular tokens
  for(x = 0; x < numtokens; x++)
  {
    if(strcmp(parser->tokennames[x], "STR") == 0)
      parser->tokens.STR = x;
    else if(strcmp(parser->tokennames[x], "CREF") == 0)
      parser->tokens.CREF = x;
    else if(strcmp(parser->tokennames[x], "HREF") == 0)
      parser->tokens.HREF = x;
    else if(strcmp(parser->tokennames[x], "BOOL") == 0)
      parser->tokens.BOOL = x;
    else if(strcmp(parser->tokennames[x], "FLOAT") == 0)
      parser->tokens.FLOAT = x;
    else if (strcmp(parser->tokennames[x], "HEX8") == 0)
      parser->tokens.HEX8 = x;
    else if (strcmp(parser->tokennames[x], "HEX16") == 0)
      parser->tokens.HEX16 = x;
    else if(strcmp(parser->tokennames[x], "HEX32") == 0)
      parser->tokens.HEX32 = x;
    else if(strcmp(parser->tokennames[x], "HEX64") == 0)
      parser->tokens.HEX64 = x;
    else if (strcmp(parser->tokennames[x], "UINTMAX8") == 0)
      parser->tokens.UINTMAX8 = x;
    else if (strcmp(parser->tokennames[x], "UINTMAX16") == 0)
      parser->tokens.UINTMAX16 = x;
    else if(strcmp(parser->tokennames[x], "UINTMAX32") == 0)
      parser->tokens.UINTMAX32 = x;
    else if(strcmp(parser->tokennames[x], "UINTMAX64") == 0)
      parser->tokens.UINTMAX64 = x;
    else if(strcmp(parser->tokennames[x], "INTMIN8") == 0)
      parser->tokens.INTMIN8 = x;
    else if(strcmp(parser->tokennames[x], "INTMIN16") == 0)
      parser->tokens.INTMIN16 = x;
    else if(strcmp(parser->tokennames[x], "INTMIN32") == 0)
      parser->tokens.INTMIN32 = x;
    else if(strcmp(parser->tokennames[x], "INTMIN64") == 0)
      parser->tokens.INTMIN64 = x;
    else if(strcmp(parser->tokennames[x], "INTMAX8") == 0)
      parser->tokens.INTMAX8 = x;
    else if(strcmp(parser->tokennames[x], "INTMAX16") == 0)
      parser->tokens.INTMAX16 = x;
    else if(strcmp(parser->tokennames[x], "INTMAX32") == 0)
      parser->tokens.INTMAX32 = x;
    else if(strcmp(parser->tokennames[x], "INTMAX64") == 0)
      parser->tokens.INTMAX64 = x;
  }

  finally : coda;
}

API xapi yyu_parser_init_states(
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

API xapi yyu_parse(
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
  memset(&parser->scanerr, 0, offsetof(typeof(*parser), str_token_table));

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

API xapi yyu_define_tokenrange(yyu_parser * restrict parser, int first, int last)
{
  enter;

  int x;
  bool in = false;

  for(x = 0; x < parser->numtokens; x++)
  {
    if(in || parser->tokennumbers[x] == first)
    {
      in = true;

      uint16_t token = parser->tokennumbers[x];
      const char * name = parser->tokenstrings[x];
      size_t namel = strlen(name);

      if(namel >= 2 && name[0] == '"' && name[namel - 1] == '"')
      {
        name++;
        namel -= 2;
      }

      str_token_entry entry = {
          token : token
        , lval : token
        , name : name
        , namel : namel
      };

      fatal(hashtable_put, parser->str_token_table, &entry);
    }

    if(parser->tokennumbers[x] == last)
      break;
  }

  finally : coda;
}
