/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "varsets.h"

//
// varset
//

static void varset_usage(command * restrict cmd)
{
  printf(
"varset options (none)\n"
  );
}

//
// varsets
//

static void varsets_usage(command * restrict cmd)
{
  printf(
"\n"
"varsets options (none)\n"
  );
}

typeof(varset_command) varset_command = {
    name : "varset"
  , usage : varset_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//varsets"
  , visit : "varset"
};

typeof(varsets_command) varsets_command = {
    name : "varsets"
  , usage : varsets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//varsets"
  , visit : "varset"
};
