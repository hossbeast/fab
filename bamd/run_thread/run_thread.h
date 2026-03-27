/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

There is up to one run thread

*/

#include "request.h"
#include "command.h"

struct channel_thread;
struct request_handler;

extern bool run_thread_autorun;

/*
 * launch the run thread
 *
 * [handler] - handler thread to notify if any
 *
 * returns - true if the runlock was acquired and the run-thread was created
 */
bool run_thread_launch(struct request_handler * restrict handler, struct channel_thread * restrict owner, command_type cmd);

void run_thread_autorun_set(bool autorun);

void autorun_set_event(struct request_handler * restrict handler);
