# libxapi

libxapi is a light-weight exception handling library for C.

xapi is a calling convention in which the return value of a function is reserved to communicate
the success or failure of the function.

An application using libxapi will generally follow the xapi convention everywhere

## Usage

First, specify a table of error codes that your component can encounter.

    # APP.errtab
    1 FOO  the foo has failed
    2 BAR  the bar is broken

Use the fail macro to raise an exception, and use the fatal macro to invoke other
xapi-enabled functions

    int foo() {
      enter;         // first line in any xapi-enabled function

      char * str = 0;
      fatal(xmalloc, &str, 100);    // call another xapi-enabled function
    
      fail(EBAD);    // raise an exception
    
    finally:         // xapi-enabled functions end with finally/coda
      free(str);     // cleanup
    coda;
    }

## How it works

When an exception is raised, the callstack will unwind and, from the perspective of
the top-level xapi-enabled function, one of two things will happen

### XAPI_ERRCODE

If you compile with -DXAPI_ERRCODE, the function returns an int that encodes the error
table and code for the exception, which you can use to get more information.

    void main() {
      int res = foo();                      // foo is a xapi-enabled function
      if((res = foo())) {
        char * name = libfoo_errname(res);  // "ENOMEM"
        char * desc = libfoo_errdesc(res);  // "not enough space"
        char * str  = libfoo_errstr(res);   // "ENOMEM : not enough space"
      }
    }

It is not necessary to link with libxapi.so when compiling with -DXAPI_ERRCODE

### XAPI_UNWIND

If you compile with -DXAPI_UNWIND, the top-level xapi function can produce a backtrace

    int foo() {
      enter;

      finally:
       if(XAPI_UNWINDING)
         xapi_backtrace();    // print a backtrace to stderr
      coda;
    }

libxapi does no allocation or counting until an exception is raised. This mean that
in the case of an actual ENOMEM, constructing the calltree will fail, so it is
recommended to pre-allocate memory to libxapi, viz.

 #include "xapi/mm.h"

 int main() {
   xapi_allocate(100);
   ...
   finally : coda;
 }

## Features

* For various backtrace options, see xapi/trace.h
* To catch an exception during unwinding, inspect it, and conditionall discard it, see xapi_calltree_unwind
* To serialize/deserialize a calltree, see xapi_calltree_freeze / memblk.h

## Implementation Details

The body of a xapi function consists of two parts, the main body and the finally block. The fail
macro may be used in either block. The only difference is that, if fail is invoked in the
finally block, the remainder of the finally block will be skipped, and the function will return

## Notes

libxlinux is a library that provides xapi wrappers for most linux calls (open, read, write, etc)
