/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

enum value_type;

typedef enum {
  INPUT_FILE = 1,
  INPUT_TEXT
} input_type;

extern struct g_args_t {
  struct {
    input_type type;
    enum value_type container;
    char * s;
  } * inputs;
  uint16_t inputsl;
} g_args;

void args_parse(int argc, char ** argv);
void args_teardown(void);
