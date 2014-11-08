#!/bin/bash

# Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>
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
NAMES[0]='@listwise.install.final'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d										$destdir/$bindir
	install ./listwise/listwise/listwise.final		$destdir/$bindir/listwise
	ln -vfs listwise					$destdir/$bindir/lw


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='@fab.install.final'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $fabtmpdir ]] || local fabtmpdir='/var/tmp/fab'
  [[ $fabcachedir ]] || local fabcachedir='/var/cache/fab'
  [[ $destdir ]] || local destdir=''
  [[ $fabinvokedir ]] || local fabinvokedir='/usr/lib/fab/fablib'
  [[ $fablwopdir ]] || local fablwopdir='/usr/lib/fab/listwise'
  
	chown fabsys:fabsys		./fab/fab.final
	chmod ug+s 						./fab/fab.final
	install -d																			$destdir/$bindir
	install ./fab/fab.final													$destdir/$bindir/fab
	install -d 																			$destdir/$fabcachedir
	chown fabsys:fabsys															$destdir/$fabcachedir
	install -d 																			$destdir/$fabtmpdir
	chown fabsys:fabsys															$destdir/$fabtmpdir
	install -d 																			$destdir//var/run/fab
	chown fabsys:fabsys															$destdir//var/run/fab

	rm -rf																					$destdir/$fabinvokedir
	install -d																			$destdir/$fabinvokedir/std
	install -d																			$destdir/$fabinvokedir/std/flex
	install -d																			$destdir/$fabinvokedir/std/bison
	install -d																			$destdir/$fabinvokedir/std/xapi
	install ./fab/fablib/std/c.fab							$destdir/$fabinvokedir/std/c.fab
	install ./fab/fablib/std/l.fab							$destdir/$fabinvokedir/std/l.fab
	install ./fab/fablib/std/y.fab							$destdir/$fabinvokedir/std/y.fab
	install ./fab/fablib/std/flex/states.fab		$destdir/$fabinvokedir/std/flex/states.fab
	install ./fab/fablib/std/bison/tokens.fab		$destdir/$fabinvokedir/std/bison/tokens.fab
	install ./fab/fablib/std/xapi/errtab.fab		$destdir/$fabinvokedir/std/xapi/errtab.fab

	rm -rf 																					$destdir/$fablwopdir 2>/dev/null
	install -d																			$destdir/$fablwopdir
	install ./fab/fablw/op/fi/fi.final.so				$destdir/$fablwopdir/fi.so
	install ./fab/fablw/op/fg/fg.final.so				$destdir/$fablwopdir/fg.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[2]='@liblistwise.install.final'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $lwopdir ]] || local lwopdir='/usr/lib/listwise'
  
	install -d																				$destdir/$libdir
	install ./listwise/liblistwise/liblistwise.final.so										$destdir/$libdir/liblistwise.so

	rm -rf																						$destdir/$lwopdir 2>/dev/null
	install -d																				$destdir/$lwopdir

	install listwise/liblistwise/op/-X/-X.final.so					$destdir/$lwopdir/-X.so
	install listwise/liblistwise/op/C/C.final.so						$destdir/$lwopdir/C.so
	install listwise/liblistwise/op/b/b.final.so						$destdir/$lwopdir/b.so
	install listwise/liblistwise/op/bn/bn.final.so					$destdir/$lwopdir/bn.so
	install listwise/liblistwise/op/c/c.final.so						$destdir/$lwopdir/c.so
	install listwise/liblistwise/op/can/can.final.so				$destdir/$lwopdir/can.so
	install listwise/liblistwise/op/cons/cons.final.so			$destdir/$lwopdir/cons.so
	install listwise/liblistwise/op/cp/cp.final.so					$destdir/$lwopdir/cp.so
	install listwise/liblistwise/op/d/d.final.so						$destdir/$lwopdir/d.so
	install listwise/liblistwise/op/datef/datef.final.so		$destdir/$lwopdir/datef.so
	install listwise/liblistwise/op/dj/dj.final.so					$destdir/$lwopdir/dj.so
	install listwise/liblistwise/op/dn/dn.final.so					$destdir/$lwopdir/dn.so
	install listwise/liblistwise/op/f/f.final.so						$destdir/$lwopdir/f.so
	install listwise/liblistwise/op/fn/fn.final.so					$destdir/$lwopdir/fn.so
	install listwise/liblistwise/op/g/g.final.so						$destdir/$lwopdir/g.so
	install listwise/liblistwise/op/hl/hl.final.so					$destdir/$lwopdir/hl.so
	install listwise/liblistwise/op/j/j.final.so						$destdir/$lwopdir/j.so
	install listwise/liblistwise/op/l/l.final.so						$destdir/$lwopdir/l.so
	install listwise/liblistwise/op/ls/ls.final.so					$destdir/$lwopdir/ls.so
	install listwise/liblistwise/op/lx/lx.final.so					$destdir/$lwopdir/lx.so
	install listwise/liblistwise/op/o/o.final.so						$destdir/$lwopdir/o.so
	install listwise/liblistwise/op/p/p.final.so						$destdir/$lwopdir/p.so
	install listwise/liblistwise/op/pop/pop.final.so				$destdir/$lwopdir/pop.so
	install listwise/liblistwise/op/r/r.final.so						$destdir/$lwopdir/r.so
	install listwise/liblistwise/op/rb/rb.final.so					$destdir/$lwopdir/rb.so
	install listwise/liblistwise/op/re/re.final.so					$destdir/$lwopdir/re.so
	install listwise/liblistwise/op/rp/rp.final.so					$destdir/$lwopdir/rp.so
	install listwise/liblistwise/op/rx/rx.final.so					$destdir/$lwopdir/rx.so
	install listwise/liblistwise/op/s/s.final.so						$destdir/$lwopdir/s.so
	install listwise/liblistwise/op/shift/shift.final.so		$destdir/$lwopdir/shift.so
	install listwise/liblistwise/op/sort/sort.final.so			$destdir/$lwopdir/sort.so
	install listwise/liblistwise/op/stat/stat.final.so			$destdir/$lwopdir/stat.so
	install listwise/liblistwise/op/sx/sx.final.so					$destdir/$lwopdir/sx.so
	install listwise/liblistwise/op/t/t.final.so						$destdir/$lwopdir/t.so
	install listwise/liblistwise/op/u/u.final.so						$destdir/$lwopdir/u.so
	install listwise/liblistwise/op/up/up.final.so					$destdir/$lwopdir/up.so
	install listwise/liblistwise/op/uu/uu.final.so					$destdir/$lwopdir/uu.so
	install listwise/liblistwise/op/v/v.final.so						$destdir/$lwopdir/v.so
	install listwise/liblistwise/op/vf/vf.final.so					$destdir/$lwopdir/vf.so
	install listwise/liblistwise/op/vp/vp.final.so					$destdir/$lwopdir/vp.so
	install listwise/liblistwise/op/w/w.final.so						$destdir/$lwopdir/w.so
	install listwise/liblistwise/op/wv/wv.final.so					$destdir/$lwopdir/wv.so
	install listwise/liblistwise/op/wvf/wvf.final.so				$destdir/$lwopdir/wvf.so
	install listwise/liblistwise/op/wvp/wvp.final.so				$destdir/$lwopdir/wvp.so
	install listwise/liblistwise/op/wz/wz.final.so					$destdir/$lwopdir/wz.so
	install listwise/liblistwise/op/x/x.final.so						$destdir/$lwopdir/x.so
	install listwise/liblistwise/op/xch/xch.final.so				$destdir/$lwopdir/xch.so
	install listwise/liblistwise/op/y/y.final.so						$destdir/$lwopdir/y.so
	install listwise/liblistwise/op/z/z.final.so						$destdir/$lwopdir/z.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[3]='@libpstring.install.final'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																					$destdir/$libdir
	install ./libpstring/libpstring.final.so												$destdir/$libdir/libpstring.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[4]='@libxapi.install.final'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																$destdir/$libdir
	install ./libxapi/libxapi.final.so								$destdir/$libdir/libxapi.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[5]='@libxlinux.install.final'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																					$destdir/$libdir
	install ./libxlinux/libxlinux.final.so												$destdir/$libdir/libxlinux.so


  X=$?
  echo 4 1>&99
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
  ((SKP+=5))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_4 & PIDS[4]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 5 ]]; do
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

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
