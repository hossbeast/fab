# fabd (fab daemon)

# core dumps

If fabd dies and dumps core, the client should print a message like, "fabd terminated abnormally".

fabd maintains cwd of FABIPCDIR/hash/fabd to make it easy to find these coredumps.

To enable coredumps:
* echo core > /proc/sys/kernel/core_pattern
* ulimit -c unlimited
