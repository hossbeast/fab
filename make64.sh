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
NAMES[0]='common/cksum.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/cksum.c -o common/cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/coll.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/coll.c -o common/coll.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/coll.pic.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -fPIC -c common/coll.c -o common/coll.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/parseint.c -o common/parseint.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/parseint.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -fPIC -c common/parseint.c -o common/parseint.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/pstring.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/pstring.c -o common/pstring.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/unitstring.c -o common/unitstring.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/xmem.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/xmem.c -o common/xmem.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/xmem.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -fPIC -c common/xmem.c -o common/xmem.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='common/xstring.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -c common/xstring.c -o common/xstring.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[10]='common/xstring.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Icommon -O3 -fPIC -c common/xstring.c -o common/xstring.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[11]='fab/canon/canon.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/canon/canon.c -o fab/canon/canon.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[12]='fab/depblock/depblock.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/depblock/depblock.c -o fab/depblock/depblock.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[13]='fab/dirutil/dirutil.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[14]='fab/fablw/op/fg/fg.pic.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[15]='fab/fablw/op/fi/fi.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[16]='{fab/ff/ff.dsc.tab.h, fab/ff/ff.dsc.tab.c}'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	bison --warnings=error -o fab/ff/ff.dsc.tab.c -d	fab/ff/ff.dsc.y


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[17]='{fab/ff/ff.lex.h, fab/ff/ff.lex.c}'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	  flex  -o fab/ff/ff.lex.c  --header-file=fab/ff/ff.lex.h  fab/ff/ff.l


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[18]='{fab/ff/ff.list.tab.h, fab/ff/ff.list.tab.c}'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o fab/ff/ff.list.tab.c -d	fab/ff/ff.list.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[19]='{fab/ff/ff.tab.h, fab/ff/ff.tab.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o fab/ff/ff.tab.c -d	fab/ff/ff.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[20]='{fab/ff/ff.var.tab.h, fab/ff/ff.var.tab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	bison --warnings=error -o fab/ff/ff.var.tab.c -d	fab/ff/ff.var.y


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[21]='fab/gn/gnlw.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/gn/gnlw.c -o fab/gn/gnlw.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[22]='fab/hashblock/hashblock.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[23]='fab/identity/identity.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/identity/identity.c -o fab/identity/identity.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[24]='fab/log/log.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/log/log.c -o fab/log/log.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[25]='fab/lwutil/lwutil.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[26]='fab/map/map.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/map/map.c -o fab/map/map.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[27]='fab/path/path.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/path/path.c -o fab/path/path.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[28]='fab/strstack/strstack.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/strstack/strstack.c -o fab/strstack/strstack.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[29]='fab/traverse/traverse.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/traverse/traverse.c -o fab/traverse/traverse.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[30]='liblistwise/apidata/apidata.pic.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/apidata/apidata.c -o liblistwise/apidata/apidata.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[31]='liblistwise/exec/exec.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/exec/exec.c -o liblistwise/exec/exec.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[32]='{liblistwise/generator/generator.lex.h, liblistwise/generator/generator.lex.c}'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	  flex  -o liblistwise/generator/generator.lex.c  --header-file=liblistwise/generator/generator.lex.h  liblistwise/generator/generator.l


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[33]='{liblistwise/generator/generator.tab.h, liblistwise/generator/generator.tab.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	bison --warnings=error -o liblistwise/generator/generator.tab.c -d	liblistwise/generator/generator.y


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[34]='liblistwise/idx/idx.pic.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/idx/idx.c -o liblistwise/idx/idx.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[35]='liblistwise/last/last.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/last/last.c -o liblistwise/last/last.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[36]='liblistwise/lstack/lstack.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/lstack/lstack.c -o liblistwise/lstack/lstack.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[37]='liblistwise/object/object.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/object/object.c -o liblistwise/object/object.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[38]='liblistwise/op/-X/-X.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/-X/-X.c -o liblistwise/op/-X/-X.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[39]='liblistwise/op/bn/bn.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/bn/bn.c -o liblistwise/op/bn/bn.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[40]='liblistwise/op/c/c.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/c/c.c -o liblistwise/op/c/c.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[41]='liblistwise/op/cp/cp.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/cp/cp.c -o liblistwise/op/cp/cp.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[42]='liblistwise/op/d/d.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/d/d.c -o liblistwise/op/d/d.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[43]='liblistwise/op/datef/datef.pic.o'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/datef/datef.c -o liblistwise/op/datef/datef.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[44]='liblistwise/op/dj/dj.pic.o'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/dj/dj.c -o liblistwise/op/dj/dj.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[45]='liblistwise/op/dn/dn.pic.o'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/dn/dn.c -o liblistwise/op/dn/dn.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[46]='liblistwise/op/fn/fn.pic.o'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/fn/fn.c -o liblistwise/op/fn/fn.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[47]='liblistwise/op/h/h.pic.o'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/h/h.c -o liblistwise/op/h/h.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[48]='liblistwise/op/j/j.pic.o'
fml_1_48()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/j/j.c -o liblistwise/op/j/j.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[49]='liblistwise/op/ls/ls.pic.o'
fml_1_49()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/ls/ls.c -o liblistwise/op/ls/ls.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[50]='liblistwise/op/m/m.pic.o'
fml_1_50()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/m/m.c -o liblistwise/op/m/m.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[51]='liblistwise/op/o/o.pic.o'
fml_1_51()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/o/o.c -o liblistwise/op/o/o.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[52]='liblistwise/op/op.pic.o'
fml_1_52()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/op.c -o liblistwise/op/op.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[53]='liblistwise/op/pop/pop.pic.o'
fml_1_53()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/pop/pop.c -o liblistwise/op/pop/pop.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[54]='liblistwise/op/r/r.pic.o'
fml_1_54()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/r/r.c -o liblistwise/op/r/r.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[55]='liblistwise/op/rp/rp.pic.o'
fml_1_55()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/rp/rp.c -o liblistwise/op/rp/rp.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[56]='liblistwise/op/rx/rx.pic.o'
fml_1_56()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/rx/rx.c -o liblistwise/op/rx/rx.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[57]='liblistwise/op/s/s.pic.o'
fml_1_57()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/s/s.c -o liblistwise/op/s/s.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[58]='liblistwise/op/shift/shift.pic.o'
fml_1_58()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/shift/shift.c -o liblistwise/op/shift/shift.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[59]='liblistwise/op/sn/sn.pic.o'
fml_1_59()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/sn/sn.c -o liblistwise/op/sn/sn.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[60]='liblistwise/op/ss/ss.pic.o'
fml_1_60()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/ss/ss.c -o liblistwise/op/ss/ss.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[61]='liblistwise/op/u/u.pic.o'
fml_1_61()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/u/u.c -o liblistwise/op/u/u.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[62]='liblistwise/op/uu/uu.pic.o'
fml_1_62()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/uu/uu.c -o liblistwise/op/uu/uu.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[63]='liblistwise/op/v/v.pic.o'
fml_1_63()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/v/v.c -o liblistwise/op/v/v.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[64]='liblistwise/op/w/w.pic.o'
fml_1_64()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/w/w.c -o liblistwise/op/w/w.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[65]='liblistwise/op/xch/xch.pic.o'
fml_1_65()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xch/xch.c -o liblistwise/op/xch/xch.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[66]='liblistwise/op/xm/xm.pic.o'
fml_1_66()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xm/xm.c -o liblistwise/op/xm/xm.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[67]='liblistwise/op/xmf/xmf.pic.o'
fml_1_67()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xmf/xmf.c -o liblistwise/op/xmf/xmf.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[68]='liblistwise/op/xs/xs.pic.o'
fml_1_68()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xs/xs.c -o liblistwise/op/xs/xs.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[69]='liblistwise/op/xsf/xsf.pic.o'
fml_1_69()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xsf/xsf.c -o liblistwise/op/xsf/xsf.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[70]='liblistwise/op/y/y.pic.o'
fml_1_70()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/y/y.c -o liblistwise/op/y/y.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[71]='liblistwise/op/z/z.pic.o'
fml_1_71()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/z/z.c -o liblistwise/op/z/z.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[72]='liblistwise/re/re.pic.o'
fml_1_72()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/re/re.c -o liblistwise/re/re.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[73]='liblistwise/sel/sel.pic.o'
fml_1_73()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/sel/sel.c -o liblistwise/sel/sel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[74]='listwise/args.o'
fml_1_74()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ilistwise/../common -O3 -Ilistwise -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -c listwise/args.c -o listwise/args.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[75]='listwise/main.o'
fml_1_75()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ilistwise/../common -O3 -Ilistwise -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -c listwise/main.c -o listwise/main.o


  X=$?
  echo 3 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[76]='{fab/ff/ff.tokens.c, fab/ff/ff.tokens.h}'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	exec 1>fab/ff/ff.tokens.h

echo "#ifndef ff_TOK_H"
echo "#define ff_TOK_H"
echo "#define YY_NULL 0"
echo "const char* ff_tokname(int);"
sed   -ne '/yytokentype/,/;/ p' 					\
	  <fab/ff/ff.tab.c										\
| sed -e 's/\(yytokentype\)/ff_\1/'			\
| sed -e '/ *[A-Z]/s/\([A-Z]\)/ff_\1/'
echo "#endif"

	exec 1>fab/ff/ff.tokens.c

echo "#define YY_NULL 0"
echo "typedef unsigned short yytype_uint16;"
sed -ne '/yytname\[\]/,/};/ p'						\
	  <fab/ff/ff.tab.c										\
| sed -e 's/\\"\|'"'"'//g'
sed -ne '/yytoknum\[\]/,/};/ p'						\
	  <fab/ff/ff.tab.c
grep 'define YYNTOKENS'										\
	  <fab/ff/ff.tab.c
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

NAMES[77]='liblistwise/generator/generator.lex.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.lex.c -o liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[78]='liblistwise/generator/generator.pic.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.c -o liblistwise/generator/generator.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[79]='liblistwise/generator/generator.tab.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Iliblistwise/../common -O3 -fPIC -Iliblistwise -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.tab.c -o liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[80]='fab/args/args.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/args/args.c -o fab/args/args.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[81]='fab/bake/bake.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/bake/bake.c -o fab/bake/bake.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[82]='fab/bp/bp.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/bp/bp.c -o fab/bp/bp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[83]='fab/dep/dep.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dep/dep.c -o fab/dep/dep.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[84]='fab/dsc/dsc.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dsc/dsc.c -o fab/dsc/dsc.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[85]='fab/enclose/enclose.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/enclose/enclose.c -o fab/enclose/enclose.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[86]='fab/ff/ff.dsc.tab.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[87]='fab/ff/ff.lex.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.lex.c -o fab/ff/ff.lex.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[88]='fab/ff/ff.list.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[89]='fab/ff/ff.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.c -o fab/ff/ff.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[90]='fab/ff/ff.tab.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.tab.c -o fab/ff/ff.tab.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[91]='fab/ff/ff.tokens.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[92]='fab/ff/ff.var.tab.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[93]='fab/ffn/ffn.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ffn/ffn.c -o fab/ffn/ffn.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[94]='fab/ffproc/ffproc.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[95]='fab/fml/fml.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/fml/fml.c -o fab/fml/fml.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[96]='fab/gn/gn.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/gn/gn.c -o fab/gn/gn.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[97]='fab/list/list.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/list/list.c -o fab/list/list.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[98]='fab/main.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/main.c -o fab/main.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[99]='fab/selector/selector.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/selector/selector.c -o fab/selector/selector.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[100]='fab/tmp/tmp.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/tmp/tmp.c -o fab/tmp/tmp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[101]='fab/ts/ts.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ts/ts.c -o fab/ts/ts.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[102]='fab/var/var.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/var/var.c -o fab/var/var.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[103]='liblistwise/liblistwise.so'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	  exec 1>liblistwise/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- liblistwise/apidata/apidata.c liblistwise/exec/exec.c liblistwise/re/re.c liblistwise/last/last.c liblistwise/idx/idx.c liblistwise/object/object.c liblistwise/op/w/w.c liblistwise/op/y/y.c liblistwise/op/bn/bn.c liblistwise/op/xsf/xsf.c liblistwise/op/dj/dj.c liblistwise/op/rp/rp.c liblistwise/op/xs/xs.c liblistwise/op/xch/xch.c liblistwise/op/d/d.c liblistwise/op/u/u.c liblistwise/op/fn/fn.c liblistwise/op/s/s.c liblistwise/op/ss/ss.c liblistwise/op/uu/uu.c liblistwise/op/shift/shift.c liblistwise/op/v/v.c liblistwise/op/op.c liblistwise/op/h/h.c liblistwise/op/o/o.c liblistwise/op/rx/rx.c liblistwise/op/pop/pop.c liblistwise/op/c/c.c liblistwise/op/z/z.c liblistwise/op/xm/xm.c liblistwise/op/-X/-X.c liblistwise/op/ls/ls.c liblistwise/op/datef/datef.c liblistwise/op/dn/dn.c liblistwise/op/cp/cp.c liblistwise/op/m/m.c liblistwise/op/xmf/xmf.c liblistwise/op/j/j.c liblistwise/op/sn/sn.c liblistwise/op/r/r.c liblistwise/sel/sel.c liblistwise/lstack/lstack.c liblistwise/generator/generator.c )
echo "local: *; };"

	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/liblistwise.so common/coll.pic.o common/xmem.pic.o common/xstring.pic.o liblistwise/apidata/apidata.pic.o liblistwise/exec/exec.pic.o liblistwise/generator/generator.lex.pic.o liblistwise/generator/generator.pic.o liblistwise/generator/generator.tab.pic.o liblistwise/idx/idx.pic.o liblistwise/last/last.pic.o liblistwise/lstack/lstack.pic.o liblistwise/object/object.pic.o liblistwise/op/op.pic.o liblistwise/re/re.pic.o liblistwise/sel/sel.pic.o -lpcre -ldl -shared -Wl,--version-script=liblistwise/exports -Wl,-soname,liblistwise.so.0.1


  X=$?
  echo 5 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[104]='fab/fab'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -export-dynamic -o fab/fab common/xmem.o common/xstring.o common/cksum.o fab/log/log.o fab/canon/canon.o fab/path/path.o common/unitstring.o common/coll.o fab/identity/identity.o fab/dirutil/dirutil.o fab/hashblock/hashblock.o fab/map/map.o fab/ffn/ffn.o fab/ff/ff.tab.o fab/ff/ff.dsc.tab.o fab/ff/ff.var.tab.o fab/ff/ff.list.tab.o fab/ff/ff.lex.o fab/depblock/depblock.o fab/strstack/strstack.o common/pstring.o fab/ts/ts.o fab/traverse/traverse.o fab/gn/gnlw.o fab/lwutil/lwutil.o fab/var/var.o fab/list/list.o fab/enclose/enclose.o fab/fml/fml.o common/parseint.o fab/gn/gn.o fab/ff/ff.o fab/selector/selector.o fab/args/args.o fab/bp/bp.o fab/dep/dep.o fab/dsc/dsc.o fab/tmp/tmp.o fab/bake/bake.o fab/ffproc/ffproc.o fab/main.o fab/ff/ff.tokens.o liblistwise/liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[105]='fab/fablw/op/fg/fg.so'
