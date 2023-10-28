/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 narrator/fd

SUMMARY
 narrator that passes all writes to a file descriptor

*/

#include "types.h"

#include "narrator.h"

typedef struct narrator_fd
{
  narrator base;
  int fd;
  int off;

  /* errno if the writer experienced an error writing to the fd */
  int err;
} narrator_fd;

extern struct narrator_vtable narrator_fd_vtable;

/// narrator_fd_create
//
// SUMMARY
//  allocate a fd narrator
//
// PARAMETERS
//  n  - (returns) narrator
//  fd - file descriptor
//
void narrator_fd_create(narrator_fd ** restrict n, int fd)
  __attribute__((nonnull));

void narrator_fd_free(narrator_fd * restrict n);

void narrator_fd_init(narrator_fd * restrict n, int fd)
  __attribute__((nonnull));

void narrator_fd_destroy(narrator_fd * restrict n);
