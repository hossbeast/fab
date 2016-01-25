# L_TRACE

L_TRACE works by instrumenting every call site with __FILE__, __LINE__, __FUNCTION__. I think there's a better way.

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

* instead of instrumenting call sites upfront, gather the call site address from down the stack when L_TRACE
* use libdwarf or similar to convert to file / function / line number
* requires code to be compiled with -g or similar
 * seems like an appropriate tradeoff
* this approach could also be used for libxapi stack trace production
