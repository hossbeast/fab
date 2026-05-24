/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "xunit.h"

extern xunit_unit xunit_units;

#define units_foreach(item) \
  for(item = xunit_units.next; item != &xunit_units; item = item->next)
