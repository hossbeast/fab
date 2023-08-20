/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <pwd.h>

/// uxgetpwuid_r
//
// SUMMARY
//  proxy for getpwuid_r that only fails when errno != { ENOENT, ESRCH, EBAD, EPERM }
//
void uxgetpwuid_r(uid_t uid, struct passwd * pwd, char * buf, size_t buflen, struct passwd ** result);

/// xgetpwuid
//
// SUMMARY
//  proxy for getpwuid
//
struct passwd * xgetpwuid(uid_t uid);