fml_4_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o fab/fablw/op/fg/fg.so	fab/fablw/op/fg/fg.pic.o  liblistwise/liblistwise.so -shared -Wl,-soname,fg.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[106]='fab/fablw/op/fi/fi.so'
fml_4_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -Ifab/../common -O3 -fms-extensions -fno-builtin-log -Ifab/dsc -Ifab/enclose -Ifab/dep -Ifab/bake -Ifab/tmp -Ifab/var -Ifab/args -Ifab/bp -Ifab/gn -Ifab/ffproc -Ifab/strstack -Ifab/log -Ifab/path -Ifab/ff -Ifab/ffn -Ifab/traverse -Ifab/lwutil -Ifab/map -Ifab/selector -Ifab/canon -Ifab -Ifab/identity -Ifab/depblock -Ifab/fml -Ifab/hashblock -Ifab/ts -Ifab/dirutil -Ifab/list -I./liblistwise -DFABVERSIONS="v0.4.3.1" -DFABVERSIONN="431" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o fab/fablw/op/fi/fi.so	fab/fablw/op/fi/fi.pic.o  liblistwise/liblistwise.so -shared -Wl,-soname,fi.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[107]='liblistwise/op/-X/-X.so'
fml_4_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/-X/-X.so			\
	common/parseint.pic.o liblistwise/op/-X/-X.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,-X.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[108]='liblistwise/op/bn/bn.so'
