# fab/test

contains common functionality for unit tests using xunit used throughout the fab codebase

xunit depends on

1. libxapi
2. libxlinux
3. liblogger
4. libvalyria
5. libnarrator

notably, the unit tests for libxapi do not use xunit, as xunit itself depends on libxapi
