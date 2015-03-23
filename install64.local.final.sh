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

# fab --bslic-fab -k install64.local.final.sh +n @install.local.final -K destdir -K incdir -K bindir -K mandir -K libdir -K dlibdir -K lwopdir -K fablwopdir -K fabtmpdir -K fabipcdir -K fabinvokedir

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='@fabcore.install.base'
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
	rm -rf																							$destdir/$fabipcdir 2>/dev/null
	install -d 																					$destdir/$fabipcdir

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


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='@fabcore.install.final'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $destdir ]] || local destdir=''
  [[ $fablwopdir ]] || local fablwopdir='/usr/lib/fab/listwise'
  
	install ./fab/fablw/op/fi/fi.xapi.final.so	$destdir/$fablwopdir/fi.so
	install ./fab/fablw/op/fg/fg.xapi.final.so	$destdir/$fablwopdir/fg.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[2]='@fabcore.install.local'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $fabtmpdir ]] || local fabtmpdir='/var/tmp/fab'
  [[ $fabipcdir ]] || local fabipcdir='/var/run/fab'
  [[ $destdir ]] || local destdir=''
  
	chmod 777																						$destdir/$fabtmpdir
	chmod 777																						$destdir/$fabipcdir


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[3]='@libxapi.install.final.xapi'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																$destdir/$libdir
	install ./libxapi/libxapi-xapi.final.so						$destdir/$libdir/libxapi-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[4]='@libxlinux.install.final.xapi'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$libdir
	install ./libxlinux/libxlinux-xapi.final.so								$destdir/$libdir/libxlinux-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[5]='@libpstring.install.final.xapi'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$libdir
	install ./libpstring/libpstring-xapi.final.so							$destdir/$libdir/libpstring-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[6]='@fabcore.fab.install.base.final'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fab 2>/dev/null
	install ./fab/fab/fab.final													$destdir/$bindir/fab


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[7]='@fabcore.fabd.install.base.final'
fml_5_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fabd 2>/dev/null
	install ./fab/fabd/fabd.final													$destdir/$bindir/fabd


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[8]='@fabcore.fabw.install.base.final'
fml_5_2()
{
  exec 1>/dev/null
  exec 2>&102

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	install -d																			$destdir/$bindir
	rm																							$destdir/$bindir/fabw 2>/dev/null
	install ./fab/fabw/fabw.final													$destdir/$bindir/fabw


  X=$?
  echo 2 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[9]='@fabcore.fab.install.local.final'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	chown fabsys:fabsys															$destdir/$bindir/fab
	chmod ug+s																			$destdir/$bindir/fab


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[10]='@fabcore.fabd.install.local.final'
fml_6_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	chown fabsys:fabsys															$destdir/$bindir/fabd
	chmod ug+s																			$destdir/$bindir/fabd


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[11]='@fabcore.fabw.install.local.final'
fml_6_2()
{
  exec 1>/dev/null
  exec 2>&102

  [[ $bindir ]] || local bindir='/usr/local/bin'
  [[ $destdir ]] || local destdir=''
  
	chown fabsys:fabsys															$destdir/$bindir/fabw
	chmod ug+s																			$destdir/$bindir/fabw


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[12]='@liblistwise.install.final.xapi'
fml_6_3()
{
  exec 1>/dev/null
  exec 2>&103

  [[ $libdir ]] || local libdir='/usr/lib/x86_64-linux-gnu'
  [[ $destdir ]] || local destdir=''
  [[ $lwopdir ]] || local lwopdir='/usr/lib/listwise'
  
	install -d																									$destdir/$libdir
	install ./listwise/liblistwise/liblistwise-xapi.final.so													$destdir/$libdir/liblistwise-xapi.so

	rm -rf																											$destdir/$lwopdir 2>/dev/null
	install -d																									$destdir/$lwopdir

	install listwise/liblistwise/op/-X/-X.xapi.final.so					$destdir/$lwopdir/-X-xapi.so
	install listwise/liblistwise/op/C/C.xapi.final.so						$destdir/$lwopdir/C-xapi.so
	install listwise/liblistwise/op/b/b.xapi.final.so						$destdir/$lwopdir/b-xapi.so
	install listwise/liblistwise/op/bn/bn.xapi.final.so					$destdir/$lwopdir/bn-xapi.so
	install listwise/liblistwise/op/c/c.xapi.final.so						$destdir/$lwopdir/c-xapi.so
	install listwise/liblistwise/op/can/can.xapi.final.so				$destdir/$lwopdir/can-xapi.so
	install listwise/liblistwise/op/cons/cons.xapi.final.so			$destdir/$lwopdir/cons-xapi.so
	install listwise/liblistwise/op/cp/cp.xapi.final.so					$destdir/$lwopdir/cp-xapi.so
	install listwise/liblistwise/op/d/d.xapi.final.so						$destdir/$lwopdir/d-xapi.so
	install listwise/liblistwise/op/datef/datef.xapi.final.so		$destdir/$lwopdir/datef-xapi.so
	install listwise/liblistwise/op/dj/dj.xapi.final.so					$destdir/$lwopdir/dj-xapi.so
	install listwise/liblistwise/op/dn/dn.xapi.final.so					$destdir/$lwopdir/dn-xapi.so
	install listwise/liblistwise/op/f/f.xapi.final.so						$destdir/$lwopdir/f-xapi.so
	install listwise/liblistwise/op/fn/fn.xapi.final.so					$destdir/$lwopdir/fn-xapi.so
	install listwise/liblistwise/op/g/g.xapi.final.so						$destdir/$lwopdir/g-xapi.so
	install listwise/liblistwise/op/hl/hl.xapi.final.so					$destdir/$lwopdir/hl-xapi.so
	install listwise/liblistwise/op/j/j.xapi.final.so						$destdir/$lwopdir/j-xapi.so
	install listwise/liblistwise/op/l/l.xapi.final.so						$destdir/$lwopdir/l-xapi.so
	install listwise/liblistwise/op/ls/ls.xapi.final.so					$destdir/$lwopdir/ls-xapi.so
	install listwise/liblistwise/op/lx/lx.xapi.final.so					$destdir/$lwopdir/lx-xapi.so
	install listwise/liblistwise/op/o/o.xapi.final.so						$destdir/$lwopdir/o-xapi.so
	install listwise/liblistwise/op/p/p.xapi.final.so						$destdir/$lwopdir/p-xapi.so
	install listwise/liblistwise/op/pop/pop.xapi.final.so				$destdir/$lwopdir/pop-xapi.so
	install listwise/liblistwise/op/r/r.xapi.final.so						$destdir/$lwopdir/r-xapi.so
	install listwise/liblistwise/op/rb/rb.xapi.final.so					$destdir/$lwopdir/rb-xapi.so
	install listwise/liblistwise/op/re/re.xapi.final.so					$destdir/$lwopdir/re-xapi.so
	install listwise/liblistwise/op/rp/rp.xapi.final.so					$destdir/$lwopdir/rp-xapi.so
	install listwise/liblistwise/op/rx/rx.xapi.final.so					$destdir/$lwopdir/rx-xapi.so
	install listwise/liblistwise/op/s/s.xapi.final.so						$destdir/$lwopdir/s-xapi.so
	install listwise/liblistwise/op/shift/shift.xapi.final.so		$destdir/$lwopdir/shift-xapi.so
	install listwise/liblistwise/op/sort/sort.xapi.final.so			$destdir/$lwopdir/sort-xapi.so
	install listwise/liblistwise/op/stat/stat.xapi.final.so			$destdir/$lwopdir/stat-xapi.so
	install listwise/liblistwise/op/sx/sx.xapi.final.so					$destdir/$lwopdir/sx-xapi.so
	install listwise/liblistwise/op/t/t.xapi.final.so						$destdir/$lwopdir/t-xapi.so
	install listwise/liblistwise/op/u/u.xapi.final.so						$destdir/$lwopdir/u-xapi.so
	install listwise/liblistwise/op/up/up.xapi.final.so					$destdir/$lwopdir/up-xapi.so
	install listwise/liblistwise/op/uu/uu.xapi.final.so					$destdir/$lwopdir/uu-xapi.so
	install listwise/liblistwise/op/v/v.xapi.final.so						$destdir/$lwopdir/v-xapi.so
	install listwise/liblistwise/op/vf/vf.xapi.final.so					$destdir/$lwopdir/vf-xapi.so
	install listwise/liblistwise/op/vp/vp.xapi.final.so					$destdir/$lwopdir/vp-xapi.so
	install listwise/liblistwise/op/w/w.xapi.final.so						$destdir/$lwopdir/w-xapi.so
	install listwise/liblistwise/op/wvf/wvf.xapi.final.so				$destdir/$lwopdir/wvf-xapi.so
	install listwise/liblistwise/op/wvp/wvp.xapi.final.so				$destdir/$lwopdir/wvp-xapi.so
	install listwise/liblistwise/op/x/x.xapi.final.so						$destdir/$lwopdir/x-xapi.so
	install listwise/liblistwise/op/xch/xch.xapi.final.so				$destdir/$lwopdir/xch-xapi.so
	install listwise/liblistwise/op/y/y.xapi.final.so						$destdir/$lwopdir/y-xapi.so
	install listwise/liblistwise/op/z/z.xapi.final.so						$destdir/$lwopdir/z-xapi.so
	install listwise/liblistwise/op/ln/ln.xapi.final.so					$destdir/$lwopdir/ln-xapi.so


  X=$?
  echo 3 1>&99
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
    I=$((4+$idx))
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
    I=$((5+$idx))
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
    I=$((6+$idx))
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
  ((SKP+=4))
else
  # launch stage 6.0
  exec 100>$tmp ; rm -f $tmp ; fml_6_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_6_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_6_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_6_3 & PIDS[3]=$!

  # harvest stage 6.0
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((9+$idx))
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
