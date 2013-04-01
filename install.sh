#!/bin/bash

# Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>
#
# This file is part of fab.
#
# fab is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# fab is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with fab.  If not, see <http://www.gnu.org/licenses/>. */

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
	install -d															//usr/lib/fab/lib/std
	install -d															//usr/lib/fab/lib/std/c
	install -d															//usr/lib/fab/lib/std/l
	install -d															//usr/lib/fab/lib/std/y
	install ./fablib/std/c.fab			//usr/lib/fab/lib/std/c.fab
	install ./fablib/std/l.fab			//usr/lib/fab/lib/std/l.fab
	install ./fablib/std/y.fab			//usr/lib/fab/lib/std/y.fab
	install -d															//usr/lib/fab/listwise
	install ./fablw/op/fx/fx.so			//usr/lib/fab/listwise/fx.so
	install ./fablw/op/fxc/fxc.so		//usr/lib/fab/listwise/fxc.so
	install ./fablw/op/fxw/fxw.so		//usr/lib/fab/listwise/fxw.so


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
