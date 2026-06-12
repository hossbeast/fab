/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"


#include "formulas.h"

//
// formula
//

static void formulas_usage(command * restrict cmd)
{
  printf(
"bam formulas - list formulas\n"
"\n"
"usage: bam formulas\n"
  );
}

static void formula_usage(command * restrict cmd)
{
  printf(
"bam formula - show formula metadata\n"
"\n"
"usage: bam formula (<id>|<path>)\n"
" <id>                         formula id\n"
" <path>                       path to formula file\n"
  );
}

//
// formulas
//

typeof(formula_command) formula_command = {
    name : "formula"
  , usage : formula_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//bm/formulas"
  , visit : "formula"
};

typeof(formulas_command) formulas_command = {
    name : "formulas"
  , usage : formulas_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/formulas"
  , visit : "formula"
};
