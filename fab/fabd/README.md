fabd (fab daemon)
===

The fab daemon is a long-running process that maintains the dependency graph
for a project as identified by the initial fabfile directory. It runs as the
fabsys user and carries out tasks appropriate to those credentials such as
manipulating files in the tmp and cache directories