fml_4_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/bn/bn.so			\
	liblistwise/op/bn/bn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,bn.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[109]='liblistwise/op/c/c.so'
fml_4_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/c/c.so			\
	liblistwise/op/c/c.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,c.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[110]='liblistwise/op/cp/cp.so'
fml_4_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/cp/cp.so			\
	common/xmem.pic.o liblistwise/op/cp/cp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,cp.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[111]='liblistwise/op/d/d.so'
fml_4_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/d/d.so			\
	liblistwise/op/d/d.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,d.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[112]='liblistwise/op/datef/datef.so'
fml_4_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/datef/datef.so			\
	common/parseint.pic.o liblistwise/op/datef/datef.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,datef.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[113]='liblistwise/op/dj/dj.so'
fml_4_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/dj/dj.so			\
	liblistwise/op/dj/dj.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dj.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[114]='liblistwise/op/dn/dn.so'
fml_4_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/dn/dn.so			\
	liblistwise/op/dn/dn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dn.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[115]='liblistwise/op/fn/fn.so'
fml_4_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/fn/fn.so			\
	liblistwise/op/fn/fn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,fn.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[116]='liblistwise/op/h/h.so'
fml_4_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/h/h.so			\
	liblistwise/op/h/h.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,h.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[117]='liblistwise/op/j/j.so'
