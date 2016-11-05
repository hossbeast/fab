# fabd (fab daemon)

The fab daemon is a long-running process that maintains the dependency graph
for a project as identified by the initial fabfile directory. It runs as the
fabsys user and carries out tasks appropriate to those credentials such as
manipulating files in the tmp and cache directories

duties:
   - masters the dependency graph
r   - processes fabfiles to create the dependency graph
    - consumes discovery output to update the graph
   - masters the fabfile list
   - process selectors
    - (no fs access)
   - creates the buildplan
    - creates the buildscript file
   - orchestrates buildplan execution
   - orchestrates dependency discovery

# core dumps

If fabd dies and dumps core, the client should print a message like, "fabd terminated abnormally".

fabd maintains cwd of FABIPCDIR/hash/fabd to make it easy to find these coredumps.

To enable coredumps:
* echo core > /proc/sys/kernel/core_pattern
* ulimit -c unlimited
