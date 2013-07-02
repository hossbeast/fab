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

# formulas and names for stage 1
NAMES[0]='./common/cksum.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/cksum.c -o ./common/cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='./common/coll.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/coll.c -o ./common/coll.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='./common/coll.pic.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/coll.c -o ./common/coll.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='./common/parseint.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/parseint.c -o ./common/parseint.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='./common/parseint.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/parseint.c -o ./common/parseint.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[5]='./common/pstring.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/pstring.c -o ./common/pstring.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[6]='./common/unitstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/unitstring.c -o ./common/unitstring.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[7]='./common/xmem.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/xmem.c -o ./common/xmem.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[8]='./common/xmem.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/xmem.c -o ./common/xmem.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[9]='./common/xstring.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/xstring.c -o ./common/xstring.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[10]='./common/xstring.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/xstring.c -o ./common/xstring.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[11]='./fabdev/canon/canon.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/canon/canon.c -o ./fabdev/canon/canon.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[12]='./fabdev/depblock/depblock.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/depblock/depblock.c -o ./fabdev/depblock/depblock.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[13]='./fabdev/dirutil/dirutil.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/dirutil/dirutil.c -o ./fabdev/dirutil/dirutil.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[14]='./fabdev/fablw/op/fg/fg.pic.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c ./fabdev/fablw/op/fg/fg.c -o ./fabdev/fablw/op/fg/fg.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[15]='./fabdev/fablw/op/fi/fi.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c ./fabdev/fablw/op/fi/fi.c -o ./fabdev/fablw/op/fi/fi.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[16]='{./fabdev/ff/ff.dsc.tab.h, ./fabdev/ff/ff.dsc.tab.c}'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o ./fabdev/ff/ff.dsc.tab.c -d	./fabdev/ff/ff.dsc.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[17]='{./fabdev/ff/ff.lex.h, ./fabdev/ff/ff.lex.c}'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	  flex  -o ./fabdev/ff/ff.lex.c  --header-file=./fabdev/ff/ff.lex.h  ./fabdev/ff/ff.l


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[18]='{./fabdev/ff/ff.list.tab.h, ./fabdev/ff/ff.list.tab.c}'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&102

  
	bison --warnings=error -o ./fabdev/ff/ff.list.tab.c -d	./fabdev/ff/ff.list.y


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[19]='{./fabdev/ff/ff.tab.h, ./fabdev/ff/ff.tab.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&103

  
	bison --warnings=error -o ./fabdev/ff/ff.tab.c -d	./fabdev/ff/ff.y


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[20]='{./fabdev/ff/ff.var.tab.h, ./fabdev/ff/ff.var.tab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o ./fabdev/ff/ff.var.tab.c -d	./fabdev/ff/ff.var.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[21]='./fabdev/gn/gnlw.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/gn/gnlw.c -o ./fabdev/gn/gnlw.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[22]='./fabdev/hashblock/hashblock.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/hashblock/hashblock.c -o ./fabdev/hashblock/hashblock.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[23]='./fabdev/identity/identity.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/identity/identity.c -o ./fabdev/identity/identity.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[24]='./fabdev/log/log.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/log/log.c -o ./fabdev/log/log.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[25]='./fabdev/lwutil/lwutil.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/lwutil/lwutil.c -o ./fabdev/lwutil/lwutil.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[26]='./fabdev/map/map.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/map/map.c -o ./fabdev/map/map.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[27]='./fabdev/path/path.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/path/path.c -o ./fabdev/path/path.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[28]='./fabdev/strstack/strstack.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/strstack/strstack.c -o ./fabdev/strstack/strstack.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[29]='./fabdev/traverse/traverse.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/traverse/traverse.c -o ./fabdev/traverse/traverse.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[30]='./liblistwise/apidata/apidata.pic.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/apidata/apidata.c -o ./liblistwise/apidata/apidata.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[31]='./liblistwise/exec/exec.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/exec/exec.c -o ./liblistwise/exec/exec.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[32]='{./liblistwise/generator/generator.lex.h, ./liblistwise/generator/generator.lex.c}'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	  flex  -o ./liblistwise/generator/generator.lex.c  --header-file=./liblistwise/generator/generator.lex.h  ./liblistwise/generator/generator.l


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[33]='{./liblistwise/generator/generator.tab.h, ./liblistwise/generator/generator.tab.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o ./liblistwise/generator/generator.tab.c -d	./liblistwise/generator/generator.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[34]='./liblistwise/idx/idx.pic.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/idx/idx.c -o ./liblistwise/idx/idx.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[35]='./liblistwise/last/last.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/last/last.c -o ./liblistwise/last/last.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[36]='./liblistwise/lstack/lstack.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/lstack/lstack.c -o ./liblistwise/lstack/lstack.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[37]='./liblistwise/object/object.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/object/object.c -o ./liblistwise/object/object.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[38]='./liblistwise/op/-X/-X.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/-X/-X.c -o ./liblistwise/op/-X/-X.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[39]='./liblistwise/op/bn/bn.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/bn/bn.c -o ./liblistwise/op/bn/bn.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[40]='./liblistwise/op/c/c.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/c/c.c -o ./liblistwise/op/c/c.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[41]='./liblistwise/op/cp/cp.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/cp/cp.c -o ./liblistwise/op/cp/cp.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[42]='./liblistwise/op/d/d.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/d/d.c -o ./liblistwise/op/d/d.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[43]='./liblistwise/op/dj/dj.pic.o'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/dj/dj.c -o ./liblistwise/op/dj/dj.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[44]='./liblistwise/op/dn/dn.pic.o'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/dn/dn.c -o ./liblistwise/op/dn/dn.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[45]='./liblistwise/op/fn/fn.pic.o'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/fn/fn.c -o ./liblistwise/op/fn/fn.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[46]='./liblistwise/op/h/h.pic.o'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/h/h.c -o ./liblistwise/op/h/h.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[47]='./liblistwise/op/j/j.pic.o'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/j/j.c -o ./liblistwise/op/j/j.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[48]='./liblistwise/op/ls/ls.pic.o'
fml_1_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/ls/ls.c -o ./liblistwise/op/ls/ls.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[49]='./liblistwise/op/m/m.pic.o'
fml_1_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/m/m.c -o ./liblistwise/op/m/m.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[50]='./liblistwise/op/o/o.pic.o'
fml_1_50()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/o/o.c -o ./liblistwise/op/o/o.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[51]='./liblistwise/op/op.pic.o'
fml_1_51()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/op.c -o ./liblistwise/op/op.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[52]='./liblistwise/op/pop/pop.pic.o'
fml_1_52()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/pop/pop.c -o ./liblistwise/op/pop/pop.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[53]='./liblistwise/op/r/r.pic.o'
fml_1_53()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/r/r.c -o ./liblistwise/op/r/r.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[54]='./liblistwise/op/rp/rp.pic.o'
fml_1_54()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/rp/rp.c -o ./liblistwise/op/rp/rp.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[55]='./liblistwise/op/rx/rx.pic.o'
fml_1_55()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/rx/rx.c -o ./liblistwise/op/rx/rx.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[56]='./liblistwise/op/s/s.pic.o'
fml_1_56()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/s/s.c -o ./liblistwise/op/s/s.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[57]='./liblistwise/op/shift/shift.pic.o'
fml_1_57()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/shift/shift.c -o ./liblistwise/op/shift/shift.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[58]='./liblistwise/op/sn/sn.pic.o'
fml_1_58()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/sn/sn.c -o ./liblistwise/op/sn/sn.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[59]='./liblistwise/op/ss/ss.pic.o'
fml_1_59()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/ss/ss.c -o ./liblistwise/op/ss/ss.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[60]='./liblistwise/op/u/u.pic.o'
fml_1_60()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/u/u.c -o ./liblistwise/op/u/u.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[61]='./liblistwise/op/uu/uu.pic.o'
fml_1_61()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/uu/uu.c -o ./liblistwise/op/uu/uu.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[62]='./liblistwise/op/v/v.pic.o'
fml_1_62()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/v/v.c -o ./liblistwise/op/v/v.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[63]='./liblistwise/op/w/w.pic.o'
fml_1_63()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/w/w.c -o ./liblistwise/op/w/w.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[64]='./liblistwise/op/xch/xch.pic.o'
fml_1_64()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/xch/xch.c -o ./liblistwise/op/xch/xch.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[65]='./liblistwise/op/xm/xm.pic.o'
fml_1_65()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/xm/xm.c -o ./liblistwise/op/xm/xm.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[66]='./liblistwise/op/xmf/xmf.pic.o'
fml_1_66()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/xmf/xmf.c -o ./liblistwise/op/xmf/xmf.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[67]='./liblistwise/op/xs/xs.pic.o'
fml_1_67()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/xs/xs.c -o ./liblistwise/op/xs/xs.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[68]='./liblistwise/op/xsf/xsf.pic.o'
fml_1_68()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/xsf/xsf.c -o ./liblistwise/op/xsf/xsf.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[69]='./liblistwise/op/y/y.pic.o'
fml_1_69()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/y/y.c -o ./liblistwise/op/y/y.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[70]='./liblistwise/op/z/z.pic.o'
fml_1_70()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/op/z/z.c -o ./liblistwise/op/z/z.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[71]='./liblistwise/re/re.pic.o'
fml_1_71()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/re/re.c -o ./liblistwise/re/re.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[72]='./liblistwise/sel/sel.pic.o'
fml_1_72()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/sel/sel.c -o ./liblistwise/sel/sel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[73]='./listwisedev/args.o'
fml_1_73()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./listwisedev/../common -O3 -I./listwisedev -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -c ./listwisedev/args.c -o ./listwisedev/args.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[74]='./listwisedev/main.o'
fml_1_74()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./listwisedev/../common -O3 -I./listwisedev -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -c ./listwisedev/main.c -o ./listwisedev/main.o


  X=$?
  echo 2 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[75]='{./fabdev/ff/ff.tokens.c, ./fabdev/ff/ff.tokens.h}'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	exec 1>./fabdev/ff/ff.tokens.h

