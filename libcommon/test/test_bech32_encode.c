/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "unit.h"
#include "bech32.h"

#define ENCODE(e, ...) run_encode((uint8_t[]) { __VA_ARGS__ }, sizeof((uint8_t[]) { __VA_ARGS__ }), e)

static void run_encode(uint8_t *data, uint16_t data_len, const char *expected)
{
  char out[128];
  uint16_t out_len;
  const char *hrp;
  const char *hrp_end;

  /* get the hrp from the expected data */
  hrp = expected;
  hrp_end = strchr(expected, '1');

  out_len = bech32_encode(out, sizeof(out), hrp, hrp_end - hrp, data, data_len);
  out[out_len] = 0;

  assert_eq_s(expected, out);
}

int main()
{
  /* basic cases */
  ENCODE("foo1qyk86nn7", 1);
  ENCODE("foo1qypqykpwk7", 1, 2);
  ENCODE("foo1qypqxt4dhv4", 1, 2, 3);
  ENCODE("foo1qypqxpq20gxqu", 1, 2, 3, 4);
  ENCODE("foo1qypqxpq975h7zq", 1, 2, 3, 4, 5);

  /* 20 byte blobs */
  ENCODE("bm13hlv9cvt60hdv6q6gg70j7rqg0h24d50sw3e5m"
    , 0x8d, 0xfe, 0xc2, 0xe1, 0x8b, 0xd3, 0xee, 0xd6, 0x68, 0x1a
    , 0x42, 0x3c, 0xf9, 0x78, 0x60, 0x43, 0xee, 0xaa, 0xb6, 0x8f
  );
  ENCODE("bm1qaukh0640vp7aq9yfrkqs0daape72aa6wgakdm"
    , 0x07, 0x79, 0x6b, 0xbf, 0x55, 0x7b, 0x03, 0xee, 0x80, 0xa4
    , 0x48, 0xec, 0x08, 0x3d, 0xbd, 0xe8, 0x73, 0xe5, 0x77, 0xba
  );

  /* longer / other-length data */
  ENCODE("bm1zp6hl53whzq6qgw5vvzwar4m6c5p02y6"
    , 0x10, 0x75, 0x7f, 0xd2, 0x2e, 0xb8, 0x81, 0xa0, 0x21, 0xd4
    , 0x63, 0x04, 0xee, 0x8e, 0xbb, 0xd6
  );

  ENCODE("bm1j72sz0r77e94zpa87a7605k0yvc4gsf3jp3ffldc9hx7epljw546d87vm5pd2reusn0"
    , 0x97, 0x95, 0x01, 0x3c, 0x7e, 0xf6, 0x4b, 0x51, 0x07, 0xa7, 0xf7, 0x7d
    , 0xa7, 0xd2, 0xcf, 0x23, 0x31, 0x54, 0x41, 0x31, 0x90, 0x62, 0x94, 0xfd
    , 0xb8, 0x2d, 0xcd, 0xec, 0x87, 0xf2, 0x75, 0x2b, 0xa6, 0x9f, 0xcc, 0xdd
    , 0x02, 0xd5
  );

  summarize;
  return 0;
}
