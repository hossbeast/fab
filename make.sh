#!/bin/bash

# Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>
#
# This file is part of listwise.
# 
# listwise is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# listwise is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with listwise.  If not, see <http://www.gnu.org/licenses/>. */

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0

# formulas and names for stage 1
NAMES[0]='./cksum.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./cksum.c -o ./cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='./coll.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./coll.c -o ./coll.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='./parseint.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./parseint.c -o ./parseint.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='./pstring.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./pstring.c -o ./pstring.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='./unitstring.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./unitstring.c -o ./unitstring.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='./xmem.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./xmem.c -o ./xmem.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='./xstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -O3 -Wall -Werror -g -m64 -c ./xstring.c -o ./xstring.o


  X=$?
  echo 6 1>&99
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
  ((SKP+=7))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_6 & PIDS[6]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 7 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((0+$idx))
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
