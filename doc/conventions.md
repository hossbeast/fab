# project organization
* the project is organized into units, one per directory
* each unit may have its own unit tests, in the file test.c

## library project organization

*  _name_ .h contains the majority/essential public api
  * additional public header files are at  _name_ / _unit_ .h
* internal.h included in every source file
  * #includes the public header file
  * defines visibility constants API, APIDATA
  * may re-define perrtab, the default error table
  * does NOT include any other internal header files

## executable project organization

* global.h is included by every source file
  * should define perrtab, the default error table
  * does NOT include any other internal header files

# data types

## attr : options / modifiers / flags

attributes / options
* bit-range or variable that contains one or more option / modifiers regions
* the range is named FOO_ATTR

option
* bit-range which contains one from a set of options, with zero as the default
* the range is named FOO_OPT
* the options are named FOO_BAR

modifiers / flags
* bit-range which contains zero or more options from a set
* the range is named FOO_MOD
* the options are named FOO_BAR

# describing objects

using libnarrator

```
xapi transform_canon_say(const transform * trans, narrator * const restrict _narrator) notnull(2)
xapi bits_say(const uint64_t bits, narrator * const restrict _narrator) notnull(2)
```

# module lifecycle management

single-invocation setup and teardown

```
void module_init()              # affirmative initialization
xapi module_setup()             # affirmative initialization
void module_teardown()          # affirmative resource cleanup
xapi module_cleanup()           # affirmative resource cleanup that may fail (close, dlclose)
xapi module_finalize()          # late binding / finish initialization
```

## load / unload

reference-counted setup and teardown

```
xapi libname_load()         # increment reference count, initialization on zero
xapi libname_unload()       # decrement reference count, release on zero
```

# object lifecycle management

## object allocation and initialization

```
xapi foo_alloc(foo ** const) notnull          # allocate foo
xapi foo_create(foo ** const) notnull         # synonym
xapi foo_mk(foo ** rv, foo * e) notnull(1)    # like _alloc, reuse an existing instance
xapi foo_initialize(foo *)                    # does not allocate foo
```

## releasing resources

X - can fail
F - frees foo
G - guarded
I - idempotent

```
void foo_free(foo *)                          #  FG
void foo_ifree(foo ** const ) notnull         #  FGI

xapi foo_xfree(foo *)                         # XFG
xapi foo_ixfree(foo ** const) notnull         # XFGI (calls release)

void foo_destroy(foo * const)  notnull        #
xapi foo_xdestroy(foo * const) notnull        # X
```

## restoring an object to its initial state

```
void foo_reset(foo *) notnull                 #    I set lengths to zero, preserve allocations
void foo_clear(foo *) notnull                 #    I synonym
xapi foo_recycle(foo *) notnull               # X  I like reset, can fail
```

# function naming

These modifiers specify some property about the semantics of the function.

If a function has both a failure mode specifier and operation modifier, the operation modifier is
first.

## failure mode specifier

w _name_ (wrapper)
* non-xapi wrapper function that follows the zero-return-success model and
  fails exactly when the underlying function fails

x _name_ (xapi)
* xapi-enabled proxy function that fails exactly when the underlying function fails

u _name_ (union)
* fails only in some subset of failure cases of the underlying function as
  specified

a _name_ (also)
* fails in some superset of failure cases of the underlying function as
  specified

## operation modifiers

e _name_  (extended)
* performs the same operation as the underlying function on another data type

i _name_  (idempotent)
* accepts a pointer to a handle for some resource and, upon successful
  completion, sets the value pointed at to the identity value for that resource
  type

d _name_ (dirfd)
* takes a dirfd a la openat

_name_ vf
* takes format string and va_list

_name_ f
* takes format string and varargs

_name_ s (string)
* takes string

_name_ w
* takes pointer/length pair

_name_ c (character)
* takes a character

_name_ x (extra)
* takes an extra options parameter

# testing

All tests use xunit, except those in the transitive dependencies of xunit (xapi, xlinux, logger, valyria, narrator)

## unit tests

should use internal apis, #include "foo.internal.h"

```
module
  unit
    unit.c
    unit.h
    test.c
```

or

```
module
  unit
    unit.c
    unit.h
  test
    test_unit.c
```

## integration tests

should NOT use internal apis, #include "foo.h"

```
module
  integ
    test_foo.c
    test_bar.c
```

## examples

are NOT run automatically

```
module
  example
    example_foo.c
    example_bar.c
```
