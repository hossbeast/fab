# blackfyre


# xunit

xunit is a testing framework for code that follows the xapi calling convention (see libxapi/xapi.h). It consists of two parts.

xunit is a program that loads the code under test, and the testing code, and executes the tests.

libxunit is a library which is linked by the testing code.

Optionally, a valgrind test runner is provided, which may be used to wrap invocations of xunit in order to assert the non-existence of errors, as determined by valgrind.

# Terminology

integration tests depend on the top-level artifact(s) in the fabfile, and exercise the public apis.

unit tests have a smaller scope. They typically

# Tenets

1. Tests should depend minimally on the code being tested, i.e. the test .so should not depend on the entire library .so.

# xunit

