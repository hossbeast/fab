/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/shm.h>

#include "types.h"

/// uxshmget
//
// SUMMARY
//  xapi proxy for shmget that only fails when errno != ENOENT
//
int uxshmget(key_t key, size_t size, int shmflg);

/// xshmget
//
// SUMMARY
//  xapi proxy for shmget
//
int xshmget(key_t key, size_t size, int shmflg);

/// xshmctl
//
// SUMMARY
//  xapi proxy for shmctl
//
void xshmctl(int shmid, int cmd, struct shmid_ds * const restrict buf);

/// xshmat
//
// SUMMARY
//  xapi proxy for shmat
//
void * xshmat(int shmid, const void * restrict shmaddr, int shmflg);

/// xshmdt
//
// SUMMARY
//  xapi proxy for shmdt - free semantics
//
// PARAMETERS
//  shmaddr
//
void xshmdt(const void * restrict shmaddr);

/// ixshmdt
//
// SUMMARY
//  idempotent xapi proxy for shmdt - iwfree semantics
//
// PARAMETERS
//  shmaddr
//
void ixshmdt(void ** shmaddr)
  __attribute__((nonnull));
