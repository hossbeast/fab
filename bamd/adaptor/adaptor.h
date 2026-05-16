/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct iovec;

/*
 * an adaptor provides mediated access to a buffer
 *
 * writes are always at the tail, and only the most recent N bytes are preseved
 *
 * reads are always from the head
 */

typedef struct adaptor {
  char *buf;        // buffer
  uint32_t sz;      // buffer size
  uint32_t *head;   // pointer to head
  uint32_t *tail;   // pointer to tail
} adaptor;

void adaptor_init(struct adaptor * restrict adaptor, void *buf, uint32_t sz, uint32_t * restrict headp, uint32_t * restrict tailp)
  __attribute__((nonnull));

/*
 * copy data into the buffer at the tail, without overwriting at the head.
 *
 * src - pointer to data to copy
 * len - number of bytes to copy
 *
 * returns the number of bytes copied
 */
uint32_t adaptor_push(struct adaptor * restrict adaptor, const void * restrict src, uint32_t len)
  __attribute__((nonnull));

/*
 * copy data into the buffer at the tail, overwriting at the head as needed.
 *
 * iov    - pointer to iovs
 * iovcnt - number of iovs
 *
 * returns the number of bytes copied
 */
uint32_t adaptor_overwritev(struct adaptor * restrict adaptor, const struct iovec * restrict iov, int iovcnt)
  __attribute__((nonnull));

uint32_t adaptor_overwrite(struct adaptor * restrict adaptor, const void * restrict src, uint32_t len)
  __attribute__((nonnull));

uint32_t adaptor_overwrite_read(struct adaptor * restrict adaptor, int fd, uint32_t max)
  __attribute__((nonnull));

/*
 * copy data into the buffer at the tail, without overwriting at the head. the data to copy is read from a file
 * descriptor.
 *
 * fd  - file descriptor to read from
 * max - dont read more than max
 *
 * returns the number of bytes copied
 */
uint32_t adaptor_push_read(struct adaptor * restrict adaptor, int fd, uint32_t max)
  __attribute__((nonnull));

/*
 * remove data from the buffer at the head
 *
 * cons - stop copying at cons
 * dst  - buffer to copy the data to
 * sz   - size of dst
 *
 * returns the number of bytes removed
 */
uint32_t adaptor_pop(struct adaptor * restrict adaptor, uint32_t cons, void *dst, uint32_t sz)
  __attribute__((nonnull));

/*
 * get a pointer to the first region
 *
 * bufp - returns a pointer to the start of the first region
 *
 * returns number of bytes in the first region
 */
uint32_t adaptor_dump0(const struct adaptor * restrict adaptor, void **bufp)
  __attribute__((nonnull));

/* get a pointer to the second region */
uint32_t adaptor_dump1(const struct adaptor * restrict adaptor, void **bufp)
  __attribute__((nonnull));

/*
 * copy the trailing data from the buffer, ending at the tail
 *
 * len  - amount of data to copy
 * dst  - buffer to copy the data to
 * sz   - size of dst
 *
 * returns the number of bytes copied
 */
uint32_t adaptor_last(struct adaptor * restrict adaptor, uint32_t len, void *dst, uint32_t sz)
  __attribute__((nonnull));
