/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <arpa/inet.h>

void xinet_pton(int af, const char * restrict src, void * restrict dst);
