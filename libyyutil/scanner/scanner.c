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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "common/parseint.h"
#include "common/stresc.h"
#include "xlinux/xstdlib.h"

#include "scanner.internal.h"
#include "parser.internal.h"

#include "macros.h"
#include "probes.h"

static int token_table_cmp_key(const void * _key, const void * _token)
{
  const yyu_token *key = _key;
  const yyu_token *token = _token;

  return memncmp(key->string, key->string_len, token->string, token->string_len);
}

int token_table_bytoken_cmp_items(const void *_a, const void *_b)
{
  const yyu_token * const * a = _a;
  const yyu_token * const * b = _b;

  return INTCMP((*a)->token, (*b)->token);
}

static int token_table_bytoken_cmp_key(const void * _key, const void * _token)
{
  const int *key = _key;
  const yyu_token * const *token = _token;

  return INTCMP(*key, (*token)->token);
}

//
// internal
//

const char * scanner_tokenname(const yyu_parser * restrict parser, int token)
{
  yyu_token **tok;

  tok = bsearch(&token, parser->token_table_bytoken, parser->token_table_size, sizeof(*parser->token_table_bytoken), token_table_bytoken_cmp_key);

  if(!tok)
    return NULL;

  return (*tok)->name;
}

const char * scanner_statename(const yyu_parser * restrict parser, int state)
{
  if(state < 0 || state >= parser->numstates)
    return 0;

  return parser->statenames[state];
}

//
// api
//

void API yyu_locreset(yyu_location * const lloc, yyu_parser * const xtra, char * const text)
{
  if(!xtra->yybuffer)
    xtra->yybuffer = text;

  // assign location for current token
  lloc->f_col = xtra->loc.f_col;
  lloc->l_col = xtra->loc.l_col - 1;
  lloc->f_lin = xtra->loc.f_lin;
  lloc->l_lin = xtra->loc.l_lin;
  lloc->s = xtra->loc.s;
  lloc->e = xtra->loc.e;
  lloc->l = lloc->e - lloc->s;

  // reset running location track
  xtra->loc.f_lin = xtra->loc.l_lin + 1;
  xtra->loc.l_lin = xtra->loc.f_lin;
  xtra->loc.f_col = 0;
  xtra->loc.l_col = 0;
}

void API yyu_locwrite(
    yyu_location * const lloc
  , yyu_parser * const xtra
  , char * const text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
)
{
  if(!xtra->yybuffer)
    xtra->yybuffer = text;

  // update running location track
  xtra->loc.f_lin = xtra->loc.l_lin;
  xtra->loc.f_col = xtra->loc.l_col;
  xtra->loc.l_lin = end_line;
  xtra->loc.l_col = end_col;
  xtra->loc.s     = text;
  xtra->loc.e     = text + bytes;
  xtra->loc.l     = bytes;

  // assign location to the current token
  *lloc = xtra->loc;
}

void API yyu_pushstate(int next, yyu_parser * const parser)
{
  int prev;

  prev = yyu_nstate(parser, 0);
  parser->states[parser->states_n++] = next;

  if(yyu_pushstate_semaphore) {
    pushstate_probe(parser, prev, next);
  }
}

void API yyu_popstate(yyu_parser * const parser)
{
  int prev;
  int next;

  prev = yyu_nstate(parser, 0);
  parser->states_n--;
  next = yyu_nstate(parser, 0);

  if(yyu_popstate_semaphore) {
    popstate_probe(parser, prev, next);
  }
}

int API yyu_nstate(yyu_parser * const xtra, int n)
{
  return (xtra->states_n > n) ? xtra->states[xtra->states_n - n - 1] : -1;
}

void API yyu_errorf(yyu_parser * restrict parser, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  parser->error_len = vsnprintf(parser->error_str, sizeof(parser->error_str), fmt, va);
  va_end(va);
}

void API yyu_save_error_loc(yyu_parser * const parser, yyu_location * const lloc)
{
  parser->scanerr = 1;

  // save the location
  memcpy(&parser->error_loc, lloc, sizeof(parser->error_loc));
}

