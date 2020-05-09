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
#include "xunit.h"
#include "xunit/assert.h"

#include "formula_parser.internal.h"

#include "macros.h"

typedef struct formula_parser_extract_test
{
  XUNITTEST;

  char * formula;     // formula text

  /* expected */
  bool shebang;
  char * bacon;       // embedded bacon text
} formula_parser_extract_test;

static void trim(char ** s, size_t * l)
{
  while(**s == ' ' || **s == '\n')
  {
    (*s)++;
    (*l)--;
  }

  while(*l && ((*s)[(*l) - 1] == ' ' || (*s)[(*l) - 1] == '\n'))
  {
    (*l)--;
  }
}

//
// tests
//

static xapi formula_parser_extract_test_entry(xunit_test * _test)
{
  enter;

  char * bacon = 0;
  size_t bacon_len = 0;
  char * exp;
  size_t exp_len;
  bool shebang = false;

  formula_parser_extract_test * test = (formula_parser_extract_test *)_test;

  formula_parser_extract(test->formula, strlen(test->formula), &shebang, &bacon, &bacon_len);
  if(bacon)
    trim(&bacon, &bacon_len);

  exp = test->bacon;
  exp_len = strlen(test->bacon);
  trim(&exp, &exp_len);

  assert_eq_w(test->bacon, strlen(test->bacon), bacon, bacon_len);
  assert_eq_b(test->shebang, shebang);

  finally : coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_entry : formula_parser_extract_test_entry
  , xu_tests : (formula_parser_extract_test*[]) {
      /* shebang */
      (formula_parser_extract_test[]) {{
          formula : ""
"#!/usr/bin/env bash\n"
"\n"
"cat >/dev/null <<'__DATA__'\n"
"\n"
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")\n"
"\n"
"__DATA__\n"
        , shebang : true
        , bacon : ""
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")"
      }}
    , (formula_parser_extract_test[]) {{
          formula : ""
"#!/usr/bin/env perl\n"
"\n"
"use strict;\n"
"use warnings;\n"
"\n"
"my $x = <<'__DATA__';\n"
"\n"
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")\n"
"\n"
"__DATA__\n"
        , shebang : true
        , bacon : ""
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")"
      }}
    , (formula_parser_extract_test[]) {{
          formula : ""
"#!/usr/bin/env perl\n"
"\n"
"use strict;\n"
"use warnings;\n"
"\n"
"__DATA__\n"
"\n"
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")\n"
        , shebang : true
        , bacon : ""
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")"
      }}

      /* no shebang */
    , (formula_parser_extract_test[]) {{
          formula : ""
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")\n"
        , shebang : false
        , bacon : ""
"env (\n"
"  deps {\n"
"    type traversal\n"
"  }\n"
")"
      }}


// degenerate cases
    , (formula_parser_extract_test[]) {{
          formula : ""
        , bacon : ""
      }}
    , (formula_parser_extract_test[]) {{
          formula : "__DATA__"
        , bacon : ""
      }}
    , (formula_parser_extract_test[]) {{
          formula : "__DATA__ "
        , bacon : ""
      }}
    , (formula_parser_extract_test[]) {{
          formula : " __DATA__"
        , bacon : ""
      }}
    , (formula_parser_extract_test[]) {{
          formula : "__DATA__\n"
        , bacon : ""
      }}
    , 0
  }
};
