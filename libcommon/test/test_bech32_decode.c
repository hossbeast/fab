/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "unit.h"
#include "bech32.h"

#define DECODE(s, r, hrp, ...) run_decode(s, r, hrp, strlen(hrp), (uint8_t[]) { __VA_ARGS__ }, sizeof((uint8_t[]) { __VA_ARGS__ }))

static void run_decode(const char *s, int expected, const char *exp_hrp, uint16_t exp_hrp_len, uint8_t *exp_data, uint16_t exp_data_len)
{
  char data[256];
  size_t data_len;
  char hrp[256];
  size_t hrp_len;
  int r;

  r = bech32_decode(data, sizeof(data), &data_len, hrp, sizeof(hrp), &hrp_len, s, strlen(s));

  assert_eq_d(expected, r);
  if(r) {
    return;
  }
  assert_that(memncmp(hrp, hrp_len, exp_hrp, exp_hrp_len) == 0);
  assert_that(memncmp(data, data_len, exp_data, exp_data_len) == 0);
}

int main()
{
  /* basic cases */
  DECODE("foo1qyk86nn7", 0, "foo", 1);
  DECODE("foo1qypqykpwk7", 0, "foo", 1, 2);
  DECODE("foo1qypqxt4dhv4", 0, "foo", 1, 2, 3);
  DECODE("foo1qypqxpq20gxqu", 0, "foo", 1, 2, 3, 4);
  DECODE("foo1qypqxpq975h7zq", 0, "foo", 1, 2, 3, 4, 5);

  /* bad checksum */
  DECODE("foo1qy556nn7", -1, "");
  DECODE("foo1qyk86abc", -1, "");
  DECODE("foo1qypqxpq970h7zq", -1, "");
  DECODE("bar1qyk86nn7", -1, "");

  /* malformed */
  DECODE("", -1, "");
  DECODE("abcd", -1, "");
  DECODE("ab1", -1, "");
  DECODE("ab1ab", -1, "");

  /* 20 byte blobs */
  DECODE("bm13hlv9cvt60hdv6q6gg70j7rqg0h24d50sw3e5m", 0, "bm"
    , 0x8d, 0xfe, 0xc2, 0xe1, 0x8b, 0xd3, 0xee, 0xd6, 0x68, 0x1a
    , 0x42, 0x3c, 0xf9, 0x78, 0x60, 0x43, 0xee, 0xaa, 0xb6, 0x8f
  );
  DECODE("bm1qaukh0640vp7aq9yfrkqs0daape72aa6wgakdm", 0, "bm"
    , 0x07, 0x79, 0x6b, 0xbf, 0x55, 0x7b, 0x03, 0xee, 0x80, 0xa4
    , 0x48, 0xec, 0x08, 0x3d, 0xbd, 0xe8, 0x73, 0xe5, 0x77, 0xba
  );

  /* longer / other-length data */
  DECODE("bm1zp6hl53whzq6qgw5vvzwar4m6c5p02y6", 0, "bm"
    , 0x10, 0x75, 0x7f, 0xd2, 0x2e, 0xb8, 0x81, 0xa0, 0x21, 0xd4
    , 0x63, 0x04, 0xee, 0x8e, 0xbb, 0xd6
  );

  DECODE("bm1j72sz0r77e94zpa87a7605k0yvc4gsf3jp3ffldc9hx7epljw546d87vm5pd2reusn0", 0, "bm"
    , 0x97, 0x95, 0x01, 0x3c, 0x7e, 0xf6, 0x4b, 0x51, 0x07, 0xa7, 0xf7, 0x7d
    , 0xa7, 0xd2, 0xcf, 0x23, 0x31, 0x54, 0x41, 0x31, 0x90, 0x62, 0x94, 0xfd
    , 0xb8, 0x2d, 0xcd, 0xec, 0x87, 0xf2, 0x75, 0x2b, 0xa6, 0x9f, 0xcc, 0xdd
    , 0x02, 0xd5
  );

  summarize;
  return 0;
}
