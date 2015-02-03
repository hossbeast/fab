#!/bin/bash

# Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>
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

# fab --bslic-fab -k install64.final.sh +n @install.final -K destdir -K incdir -K bindir -K mandir -K libdir -K dlibdir -K lwopdir -K fablwopdir -K fabtmpdir -K fabipcdir -K fabinvokedir

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='@fabcore.installation'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $fabtmpdir ]] || local fabtmpdir='/var/tmp/fab'
  [[ $fabipcdir ]] || local fabipcdir='/var/run/fab'
  [[ $destdir ]] || local destdir=''
  [[ $fabinvokedir ]] || local fabinvokedir='/usr/lib/fab/fablib'
  [[ $fablwopdir ]] || local fablwopdir='/usr/lib/fab/listwise'
  
	install -d																					$destdir/$bindir
	install ./fab/util/gcc-dep 													$destdir/$bindir

	rm -rf																							$destdir/$fabtmpdir 2>/dev/null
	install -d 																					$destdir/$fabtmpdir
	chmod 777																						$destdir/$fabtmpdir
	rm -rf																							$destdir/$fabipcdir 2>/dev/null
	install -d 																					$destdir/$fabipcdir
	chmod 777																						$destdir/$fabipcdir

	rm -rf																							$destdir/$fabinvokedir 2>/dev/null
	install -d																					$destdir/$fabinvokedir/std
	install -d																					$destdir/$fabinvokedir/std/flex
	install -d																					$destdir/$fabinvokedir/std/bison
	install -d																					$destdir/$fabinvokedir/std/xapi
	install ./fab/fablib/std/c.fab							$destdir/$fabinvokedir/std/c.fab
	install ./fab/fablib/std/l.fab							$destdir/$fabinvokedir/std/l.fab
	install ./fab/fablib/std/y.fab							$destdir/$fabinvokedir/std/y.fab
	install ./fab/fablib/std/flex/states.fab		$destdir/$fabinvokedir/std/flex/states.fab
	install ./fab/fablib/std/bison/tokens.fab		$destdir/$fabinvokedir/std/bison/tokens.fab
	install ./fab/fablib/std/xapi/errtab.fab		$destdir/$fabinvokedir/std/xapi/errtab.fab

	rm -rf 																							$destdir/$fablwopdir 2>/dev/null
	install -d																					$destdir/$fablwopdir
	install ./fab/fablw/op/fi/fi.final.so				$destdir/$fablwopdir/fi.so
	install ./fab/fablw/op/fg/fg.final.so				$destdir/$fablwopdir/fg.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='@libxapi.install.final'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																$destdir/$libdir
	install ./libxapi/libxapi.final.so								$destdir/$libdir/libxapi.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[2]='@libxlinux.install.final'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																					$destdir/$libdir
	install ./libxlinux/libxlinux.final.so												$destdir/$libdir/libxlinux.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[3]='@libpstring.install.final'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																					$destdir/$libdir
	install ./libpstring/libpstring.final.so												$destdir/$libdir/libpstring.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[4]='@listwise.install.final'
fml_4_0()
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


# formulas and names for stage 5
NAMES[5]='@fabcore.fab.install.final'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fab
	install ./fab/fab/fab.final													$destdir/$bindir/fab
	chown fabsys:fabsys															$destdir/$bindir/fab
	chmod ug+s																			$destdir/$bindir/fab


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[6]='@fabcore.fabd.install.final'
fml_5_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fabd
	install ./fab/fabd/fabd.final													$destdir/$bindir/fabd
	chown fabsys:fabsys															$destdir/$bindir/fabd
	chmod ug+s																			$destdir/$bindir/fabd


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[7]='@fabcore.fabw.install.final'
fml_5_2()
{
  exec 1>/dev/null
  exec 2>&102

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fabw
	install ./fab/fabw/fabw.final													$destdir/$bindir/fabw
	chown fabsys:fabsys															$destdir/$bindir/fabw
	chmod ug+s																			$destdir/$bindir/fabw


  X=$?
  echo 2 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[8]='@liblistwise.install.final'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $lwopdir ]] || local lwopdir='/usr/lib/listwise'
  
	install -d																							$destdir/$libdir
	install ./listwise/liblistwise/liblistwise.final.so													$destdir/$libdir/liblistwise.so

	rm -rf																									$destdir/$lwopdir 2>/dev/null
	install -d																							$destdir/$lwopdir

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
	install listwise/liblistwise/op/wvf/wvf.final.so				$destdir/$lwopdir/wvf.so
	install listwise/liblistwise/op/wvp/wvp.final.so				$destdir/$lwopdir/wvp.so
	install listwise/liblistwise/op/x/x.final.so						$destdir/$lwopdir/x.so
	install listwise/liblistwise/op/xch/xch.final.so				$destdir/$lwopdir/xch.so
	install listwise/liblistwise/op/y/y.final.so						$destdir/$lwopdir/y.so
	install listwise/liblistwise/op/z/z.final.so						$destdir/$lwopdir/z.so


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

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 3.0
  exec 100>$tmp ; rm -f $tmp ; fml_3_0 & PIDS[0]=$!

  # harvest stage 3.0
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
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 4.0
  exec 100>$tmp ; rm -f $tmp ; fml_4_0 & PIDS[0]=$!

  # harvest stage 4.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((4+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=3))
else
  # launch stage 5.0
  exec 100>$tmp ; rm -f $tmp ; fml_5_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_5_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_5_2 & PIDS[2]=$!

  # harvest stage 5.0
  C=0
  while [[ $C != 3 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((5+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 5 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 6.0
  exec 100>$tmp ; rm -f $tmp ; fml_6_0 & PIDS[0]=$!

  # harvest stage 6.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((8+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 6 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
