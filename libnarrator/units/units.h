/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <time.h>

struct timespec;
struct narrator;

/// interval_say
//
// SUMMARY
//  describe an interval, specified in seconds, in terms of years / days / hours / minutes / seconds
//
// PARAMETERS
//  seconds - the interval, in seconds
//  N       - narrator to write to
//
void interval_say(time_t seconds, struct narrator * const restrict N)
  __attribute__((nonnull));

/// elapsed_say
//
// SUMMARY
//  describe an interval, specified as timespec start / end, in terms of years / days / hours /
//  minutes / seconds
//
// PARAMETERS
//  start - start of the interval
//  end   - end of the interval
//  N     - narrator to write to
//
void elapsed_say(const struct timespec * const restrict start, const struct timespec * const restrict end, struct narrator * const restrict N)
  __attribute__((nonnull));

/// bytesize_say
//
// SUMMARY
//  describe a quantity of bytes in terms of gigabytes / megabytes / kilobytes / bytes
//
// PARAMETERS
//  bytes - number of bytes
//  N     - narrator to write to
//
void bytesize_say(size_t bytes, struct narrator * const restrict N)
  __attribute__((nonnull));
