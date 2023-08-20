/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/time.h>
#include <sys/resource.h>

struct rlimit;

/// xsetrlimit
//
// SUMMARY
//  xapi proxy for setrlimit
//
void xsetrlimit(int resource, const struct rlimit * restrict rlim)
  __attribute__((nonnull));
