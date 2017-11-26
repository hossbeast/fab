# Overview

fabfiles are the build description files for fab. They end with the ".fab" extension.

# Terms

## build

encompasses everything fab knows about, how to build, all source files, etc.
encompasses all of the modules, artifacts, rules, formulas, primary and secondary files

## module

The smallest scope at which fab operates.

Logically, a module consists of a set of artifacts, and all of the necessary source files, rules,
and formulas for building and updating those artifacts.

At the filesystem level, a module is a directory, together with all of its subdirectories which are
not themselves modules.

The top-level directory for a module has a file named "module.fab"

Modules declare 1+ artifacts

## template|language

A collection of rules, formulas, and tasks which may be applied to a module with the use directive

collections of templates can be referenced by the directory name, or individually by <name>.fab

templates cannot have artifacts

## formula

A small program which creates 1+ secondary files from 0+ primary files

## artifact

A secondary file declared by a module

examples

 artifact libxlinux.xapi.so : libxapi^.so

## Variant

rules may include the variant token '?' which is matched in a wildcard fashion against artifacts in the module.
This allows a single rule to apply to each of several similarly named artifacts

## Task

Named formula which may have dependencies on externally defined artifacts.

Tasks are executed upon request ; they are never considered 'up-to-date'

tasks are specified as @task-name

## Rule

An expression which generates dependencies between files. Rules are evaluated at the module level.

## primary files

A file with no dependencies

## secondary files

A file which is fabricated by some formula

## generated files

A secondary file whose formula does not have any primary files

## dependency discovery aka rule generation

A process by which dependencies are generated from primary files. Requires language level parsing of
primary files.

Includes both weak dependencies between object files, and dependencies on external artifacts /
libraries

### example

```
# foo.c, part of program

#include "valyria.h"      // program^ : libvalyria^.so
#include "valyria/map.h"  // -none-
#include "common/snarf.h" // foo^.o : common/snarf^.o

#include "args.h"         // foo^.o : args^.o
```

# Processing Model

fab is always executed from a module directory. Traversing upwards, "common.fab" files are
applied in reverse order.

The immediate module is loaded, as well as all modules in child directories.

1. apply common fabfiles
2. load the immediate module
3. load modules in child directories

1. assemble the list of artifacts
2. for each artifact
 3. recursively apply rules to build the graph
4. for each primary file
 5. dependency discovery

## how fab resolves use / require references

files and directories can be referenced by name

fabfiles can be referenced by basename

files and directories in modules that are part of the build are indexed for reference

In addition, files and directories can be indexed for reference with the following config entries

index.recursive [ paths ]
index.directory [ paths ]
index.file      [ paths ]

## variables

variables contain lists of strings, which are rendered to a single value during interpolation

## fabfile rules

## inclusion

Types of inclusion

## use

Allows a module to import rules and formulas from a language/template

(Those rules are applied to *this* module)

ex. use C           // apply rules for building c sources to this module

## require

Declare a module-level dependency between the current module and another module. The other module
may be a child module, or a directory that is found by index/extern lookup

1. Resolve the reference to 1) a loaded module, or 2) an index/extern directory
2. Add its top-level path to my $FAB_REQUIRES list for cflags inclusion
3. Place the resolved reference at the top of the lookup scope in the current module

ex. require common      //
ex. require libxlinux

# example

```
var cflags      -g -O3    # applies globally
var cflags.xapi -DXAPI    # applies only when the variant = "xapi"
var cflags.eapi -DEAPI    # applies only when the variant = "eapi"

# apply rules for c sources to source files in this module
use c

# load the common module, and make its files available for this module to use
require fab/common

# load the libxlinux module
require libxlinux

# an artifact with two variants
artifact program.{xapi,eapi}?

# how to buid programs
rule program? : main?.o
{
  $cc -o $@ $^
}

# main.c -> main.xapi.o
# main.c -> main.eapi.o
rule %?.o : %.c
{
  $cc -o $@ $^
}

# install arifacts with : fab @install
#  @install -> program.eapi
#  @install -> program.xapi
task install : program?
{
}

# more rules
rule %.y : %.y.header, %.y.rules
rule %.tab.{c,h} : %.y
rule %.tokens.{c,h} : %.tab.c
rule %.lex.{c,h} : %.l
rule %.states.{c,h} : %.lex.h
```
