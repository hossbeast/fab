/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

enum bacon_type;

enum {
  IN_FILE = 1,
  IN_TEXT = 2,
  OP_MERGE = 4,
  OP_EVAL = 8,
};

extern struct g_args_t {
  struct {
    int input;
    int oper;
    enum bacon_type container;
    char * s;
  } * inputs;
  uint16_t inputsl;
} g_args;

void args_parse(int argc, char ** argv);
void args_teardown(void);