int API yyu_lexify(
    yyu_parser * const restrict xtra
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
{
  yyu_locwrite(lloc, xtra, text, end_line, end_col, bytes);

  if(yyu_token_semaphore) {
    token_probe(xtra, token, lval, lloc, text, bytes);
  }

  // store location for error reporting
  memcpy(&xtra->last_loc, lloc, sizeof(xtra->last_loc));
  xtra->last_token = token;

  // store lval for error reporting
  if(lvalsz >= xtra->last_lval_aloc)
  {
    xrealloc(&xtra->last_lval, lvalsz + 1, 1, xtra->last_lval_aloc);
    xtra->last_lval_aloc = lvalsz + 1;
  }

  memcpy(xtra->last_lval, lval, lvalsz);
  ((char*)xtra->last_lval)[lvalsz] = 0;

  return token;
}

int API yyu_lexify_valtoken(
    yyu_parser * const restrict xtra
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
{
  yyu_lval* lval = (typeof(lval))_lval;

  lval->u16 = token;

  return yyu_lexify(xtra, lval, lvalsz, lloc, text, end_line, end_col, bytes, token);
}

int API yyu_lexify_attrs(
    yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
{
  int token;

  yyu_token key = { string : text, string_len : leng };
  yyu_token *ent = bsearch(&key, parser->token_table, parser->token_table_size, sizeof(*parser->token_table), token_table_cmp_key);
  if(ent)
  {
    token = ent->token;
  }
  else
  {
    token = parser->token_table[parser->tokens.STR].token;
  }

  return yyu_lexify(parser, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_enum(
    yyu_parser * const restrict parser
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
{
  int token = 0;

  yyu_token key = { string : text, string_len : leng };
  yyu_token *ent = bsearch(&key, parser->token_table, parser->token_table_size, sizeof(*parser->token_table), token_table_cmp_key);
  if(ent)
  {
    token = ent->token;
  }
  else
  {
    token = parser->token_table[parser->tokens.YYUNDEF].token;
  }

  return yyu_lexify(parser, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_int(
    yyu_parser * const restrict parser
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , char * restrict fmt
)
{
  yyu_lval* lval = (typeof(lval))_lval;

  int token = 0;
  if(strchr(fmt, 'd') || strchr(fmt, 'i'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->imax);

    if(errno == ERANGE) {
      return -ERANGE;
    }

    if(lval->imax < INT32_MIN)
      token = parser->token_table[parser->tokens.INTMIN64].token;
    else if(lval->imax < INT16_MIN)
      token = parser->token_table[parser->tokens.INTMIN32].token;
    else if(lval->imax < INT8_MIN)
      token = parser->token_table[parser->tokens.INTMIN16].token;
    else if(lval->imax < 0)
      token = parser->token_table[parser->tokens.INTMIN8].token;
    else if(lval->imax <= INT8_MAX)
      token = parser->token_table[parser->tokens.INTMAX8].token;
    else if(lval->imax <= UINT8_MAX)
      token = parser->token_table[parser->tokens.UINTMAX8].token;
    else if(lval->imax <= INT16_MAX)
      token = parser->token_table[parser->tokens.INTMAX16].token;
    else if(lval->imax <= UINT16_MAX)
      token = parser->token_table[parser->tokens.UINTMAX16].token;
    else if(lval->imax <= INT32_MAX)
      token = parser->token_table[parser->tokens.INTMAX32].token;
    else if(lval->imax <= UINT32_MAX)
      token = parser->token_table[parser->tokens.UINTMAX32].token;
    else
      token = parser->token_table[parser->tokens.INTMAX64].token;
  }
  else if(strchr(fmt, 'u'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->umax);

    if(errno == ERANGE) {
      return -ERANGE;
    }

    if(lval->umax <= INT8_MAX)
      token = parser->token_table[parser->tokens.INTMAX8].token;
    else if(lval->umax <= UINT8_MAX)
      token = parser->token_table[parser->tokens.UINTMAX8].token;
    else if(lval->umax <= INT16_MAX)
      token = parser->token_table[parser->tokens.INTMAX16].token;
    else if(lval->umax <= UINT16_MAX)
      token = parser->token_table[parser->tokens.UINTMAX16].token;
    else if(lval->umax <= INT32_MAX)
      token = parser->token_table[parser->tokens.INTMAX32].token;
    else if(lval->umax <= UINT32_MAX)
      token = parser->token_table[parser->tokens.UINTMAX32].token;
    else if(lval->umax <= INT64_MAX)
      token = parser->token_table[parser->tokens.INTMAX64].token;
    else
      token = parser->token_table[parser->tokens.UINTMAX64].token;
  }
  else if(strchr(fmt, 'x'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->umax);

    if(errno == ERANGE) {
      return -ERANGE;
    }

    if(lval->umax <= UINT8_MAX)
      token = parser->token_table[parser->tokens.HEX8].token;
    else if(lval->umax <= UINT16_MAX)
      token = parser->token_table[parser->tokens.HEX16].token;
    else if(lval->umax <= UINT32_MAX)
      token = parser->token_table[parser->tokens.HEX32].token;
    else
      token = parser->token_table[parser->tokens.HEX64].token;
  }

  return yyu_lexify(parser, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_cref(
    yyu_parser * const restrict parser
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
{
  int token;
  yyu_lval* lval = (typeof(lval))_lval;

  if(text[off] == '0')
    lval->c = 0;
  else if(text[off] == 'a')
    lval->c = 0x7;
  else if(text[off] == 'b')
    lval->c = 0x8;
  else if(text[off] == 't')
    lval->c = 0x9;
  else if(text[off] == 'n')
    lval->c = 0xa;
  else if(text[off] == 'v')
    lval->c = 0xb;
  else if(text[off] == 'f')
    lval->c = 0xc;
  else if(text[off] == 'r')
    lval->c = 0xd;
  else if(text[off] == 's')
    lval->c = 0x20;
  else if(text[off] == '\\')
    lval->c = '\\';
  else if(text[off] == '"')
    lval->c = '"';

  token = parser->token_table[parser->tokens.CREF].token;
  return yyu_lexify(parser, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_href(
    yyu_parser * const restrict parser
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
{
  int token;
  yyu_lval* lval = (typeof(lval))_lval;

  parseuint(text + off, "hhx", 0, UINT8_MAX, 1, 2, &lval->u8, 0);

  token = parser->token_table[parser->tokens.HREF].token;
  return yyu_lexify(parser, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_bool(
    yyu_parser * const restrict parser
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
)
{
  yyu_lval* lval = (typeof(lval))_lval;
  int token;

  if(memncmp(text, leng, "true", 4) == 0)
    lval->b = true;
  else
    lval->b = false;

  token = parser->token_table[parser->tokens.BOOL].token;
  return yyu_lexify(parser, lval, sizeof(*lval), lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}

int API yyu_lexify_float(
    yyu_parser * const restrict parser
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
)
{
  yyu_lval* lval = (typeof(lval))_lval;
  int token;

  errno = 0;
  sscanf(text, "%lf", &lval->f);

  if(errno != 0) {
    RUNTIME_ASSERT(errno > 0);
    return -errno;
  }

  token = parser->token_table[parser->tokens.FLOAT].token;
  return yyu_lexify(parser, lval, sizeof(*lval), lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);
}
