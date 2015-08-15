# libxapi unit tests

These tests are unique in the fab codebase in that they do not use libxunit. The
reason is of course because libxunit uses libxapi.

## Organization

The tests are divided into three groups based on which mode they apply to:

* unwinding : code using DXAPI_UNWINDING
* errcode : code using DXAPI_ERRCODE
* both : both
