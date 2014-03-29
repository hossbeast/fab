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
NAMES[0]='@listwise.install'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d										$destdir/$bindir
	install listwise/../common/../listwise/listwise					$destdir/$bindir/listwise
	ln -vfs listwise							$destdir/$bindir/lw


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='@liblistwise.install'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $incdir ]] || local incdir='/usr/include'
  [[ $lwopdir ]] || local lwopdir='/home/todd/fab/liblistwise/op'
  
	install -d																				$destdir/$libdir
	install liblistwise/../common/../liblistwise/liblistwise.so													$destdir/$libdir/liblistwise.so
	install liblistwise/../common/../liblistwise/listwise.h							$destdir/$incdir/listwise.h

	install -d																				$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/fs.h						$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/generator.h		$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/internal.h			$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/iterate.h			$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/lwx.h					$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/object.h				$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/operator.h			$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/ops.h					$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/re.h						$destdir/$incdir/listwise
	install liblistwise/../common/../liblistwise/listwise/xtra.h					$destdir/$incdir/listwise

	rm -rf																						$destdir/$lwopdir 2>/dev/null
	install -d																				$destdir/$lwopdir
	for x in liblistwise/../common/../liblistwise/op/wz/wz.so liblistwise/../common/../liblistwise/op/z/z.so liblistwise/../common/../liblistwise/op/o/o.so liblistwise/../common/../liblistwise/op/vf/vf.so liblistwise/../common/../liblistwise/op/wvp/wvp.so liblistwise/../common/../liblistwise/op/sort/sort.so liblistwise/../common/../liblistwise/op/w/w.so liblistwise/../common/../liblistwise/op/shift/shift.so liblistwise/../common/../liblistwise/op/dj/dj.so liblistwise/../common/../liblistwise/op/uu/uu.so liblistwise/../common/../liblistwise/op/u/u.so liblistwise/../common/../liblistwise/op/bn/bn.so liblistwise/../common/../liblistwise/op/re/re.so liblistwise/../common/../liblistwise/op/rp/rp.so liblistwise/../common/../liblistwise/op/f/f.so liblistwise/../common/../liblistwise/op/ls/ls.so liblistwise/../common/../liblistwise/op/lx/lx.so liblistwise/../common/../liblistwise/op/hl/hl.so liblistwise/../common/../liblistwise/op/rb/rb.so liblistwise/../common/../liblistwise/op/j/j.so liblistwise/../common/../liblistwise/op/wv/wv.so liblistwise/../common/../liblistwise/op/-X/-X.so liblistwise/../common/../liblistwise/op/up/up.so liblistwise/../common/../liblistwise/op/cp/cp.so liblistwise/../common/../liblistwise/op/p/p.so liblistwise/../common/../liblistwise/op/c/c.so liblistwise/../common/../liblistwise/op/r/r.so liblistwise/../common/../liblistwise/op/xch/xch.so liblistwise/../common/../liblistwise/op/t/t.so liblistwise/../common/../liblistwise/op/s/s.so liblistwise/../common/../liblistwise/op/wvf/wvf.so liblistwise/../common/../liblistwise/op/dn/dn.so liblistwise/../common/../liblistwise/op/stat/stat.so liblistwise/../common/../liblistwise/op/g/g.so liblistwise/../common/../liblistwise/op/d/d.so liblistwise/../common/../liblistwise/op/vp/vp.so liblistwise/../common/../liblistwise/op/can/can.so liblistwise/../common/../liblistwise/op/x/x.so liblistwise/../common/../liblistwise/op/b/b.so liblistwise/../common/../liblistwise/op/C/C.so liblistwise/../common/../liblistwise/op/rx/rx.so liblistwise/../common/../liblistwise/op/y/y.so liblistwise/../common/../liblistwise/op/fn/fn.so liblistwise/../common/../liblistwise/op/pop/pop.so liblistwise/../common/../liblistwise/op/sx/sx.so liblistwise/../common/../liblistwise/op/l/l.so liblistwise/../common/../liblistwise/op/v/v.so; do \
		install $x																			$destdir/$lwopdir
	done


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[2]='@libxlinux.install'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $incdir ]] || local incdir='/usr/include'
  
	install -d																					$destdir/$libdir
	install libxlinux/../common/../libxlinux/libxlinux.so															$destdir/$libdir/libxlinux.so
	install libxlinux/../common/../libxlinux/xlinux.h										$destdir/$incdir/xlinux.h

	install -d																					$destdir/$incdir/xlinux
	install libxlinux/../common/../libxlinux/xlinux/wstdlib/wstdlib.h		$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xdirent/xdirent.h		$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xdlfcn/xdlfcn.h			$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xfcntl/xfcntl.h			$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xftw/xftw.h					$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xgrp/xgrp.h					$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xmman/xmman.h				$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xpwd/xpwd.h					$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xstat/xstat.h				$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xstdlib/xstdlib.h		$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xtime/xtime.h				$destdir/$incdir/xlinux/
	install libxlinux/../common/../libxlinux/xlinux/xunistd/xunistd.h		$destdir/$incdir/xlinux/


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[3]='@fab.install'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $fabtmpdir ]] || local fabtmpdir='/var/tmp/fab'
  [[ $fabcachedir ]] || local fabcachedir='/var/cache/fab'
  [[ $destdir ]] || local destdir=''
  [[ $fabinvokedir ]] || local fabinvokedir='fab/../common/../fab/fablib'
  [[ $fablwopdir ]] || local fablwopdir='fab/../common/../fab/fablw/op'
  
	chown fabsys:fabsys		fab/../common/../fab/fab
	chmod u+s 						fab/../common/../fab/fab
	chmod g+s 						fab/../common/../fab/fab
	install -d																			$destdir/$bindir
	install fab/../common/../fab/fab																$destdir/$bindir
	install fab/../common/../fab/gcc-dep												$destdir/$bindir
	install -d 																			$destdir/$fabcachedir
	chown fabsys:fabsys															$destdir/$fabcachedir
	install -d 																			$destdir/$fabtmpdir
	chown fabsys:fabsys															$destdir/$fabtmpdir
	install -d 																			$destdir//var/run/fab
	chown fabsys:fabsys															$destdir//var/run/fab
	install -d																			$destdir/$fabinvokedir/std
	install -d																			$destdir/$fabinvokedir/std/c
	install -d																			$destdir/$fabinvokedir/std/l
	install -d																			$destdir/$fabinvokedir/std/y
	install -d																			$destdir/$fabinvokedir/std/flex
	install -d																			$destdir/$fabinvokedir/std/bison
	install fab/../common/../fab/fablib/std/c.fab							$destdir/$fabinvokedir/std/c.fab
	install fab/../common/../fab/fablib/std/l.fab							$destdir/$fabinvokedir/std/l.fab
	install fab/../common/../fab/fablib/std/y.fab							$destdir/$fabinvokedir/std/y.fab
	install fab/../common/../fab/fablib/std/flex/states.fab		$destdir/$fabinvokedir/std/flex/states.fab
	install fab/../common/../fab/fablib/std/bison/tokens.fab		$destdir/$fabinvokedir/std/bison/tokens.fab
  

	rm -rf 																					$destdir/$fablwopdir 2>/dev/null
	install -d																			$destdir/$fablwopdir
	install fab/../common/../fab/fablw/op/fi/fi.so							$destdir/$fablwopdir
	install fab/../common/../fab/fablw/op/fg/fg.so							$destdir/$fablwopdir


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
  ((SKP+=2))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 2 ]]; do
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
    I=$((3+$idx))
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
