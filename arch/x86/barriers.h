/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* memory barrier */
#define smp_mb() asm volatile ("mfence" ::: "memory")

/* write memory barrier */
#define smp_wmb() asm volatile ("sfence" ::: "memory")

/* read memory barrier */
#define smp_rmb() asm volatile("lfence" ::: "memory")
