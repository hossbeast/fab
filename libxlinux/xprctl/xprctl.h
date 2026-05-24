/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/// xprctl
//
// SUMMARY
//  proxy for prctl
//
// PARAMETERS
//
void xprctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);
