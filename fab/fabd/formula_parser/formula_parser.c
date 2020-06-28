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

#include "xlinux/xstdlib.h"
#include "xlinux/KERNEL.errtab.h"

/* flex and bison do not agree on these names */
#define YYSTYPE FORMULA_YYSTYPE
#define YYLTYPE yyu_location

#include "formula_parser.internal.h"
#include "logging.h"
#include "FORMULA.errtab.h"
#include "formula.tab.h"
#include "formula.tokens.h"
#include "formula.lex.h"
#include "formula.states.h"
#include "selector_parser.internal.h"
#include "node.h"

/* marks the start of the embedded bacon block */
static const char * marker = "__DATA__";
static size_t marker_len = 8;

static YYU_VTABLE(vtable, formula, formula);

//
// internal
//

void formula_parser_extract(const char * restrict text, size_t text_len, bool * restrict shebang, char ** restrict baconp, size_t * restrict bacon_lenp)
{
  const char * bacon = 0;
  size_t bacon_len = 0;
  uint16_t shebang_len = 0;
  const char * s; // start of line
  const char * e; // end of line
  size_t l;
  int x;
  int i;
  bool first_line = true;

  // seek for a line ending with the marker, optionally followed by quotes/double quotes/semicolons
  s = e = text;
  for(i = 0; i < text_len; i++, e++)
  {
    /* check if the first line is a valid shebang */
    if(i == 0 && shebang_len == 0 && text[i] == '#')
      shebang_len++;
    else if(i == 1 && shebang_len == 1 && text[i] == '!')
      shebang_len++;
    else if(first_line && (
            (text[i] >= '0' && text[i] <= '9') ||
            (text[i] >= 'a' && text[i] <= 'z') ||
            (text[i] >= 'A' && text[i] <= 'Z') ||
            text[i] == '-' ||
            text[i] == '_' ||
            text[i] == '/'))
    {
      shebang_len++;
    }

    if(*e == '\n')
    {
      if(first_line && (shebang_len + 1) == i)
      {
        *shebang = true;
      }
      else if(first_line)
      {
        /* no shebang - start at the beginning */
        *shebang = false;
        bacon = text;
      }

      first_line = false;
      if(!s)
        continue;

      if(((l = e - s) < marker_len))
        continue;

      // tail of quotes and semicolons
      for(x = l - 1; x > marker_len; x--)
      {
        if(s[x] != '"' && s[x] != '\'' && s[x] != ';')
          break;
      }

      x -= (marker_len - 1);
      if(memcmp(s + x, marker, marker_len) == 0)
      {
        bacon = e + 1;
        break;
      }

      s = 0;
    }
    else if(!s)
    {
      s = e;
    }
  }

  if(!bacon)
    return;

  s = 0;
  for(; (e - text) < text_len; e++)
  {
    if(*e == '\n')
    {
      l = e - s;
      if(l == marker_len)
      {
        if(memcmp(s, marker, marker_len) == 0)
        {
          bacon_len = s - bacon;
          break;
        }
      }

      s = 0;
    }
    else if(!s)
    {
      s = e;
    }
  }

  if(!bacon_len)
  {
    bacon_len = e - bacon;
  }

  *baconp = (char*)bacon;
  *bacon_lenp = bacon_len;
}

xapi formula_parser_bacon_parse(
    formula_parser * restrict parser
  , char * const restrict vars_text
  , size_t vars_text_len
  , const char * restrict fname
  , formula_value ** restrict file
  , formula_value ** restrict args
  , formula_value ** restrict envs
)
{
  enter;

  parser->file = 0;
  parser->args = 0;
  parser->envs = 0;

  fatal(yyu_parse, &parser->yyu, vars_text, vars_text_len, fname, YYU_INPLACE, 0, 0);

  *file = parser->file;
  *args = parser->args;
  *envs = parser->envs;

  finally : coda;
}

//
// public
//

xapi formula_parser_create(formula_parser ** const rv)
{
  enter;

  formula_parser * p = 0;

  fatal(xmalloc, &p, sizeof(*p));
  fatal(yyu_parser_init, &p->yyu, &vtable, FORMULA_SYNTAX);

  fatal(yyu_parser_init_tokens
    , &p->yyu
    , formula_numtokens
    , formula_mintoken
    , formula_maxtoken
    , formula_tokenindexes
    , formula_tokennumbers
    , formula_tokennames
    , formula_tokenstrings
    , formula_tokenstring_tokens
  );

  fatal(yyu_parser_init_states
    , &p->yyu
    , formula_numstates
    , formula_statenumbers
    , formula_statenames
  );

#if DEBUG || DEVEL || XUNIT
  p->yyu.logs = L_FORMULA;
#endif

  fatal(yyu_define_tokenrange, &p->yyu, formula_ARGS, formula_SUFFIX);

  fatal(selector_parser_create, &p->selector_parser);

  *rv = p;
  p = 0;

finally:
  fatal(formula_parser_xfree, p);
coda;
}

xapi formula_parser_xfree(formula_parser* const p)
{
  enter;

  if(p)
  {
    fatal(selector_parser_xfree, p->selector_parser);
    fatal(yyu_parser_xdestroy, &p->yyu);
  }

  wfree(p);

  finally : coda;
}

xapi formula_parser_ixfree(formula_parser ** const p)
{
  enter;

  fatal(formula_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi formula_parser_parse(
    formula_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , formula * restrict fml
)
{
  enter;

  char * bacon = 0;
  size_t bacon_len = 0;
  bool shebang;
  formula_parser_extract(text, len, &shebang, &bacon, &bacon_len);

  if(bacon && bacon_len)
  {
    fatal(formula_parser_bacon_parse, parser, bacon, bacon_len + 2, fname, &fml->file, &fml->args, &fml->envs);
  }

  if(!shebang && !fml->file)
  {
    fail(FORMULA_NOPATH);
  }

finally:
  xapi_infos("path", fname);
coda;
}
