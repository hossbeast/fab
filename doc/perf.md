perf record -F max -g /home/todd/fab/fab/fab/fab.trace.xapi metadata
perf script | ~/FlameGraph/stackcollapse-perf.pl > out.perf-folded
~/FlameGraph/flamegraph.pl out.perf-folded > /tmp/perf.svg 
