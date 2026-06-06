/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "formulas.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

//
// formula
//

static void formula_usage(command * restrict cmd)
{
  printf(
"formula options (none)\n"
  );
}

//
// formulas
//

static void formulas_usage(command * restrict cmd)
{
  printf(
"formulas options (none)\n"
  );
}

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
