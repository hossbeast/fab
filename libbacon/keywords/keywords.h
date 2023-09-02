/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/hashtable.h"

extern hashtable keywords;

typedef struct keyword {
  const char *s;
  uint16_t len;
} keyword;

void keywords_setup(void);
void keywords_cleanup(void);
