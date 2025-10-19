/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

/*
 * taken and adapted from https://github.com/sipa/bech32/blob/master/ref/c/segwit_addr.c
 *
 * Copyright (c) 2017, 2021 Pieter Wuille
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 */

/*
Tested with,
 https://slowli.github.io/bech32-buffer
 https://blockchain-academy.hs-mittweida.de/bech32-tool
*/

#include "bech32.h"
#include "zcat.h"

static char alphabet[] = {
    'q', 'p', 'z', 'r', 'y', '9', 'x', '8'
  , 'g', 'f', '2', 't', 'v', 'd', 'w', '0'
  , 's', '3', 'j', 'n', '5', '4', 'k', 'h'
  , 'c', 'e', '6', 'm', 'u', 'a', '7', 'l'
};

static const int charset_rev[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  15, -1, 10, 17, 21, 20, 26, 30,  7,  5, -1, -1, -1, -1, -1, -1,
  -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
   1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1,
  -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
   1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1
};

static uint32_t bech32_polymod_step(uint32_t pre)
{
    uint8_t b;

    b = pre >> 25;
    return ((pre & 0x1FFFFFF) << 5) ^
        (-((b >> 0) & 1) & 0x3b6a57b2UL) ^
        (-((b >> 1) & 1) & 0x26508e6dUL) ^
        (-((b >> 2) & 1) & 0x1ea119faUL) ^
        (-((b >> 3) & 1) & 0x3d4233ddUL) ^
        (-((b >> 4) & 1) & 0x2a1462b3UL);
}

size_t bech32_encode(char * restrict dst, size_t sz, const char * restrict hrp, uint16_t hrp_len, const void * restrict data, uint16_t len)
{
  uint32_t val;
  int bits;
  const uint8_t *in;
  size_t z;
  uint32_t ch;
  uint32_t chk;
  int x;

  in = data;
  z = 0;

  /* hrp first pass */
  chk = 1;
  for(x = 0; x < hrp_len; x++) {
    ch = hrp[x];
    chk = bech32_polymod_step(chk) ^ (ch >> 5);
  }
  chk = bech32_polymod_step(chk);

  /* hrp 2nd pass */
  for(x = 0; x < hrp_len; x++) {
    ch = hrp[x];
    chk = bech32_polymod_step(chk) ^ (ch & 0x1f);

    z += zcatc(dst + z, sz - z, ch);
  }

  /* separator */
  z += zcatc(dst + z, sz - z, '1');

  /* 8 -> 5 conversion */
  val = 0;
  bits = 0;
  while(len--) {
    val = (val << 8) | *(in++);
    bits += 8;
    while(bits >= 5) {
      bits -= 5;
      ch = (val >> bits) & 0x1f;
      chk = bech32_polymod_step(chk) ^ ch;
      z += zcatc(dst + z, sz - z, alphabet[ch]);
    }
  }

  /* remainder */
  if(bits) {
    ch = (val << (5 - bits)) & 0x1f;
    chk = bech32_polymod_step(chk) ^ ch;
    z += zcatc(dst + z, sz - z, alphabet[ch]);
  }

  /* finalize checksum */
  for (x = 0; x < 6; x++) {
    chk = bech32_polymod_step(chk);
  }
  chk ^= 0x2bc830a3;

  /* append checksum */
  for (x = 0; x < 6; x++) {
    ch = (chk >> ((5 - x) * 5)) & 0x1f;
    z += zcatc(dst + z, sz - z, alphabet[ch]);
  }

  return z;
}

int bech32_decode(
    void * restrict data
  , size_t data_size
  , size_t * restrict data_lenp
  , char * restrict hrp
  , size_t hrp_size
  , size_t * restrict hrp_lenp
  , const char * restrict input
  , size_t input_len
)
{
  uint32_t chk;
  bool have_lower;
  bool have_upper;
  size_t data_len;
  size_t dataz;
  size_t hrp_len;
  size_t hrpz;
  int x;
  uint32_t ch;
  uint32_t val;
  int bits;

  have_lower = false;
  have_upper = false;

  /* locate the end of the hrp, the rightmost 1 character */
  data_len = 0;
  while(data_len < input_len && input[(input_len - 1) - data_len] != '1') {
    data_len++;
  }
  hrp_len = input_len - (1 + data_len);

  /* checksum is 6 characters long */
  if(((1 + data_len) >= input_len) || (data_len < 6)) {
    return -1;
  }
  data_len -= 6;

  /* hrp first pass */
  chk = 1;
  hrpz = 0;
  for(x = 0; x < hrp_len; x++) {
    ch = input[x];
    if (ch < 33 || ch > 126) {
      return -1;
    }
    if (ch >= 'a' && ch <= 'z') {
      have_lower = true;
    } else if (ch >= 'A' && ch <= 'Z') {
      have_upper = true;
      ch = (ch - 'A') + 'a';
    }
    chk = bech32_polymod_step(chk) ^ (ch >> 5);

    hrpz += zcatc(hrp + hrpz, hrp_size - hrpz, ch);
  }

  /* hrp 2nd pass */
  chk = bech32_polymod_step(chk);
  for(x = 0; x < hrp_len; x++) {
    ch = input[x];
    chk = bech32_polymod_step(chk) ^ (ch & 0x1f);
  }

  val = 0;
  bits = 0;
  dataz = 0;
  for(x++; x < input_len; x++) {
    ch = input[x];
    if(ch & 0x80) {
      ch = -1;
    } else {
      ch = charset_rev[ch];
    }
    if(ch == -1) {
      return -1;
    }
    if(input[x] >= 'a' && input[x] <= 'z') {
      have_lower = true;
    } else if(input[x] >= 'A' && input[x] <= 'Z') {
      have_upper = true;
    }
    chk = bech32_polymod_step(chk) ^ ch;

    /* dont copy checksum into the output */
    if((x + 6) >= input_len) {
      continue;
    }

    /* 5 -> 8 conversion */
    val = (val << 5) | ch;
    bits += 5;
    while(bits >= 8) {
      bits -= 8;
      ch = (val >> bits) & 0xff;
      dataz += zcatc(data + dataz, data_size - dataz, ch);
    }
  }

  /* remainder */
  if(((val << (8 - bits)) & 0xff) || bits >= 5) {
    return -1;
  }

  /* mixed-case illegal input */
  if(have_lower && have_upper) {
    return -1;
  }

  if(chk != 0x2bc830a3) {
    return -1;
  }

  *data_lenp = dataz;
  *hrp_lenp = hrpz;
  return 0;
}
