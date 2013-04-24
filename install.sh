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
# along with fab.  If not, see <http://www.gnu.org/licenses/>.

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='/../fab/install'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	install -d															//usr/local/bin
	install ./fabdev/fab												//usr/local/bin/fab
	chown fabsys:fabsys											//usr/local/bin/fab
	chmod u+s 															//usr/local/bin/fab
	chmod g+s 															//usr/local/bin/fab
	install ./fabdev/gcc-dep								//usr/local/bin/gcc-dep
	install -d 															//var/cache/fab
	chown fabsys:fabsys											//var/cache/fab
	install -d 															//var/tmp/fab
	chown fabsys:fabsys											//var/tmp/fab
	install -d															//usr/lib/fab/lib/std
	install -d															//usr/lib/fab/lib/std/c
	install -d															//usr/lib/fab/lib/std/l
	install -d															//usr/lib/fab/lib/std/y
	install ./fabdev/fablib/std/c.fab			//usr/lib/fab/lib/std/c.fab
	install ./fabdev/fablib/std/l.fab			//usr/lib/fab/lib/std/l.fab
	install ./fabdev/fablib/std/y.fab			//usr/lib/fab/lib/std/y.fab
	install -d															//usr/lib/fab/listwise
	install ./fabdev/fablw/op/fx/fx.so			//usr/lib/fab/listwise/fx.so
	install ./fabdev/fablw/op/fxc/fxc.so		//usr/lib/fab/listwise/fxc.so
	install ./fabdev/fablw/op/fxw/fxw.so		//usr/lib/fab/listwise/fxw.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='/../liblistwise/install'
fml_0_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	install -d																				//usr/lib/x86_64-linux-gnu
	install	./liblistwise/liblistwise.so													//usr/lib/x86_64-linux-gnu/liblistwise.so.0.1
	ln -vfs liblistwise.so.0.1													//usr/lib/x86_64-linux-gnu/liblistwise.so
	install -d																				//usr/include
	install -d																				//usr/include/listwise
	install ./liblistwise/listwise.h							//usr/include/listwise.h
	install ./liblistwise/listwise/operator.h			//usr/include/listwise/operator.h
	install ./liblistwise/listwise/generator.h 	//usr/include/listwise/generator.h
	install ./liblistwise/listwise/ops.h					//usr/include/listwise/ops.h
	install ./liblistwise/listwise/lstack.h 			//usr/include/listwise/lstack.h
	install ./liblistwise/listwise/object.h 			//usr/include/listwise/object.h

	install -d																				//usr/lib/listwise
	for x in ./liblistwise/op/bn/bn.so ./liblistwise/op/v/v.so ./liblistwise/op/xm/xm.so ./liblistwise/op/rp/rp.so ./liblistwise/op/rx/rx.so ./liblistwise/op/m/m.so ./liblistwise/op/-d/-d.so ./liblistwise/op/c/c.so ./liblistwise/op/xch/xch.so ./liblistwise/op/xsf/xsf.so ./liblistwise/op/-l/-l.so ./liblistwise/op/s/s.so ./liblistwise/op/xs/xs.so ./liblistwise/op/h/h.so ./liblistwise/op/pop/pop.so ./liblistwise/op/w/w.so ./liblistwise/op/o/o.so ./liblistwise/op/dj/dj.so ./liblistwise/op/dn/dn.so ./liblistwise/op/-f/-f.so ./liblistwise/op/cp/cp.so ./liblistwise/op/xmf/xmf.so ./liblistwise/op/u/u.so ./liblistwise/op/y/y.so ./liblistwise/op/ss/ss.so ./liblistwise/op/j/j.so ./liblistwise/op/ls/ls.so ./liblistwise/op/shift/shift.so ./liblistwise/op/lsr/lsr.so ./liblistwise/op/uu/uu.so ./liblistwise/op/r/r.so ./liblistwise/op/sn/sn.so ./liblistwise/op/d/d.so ./liblistwise/op/z/z.so ./liblistwise/op/fn/fn.so; do \
		install $x																			//usr/lib/listwise
	done


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='/../listwise/install'
fml_0_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	install -d										//usr/local/bin
	install ./listwisedev/listwise					//usr/local/bin/listwise
	ln -vfs listwise							//usr/local/bin/lw


  X=$?
  echo 2 1>&99
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
  ((SKP+=3))
else
  # launch stage 0.0
  exec 100>$tmp ; rm -f $tmp ; fml_0_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_0_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_0_2 & PIDS[2]=$!

  # harvest stage 0.0
  C=0
  while [[ $C != 3 ]]; do
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
