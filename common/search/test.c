#include <stdio.h>

#include "search.h"

int main()
{
  int list[] = { 1, 1, 1, 1, 2, 3, 5, 11, 13, 14, 24 };

  int compar(const void * _A, const void * _B, void * arg)
  {
    int * A = (int*)_A;
    int * B = (int*)_B;

    return *A - *B;
  };
  int key = 1;
  int * element = 0;
  int found = search_r(&key, list, sizeof(list) / sizeof(list[0]), sizeof(list[0]), compar, 0, &element);
  int index = element - list;

  printf("list\n");
  int x;
  for(x = 0; x < sizeof(list) / sizeof(list[0]); x++)
  {
    printf("[%3d]{%8p} %d", x, &list[x], list[x]);
    if(x == index)
      printf(" <--");
    printf("\n");
  }

  printf("%5s %d\n", "key", key);
  printf("%5s %d\n", "found", found);
  printf("%5s %d\n", "index", index);
  printf("%5s %p\n", "element", element);
  printf("%5s %d\n", "val", *element);

  return 0;
}