echo "#ifndef ff_TOK_H"
echo "#define ff_TOK_H"
echo "const char* ff_tokname(int);"
sed   -ne '/yytokentype/,/;/ p' 					\
	  <./fabdev/ff/ff.tab.c										\
| sed -e 's/\(yytokentype\)/ff_\1/'			\
| sed -e '/ *[A-Z]/s/\([A-Z]\)/ff_\1/'
echo "#endif"

	exec 1>./fabdev/ff/ff.tokens.c

echo "typedef unsigned short yytype_uint16;"
sed -ne '/yytname\[\]/,/};/ p'						\
	  <./fabdev/ff/ff.tab.c										\
| sed -e 's/\\"\|'"'"'//g'
sed -ne '/yytoknum\[\]/,/};/ p'						\
	  <./fabdev/ff/ff.tab.c
grep 'define YYNTOKENS'										\
	  <./fabdev/ff/ff.tab.c
echo '#define API __attribute__((visibility("protected")))'
echo '#include <stdio.h>'
echo "const char API * ff_tokname(int tok)"
echo "{"
echo '  int x;'
echo '  for(x = 0; x < YYNTOKENS; x++)'
echo '  {'
echo '  	 if(yytoknum[x] == tok)'
echo '		 {'
echo '      return yytname[x];'
echo '    }'
echo '  }'
echo '  return 0;'
echo '}'


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[76]='./liblistwise/generator/generator.lex.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/generator/generator.lex.c -o ./liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[77]='./liblistwise/generator/generator.pic.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/generator/generator.c -o ./liblistwise/generator/generator.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[78]='./liblistwise/generator/generator.tab.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -DLWOPDIR="/usr/lib/listwise" -c ./liblistwise/generator/generator.tab.c -o ./liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[79]='./fabdev/args/args.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/args/args.c -o ./fabdev/args/args.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[80]='./fabdev/bake/bake.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/bake/bake.c -o ./fabdev/bake/bake.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[81]='./fabdev/bp/bp.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/bp/bp.c -o ./fabdev/bp/bp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[82]='./fabdev/dep/dep.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/dep/dep.c -o ./fabdev/dep/dep.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[83]='./fabdev/dsc/dsc.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/dsc/dsc.c -o ./fabdev/dsc/dsc.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[84]='./fabdev/enclose/enclose.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/enclose/enclose.c -o ./fabdev/enclose/enclose.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[85]='./fabdev/ff/ff.dsc.tab.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.dsc.tab.c -o ./fabdev/ff/ff.dsc.tab.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[86]='./fabdev/ff/ff.lex.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.lex.c -o ./fabdev/ff/ff.lex.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[87]='./fabdev/ff/ff.list.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.list.tab.c -o ./fabdev/ff/ff.list.tab.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[88]='./fabdev/ff/ff.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.c -o ./fabdev/ff/ff.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[89]='./fabdev/ff/ff.tab.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.tab.c -o ./fabdev/ff/ff.tab.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[90]='./fabdev/ff/ff.tokens.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.tokens.c -o ./fabdev/ff/ff.tokens.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[91]='./fabdev/ff/ff.var.tab.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ff/ff.var.tab.c -o ./fabdev/ff/ff.var.tab.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[92]='./fabdev/ffn/ffn.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ffn/ffn.c -o ./fabdev/ffn/ffn.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[93]='./fabdev/ffproc/ffproc.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ffproc/ffproc.c -o ./fabdev/ffproc/ffproc.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[94]='./fabdev/fml/fml.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/fml/fml.c -o ./fabdev/fml/fml.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[95]='./fabdev/gn/gn.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/gn/gn.c -o ./fabdev/gn/gn.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[96]='./fabdev/list/list.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/list/list.c -o ./fabdev/list/list.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[97]='./fabdev/main.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/main.c -o ./fabdev/main.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[98]='./fabdev/selector/selector.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/selector/selector.c -o ./fabdev/selector/selector.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[99]='./fabdev/tmp/tmp.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/tmp/tmp.c -o ./fabdev/tmp/tmp.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[100]='./fabdev/ts/ts.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/ts/ts.c -o ./fabdev/ts/ts.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[101]='./fabdev/var/var.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c ./fabdev/var/var.c -o ./fabdev/var/var.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[102]='./liblistwise/liblistwise.so'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&103

  
	  exec 1>./liblistwise/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- ./liblistwise/apidata/apidata.c ./liblistwise/op/xsf/xsf.c ./liblistwise/op/ls/ls.c ./liblistwise/op/xch/xch.c ./liblistwise/op/y/y.c ./liblistwise/op/o/o.c ./liblistwise/op/dj/dj.c ./liblistwise/op/h/h.c ./liblistwise/op/c/c.c ./liblistwise/op/rx/rx.c ./liblistwise/op/xm/xm.c ./liblistwise/op/op.c ./liblistwise/op/m/m.c ./liblistwise/op/r/r.c ./liblistwise/op/rp/rp.c ./liblistwise/op/w/w.c ./liblistwise/op/j/j.c ./liblistwise/op/dn/dn.c ./liblistwise/op/ss/ss.c ./liblistwise/op/sn/sn.c ./liblistwise/op/v/v.c ./liblistwise/op/-X/-X.c ./liblistwise/op/s/s.c ./liblistwise/op/shift/shift.c ./liblistwise/op/d/d.c ./liblistwise/op/bn/bn.c ./liblistwise/op/z/z.c ./liblistwise/op/u/u.c ./liblistwise/op/fn/fn.c ./liblistwise/op/pop/pop.c ./liblistwise/op/cp/cp.c ./liblistwise/op/uu/uu.c ./liblistwise/op/xmf/xmf.c ./liblistwise/op/xs/xs.c ./liblistwise/last/last.c ./liblistwise/object/object.c ./liblistwise/generator/generator.lex.c ./liblistwise/generator/generator.c ./liblistwise/generator/generator.tab.c ./liblistwise/exec/exec.c ./liblistwise/sel/sel.c ./liblistwise/lstack/lstack.c ./liblistwise/idx/idx.c ./liblistwise/re/re.c )
