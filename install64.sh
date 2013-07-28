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
NAMES[0]='/../listwise/install'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d										$destdir/$bindir
	install listwise/listwise					$destdir/$bindir/listwise
	ln -vfs listwise							$destdir/$bindir/lw


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='/../liblistwise/install'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $incdir ]] || local incdir='/usr/include'
  [[ $lwopdir ]] || local lwopdir='/usr/lib/listwise'
  
	install -d																				$destdir/$libdir
	install	liblistwise/liblistwise.so													$destdir/$libdir/liblistwise.so.0.1
	ln -vfs liblistwise.so.0.1																$destdir/$libdir/liblistwise.so
	install -d																				$destdir/$incdir/listwise
	install liblistwise/listwise.h							$destdir/$incdir/listwise.h
	install liblistwise/listwise/operator.h			$destdir/$incdir/listwise/operator.h
	install liblistwise/listwise/generator.h 	$destdir/$incdir/listwise/generator.h
	install liblistwise/listwise/ops.h					$destdir/$incdir/listwise/ops.h
	install liblistwise/listwise/lstack.h 			$destdir/$incdir/listwise/lstack.h
	install liblistwise/listwise/object.h 			$destdir/$incdir/listwise/object.h

	rm -rf																						$destdir/$lwopdir 2>/dev/null
	install -d																				$destdir/$lwopdir
	for x in liblistwise/op/h/h.so liblistwise/op/r/r.so liblistwise/op/xm/xm.so liblistwise/op/sn/sn.so liblistwise/op/rp/rp.so liblistwise/op/xs/xs.so liblistwise/op/xsf/xsf.so liblistwise/op/xmf/xmf.so liblistwise/op/fn/fn.so liblistwise/op/bn/bn.so liblistwise/op/dn/dn.so liblistwise/op/m/m.so liblistwise/op/z/z.so liblistwise/op/rx/rx.so liblistwise/op/xch/xch.so liblistwise/op/j/j.so liblistwise/op/dj/dj.so liblistwise/op/cp/cp.so liblistwise/op/-X/-X.so liblistwise/op/s/s.so liblistwise/op/pop/pop.so liblistwise/op/shift/shift.so liblistwise/op/u/u.so liblistwise/op/d/d.so liblistwise/op/w/w.so liblistwise/op/ls/ls.so liblistwise/op/o/o.so liblistwise/op/c/c.so liblistwise/op/uu/uu.so liblistwise/op/v/v.so liblistwise/op/y/y.so liblistwise/op/ss/ss.so; do \
		install $x																			$destdir/$lwopdir
	done


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[2]='/../fab/install'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $fabtmpdir ]] || local fabtmpdir='/var/tmp/fab'
  [[ $fabcachedir ]] || local fabcachedir='/var/cache/fab'
  [[ $destdir ]] || local destdir=''
  [[ $fabinvokedir ]] || local fabinvokedir='/usr/lib/fab/listwise'
  [[ $fablwopdir ]] || local fablwopdir='/usr/lib/fab/lib'
  
	install -d															$destdir/$bindir
	install fab/fab												$destdir/$bindir/fab
	chown fabsys:fabsys											$destdir/$bindir/fab
	chmod u+s 															$destdir/$bindir/fab
	chmod g+s 															$destdir/$bindir/fab
	install fab/gcc-dep								$destdir/$bindir/gcc-dep
	install -d 															$destdir/$fabcachedir
	chown fabsys:fabsys											$destdir/$fabcachedir
	install -d 															$destdir/$fabtmpdir
	chown fabsys:fabsys											$destdir/$fabtmpdir
	install -d															$destdir/$fabinvokedir/std
	install -d															$destdir/$fabinvokedir/std/c
	install -d															$destdir/$fabinvokedir/std/l
	install -d															$destdir/$fabinvokedir/std/y
	install fab/fablib/std/c.fab			$destdir/$fabinvokedir/std/c.fab
	install fab/fablib/std/l.fab			$destdir/$fabinvokedir/std/l.fab
	install fab/fablib/std/y.fab			$destdir/$fabinvokedir/std/y.fab

	rm -rf 																	$destdir/$fablwopdir 2>/dev/null
	install -d															$destdir/$fablwopdir
	install fab/fablw/op/fi/fi.so			$destdir/$fablwopdir
	install fab/fablw/op/fg/fg.so			$destdir/$fablwopdir


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

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((1+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 2.0
  exec 100>$tmp ; rm -f $tmp ; fml_2_0 & PIDS[0]=$!

  # harvest stage 2.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((2+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
