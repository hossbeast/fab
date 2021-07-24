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

#include "valyria/load.h"
#include "moria/load.h"
#include "xlinux/xstdlib.h"
#include "value/load.h"

#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/set.h"
#include "value.h"
#include "value/parser.h"
#include "moria/graph.h"
#include "moria/operations.h"

#include "xunit.h"
#include "xunit/assert.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "rule.h"
#include "fsent.h"
#include "pattern.h"
#include "pattern_parser.h"
#include "module.internal.h"
#include "var.h"
#include "variant.h"

typedef struct var_test {
  XUNITTEST;

  // inputs
  char * text;            // value text

  struct variants {
    const char *name;
    char ** values;
  } **variants;
} var_test;

static void var_test_unit_setup(xunit_unit * unit)
{
  value_load();
  logging_finalize();

  variant_setup();
}

static void var_test_unit_cleanup(xunit_unit * unit)
{
  value_unload();
  variant_cleanup();
}

//
// tests
//

static void var_test_entry(xunit_test * _test)
{
  var_test * test = containerof(_test, var_test, xu);
  value * vars;
  value_parser * parser = 0;
  variant * var = 0;
  value * bag = 0;
  int x;
  value * mapping;

  // arrange
  value_parser_create(&parser);
  value_parser_parse(parser, test->text, strlen(test->text), "-test-", VALUE_TYPE_SET, &vars);

  typeof(*test->variants) *variants = test->variants;
  while(*variants)
  {
    var = 0;
    if((*variants)->name)
      variant_get((*variants)->name, strlen((*variants)->name), &var);

    // act
    var_denormalize(parser, var, vars, &bag);

    assert_eq_d(VALUE_TYPE_SET, bag->type);

    // assert
    for(x = 0; x < bag->els->table_size; x++)
    {
      if(!(mapping = set_table_get(bag->els, x)))
        continue;
    }

    variants++;
  }

finally:
  value_parser_xfree(parser);
coda;
}

//
// manifest
//

xunit_unit xunit = {
    xu_setup : var_test_unit_setup
  , xu_cleanup : var_test_unit_cleanup
  , xu_entry : var_test_entry
  , xu_tests : (var_test*[]) {
     /* */
      (var_test[]) {{
          text : " cc : [ a ]"
                 " cc.eapi : [ b ]"
                 " cc.eapi.final : [ c ]"
        , variants : (struct variants *[]){
              (struct variants[]) {{
                  name : "eapi"
                , values : (char*[]) {
                      "cc=[ a b ]"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  name : "eapi.final"
                , values : (char*[]) {
                      "cc=[ a b c ]"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  values : (char*[]) {
                      "cc=[ a ]"
                    , NULL
                  }
              }}
            , 0
          }
      }}
    , (var_test[]) {{
          text : " cc : a"
                 " cc.eapi : b"
                 " cc.eapi.final : c"
        , variants : (struct variants *[]){
              (struct variants[]) {{
                  name : "eapi"
                , values : (char*[]) {
                      "cc=a b"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  name : "eapi.final"
                , values : (char*[]) {
                      "cc=a b c"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  values : (char*[]) {
                      "cc=a"
                    , NULL
                  }
              }}
            , 0
          }
      }}
    , (var_test[]) {{
          text : " cc : { a b }"
                 " cc.eapi : { c d }"
                 " cc.eapi.final : { e f }"
        , variants : (struct variants *[]){
              (struct variants[]) {{
                  name : "eapi"
                , values : (char*[]) {
                      "cc=a b c d"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  name : "eapi.final"
                , values : (char*[]) {
                      "cc=a b c d e f"
                    , NULL
                  }
              }}
            , (struct variants[]) {{
                  values : (char*[]) {
                      "cc=a b"
                    , NULL
                  }
              }}
            , 0
          }
      }}
    , 0
  }
};