echo "local: *; };"

	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/liblistwise.so ./common/coll.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/apidata/apidata.pic.o ./liblistwise/exec/exec.pic.o ./liblistwise/generator/generator.lex.pic.o ./liblistwise/generator/generator.pic.o ./liblistwise/generator/generator.tab.pic.o ./liblistwise/idx/idx.pic.o ./liblistwise/last/last.pic.o ./liblistwise/lstack/lstack.pic.o ./liblistwise/object/object.pic.o ./liblistwise/op/op.pic.o ./liblistwise/re/re.pic.o ./liblistwise/sel/sel.pic.o -lpcre -ldl -shared -Wl,--version-script=./liblistwise/exports -Wl,-soname,liblistwise.so.0.1


  X=$?
  echo 3 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[103]='./fabdev/fab'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -export-dynamic -o ./fabdev/fab ./common/xmem.o ./common/xstring.o ./common/cksum.o ./fabdev/log/log.o ./fabdev/canon/canon.o ./fabdev/path/path.o ./common/unitstring.o ./common/coll.o ./fabdev/identity/identity.o ./fabdev/dirutil/dirutil.o ./fabdev/hashblock/hashblock.o ./fabdev/map/map.o ./fabdev/ff/ff.tokens.o ./fabdev/ffn/ffn.o ./fabdev/ff/ff.tab.o ./fabdev/ff/ff.dsc.tab.o ./fabdev/ff/ff.var.tab.o ./fabdev/ff/ff.list.tab.o ./fabdev/ff/ff.lex.o ./fabdev/depblock/depblock.o ./fabdev/strstack/strstack.o ./common/pstring.o ./fabdev/ts/ts.o ./fabdev/traverse/traverse.o ./fabdev/gn/gnlw.o ./fabdev/lwutil/lwutil.o ./fabdev/var/var.o ./fabdev/list/list.o ./fabdev/enclose/enclose.o ./fabdev/fml/fml.o ./common/parseint.o ./fabdev/gn/gn.o ./fabdev/ff/ff.o ./fabdev/selector/selector.o ./fabdev/args/args.o ./fabdev/bp/bp.o ./fabdev/dep/dep.o ./fabdev/dsc/dsc.o ./fabdev/tmp/tmp.o ./fabdev/bake/bake.o ./fabdev/ffproc/ffproc.o ./fabdev/main.o ./liblistwise/liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[104]='./fabdev/fablw/op/fg/fg.so'
