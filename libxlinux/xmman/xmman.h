/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/mman.h>

/// xmmap
//
// SUMMARY
//  proxy for mmap
//
void * xmmap(void * restrict addr, size_t length, int prot, int flags, int fd, off_t offset);

/// xmunmap
//
// SUMMARY
//  proxy for munmap
//
void xmunmap(void * addr, size_t length);

/// ixmunmap
//
// SUMMARY
//  idempotent proxy for munmap
//
// PARAMETERS
//  [addr] - if not zero, munmap *addr
//  length - size of mapping
//
void ixmunmap(void * addr, size_t length);
