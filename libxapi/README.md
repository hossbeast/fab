# libxapi

xapi is a calling convention in which the return value of a function communicates
its success or failure.

libxapi is an error handling library for xapi code.

## Usage

First, specify a table of error codes that your component can encounter.

    # APP.errtab
    1 FOO  the foo has failed
    2 BAR  the bar is broken

Use the fail macro to raise an error, and use the fatal macro to invoke other
xapi-enabled functions

    xapi foo()
    {
      enter;                        // first line in any xapi-enabled function

      char * str = 0;
      fatal(xmalloc, &str, 100);    // call another xapi-enabled function
    
      fail(EFOO);                   // raise an error
    
      finally:                      // xapi-enabled functions end with finally/coda
        free(str);                  // cleanup
      coda;
    }

## How it works

When an error is raised, the callstack will unwind and, from the perspective of
the top-level xapi-enabled function, one of two things will happen.

### XAPI_ERRCODE

If you compile with -DXAPI_ERRCODE, the function returns an exit value : an unsigned
integer that encodes the error table (in the high bits) and error code (in the low bits).

    void main()

It is not necessary to link with libxapi.so when compiling with -DXAPI_ERRCODE

### XAPI_UNWIND

If you compile with -DXAPI_UNWIND, the top-level xapi function still returns an
unsigned integer that encodes the error table and code. libxapi provides functions
for getting more information from the exit value.

 the top-level xapi function can produce a backtrace

    xapi foo()
    {
      enter;
      ...
      finally:
       if(XAPI_UNWINDING)
         xapi_backtrace();      // print a backtrace to stderr
      coda;
    }

    int main()
    {
      int res = foo();                    // foo is a xapi-enabled function
      if(res)) {
        char * name = xapi_errname(res);  // "ENOMEM"
        char * desc = xapi_errdesc(res);  // "not enough space"
        char * str  = xapi_errstr(res);   // "ENOMEM : not enough space"
      }
    }

libxapi does no allocation or counting until an error is raised. This mean that
in the case of an actual ENOMEM, constructing the calltree will fail, so it is
recommended to pre-allocate memory to libxapi, viz.

 #include "xapi/mm.h"

 int main() {
   xapi_allocate(10 * 1024);     // in bytes
   ...
   finally : coda;
 }

## Features

* For various backtrace options, see xapi/trace.h
* To catch an error during unwinding, inspect it, and conditionally discard it, see xapi_calltree_unwind
* To serialize/deserialize a calltree, see xapi_calltree_freeze / memblk.h

* calltree.h

## Implementation Details

A xapi function consists of two sections, the body and the finally block. fail/finally may be used
in either block. The only difference is that, if fail is invoked in the finally block, the remainder
of the finally block is skipped, and the function returns. This is consistent with the way exception
handling works in other implementations, such as C# and Java. However, with xapi, the original
error is not discarded. Instead, the stack for the new error is rooted in the frame whose finally
block was executing when it was raised.

## Notes

libxlinux is a library that provides xapi wrappers for most linux calls (open, read, write, etc)

The following is a list of the reserved words.

* xapi
* enter
* fatal
* fail
* finally
* coda
* invoke
* conclude
* xproxy

../calltree/calltree.h
../error/error.h
../errtab/errtab.h
../frame/frame.h
../mm/mm.h
../error/SYS.errtab.h
../trace/trace.h
../tune/tune.h
../error/XAPI.errtab.h
