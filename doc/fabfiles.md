# fabfile format

fab maintains an up-to-date picture of the file system at all times via notify

for directory trees which are not marked for notify, fab will scan them to
update its picture of the filesystem on each request

newly removed files are pruned from the graph

newly discovered files are evaluated against rules and discovery performed on
them

## goals

### composability

fab can build all targets in the current directory, recursively
at each level, building is fully defined by the fabfile at the nearest level
fabfiles share common logic (variant definitions, meta targets, build flags) via inclusion

### inclusion

can include a directory outside the current directory hierarchy (common)
  include directory (directory/fabfile)

can include
  include common logic (directory/vars.fab)

## supported use cases

### standalone formulas

formula in a separate file, can be invoked outside of fab

supports the notion of executing in the fabd process in an embedded interpreter,
which would be done based on file type detection of the shebang line

supports variants

 #!perl

 @cmd = (
     "gcc"
   , "-o"
   , "$target"
   , "$primary"
   , "-c"
   , "$cflags"
 );

 exec { $cmd[0] } @cmd;

### inline formulas

for ad-hoc targets, you can inline the build formula in the fabfile
