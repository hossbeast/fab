# bam integration tests

These tests exercise the devel builds of bam, and bamd.

As a rule, every test should run in both stat, and notify mode. This ensures that user-facing behavior is the same,
regardless of whether stat invalidations, or notify invalidations, are being used.

Notify invalidations are cheaper, and allow for reconciling and rebuilding automatically, in response to changes to
filesystem state. But there is no guarantee that changes are processed via notify invalidations until after two sweeper
periods have elapsed.

For this reason, it's critical that in all tests, after filesystem state has been mutated, an explicit reconcile() call
is made, before making any assertions.
