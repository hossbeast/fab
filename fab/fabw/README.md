fabw (fab watcher)
===

The fab watcher exists for a single purpose ; to be the parent process of fabd
which allows it to receive the SIGCHLD signal if that process dies ; and, in
that case, to send a signal to the fab process so that it does not wait
indefinitely for a response

fabw does not participate in signal-exchange

It may be removed at some future date, or relegated to debug/devel builds only
