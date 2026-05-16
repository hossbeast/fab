/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/uio.h>
#include <errno.h>

#include "adaptor.h"

#include "zcat.h"

uint32_t adaptor_overwritev(struct adaptor * restrict adaptor, const struct iovec * restrict iov, int iovcnt)
{
  int x;
  uint32_t sz;
  uint32_t off;
  uint32_t sum;
  uint32_t xt;

  sum = 0;
  for(x = 0; x < iovcnt; x++)
  {
    off = 0;
    while(off < iov[x].iov_len) {
      xt = *adaptor->tail % adaptor->sz;
      sz = MIN(adaptor->sz - xt, iov[x].iov_len - off);
      memcpy(adaptor->buf + xt, iov[x].iov_base + off, sz);
      *adaptor->tail += sz;
      off += sz;
    }
    sum += iov[x].iov_len;
  }

  /* recompute head after rollover */
  if((*adaptor->tail - *adaptor->head) > adaptor->sz) {
    *adaptor->head = *adaptor->tail - adaptor->sz;
  }

  return sum;
}

uint32_t adaptor_overwrite(struct adaptor * restrict adaptor, const void * restrict src, uint32_t len)
{
  struct iovec iovs[1];
  iovs[0].iov_base = (void*)src;
  iovs[0].iov_len = len;
  return adaptor_overwritev(adaptor, iovs, ARRAY_LEN(iovs));
}

uint32_t adaptor_push(struct adaptor * restrict adaptor, const void * restrict src, uint32_t len)
{
  uint32_t z;
  uint32_t xh;
  uint32_t xt;
  uint32_t space;

  xh = *adaptor->head % adaptor->sz;
  xt = *adaptor->tail % adaptor->sz;

  if(xh <= xt) {
    space = adaptor->sz - xt;
  } else {
    space = xh - xt;
  }

  z = MIN(len, space);
  memcpy(adaptor->buf + xt, src, z);

  *adaptor->tail += z;

  return z;
}

static int iovs_clamp(struct iovec * restrict iovs, int iovcnt, uint32_t max)
{
  int x;

  /* clamp the cumulative sum to the max */
  for(x = 0; x < iovcnt; x++) {
    if(max < iovs[x].iov_len) {
      iovs[x].iov_len = max;
      if(max > 0) {
        x++;
      }
      break;
    }
    max -= iovs[x].iov_len;
  }

  return x;
}

uint32_t adaptor_overwrite_read(struct adaptor * restrict adaptor, int fd, uint32_t max)
{
  uint32_t xt;
  ssize_t r;
  uint32_t z;
  struct iovec iovs[2];
  int x;

  xt = *adaptor->tail % adaptor->sz;

  /* tail -> end */
  iovs[0].iov_base = adaptor->buf + xt;
  iovs[0].iov_len = adaptor->sz - xt;

  /* 0 -> tail */
  iovs[1].iov_base = adaptor->buf;
  iovs[1].iov_len = xt;

  /* enforce the max */
  x = iovs_clamp(iovs, ARRAY_LEN(iovs), max);

  r = readv(fd, iovs, x);
  if(r < 0) {
    RUNTIME_ASSERT(errno == EAGAIN);
    return 0;
  }

  RUNTIME_ASSERT(r >= 0);
  z = r;

  *adaptor->tail += z;

  /* recompute head after rollover */
  if((*adaptor->tail - *adaptor->head) > adaptor->sz) {
    *adaptor->head = *adaptor->tail - adaptor->sz;
  }

  return z;
}

uint32_t adaptor_push_read(struct adaptor * restrict adaptor, int fd, uint32_t max)
{
  uint32_t xh;
  uint32_t xt;
  ssize_t r;
  uint32_t z;
  struct iovec iovs[2];
  uint32_t x;

  xh = *adaptor->head % adaptor->sz;
  xt = *adaptor->tail % adaptor->sz;

  /* tail -> end */
  iovs[0].iov_base = adaptor->buf + xt;
  iovs[0].iov_len = adaptor->sz - xt;

  /* 0 -> head */
  iovs[1].iov_base = adaptor->buf;
  iovs[1].iov_len = xh;

  /* enforce the max */
  x = iovs_clamp(iovs, ARRAY_LEN(iovs), max);

  r = readv(fd, iovs, x);
  RUNTIME_ASSERT(r >= 0);
  z = r;

  *adaptor->tail += z;

  return z;
}

uint32_t adaptor_dump0(const struct adaptor * restrict adaptor, void **bufp)
{
  uint32_t xh, xt;

  xh = *adaptor->head;
  xh %= adaptor->sz;
  xt = *adaptor->tail;
  xt %= adaptor->sz;

  *bufp = adaptor->buf + xh;
  if(xh < xt) {
    return xt - xh;
  }

  return adaptor->sz - xh;
}

uint32_t adaptor_dump1(const struct adaptor * restrict adaptor, void **bufp)
{
  uint32_t xh, xt;

  xh = *adaptor->head;
  xh %= adaptor->sz;
  xt = *adaptor->tail;
  xt %= adaptor->sz;

  *bufp = adaptor->buf;
  if(xh < xt) {
    return 0;
  }

  return xt;
}

uint32_t adaptor_pop(struct adaptor * restrict adaptor, uint32_t cons, void *dst, uint32_t sz)
{
  uint32_t z;
  uint32_t xh, xc;

  xh = *adaptor->head;
  xh %= adaptor->sz;
  xc = cons;
  xc %= adaptor->sz;

  if(xh < xc) {
    z = zcatw(dst, sz, adaptor->buf + xh, xc - xh);
    goto end;
  }

  /* xh -> end */
  z = 0;
  z += zcatw(dst + z, sz - z, adaptor->buf + xh, adaptor->sz - xh);

  /* 0 -> xc */
  z += zcatw(dst + z, sz - z, adaptor->buf, xc);

end:
  *adaptor->head += z;
  return z;
}

uint32_t adaptor_last(struct adaptor * restrict adaptor, uint32_t len, void *dst, uint32_t sz)
{
  uint32_t z;
  uint32_t xh, xt, xc;
  uint32_t cnt;

  xh = *adaptor->head;
  xt = *adaptor->tail;
  cnt = xt - xh;
  cnt = MIN(cnt, len);
  xc = xt - cnt;

  xc %= adaptor->sz;
  xt %= adaptor->sz;

  /* xc -> xt */
  if(xc <= xt) {
    return zcatw(dst, sz, adaptor->buf + xc, xt - xc);
  }

  /* xc -> end */
  z = 0;
  z += zcatw(dst + z, sz - z, adaptor->buf + xc, adaptor->sz - xc);

  /* 0 -> xt */
  z += zcatw(dst + z, sz - z, adaptor->buf, xt);

  return z;
}

void adaptor_init(struct adaptor * restrict adaptor, void *buf, uint32_t sz, uint32_t * restrict headp, uint32_t * restrict tailp)
{
  adaptor->buf = buf;
  adaptor->sz = sz;
  adaptor->head = headp;
  adaptor->tail = tailp;
}
