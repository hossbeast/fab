# L_TRACE

Previous iterations of the functionality caused every call site to be instrumented with __FILE__, __LINE__, __FUNCTION__. I think there's a better way.

```
#include <stdio.h>

void bar()
{
  printf("bar -> %p\n", __builtin_extract_return_addr(__builtin_return_address(0)));
}

void foo()
{
  bar();
  printf("foo -> %p\n", __builtin_extract_return_addr(__builtin_return_address(0)));
  bar();
}

int main()
{
  foo();

  return 0;
}
```