fml_4_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o ./fabdev/fablw/op/fg/fg.so	./fabdev/fablw/op/fg/fg.pic.o  ./liblistwise/liblistwise.so -shared -Wl,-soname,fg.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[105]='./fabdev/fablw/op/fi/fi.so'
fml_4_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -fms-extensions -fno-builtin-log -I./fabdev/enclose -I./fabdev/dep -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/ffproc -I./fabdev/ffn -I./fabdev -I./fabdev/canon -I./fabdev/ff -I./fabdev/gn -I./fabdev/var -I./fabdev/ts -I./fabdev/tmp -I./fabdev/strstack -I./fabdev/fml -I./fabdev/traverse -I./fabdev/bp -I./fabdev/lwutil -I./fabdev/depblock -I./fabdev/list -I./fabdev/log -I./fabdev/args -I./fabdev/dirutil -I./fabdev/selector -I./fabdev/bake -I./fabdev/dsc -I./fabdev/map -I./fabdev/path -I./liblistwise -DFABVERSIONS="v0.4.2.0" -DFABVERSIONN="420" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o ./fabdev/fablw/op/fi/fi.so	./fabdev/fablw/op/fi/fi.pic.o  ./liblistwise/liblistwise.so -shared -Wl,-soname,fi.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[106]='./liblistwise/op/-X/-X.so'
fml_4_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/-X/-X.so			\
	./common/parseint.pic.o ./liblistwise/op/-X/-X.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,-X.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[107]='./liblistwise/op/bn/bn.so'
