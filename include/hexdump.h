/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

static inline void hexdump(const void *p, uint16_t len)
{
  int x;
  int y;
  int i;
  const char *s = p;

  printf("0x%08x bytes (%u)", len, len);
  x = 0;
  y = 0;
  while(1)
  {
    if((x == len) || (x && (x % 16) == 0)) {
      for(i = x - y; i < 16; i++) {
        printf("   ");
      }
      printf(" |");
      for(i = y; i != x; i++) {
        if(s[i] >= 0x20 && s[i] <= 0x7e) {
          printf("%c", s[i]);
        } else {
          printf(" ");
        }
      }
      for(i = x - y; i < 16; i++) {
        printf(" ");
      }
      printf("|");
    }
    if(x == len) {
      break;
    }

    if((x % 16) == 0) {
      printf("\n%16p 0x%08x ", &s[x], x);
      y = x;
    }
    if((x % 8) == 0) {
      printf(" ");
    }

    printf("%02hhx ", s[x]);
    x++;
  }

  printf("\n");
}
