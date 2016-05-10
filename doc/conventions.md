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

## binary project organization

* global.h is included by every source file
  * contains #includes that are absolutely mandatory for every unit
  * typically this is only logs.h and the appropriate errtab.h file
  * should also re-define perrtab, the default error table
  * because each #include creates a dependency for every unit, global.h #includes should be minimized

# data types

## options / modifiers / flags

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
xapi transform_canon_say(const transform * trans, narrator * restrict _narrator)
xapi bits_say(const uint64_t bits, narrator * restrict _narrator)
```

# module lifecycle management

single-invocation initialization and teardown

## setup / teardown / release

```
xapi module_setup()         # affirmative initialization
void module_teardown()      # affirmative resource cleanup
xapi module_release()       # affirmative resource cleanup that may fail (close, dlclose)
```

## load / unload

reference-counted initialization and teardown

```
xapi libname_load()         # increment reference count, initialization on zero
xapi libname_unload()       # decrement reference count, release on zero
```

# object lifecycle management

## alloc / create / mk / free / ifree / reset / clear

default strategy for heap allocated objects

```
xapi foo_alloc(foo ** const) notnull        # allocate foo
xapi foo_create(foo ** const) notnull       # synonym
xapi foo_mk(foo ** rv, foo * e) notnull(1)  # like _alloc, reuse an existing instance
void foo_free(foo *)                        # release resources
void foo_ifree(foo ** const ) notnull       # safe and idempotent free
void foo_reset(foo *) notnull               # set lengths to zero, preserve allocations
void foo_clear(foo *) notnull               # synonym
```

## dispose / recycle

when releasing resources can fail (like file descriptors)

```
xapi foo_dispose(foo *)       # idempotent like ifree, can fail
xapi foo_recycle(foo *)       # idempotent like reset, can fail
```

## initialize / destroy

for statically allocated objects

```
xapi foo_initialize(foo *)    # does not allocate foo
void foo_destroy(foo *)       # does not free foo
```

# function naming

these modifiers specify some property about the semantics of the function

## failure modes

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

## modifiers

e _name_  (extended)
* performs the same operation as the underlying function on another data type

i _name_  (idempotent)
* accepts a pointer to a handle for some resource and, upon successful
  completion, sets the value pointed at to the identity value for that resource
  type

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
