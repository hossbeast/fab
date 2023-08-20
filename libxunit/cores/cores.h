/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/// print_core_backtrace
//
// best-effort print backtrace to stdout
//
void print_core_backtrace(pid_t pid);
