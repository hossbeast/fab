#ifndef _HEXDUMP_H
#define _HEXDUMP_H

static inline void hexdump(const char *s, uint16_t len)
{
  int x;
  int y;
  int i;

  printf("0x%08x bytes (%u)", len, len);
  x = 0;
  y = 0;
  while(1) // for(x = 0; x < len; x++)
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

#endif
