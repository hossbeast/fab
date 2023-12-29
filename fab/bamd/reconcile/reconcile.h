/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/* autoinc each time reconcile is run */
extern uint16_t reconciliation_epoch;

/*
 * run global reconciliation
 */
void reconcile(void);
