/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "rulesets.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

//
// ruleset
//

static void ruleset_usage(command * restrict cmd)
{
  printf(
"ruleset options (none)\n"
  );
}

//
// rulesets
//

static void rulesets_usage(command * restrict cmd)
{
  printf(
"\n"
"rulesets options (none)\n"
  );
}

typeof(ruleset_command) ruleset_command = {
    name : "ruleset"
  , usage : ruleset_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//rulesets"
  , visit : "ruleset"
};

typeof(rulesets_command) rulesets_command = {
    name : "rulesets"
  , usage : rulesets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//rulesets"
  , visit : "ruleset"
};
