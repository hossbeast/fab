/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_rule_info;

/* results from rules-read */
typedef struct bam_rule_info {
  const char *rule_id;
  uint16_t rule_id_len;

  const char *rule_text;
  uint16_t rule_text_len;

  /* file where the rule is defined */
  const char *file_abspath;
  uint16_t file_abspath_len;

  uint16_t f_lin;  // line where the location begins
  uint16_t f_col;  // column where the location begins
  uint16_t l_lin;  // line where the location ends
  uint16_t l_col;  // column where the location ends
} bam_rule_info;