fml_4_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/bn/bn.so			\
	./liblistwise/op/bn/bn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,bn.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[108]='./liblistwise/op/c/c.so'
fml_4_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/c/c.so			\
	./liblistwise/op/c/c.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,c.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[109]='./liblistwise/op/cp/cp.so'
fml_4_6()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/cp/cp.so			\
	./common/xmem.pic.o ./liblistwise/op/cp/cp.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,cp.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[110]='./liblistwise/op/d/d.so'
fml_4_7()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/d/d.so			\
	./liblistwise/op/d/d.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,d.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[111]='./liblistwise/op/dj/dj.so'
fml_4_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/dj/dj.so			\
	./liblistwise/op/dj/dj.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dj.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[112]='./liblistwise/op/dn/dn.so'
fml_4_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/dn/dn.so			\
	./liblistwise/op/dn/dn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dn.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[113]='./liblistwise/op/fn/fn.so'
fml_4_10()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/fn/fn.so			\
	./liblistwise/op/fn/fn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,fn.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[114]='./liblistwise/op/h/h.so'
fml_4_11()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/h/h.so			\
	./liblistwise/op/h/h.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,h.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[115]='./liblistwise/op/j/j.so'
fml_4_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/j/j.so			\
	./liblistwise/op/j/j.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,j.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[116]='./liblistwise/op/ls/ls.so'
