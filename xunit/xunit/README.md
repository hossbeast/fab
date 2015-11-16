# Summary

xunit is a program for executing xapi unit tests. It takes a list of objects, of three types :

* code being tested
* dependencies of that code
* unit tests

xunit dynamically loads all of the objects, and executes the tests. Due to the nature of
dynamic loading, the objects must be specified in dependency order, that is, an objects
dependencies must be specified before the object itself.

# Unit Tests

Objects containing unit tests are called units, and export a structure named "xunit" of
type xunit_unit (see libxunit/xunit.h).

xunit executes each test using the member function named entry

The test succeeds if the function returns success, according to the xapi calling convention

# Logging

xunit exports the following logging categories for unit test code to use

L_INSET : unit test input set
L_OUTSET : unit test output set
