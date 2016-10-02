# libxapi

xapi is a calling convention in which the return value of a function communicates its success or
failure.

libxapi is an error handling library for xapi code.

## Usage

First, specify a set of error codes that your component can encounter in an errtab file.

    # APP.errtab
    1 FOO  the foo has failed
    2 BAR  the bar is broken

Use the fail macro to throw an error, and use the fatal macro to invoke other xapi-enabled functions

    xapi foo()
    {
      enter;                        // first line in any xapi-enabled function

      char * str = 0;
      fatal(xmalloc, &str, 100);    // call another xapi-enabled function
    
      fail(EFOO);                   // throw an error
    
      finally:                      // xapi-enabled functions end with finally/coda
        free(str);                  // cleanup
      coda;
    }

## How it works

When an error is thrown, the callstack unwinds and the function returns an exit value : an unsigned
integer that encodes the error table (in the high bits) and error code (in the low bits).

### XAPI_MODE_ERRORCODE

If you compile with -DXAPI_MODE_ERRORCODE, unwinding is accomplished by means of the exit value.

It is not necessary to link with libxapi-xapi.so when compiling with -DXAPI_MODE_ERRORCODE

### XAPI_MODE_STACKTRACE

If you compile with -DXAPI_MODE_STACKTRACE, while unwinding, a xapi-enabled function can produce a
backtrace. In addition, libxapi provides functions for getting more information from the exit value.

You must link with libxapi-xapi.so when compiling with -DXAPI_MODE_STACKTRACE

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
      xapi res = foo();                   // foo is a xapi-enabled function
      if(res)) {
        char * name = xapi_errname(res);  // "ENOMEM"
        char * desc = xapi_errdesc(res);  // "not enough space"
        char * str  = xapi_errstr(res);   // "ENOMEM : not enough space"
      }
    }

libxapi does no allocation or counting until an error is thrown. This means that in the case of an
actual ENOMEM, constructing the calltree will fail. It is recommended to pre-allocate memory to
libxapi, viz.

 #include "xapi/mm.h"

 int main() {
   xapi_allocate(10 * 1024);     // in bytes
   ...
   finally : coda;
 }

### XAPI_MODE_STACKTRACE_CHECKS

If you compile with -DXAPI_MODE_STACKTRACE_CHECKS, the same comments apply as for XAPI_MODE_STACKTRACE.
Additionally, in this mode, code is instrumented with runtime checks that will cause errors to be thrown
when certain common mistakes are made in the use of the xapi calling convention.

This mode is intended to be used in development only.

You must link with libxapi-xapi-devel.so when compiling with -DXAPI_MODE_STACKTRACE_CHECKS

## Features

* xapi/trace.h
  * for rendering backtraces
* xapi/calltree.h
  * To catch an error during unwinding, inspect it, and conditionally discard it, see xapi_calltree_unwind
* xapi/memblk.h
  * serialize/deserialize a calltree
* xapi/exit.h
  * getting strings from exit values
* xapi/errtab.h
  * registering error tables for exit value resolution
  * getting strings from exit values

## Fail after finally

A xapi function consists of two sections, the body and the finally block. fail/finally may be used
in either block. The only difference is that, if fail is invoked in the finally block, the remainder
of the finally block is skipped, and the function returns. This is consistent with the way exception
handling works in other implementations, such as C# and Java.

With other implementations, the original error and stack trace are discarded. libxapi preserves the
original error.

In stacktrace mode, the stack for the new error is rooted in the frame whose finally block was
executing when it was thrown. For that function, the calltree includes two frames, the frame for the
body block, and a subframe, for the finally block.

In this case, the exit value encodes the original error.

## Error tables

errtab files are translated to a .c and an .h file. The .c file contains a table with all of the
error codes, and the .h file contains macros for each error code (e.g. APP_BADARGS)

In ERRORCODE mode, the error table is not required, and the .c file can be ignored, if desired.

In STACKTRACE mode, the error table should be registered with libxapi by calling
xapi_errtab_register.

## Reserved words

The following is a list of the reserved words.

* coda
* conclude
* enter
* fail
* fatal
* finally
* invoke
* xapi
* xproxy

## Notes

libxlinux is a library that provides xapi wrappers for linux calls (open, read, write, etc)