fml_4_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/ls/ls.so			\
	./common/parseint.pic.o ./liblistwise/op/ls/ls.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ls.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[117]='./liblistwise/op/m/m.so'
fml_4_14()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/m/m.so			\
	./liblistwise/op/m/m.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,m.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[118]='./liblistwise/op/o/o.so'
fml_4_15()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/o/o.so			\
	./liblistwise/op/o/o.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,o.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[119]='./liblistwise/op/pop/pop.so'
fml_4_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/pop/pop.so			\
	./liblistwise/op/pop/pop.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,pop.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[120]='./liblistwise/op/r/r.so'
fml_4_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/r/r.so			\
	./liblistwise/op/r/r.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,r.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[121]='./liblistwise/op/rp/rp.so'
fml_4_18()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/rp/rp.so			\
	./common/xmem.pic.o ./liblistwise/op/rp/rp.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rp.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[122]='./liblistwise/op/rx/rx.so'
fml_4_19()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/rx/rx.so			\
	./common/xmem.pic.o ./liblistwise/op/rx/rx.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rx.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[123]='./liblistwise/op/s/s.so'
fml_4_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/s/s.so			\
	./liblistwise/op/s/s.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,s.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[124]='./liblistwise/op/shift/shift.so'
fml_4_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/shift/shift.so			\
	./liblistwise/op/shift/shift.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,shift.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[125]='./liblistwise/op/sn/sn.so'
fml_4_22()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/sn/sn.so			\
	./common/parseint.pic.o ./liblistwise/op/sn/sn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,sn.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[126]='./liblistwise/op/ss/ss.so'
fml_4_23()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/ss/ss.so			\
	./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/ss/ss.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ss.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[127]='./liblistwise/op/u/u.so'
fml_4_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/u/u.so			\
	./common/parseint.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/u/u.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,u.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[128]='./liblistwise/op/uu/uu.so'
