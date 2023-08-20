/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

extern struct
{
  bool fork;
  bool await;
} xunit_args;

void args_parse(int argc, char ** argv);
