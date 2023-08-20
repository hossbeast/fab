/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/// xflock
//
// SUMMARY
//  proxy for flock
//
// PARAMETERS
//  fd        - pid
//  operation - signal
//
void xflock(pid_t pid, int sig);