fml_4_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/uu/uu.so			\
	./common/parseint.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/uu/uu.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,uu.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[129]='./liblistwise/op/v/v.so'
fml_4_26()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/v/v.so			\
	./liblistwise/op/v/v.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,v.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[130]='./liblistwise/op/w/w.so'
fml_4_27()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/w/w.so			\
	./liblistwise/op/w/w.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,w.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[131]='./liblistwise/op/xch/xch.so'
fml_4_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xch/xch.so			\
	./liblistwise/op/xch/xch.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xch.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[132]='./liblistwise/op/xm/xm.so'
fml_4_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xm/xm.so			\
	./liblistwise/op/xm/xm.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xm.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[133]='./liblistwise/op/xmf/xmf.so'
fml_4_30()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xmf/xmf.so			\
	./liblistwise/op/xmf/xmf.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xmf.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[134]='./liblistwise/op/xs/xs.so'
fml_4_31()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xs/xs.so			\
	./liblistwise/op/xs/xs.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xs.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[135]='./liblistwise/op/xsf/xsf.so'
fml_4_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xsf/xsf.so			\
	./liblistwise/op/xsf/xsf.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xsf.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[136]='./liblistwise/op/y/y.so'
fml_4_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/y/y.so			\
	./common/xmem.pic.o ./liblistwise/op/y/y.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,y.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[137]='./liblistwise/op/z/z.so'
fml_4_34()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/z/z.so			\
	./liblistwise/op/z/z.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,z.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[138]='./listwisedev/listwise'
