# libxapi integration tests

These tests are unique in the fab codebase in that they do not use libxunit. The
reason is of course because libxunit uses libxapi.

## Organization

The tests are divided into three groups based on which mode they apply to:

* stacktrace : code using DXAPI_MODE_STACKTRACE
* stacktrace_checks : code using DXAPI_MODE_STACKTRACE_CHECKS
* errcode : code using DXAPI_MODE_ERRCODE
