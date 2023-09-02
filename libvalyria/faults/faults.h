/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

SUMMARY
 runtime fault injection

*/

#include "types.h"

#define VALYRIA_FAULT_TABLE          \
  VALYRIA_FAULT(KEY_INDEX_BOUNDARY)  \
  VALYRIA_FAULT(KEY_HASH)            \
  VALYRIA_FAULT(HASH_INDEX)          \

typedef enum {
  VALYRIA_FAULT_RANGE_BEFORE = 0,

#define VALYRIA_FAULT(x) VALYRIA_ ## x,
VALYRIA_FAULT_TABLE
#undef VALYRIA_FAULT

  VALYRIA_FAULT_RANGE_AFTER
} valyria_fault;

void valyria_fault_activate(valyria_fault fault, ...);
void valyria_faults_reset(void);
