/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "path_normalize.h"

#include "zcat.h"

/*
upwards traversal by ".." input sections will not ascend past the root.

So,
 /foo/../../bar means /bar, and
 ./foo/../../bar means ../bar
*/

size_t path_normalize(path_normalize_context * restrict ctx, char * restrict dst, size_t sz, const char * restrict s, uint16_t sl)
{
  const char *A;
  const char *B;
  int z;
  int z0;
  const char *base;
  uint16_t base_len;

  z = 0;

  if(!ctx->base || ctx->base_len == 0) {
    ctx->base = ".";
    ctx->base_len = 1;
  }
  if(!ctx->home || ctx->home_len == 0) {
    ctx->home = "~";
    ctx->home_len = 1;
  }
  if(sl > 0 && s[0] == '/') {
    base = "";
    base_len = 0;
  } else {
    base = ctx->base;
    base_len = ctx->base_len;
  }

  for(A = B = s; (B - s) < sl; A = B + 1) {
    /* find the next slash */
    for(B = A; (B - s) < sl && *B != '/'; B++);

    /* repeated slash */
    if((B - A) == 0) {
      continue;
    }

    /* special sections handling */
    if((B - A) == 1 && A[0] == '~') {
      if(z == 0) {
        /* leading tilde */
        z += zcatw(dst + z, sz - z, ctx->home, ctx->home_len);
        continue;
      } else {
        /* internal tilde */
      }
    } else if((B - A) == 5 && memcmp(A, "$HOME", 5) == 0) {
      if(z == 0) {
        /* leading $HOME */
        z += zcatw(dst + z, sz - z, ctx->home, ctx->home_len);
        continue;
      } else {
        /* internal $HOME */
      }
    } else if((B - A) == 1 && A[0] == '.') {
      if(z == 0) {
        /* leading dot */
        z += zcatw(dst + z, sz - z, ctx->base, ctx->base_len);
      } else {
        /* internal dot */
      }

      continue;
    } else if((B - A) == 2 && A[0] == '.' && A[1] == '.') {
      /* dot-dot */
      if(z == 0) {
        z += zcatw(dst + z, sz - z, base, base_len);
        base_len = 0;
      }

      if(z == 0) {
        continue;
      }

      z0 = z - 1;
      for(;z0 != 0 && dst[z0] != '/'; z0--);
      if(dst[z0] == '/') {
        z0++;
      }
      if((z - z0) == 1 && dst[z0] == '.') {
        z += zcats(dst + z, sz - z, ".");
      } else if((z - z0) == 2 && dst[z0] == '.' && dst[z0 + 1] == '.') {
        z += zcats(dst + z, sz - z, "..");
      } else {
        z = z0 - 1;
      }

      continue;
    }

    if(z == 0) {
      z += zcatw(dst + z, sz - z, base, base_len);
      base = "/";
      base_len = 0;
    }
    z += zcatc(dst + z, sz - z, '/');
    z += zcatw(dst + z, sz - z, A, B - A);
  }

  if(z == 0) {
    z += zcatw(dst + z, sz - z, base, base_len);
  }

  if(z == 0 && sl > 0 && s[0] == '/') {
    z += zcatc(dst + z, sz - z, '/');
  } else if(z == 0) {
    z += zcatw(dst + z, sz - z, ctx->base, ctx->base_len);
  }

  dst[z] = 0;

  return z;
}