fml_4_35()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DFABVERSIONN="420" -DFABVERSIONS="v0.4.2.0" -D_GNU_SOURCE -I./liblistwise -I./listwisedev -I./listwisedev/../common -O3 -Wall -Werror -m64 -o ./listwisedev/listwise ./listwisedev/args.o ./listwisedev/main.o ./liblistwise/liblistwise.so


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
  ((SKP+=4))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_3 & PIDS[3]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 4 ]]; do
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

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_5 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_6 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_7 & PIDS[3]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((4+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.2
  exec 100>$tmp ; rm -f $tmp ; fml_1_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_9 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_10 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_11 & PIDS[3]=$!

  # harvest stage 1.2
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((8+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.3
  exec 100>$tmp ; rm -f $tmp ; fml_1_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_13 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_14 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_15 & PIDS[3]=$!

  # harvest stage 1.3
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((12+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.4
  exec 100>$tmp ; rm -f $tmp ; fml_1_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_17 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_18 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[3]=$!

  # harvest stage 1.4
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((16+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.5
  exec 100>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[3]=$!

  # harvest stage 1.5
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((20+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.6
  exec 100>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[3]=$!

  # harvest stage 1.6
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((24+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.7
  exec 100>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[3]=$!

  # harvest stage 1.7
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((28+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.8
  exec 100>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[3]=$!

  # harvest stage 1.8
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((32+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.9
  exec 100>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_39 & PIDS[3]=$!

  # harvest stage 1.9
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((36+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.10
  exec 100>$tmp ; rm -f $tmp ; fml_1_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_41 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_42 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_43 & PIDS[3]=$!

  # harvest stage 1.10
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((40+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.11
  exec 100>$tmp ; rm -f $tmp ; fml_1_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_45 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_46 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_47 & PIDS[3]=$!

  # harvest stage 1.11
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((44+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.12
  exec 100>$tmp ; rm -f $tmp ; fml_1_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_49 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_50 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_51 & PIDS[3]=$!

  # harvest stage 1.12
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((48+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+48)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.13
  exec 100>$tmp ; rm -f $tmp ; fml_1_52 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_53 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_54 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_55 & PIDS[3]=$!

  # harvest stage 1.13
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((52+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+52)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.14
  exec 100>$tmp ; rm -f $tmp ; fml_1_56 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_57 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_58 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_59 & PIDS[3]=$!

  # harvest stage 1.14
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((56+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+56)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.15
  exec 100>$tmp ; rm -f $tmp ; fml_1_60 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_61 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_62 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_63 & PIDS[3]=$!

  # harvest stage 1.15
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((60+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+60)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.16
  exec 100>$tmp ; rm -f $tmp ; fml_1_64 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_65 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_66 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_67 & PIDS[3]=$!

  # harvest stage 1.16
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((64+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+64)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.17
  exec 100>$tmp ; rm -f $tmp ; fml_1_68 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_69 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_70 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_71 & PIDS[3]=$!

  # harvest stage 1.17
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((68+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+68)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=3))
else
  # launch stage 1.18
  exec 100>$tmp ; rm -f $tmp ; fml_1_72 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_73 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_74 & PIDS[2]=$!

  # harvest stage 1.18
  C=0
  while [[ $C != 3 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((72+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+72)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 2.0
  exec 100>$tmp ; rm -f $tmp ; fml_2_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_3 & PIDS[3]=$!

  # harvest stage 2.0
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((75+$idx))
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
  ((SKP+=4))
else
  # launch stage 3.0
  exec 100>$tmp ; rm -f $tmp ; fml_3_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_3 & PIDS[3]=$!

  # harvest stage 3.0
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((79+$idx))
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
  ((SKP+=4))
else
  # launch stage 3.1
  exec 100>$tmp ; rm -f $tmp ; fml_3_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_5 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_6 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_7 & PIDS[3]=$!

  # harvest stage 3.1
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((83+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 3.2
  exec 100>$tmp ; rm -f $tmp ; fml_3_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_9 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_10 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_11 & PIDS[3]=$!

  # harvest stage 3.2
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((87+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 3.3
  exec 100>$tmp ; rm -f $tmp ; fml_3_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_13 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_14 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_15 & PIDS[3]=$!

  # harvest stage 3.3
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((91+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 3.4
  exec 100>$tmp ; rm -f $tmp ; fml_3_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_17 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_18 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[3]=$!

  # harvest stage 3.4
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((95+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 3.5
  exec 100>$tmp ; rm -f $tmp ; fml_3_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_21 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_22 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_23 & PIDS[3]=$!

  # harvest stage 3.5
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((99+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.0
  exec 100>$tmp ; rm -f $tmp ; fml_4_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_3 & PIDS[3]=$!

  # harvest stage 4.0
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((103+$idx))
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
  ((SKP+=4))
else
  # launch stage 4.1
  exec 100>$tmp ; rm -f $tmp ; fml_4_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_5 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_6 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_7 & PIDS[3]=$!

  # harvest stage 4.1
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((107+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.2
  exec 100>$tmp ; rm -f $tmp ; fml_4_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_9 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_10 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_11 & PIDS[3]=$!

  # harvest stage 4.2
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((111+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.3
  exec 100>$tmp ; rm -f $tmp ; fml_4_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_13 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_14 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_15 & PIDS[3]=$!

  # harvest stage 4.3
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((115+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.4
  exec 100>$tmp ; rm -f $tmp ; fml_4_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_17 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_18 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_19 & PIDS[3]=$!

  # harvest stage 4.4
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((119+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.5
  exec 100>$tmp ; rm -f $tmp ; fml_4_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_21 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_22 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_23 & PIDS[3]=$!

  # harvest stage 4.5
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((123+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.6
  exec 100>$tmp ; rm -f $tmp ; fml_4_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_25 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_26 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_27 & PIDS[3]=$!

  # harvest stage 4.6
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((127+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.7
  exec 100>$tmp ; rm -f $tmp ; fml_4_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_29 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_30 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_31 & PIDS[3]=$!

  # harvest stage 4.7
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((131+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 4.8
  exec 100>$tmp ; rm -f $tmp ; fml_4_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_33 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_34 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_35 & PIDS[3]=$!

  # harvest stage 4.8
  C=0
  while [[ $C != 4 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((135+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
