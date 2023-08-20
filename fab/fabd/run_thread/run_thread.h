/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is up to one run thread

*/

#include "request.internal.h"

struct handler_context;

extern bool run_thread_autorun;

/*
 * launch the run thread
 *
 * [handler] - handler thread to notify if any
 */
void run_thread_launch(struct handler_context * restrict owner, command_type cmd);