fml_4_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/j/j.so			\
	liblistwise/op/j/j.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,j.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[118]='liblistwise/op/ls/ls.so'
fml_4_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/ls/ls.so			\
	common/parseint.pic.o liblistwise/op/ls/ls.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ls.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[119]='liblistwise/op/m/m.so'
fml_4_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/m/m.so			\
	liblistwise/op/m/m.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,m.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[120]='liblistwise/op/o/o.so'
fml_4_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/o/o.so			\
	liblistwise/op/o/o.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,o.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[121]='liblistwise/op/pop/pop.so'
fml_4_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/pop/pop.so			\
	liblistwise/op/pop/pop.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,pop.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[122]='liblistwise/op/r/r.so'
fml_4_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/r/r.so			\
	liblistwise/op/r/r.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,r.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[123]='liblistwise/op/rp/rp.so'
fml_4_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/rp/rp.so			\
	common/xmem.pic.o liblistwise/op/rp/rp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rp.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[124]='liblistwise/op/rx/rx.so'
fml_4_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/rx/rx.so			\
	common/xmem.pic.o liblistwise/op/rx/rx.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rx.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[125]='liblistwise/op/s/s.so'
fml_4_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/s/s.so			\
	liblistwise/op/s/s.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,s.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[126]='liblistwise/op/shift/shift.so'
