/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/sdt.h>

#define PROBE(provider, probe) STAP_PROBE(provider, probe)
#define PROBE1(provider, probe, a) STAP_PROBE1(provider, probe, a)
#define PROBE2(provider, probe, a, b) STAP_PROBE2(provider, probe, a, b)
#define PROBE3(provider, probe, a, b, c) STAP_PROBE3(provider, probe, a, b, c)
#define PROBE4(provider, probe, a, b, c, d) STAP_PROBE4(provider, probe, a, b, c, d)
