/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "events.tab.h"

static int events_yyerror(yyu_location* loc, void *scanner, yyu_parser *pp, char const *err)
  __attribute__((weakref("yyu_grammar_error")));
