/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct config;

/* time at which the timeout thread will trip */
extern struct timespec timeout_thread_deadline;

void timeout_thread_launch(void);
void timeout_thread_reconfigure(struct config * restrict cfg, bool dryrun);
