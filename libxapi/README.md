libxapi
===

libxapi is a light-weight exception handling library for C.

First, you specify a table of error codes that your component can encounter.
Then, throughout your code, use the fatal macro to invoke other xapi-enabled functions

 int foo() {
   char * str = 0;
   fatal(xmalloc, &str, 100);    // call another xapi-enabled function

   fail(EBAD);    // raise an exception

 finally:
   free(str);     // cleanup
 coda;
 }

When a top-level xapi function is called, and an exception is raised during its
execution, one of two things will happen.

If you compile with -DXAPI_ERRCODE, the function returns an int that encodes the error
table and code for the exception, which you can use to get more information.

 int res = foo();
 if((res = foo())) {
   char * name = libfoo_errname(res);  // ENOMEM
   char * desc = libfoo_errdesc(res);  // not enough space
   char * str  = libfoo_errstr(res);   // ENOMEM : not enough space
 }

If you compile with -DXAPI_UNWIND, the top-level xapi function can produce a backtrace

 finally:
  if(XAPI_UNWINDING)
    xapi_backtrace();    // print a backtrace to stderr
 coda;

There is no overhead because libxapi does no allocation or counting until an exception
is raised. This does mean that, in the case of an actual ENOMEM, constructing the calltree
will fail, so it is recommended to pre-allocate the calltree

 int main() {
   xapi_preallocate(100);
   ...
   finally : coda;
 }

The libxlinux library provides xapi wrappers for most linux calls (open, read, write, etc)
