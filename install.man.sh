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

# fab --bslic-fab -k install.man.sh +n @install.man -K destdir -K incdir -K bindir -K mandir -K libdir -K dlibdir -K lwopdir -K fablwopdir -K fabtmpdir -K fabipcdir -K fabinvokedir

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='@fabcore.install.man1'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  [[ $destdir ]] || local destdir=''
  [[ $mandir ]] || local mandir='/usr/share/man'
  
	install -d 																		$destdir/$mandir/man1
	install ./fab/man/fab.1.gz						$destdir/$mandir/man1/fab.1.gz


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='@listwise.install.man1'
fml_0_1()
{
  exec 1>/dev/null
  exec 2>&101

  [[ $destdir ]] || local destdir=''
  [[ $mandir ]] || local mandir='/usr/share/man'
  
	install -d 																		$destdir/$mandir/man1
	install ./listwise/listwise/man/listwise.1.gz			$destdir/$mandir/man1/listwise.1.gz
	ln -fs listwise.1.gz													$destdir/$mandir/man1/lw.1.gz


  X=$?
  echo 1 1>&99
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
  ((SKP+=2))
else
  # launch stage 0.0
  exec 100>$tmp ; rm -f $tmp ; fml_0_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_0_1 & PIDS[1]=$!

  # harvest stage 0.0
  C=0
  while [[ $C != 2 ]]; do
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