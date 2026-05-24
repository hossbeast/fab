/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <grp.h>


/// uxgetgrgid_r
//
// SUMMARY
//  proxy for getgrgid_r that only fails when errno != { ENOENT, ESRCH, EBAD, EPERM }
//
void uxgetgrgid_r(gid_t uid, struct group * grp, char * buf, size_t buflen, struct group ** result);

/// xgetgrgid
//
// SUMMARY
//  proxy for getgrgid
//
struct group * xgetgrgid(gid_t gid);
