#!/bin/bash

# A build script made by fab v0.4.2.0
#  fab is free software released under the GNU General Public License.
#
#  As a special exception, build scripts made by fab v0.4.2.0 (including this
#  build script) are excluded from the license covering fab itself, even
#  if substantial portions of the fab source code are copied verbatim into
#  the build script. You may create a larger work that contains part or all
#  of the build script and distribute that work under terms of your choice

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='/../install'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	install -d															//usr/local/bin
	install ./fab												//usr/local/bin/fab
	chown fabsys:fabsys											//usr/local/bin/fab
	chmod u+s 															//usr/local/bin/fab
	chmod g+s 															//usr/local/bin/fab
	install ./gcc-dep								//usr/local/bin/gcc-dep
	install -d 															//var/cache/fab
	chown fabsys:fabsys											//var/cache/fab
	install -d 															//var/tmp/fab
	chown fabsys:fabsys											//var/tmp/fab
	install -d															//usr/lib/fab/listwise/std
	install -d															//usr/lib/fab/listwise/std/c
	install -d															//usr/lib/fab/listwise/std/l
	install -d															//usr/lib/fab/listwise/std/y
	install ./fablib/std/c.fab			//usr/lib/fab/listwise/std/c.fab
	install ./fablib/std/l.fab			//usr/lib/fab/listwise/std/l.fab
	install ./fablib/std/y.fab			//usr/lib/fab/listwise/std/y.fab

	rm -rf 																	//usr/lib/fab/lib 2>/dev/null
	install -d															//usr/lib/fab/lib
	install ./fablw/op/fi/fi.so			//usr/lib/fab/lib
	install ./fablw/op/fg/fg.so			//usr/lib/fab/lib


  X=$?
  echo 0 1>&99
  exit $X
}

# temp filename - only need the 1
tmp=`mktemp --dry-run`
# setup named pipe to manage process multiplexing

mkfifo --mode=0700 $tmp
exec 99<>$tmp
rm -f $tmp

# results aggregation
WIN=0
DIE=0
SKP=0

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 0.0
  exec 100>$tmp ; rm -f $tmp ; fml_0_0 & PIDS[0]=$!

  # harvest stage 0.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((0+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 0 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