fml_4_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/shift/shift.so			\
	liblistwise/op/shift/shift.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,shift.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[127]='liblistwise/op/sn/sn.so'
fml_4_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/sn/sn.so			\
	common/parseint.pic.o liblistwise/op/sn/sn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,sn.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[128]='liblistwise/op/ss/ss.so'
fml_4_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/ss/ss.so			\
	common/xmem.pic.o common/xstring.pic.o liblistwise/op/ss/ss.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ss.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[129]='liblistwise/op/u/u.so'
fml_4_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/u/u.so			\
	common/parseint.pic.o common/xmem.pic.o common/xstring.pic.o liblistwise/op/u/u.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,u.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[130]='liblistwise/op/uu/uu.so'
fml_4_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/uu/uu.so			\
	common/parseint.pic.o common/xmem.pic.o common/xstring.pic.o liblistwise/op/uu/uu.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,uu.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[131]='liblistwise/op/v/v.so'
fml_4_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/v/v.so			\
	liblistwise/op/v/v.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,v.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[132]='liblistwise/op/w/w.so'
fml_4_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/w/w.so			\
	liblistwise/op/w/w.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,w.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[133]='liblistwise/op/xch/xch.so'
fml_4_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/xch/xch.so			\
	liblistwise/op/xch/xch.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xch.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[134]='liblistwise/op/xm/xm.so'
fml_4_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/xm/xm.so			\
	liblistwise/op/xm/xm.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xm.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[135]='liblistwise/op/xmf/xmf.so'
fml_4_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/xmf/xmf.so			\
	liblistwise/op/xmf/xmf.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xmf.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[136]='liblistwise/op/xs/xs.so'
fml_4_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/xs/xs.so			\
	liblistwise/op/xs/xs.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xs.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[137]='liblistwise/op/xsf/xsf.so'
