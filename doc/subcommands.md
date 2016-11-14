# porcelain (high level) commands

## building

build
 execute to commands to bring targets up to date
 --nofile
 --exact
 --dryrun     - construct the build plan, do not execute it
 --force | -f - mark the build targets as out of date
 -B           - mark all nodes as out of date

autobuild
 build whenever targets dependencies become out of date
 --detatch

buildscript
 write a script to carry out the build commands

add
 specify build targets
 --nofile
 --exact

## inspecting the dependency graph

show
 display information about a node

query
 list nodes matching criteria

## ancillary

config
 query the effective config tree

# plumbing (low level) commands

invalidate
 mark nodes matching criteria as out of date