fml_4_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/xsf/xsf.so			\
	liblistwise/op/xsf/xsf.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xsf.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[138]='liblistwise/op/y/y.so'
fml_4_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/y/y.so			\
	common/xmem.pic.o liblistwise/op/y/y.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,y.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[139]='liblistwise/op/z/z.so'
fml_4_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -D_GNU_SOURCE -Iliblistwise -Iliblistwise/../common -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/re -O3 -Wall -Werror -fPIC -m64 -o liblistwise/op/z/z.so			\
	liblistwise/op/z/z.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,z.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[140]='listwise/listwise'
fml_4_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSIONN="431" -DFABVERSIONS="v0.4.3.1" -D_GNU_SOURCE -I./liblistwise -Ilistwise -Ilistwise/../common -O3 -Wall -Werror -m64 -o listwise/listwise listwise/args.o listwise/main.o liblistwise/liblistwise.so


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
  ((SKP+=9))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_8 & PIDS[8]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 9 ]]; do
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
  ((SKP+=9))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_17 & PIDS[8]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((9+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.2
  exec 100>$tmp ; rm -f $tmp ; fml_1_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[8]=$!

  # harvest stage 1.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((18+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.3
  exec 100>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[8]=$!

  # harvest stage 1.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((27+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.4
  exec 100>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_39 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_40 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_41 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_42 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_43 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_44 & PIDS[8]=$!

  # harvest stage 1.4
  C=0
  while [[ $C != 9 ]]; do
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
  ((SKP+=9))
else
  # launch stage 1.5
  exec 100>$tmp ; rm -f $tmp ; fml_1_45 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_46 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_47 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_48 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_49 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_50 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_51 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_52 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_53 & PIDS[8]=$!

  # harvest stage 1.5
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((45+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+45)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.6
  exec 100>$tmp ; rm -f $tmp ; fml_1_54 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_55 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_56 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_57 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_58 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_59 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_60 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_61 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_62 & PIDS[8]=$!

  # harvest stage 1.6
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((54+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+54)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.7
  exec 100>$tmp ; rm -f $tmp ; fml_1_63 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_64 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_65 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_66 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_67 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_68 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_69 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_70 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_71 & PIDS[8]=$!

  # harvest stage 1.7
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((63+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+63)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=4))
else
  # launch stage 1.8
  exec 100>$tmp ; rm -f $tmp ; fml_1_72 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_73 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_74 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_75 & PIDS[3]=$!

  # harvest stage 1.8
  C=0
  while [[ $C != 4 ]]; do
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
    I=$((76+$idx))
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
  ((SKP+=9))
else
  # launch stage 3.0
  exec 100>$tmp ; rm -f $tmp ; fml_3_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_8 & PIDS[8]=$!

  # harvest stage 3.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((80+$idx))
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
  ((SKP+=9))
else
  # launch stage 3.1
  exec 100>$tmp ; rm -f $tmp ; fml_3_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_17 & PIDS[8]=$!

  # harvest stage 3.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((89+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=6))
else
  # launch stage 3.2
  exec 100>$tmp ; rm -f $tmp ; fml_3_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_23 & PIDS[5]=$!

  # harvest stage 3.2
  C=0
  while [[ $C != 6 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((98+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 4.0
  exec 100>$tmp ; rm -f $tmp ; fml_4_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_4_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_4_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_4_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_4_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_4_8 & PIDS[8]=$!

  # harvest stage 4.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((104+$idx))
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
  ((SKP+=9))
else
  # launch stage 4.1
  exec 100>$tmp ; rm -f $tmp ; fml_4_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_4_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_4_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_4_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_4_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_4_17 & PIDS[8]=$!

  # harvest stage 4.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((113+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 4.2
  exec 100>$tmp ; rm -f $tmp ; fml_4_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_4_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_4_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_4_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_4_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_4_26 & PIDS[8]=$!

  # harvest stage 4.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((122+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 4.3
  exec 100>$tmp ; rm -f $tmp ; fml_4_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_4_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_4_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_4_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_4_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_4_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_4_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_4_35 & PIDS[8]=$!

  # harvest stage 4.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((131+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 4.4
  exec 100>$tmp ; rm -f $tmp ; fml_4_36 & PIDS[0]=$!

  # harvest stage 4.4
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((140+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
