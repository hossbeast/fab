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
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/parseint.c -o ./common/parseint.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='./common/pstring.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/pstring.c -o ./common/pstring.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='./common/unitstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/unitstring.c -o ./common/unitstring.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='./common/xmem.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/xmem.c -o ./common/xmem.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='./common/xmem.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/xmem.c -o ./common/xmem.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='./common/xstring.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -c ./common/xstring.c -o ./common/xstring.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[10]='./common/xstring.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./common -O3 -fPIC -c ./common/xstring.c -o ./common/xstring.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='./fabdev/args/args.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/args/args.c -o ./fabdev/args/args.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='./fabdev/canon/canon.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/canon/canon.c -o ./fabdev/canon/canon.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='./fabdev/depblock/depblock.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/depblock/depblock.c -o ./fabdev/depblock/depblock.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='./fabdev/dirutil/dirutil.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/dirutil/dirutil.c -o ./fabdev/dirutil/dirutil.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='./fabdev/fablw/op/fg/fg.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -fPIC -c ./fabdev/fablw/op/fg/fg.c -o ./fabdev/fablw/op/fg/fg.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='./fabdev/fablw/op/fi/fi.pic.o'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -fPIC -c ./fabdev/fablw/op/fi/fi.c -o ./fabdev/fablw/op/fi/fi.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='{./fabdev/ff/ff.dsc.tab.h, ./fabdev/ff/ff.dsc.tab.c}'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	bison --warnings=error -o ./fabdev/ff/ff.dsc.tab.c -d	./fabdev/ff/ff.dsc.y


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='{./fabdev/ff/ff.lex.h, ./fabdev/ff/ff.lex.c}'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	  flex  -o ./fabdev/ff/ff.lex.c  --header-file=./fabdev/ff/ff.lex.h  ./fabdev/ff/ff.l


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='{./fabdev/ff/ff.tab.h, ./fabdev/ff/ff.tab.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&119

  
	bison --warnings=error -o ./fabdev/ff/ff.tab.c -d	./fabdev/ff/ff.y


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[20]='{./fabdev/ff/ff.var.tab.h, ./fabdev/ff/ff.var.tab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&120

  
	bison --warnings=error -o ./fabdev/ff/ff.var.tab.c -d	./fabdev/ff/ff.var.y


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[21]='./fabdev/gn/gnlw.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&121

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/gn/gnlw.c -o ./fabdev/gn/gnlw.o


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[22]='./fabdev/hashblock/hashblock.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&122

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/hashblock/hashblock.c -o ./fabdev/hashblock/hashblock.o


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[23]='./fabdev/identity/identity.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&123

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/identity/identity.c -o ./fabdev/identity/identity.o


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[24]='./fabdev/log/log.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&124

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/log/log.c -o ./fabdev/log/log.o


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[25]='./fabdev/lwutil/lwutil.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&125

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/lwutil/lwutil.c -o ./fabdev/lwutil/lwutil.o


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[26]='./fabdev/map/map.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&126

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/map/map.c -o ./fabdev/map/map.o


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[27]='./fabdev/path/path.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&127

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/path/path.c -o ./fabdev/path/path.o


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[28]='./fabdev/strstack/strstack.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&128

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/strstack/strstack.c -o ./fabdev/strstack/strstack.o


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[29]='./fabdev/tmp/tmp.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&129

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/tmp/tmp.c -o ./fabdev/tmp/tmp.o


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[30]='./fabdev/traverse/traverse.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&130

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/traverse/traverse.c -o ./fabdev/traverse/traverse.o


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[31]='./liblistwise/apidata/apidata.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&131

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/apidata/apidata.c -o ./liblistwise/apidata/apidata.pic.o


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[32]='./liblistwise/exec/exec.pic.o'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&132

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/exec/exec.c -o ./liblistwise/exec/exec.pic.o


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[33]='{./liblistwise/generator/generator.lex.h, ./liblistwise/generator/generator.lex.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&133

  
	  flex  -o ./liblistwise/generator/generator.lex.c  --header-file=./liblistwise/generator/generator.lex.h  ./liblistwise/generator/generator.l


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[34]='{./liblistwise/generator/generator.tab.h, ./liblistwise/generator/generator.tab.c}'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&134

  
	bison --warnings=error -o ./liblistwise/generator/generator.tab.c -d	./liblistwise/generator/generator.y


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[35]='./liblistwise/idx/idx.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&135

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/idx/idx.c -o ./liblistwise/idx/idx.pic.o


  X=$?
  echo 35 1>&99
  exit $X
}

NAMES[36]='./liblistwise/last/last.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&136

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/last/last.c -o ./liblistwise/last/last.pic.o


  X=$?
  echo 36 1>&99
  exit $X
}

NAMES[37]='./liblistwise/object/object.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&137

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/object/object.c -o ./liblistwise/object/object.pic.o


  X=$?
  echo 37 1>&99
  exit $X
}

NAMES[38]='./liblistwise/op/-X/-X.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&138

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/-X/-X.c -o ./liblistwise/op/-X/-X.pic.o


  X=$?
  echo 38 1>&99
  exit $X
}

NAMES[39]='./liblistwise/op/bn/bn.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&139

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/bn/bn.c -o ./liblistwise/op/bn/bn.pic.o


  X=$?
  echo 39 1>&99
  exit $X
}

NAMES[40]='./liblistwise/op/c/c.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&140

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/c/c.c -o ./liblistwise/op/c/c.pic.o


  X=$?
  echo 40 1>&99
  exit $X
}

NAMES[41]='./liblistwise/op/cp/cp.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&141

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/cp/cp.c -o ./liblistwise/op/cp/cp.pic.o


  X=$?
  echo 41 1>&99
  exit $X
}

NAMES[42]='./liblistwise/op/d/d.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&142

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/d/d.c -o ./liblistwise/op/d/d.pic.o


  X=$?
  echo 42 1>&99
  exit $X
}

NAMES[43]='./liblistwise/op/dj/dj.pic.o'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&143

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/dj/dj.c -o ./liblistwise/op/dj/dj.pic.o


  X=$?
  echo 43 1>&99
  exit $X
}

NAMES[44]='./liblistwise/op/dn/dn.pic.o'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&144

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/dn/dn.c -o ./liblistwise/op/dn/dn.pic.o


  X=$?
  echo 44 1>&99
  exit $X
}

NAMES[45]='./liblistwise/op/fn/fn.pic.o'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&145

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/fn/fn.c -o ./liblistwise/op/fn/fn.pic.o


  X=$?
  echo 45 1>&99
  exit $X
}

NAMES[46]='./liblistwise/op/h/h.pic.o'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&146

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/h/h.c -o ./liblistwise/op/h/h.pic.o


  X=$?
  echo 46 1>&99
  exit $X
}

NAMES[47]='./liblistwise/op/j/j.pic.o'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&147

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/j/j.c -o ./liblistwise/op/j/j.pic.o


  X=$?
  echo 47 1>&99
  exit $X
}

NAMES[48]='./liblistwise/op/ls/ls.pic.o'
fml_1_48()
{
  exec 1>/dev/null
  exec 2>&148

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/ls/ls.c -o ./liblistwise/op/ls/ls.pic.o


  X=$?
  echo 48 1>&99
  exit $X
}

NAMES[49]='./liblistwise/op/m/m.pic.o'
fml_1_49()
{
  exec 1>/dev/null
  exec 2>&149

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/m/m.c -o ./liblistwise/op/m/m.pic.o


  X=$?
  echo 49 1>&99
  exit $X
}

NAMES[50]='./liblistwise/op/o/o.pic.o'
fml_1_50()
{
  exec 1>/dev/null
  exec 2>&150

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/o/o.c -o ./liblistwise/op/o/o.pic.o


  X=$?
  echo 50 1>&99
  exit $X
}

NAMES[51]='./liblistwise/op/op.pic.o'
fml_1_51()
{
  exec 1>/dev/null
  exec 2>&151

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/op.c -o ./liblistwise/op/op.pic.o


  X=$?
  echo 51 1>&99
  exit $X
}

NAMES[52]='./liblistwise/op/pop/pop.pic.o'
fml_1_52()
{
  exec 1>/dev/null
  exec 2>&152

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/pop/pop.c -o ./liblistwise/op/pop/pop.pic.o


  X=$?
  echo 52 1>&99
  exit $X
}

NAMES[53]='./liblistwise/op/r/r.pic.o'
fml_1_53()
{
  exec 1>/dev/null
  exec 2>&153

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/r/r.c -o ./liblistwise/op/r/r.pic.o


  X=$?
  echo 53 1>&99
  exit $X
}

NAMES[54]='./liblistwise/op/rp/rp.pic.o'
fml_1_54()
{
  exec 1>/dev/null
  exec 2>&154

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/rp/rp.c -o ./liblistwise/op/rp/rp.pic.o


  X=$?
  echo 54 1>&99
  exit $X
}

NAMES[55]='./liblistwise/op/rx/rx.pic.o'
fml_1_55()
{
  exec 1>/dev/null
  exec 2>&155

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/rx/rx.c -o ./liblistwise/op/rx/rx.pic.o


  X=$?
  echo 55 1>&99
  exit $X
}

NAMES[56]='./liblistwise/op/s/s.pic.o'
fml_1_56()
{
  exec 1>/dev/null
  exec 2>&156

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/s/s.c -o ./liblistwise/op/s/s.pic.o


  X=$?
  echo 56 1>&99
  exit $X
}

NAMES[57]='./liblistwise/op/shift/shift.pic.o'
fml_1_57()
{
  exec 1>/dev/null
  exec 2>&157

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/shift/shift.c -o ./liblistwise/op/shift/shift.pic.o


  X=$?
  echo 57 1>&99
  exit $X
}

NAMES[58]='./liblistwise/op/sn/sn.pic.o'
fml_1_58()
{
  exec 1>/dev/null
  exec 2>&158

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/sn/sn.c -o ./liblistwise/op/sn/sn.pic.o


  X=$?
  echo 58 1>&99
  exit $X
}

NAMES[59]='./liblistwise/op/ss/ss.pic.o'
fml_1_59()
{
  exec 1>/dev/null
  exec 2>&159

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/ss/ss.c -o ./liblistwise/op/ss/ss.pic.o


  X=$?
  echo 59 1>&99
  exit $X
}

NAMES[60]='./liblistwise/op/u/u.pic.o'
fml_1_60()
{
  exec 1>/dev/null
  exec 2>&160

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/u/u.c -o ./liblistwise/op/u/u.pic.o


  X=$?
  echo 60 1>&99
  exit $X
}

NAMES[61]='./liblistwise/op/uu/uu.pic.o'
fml_1_61()
{
  exec 1>/dev/null
  exec 2>&161

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/uu/uu.c -o ./liblistwise/op/uu/uu.pic.o


  X=$?
  echo 61 1>&99
  exit $X
}

NAMES[62]='./liblistwise/op/v/v.pic.o'
fml_1_62()
{
  exec 1>/dev/null
  exec 2>&162

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/v/v.c -o ./liblistwise/op/v/v.pic.o


  X=$?
  echo 62 1>&99
  exit $X
}

NAMES[63]='./liblistwise/op/w/w.pic.o'
fml_1_63()
{
  exec 1>/dev/null
  exec 2>&163

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/w/w.c -o ./liblistwise/op/w/w.pic.o


  X=$?
  echo 63 1>&99
  exit $X
}

NAMES[64]='./liblistwise/op/xch/xch.pic.o'
fml_1_64()
{
  exec 1>/dev/null
  exec 2>&164

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/xch/xch.c -o ./liblistwise/op/xch/xch.pic.o


  X=$?
  echo 64 1>&99
  exit $X
}

NAMES[65]='./liblistwise/op/xm/xm.pic.o'
fml_1_65()
{
  exec 1>/dev/null
  exec 2>&165

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/xm/xm.c -o ./liblistwise/op/xm/xm.pic.o


  X=$?
  echo 65 1>&99
  exit $X
}

NAMES[66]='./liblistwise/op/xmf/xmf.pic.o'
fml_1_66()
{
  exec 1>/dev/null
  exec 2>&166

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/xmf/xmf.c -o ./liblistwise/op/xmf/xmf.pic.o


  X=$?
  echo 66 1>&99
  exit $X
}

NAMES[67]='./liblistwise/op/xs/xs.pic.o'
fml_1_67()
{
  exec 1>/dev/null
  exec 2>&167

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/xs/xs.c -o ./liblistwise/op/xs/xs.pic.o


  X=$?
  echo 67 1>&99
  exit $X
}

NAMES[68]='./liblistwise/op/xsf/xsf.pic.o'
fml_1_68()
{
  exec 1>/dev/null
  exec 2>&168

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/xsf/xsf.c -o ./liblistwise/op/xsf/xsf.pic.o


  X=$?
  echo 68 1>&99
  exit $X
}

NAMES[69]='./liblistwise/op/y/y.pic.o'
fml_1_69()
{
  exec 1>/dev/null
  exec 2>&169

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/y/y.c -o ./liblistwise/op/y/y.pic.o


  X=$?
  echo 69 1>&99
  exit $X
}

NAMES[70]='./liblistwise/op/z/z.pic.o'
fml_1_70()
{
  exec 1>/dev/null
  exec 2>&170

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/op/z/z.c -o ./liblistwise/op/z/z.pic.o


  X=$?
  echo 70 1>&99
  exit $X
}

NAMES[71]='./liblistwise/re/re.pic.o'
fml_1_71()
{
  exec 1>/dev/null
  exec 2>&171

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/re/re.c -o ./liblistwise/re/re.pic.o


  X=$?
  echo 71 1>&99
  exit $X
}

NAMES[72]='./liblistwise/sel/sel.pic.o'
fml_1_72()
{
  exec 1>/dev/null
  exec 2>&172

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/sel/sel.c -o ./liblistwise/sel/sel.pic.o


  X=$?
  echo 72 1>&99
  exit $X
}

NAMES[73]='./listwisedev/args.o'
fml_1_73()
{
  exec 1>/dev/null
  exec 2>&173

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./listwisedev/../common -O3 -I./listwisedev -I./liblistwise -DFABVERSION=v0.4.2 -c ./listwisedev/args.c -o ./listwisedev/args.o


  X=$?
  echo 73 1>&99
  exit $X
}

NAMES[74]='./listwisedev/main.o'
fml_1_74()
{
  exec 1>/dev/null
  exec 2>&174

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./listwisedev/../common -O3 -I./listwisedev -I./liblistwise -DFABVERSION=v0.4.2 -c ./listwisedev/main.c -o ./listwisedev/main.o


  X=$?
  echo 74 1>&99
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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/generator/generator.lex.c -o ./liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[77]='./liblistwise/generator/generator.pic.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/generator/generator.c -o ./liblistwise/generator/generator.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[78]='./liblistwise/generator/generator.tab.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/generator/generator.tab.c -o ./liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[79]='./liblistwise/lstack/lstack.pic.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./liblistwise/../common -O3 -fPIC -I./liblistwise -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -c ./liblistwise/lstack/lstack.c -o ./liblistwise/lstack/lstack.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[80]='./fabdev/bake/bake.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/bake/bake.c -o ./fabdev/bake/bake.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[81]='./fabdev/bp/bp.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/bp/bp.c -o ./fabdev/bp/bp.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[82]='./fabdev/dep/dep.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/dep/dep.c -o ./fabdev/dep/dep.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[83]='./fabdev/dsc/dsc.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/dsc/dsc.c -o ./fabdev/dsc/dsc.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[84]='./fabdev/enclose/enclose.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/enclose/enclose.c -o ./fabdev/enclose/enclose.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[85]='./fabdev/ff/ff.dsc.tab.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.dsc.tab.c -o ./fabdev/ff/ff.dsc.tab.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[86]='./fabdev/ff/ff.lex.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.lex.c -o ./fabdev/ff/ff.lex.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[87]='./fabdev/ff/ff.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.c -o ./fabdev/ff/ff.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[88]='./fabdev/ff/ff.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.tab.c -o ./fabdev/ff/ff.tab.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[89]='./fabdev/ff/ff.tokens.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.tokens.c -o ./fabdev/ff/ff.tokens.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[90]='./fabdev/ff/ff.var.tab.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ff/ff.var.tab.c -o ./fabdev/ff/ff.var.tab.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[91]='./fabdev/ffn/ffn.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ffn/ffn.c -o ./fabdev/ffn/ffn.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[92]='./fabdev/ffproc/ffproc.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ffproc/ffproc.c -o ./fabdev/ffproc/ffproc.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[93]='./fabdev/fml/fml.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/fml/fml.c -o ./fabdev/fml/fml.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[94]='./fabdev/gn/gn.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/gn/gn.c -o ./fabdev/gn/gn.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[95]='./fabdev/list/list.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/list/list.c -o ./fabdev/list/list.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[96]='./fabdev/main.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/main.c -o ./fabdev/main.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[97]='./fabdev/ts/ts.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/ts/ts.c -o ./fabdev/ts/ts.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[98]='./fabdev/var/var.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -I./fabdev/../common -O3 -I./fabdev -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -DFABVERSION=v0.4.2 -fms-extensions -fno-builtin-log -c ./fabdev/var/var.c -o ./fabdev/var/var.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[99]='./liblistwise/liblistwise.so'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&119

  
	  exec 1>./liblistwise/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- ./liblistwise/last/last.c ./liblistwise/op/h/h.c ./liblistwise/op/r/r.c ./liblistwise/op/xm/xm.c ./liblistwise/op/sn/sn.c ./liblistwise/op/rp/rp.c ./liblistwise/op/xs/xs.c ./liblistwise/op/xsf/xsf.c ./liblistwise/op/xmf/xmf.c ./liblistwise/op/fn/fn.c ./liblistwise/op/bn/bn.c ./liblistwise/op/dn/dn.c ./liblistwise/op/m/m.c ./liblistwise/op/z/z.c ./liblistwise/op/rx/rx.c ./liblistwise/op/xch/xch.c ./liblistwise/op/j/j.c ./liblistwise/op/dj/dj.c ./liblistwise/op/cp/cp.c ./liblistwise/op/-X/-X.c ./liblistwise/op/s/s.c ./liblistwise/op/pop/pop.c ./liblistwise/op/shift/shift.c ./liblistwise/op/u/u.c ./liblistwise/op/d/d.c ./liblistwise/op/w/w.c ./liblistwise/op/ls/ls.c ./liblistwise/op/o/o.c ./liblistwise/op/c/c.c ./liblistwise/op/uu/uu.c ./liblistwise/op/v/v.c ./liblistwise/op/y/y.c ./liblistwise/op/ss/ss.c ./liblistwise/op/op.c ./liblistwise/object/object.c ./liblistwise/idx/idx.c ./liblistwise/exec/exec.c ./liblistwise/generator/generator.c ./liblistwise/apidata/apidata.c ./liblistwise/re/re.c ./liblistwise/lstack/lstack.c ./liblistwise/sel/sel.c )
echo "local: *; };"

	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/liblistwise.so ./common/coll.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/apidata/apidata.pic.o ./liblistwise/exec/exec.pic.o ./liblistwise/generator/generator.lex.pic.o ./liblistwise/generator/generator.pic.o ./liblistwise/generator/generator.tab.pic.o ./liblistwise/idx/idx.pic.o ./liblistwise/last/last.pic.o ./liblistwise/lstack/lstack.pic.o ./liblistwise/object/object.pic.o ./liblistwise/op/op.pic.o ./liblistwise/re/re.pic.o ./liblistwise/sel/sel.pic.o -lpcre -ldl -shared -Wl,--version-script=./liblistwise/exports -Wl,-soname,liblistwise.so


  X=$?
  echo 19 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[100]='./fabdev/fab'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSION=v0.4.2 -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -O3 -Wall -Werror -export-dynamic -fms-extensions -fno-builtin-log -m64 -o ./fabdev/fab ./common/cksum.o ./common/coll.o ./common/parseint.o ./common/pstring.o ./common/unitstring.o ./common/xmem.o ./common/xstring.o ./fabdev/args/args.o ./fabdev/bake/bake.o ./fabdev/bp/bp.o ./fabdev/canon/canon.o ./fabdev/dep/dep.o ./fabdev/depblock/depblock.o ./fabdev/dirutil/dirutil.o ./fabdev/dsc/dsc.o ./fabdev/enclose/enclose.o ./fabdev/ff/ff.dsc.tab.o ./fabdev/ff/ff.lex.o ./fabdev/ff/ff.o ./fabdev/ff/ff.tab.o ./fabdev/ff/ff.tokens.o ./fabdev/ff/ff.var.tab.o ./fabdev/ffn/ffn.o ./fabdev/ffproc/ffproc.o ./fabdev/fml/fml.o ./fabdev/gn/gn.o ./fabdev/gn/gnlw.o ./fabdev/hashblock/hashblock.o ./fabdev/identity/identity.o ./fabdev/list/list.o ./fabdev/log/log.o ./fabdev/lwutil/lwutil.o ./fabdev/main.o ./fabdev/map/map.o ./fabdev/path/path.o ./fabdev/strstack/strstack.o ./fabdev/tmp/tmp.o ./fabdev/traverse/traverse.o ./fabdev/ts/ts.o ./fabdev/var/var.o ./liblistwise/liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[101]='./fabdev/fablw/op/fg/fg.so'
fml_4_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	  gcc -DFABVERSION=v0.4.2 -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -O3 -Wall -Werror -fms-extensions -fno-builtin-log -m64 -o ./fabdev/fablw/op/fg/fg.so	./fabdev/fablw/op/fg/fg.pic.o  ./liblistwise/liblistwise.so -shared -Wl,-soname,fg.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[102]='./fabdev/fablw/op/fi/fi.so'
fml_4_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	  gcc -DFABVERSION=v0.4.2 -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/lwutil -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -I./liblistwise -O3 -Wall -Werror -fms-extensions -fno-builtin-log -m64 -o ./fabdev/fablw/op/fi/fi.so	./fabdev/fablw/op/fi/fi.pic.o  ./liblistwise/liblistwise.so -shared -Wl,-soname,fi.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[103]='./liblistwise/op/-X/-X.so'
fml_4_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/-X/-X.so			\
	./common/parseint.pic.o ./liblistwise/op/-X/-X.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,-X.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[104]='./liblistwise/op/bn/bn.so'
fml_4_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/bn/bn.so			\
	./liblistwise/op/bn/bn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,bn.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[105]='./liblistwise/op/c/c.so'
fml_4_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/c/c.so			\
	./liblistwise/op/c/c.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,c.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[106]='./liblistwise/op/cp/cp.so'
fml_4_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/cp/cp.so			\
	./common/xmem.pic.o ./liblistwise/op/cp/cp.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,cp.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[107]='./liblistwise/op/d/d.so'
fml_4_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/d/d.so			\
	./liblistwise/op/d/d.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,d.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[108]='./liblistwise/op/dj/dj.so'
fml_4_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/dj/dj.so			\
	./liblistwise/op/dj/dj.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dj.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[109]='./liblistwise/op/dn/dn.so'
fml_4_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/dn/dn.so			\
	./liblistwise/op/dn/dn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dn.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[110]='./liblistwise/op/fn/fn.so'
fml_4_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/fn/fn.so			\
	./liblistwise/op/fn/fn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,fn.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[111]='./liblistwise/op/h/h.so'
fml_4_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/h/h.so			\
	./liblistwise/op/h/h.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,h.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[112]='./liblistwise/op/j/j.so'
fml_4_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/j/j.so			\
	./liblistwise/op/j/j.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,j.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[113]='./liblistwise/op/ls/ls.so'
fml_4_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/ls/ls.so			\
	./common/parseint.pic.o ./liblistwise/op/ls/ls.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ls.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[114]='./liblistwise/op/m/m.so'
fml_4_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/m/m.so			\
	./liblistwise/op/m/m.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,m.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[115]='./liblistwise/op/o/o.so'
fml_4_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/o/o.so			\
	./liblistwise/op/o/o.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,o.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[116]='./liblistwise/op/pop/pop.so'
fml_4_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/pop/pop.so			\
	./liblistwise/op/pop/pop.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,pop.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[117]='./liblistwise/op/r/r.so'
fml_4_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/r/r.so			\
	./liblistwise/op/r/r.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,r.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[118]='./liblistwise/op/rp/rp.so'
fml_4_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/rp/rp.so			\
	./common/xmem.pic.o ./liblistwise/op/rp/rp.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rp.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[119]='./liblistwise/op/rx/rx.so'
fml_4_19()
{
  exec 1>/dev/null
  exec 2>&119

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/rx/rx.so			\
	./common/xmem.pic.o ./liblistwise/op/rx/rx.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rx.so


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[120]='./liblistwise/op/s/s.so'
fml_4_20()
{
  exec 1>/dev/null
  exec 2>&120

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/s/s.so			\
	./liblistwise/op/s/s.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,s.so


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[121]='./liblistwise/op/shift/shift.so'
fml_4_21()
{
  exec 1>/dev/null
  exec 2>&121

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/shift/shift.so			\
	./liblistwise/op/shift/shift.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,shift.so


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[122]='./liblistwise/op/sn/sn.so'
fml_4_22()
{
  exec 1>/dev/null
  exec 2>&122

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/sn/sn.so			\
	./common/parseint.pic.o ./liblistwise/op/sn/sn.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,sn.so


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[123]='./liblistwise/op/ss/ss.so'
fml_4_23()
{
  exec 1>/dev/null
  exec 2>&123

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/ss/ss.so			\
	./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/ss/ss.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ss.so


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[124]='./liblistwise/op/u/u.so'
fml_4_24()
{
  exec 1>/dev/null
  exec 2>&124

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/u/u.so			\
	./common/parseint.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/u/u.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,u.so


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[125]='./liblistwise/op/uu/uu.so'
fml_4_25()
{
  exec 1>/dev/null
  exec 2>&125

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/uu/uu.so			\
	./common/parseint.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/uu/uu.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,uu.so


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[126]='./liblistwise/op/v/v.so'
fml_4_26()
{
  exec 1>/dev/null
  exec 2>&126

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/v/v.so			\
	./liblistwise/op/v/v.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,v.so


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[127]='./liblistwise/op/w/w.so'
fml_4_27()
{
  exec 1>/dev/null
  exec 2>&127

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/w/w.so			\
	./liblistwise/op/w/w.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,w.so


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[128]='./liblistwise/op/xch/xch.so'
fml_4_28()
{
  exec 1>/dev/null
  exec 2>&128

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xch/xch.so			\
	./liblistwise/op/xch/xch.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xch.so


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[129]='./liblistwise/op/xm/xm.so'
fml_4_29()
{
  exec 1>/dev/null
  exec 2>&129

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xm/xm.so			\
	./liblistwise/op/xm/xm.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xm.so


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[130]='./liblistwise/op/xmf/xmf.so'
fml_4_30()
{
  exec 1>/dev/null
  exec 2>&130

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xmf/xmf.so			\
	./liblistwise/op/xmf/xmf.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xmf.so


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[131]='./liblistwise/op/xs/xs.so'
fml_4_31()
{
  exec 1>/dev/null
  exec 2>&131

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xs/xs.so			\
	./liblistwise/op/xs/xs.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xs.so


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[132]='./liblistwise/op/xsf/xsf.so'
fml_4_32()
{
  exec 1>/dev/null
  exec 2>&132

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/xsf/xsf.so			\
	./liblistwise/op/xsf/xsf.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xsf.so


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[133]='./liblistwise/op/y/y.so'
fml_4_33()
{
  exec 1>/dev/null
  exec 2>&133

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/y/y.so			\
	./common/xmem.pic.o ./liblistwise/op/y/y.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,y.so


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[134]='./liblistwise/op/z/z.so'
fml_4_34()
{
  exec 1>/dev/null
  exec 2>&134

  
	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -m64 -o ./liblistwise/op/z/z.so			\
	./liblistwise/op/z/z.pic.o						\
	./liblistwise/liblistwise.so -shared										\
	-Wl,-soname,z.so


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[135]='./listwisedev/listwise'
fml_4_35()
{
  exec 1>/dev/null
  exec 2>&135

  
	gcc -DFABVERSION=v0.4.2 -D_GNU_SOURCE -I./liblistwise -I./listwisedev -I./listwisedev/../common -O3 -Wall -Werror -m64 -o ./listwisedev/listwise ./listwisedev/args.o ./listwisedev/main.o ./liblistwise/liblistwise.so


  X=$?
  echo 35 1>&99
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
  ((SKP+=75))
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
  exec 109>$tmp ; rm -f $tmp ; fml_1_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_1_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_1_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_1_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_1_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_1_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_1_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_1_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_1_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_1_18 & PIDS[18]=$!
  exec 119>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[19]=$!
  exec 120>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[20]=$!
  exec 121>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[21]=$!
  exec 122>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[22]=$!
  exec 123>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[23]=$!
  exec 124>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[24]=$!
  exec 125>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[25]=$!
  exec 126>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[26]=$!
  exec 127>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[27]=$!
  exec 128>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[28]=$!
  exec 129>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[29]=$!
  exec 130>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[30]=$!
  exec 131>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[31]=$!
  exec 132>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[32]=$!
  exec 133>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[33]=$!
  exec 134>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[34]=$!
  exec 135>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[35]=$!
  exec 136>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[36]=$!
  exec 137>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[37]=$!
  exec 138>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[38]=$!
  exec 139>$tmp ; rm -f $tmp ; fml_1_39 & PIDS[39]=$!
  exec 140>$tmp ; rm -f $tmp ; fml_1_40 & PIDS[40]=$!
  exec 141>$tmp ; rm -f $tmp ; fml_1_41 & PIDS[41]=$!
  exec 142>$tmp ; rm -f $tmp ; fml_1_42 & PIDS[42]=$!
  exec 143>$tmp ; rm -f $tmp ; fml_1_43 & PIDS[43]=$!
  exec 144>$tmp ; rm -f $tmp ; fml_1_44 & PIDS[44]=$!
  exec 145>$tmp ; rm -f $tmp ; fml_1_45 & PIDS[45]=$!
  exec 146>$tmp ; rm -f $tmp ; fml_1_46 & PIDS[46]=$!
  exec 147>$tmp ; rm -f $tmp ; fml_1_47 & PIDS[47]=$!
  exec 148>$tmp ; rm -f $tmp ; fml_1_48 & PIDS[48]=$!
  exec 149>$tmp ; rm -f $tmp ; fml_1_49 & PIDS[49]=$!
  exec 150>$tmp ; rm -f $tmp ; fml_1_50 & PIDS[50]=$!
  exec 151>$tmp ; rm -f $tmp ; fml_1_51 & PIDS[51]=$!
  exec 152>$tmp ; rm -f $tmp ; fml_1_52 & PIDS[52]=$!
  exec 153>$tmp ; rm -f $tmp ; fml_1_53 & PIDS[53]=$!
  exec 154>$tmp ; rm -f $tmp ; fml_1_54 & PIDS[54]=$!
  exec 155>$tmp ; rm -f $tmp ; fml_1_55 & PIDS[55]=$!
  exec 156>$tmp ; rm -f $tmp ; fml_1_56 & PIDS[56]=$!
  exec 157>$tmp ; rm -f $tmp ; fml_1_57 & PIDS[57]=$!
  exec 158>$tmp ; rm -f $tmp ; fml_1_58 & PIDS[58]=$!
  exec 159>$tmp ; rm -f $tmp ; fml_1_59 & PIDS[59]=$!
  exec 160>$tmp ; rm -f $tmp ; fml_1_60 & PIDS[60]=$!
  exec 161>$tmp ; rm -f $tmp ; fml_1_61 & PIDS[61]=$!
  exec 162>$tmp ; rm -f $tmp ; fml_1_62 & PIDS[62]=$!
  exec 163>$tmp ; rm -f $tmp ; fml_1_63 & PIDS[63]=$!
  exec 164>$tmp ; rm -f $tmp ; fml_1_64 & PIDS[64]=$!
  exec 165>$tmp ; rm -f $tmp ; fml_1_65 & PIDS[65]=$!
  exec 166>$tmp ; rm -f $tmp ; fml_1_66 & PIDS[66]=$!
  exec 167>$tmp ; rm -f $tmp ; fml_1_67 & PIDS[67]=$!
  exec 168>$tmp ; rm -f $tmp ; fml_1_68 & PIDS[68]=$!
  exec 169>$tmp ; rm -f $tmp ; fml_1_69 & PIDS[69]=$!
  exec 170>$tmp ; rm -f $tmp ; fml_1_70 & PIDS[70]=$!
  exec 171>$tmp ; rm -f $tmp ; fml_1_71 & PIDS[71]=$!
  exec 172>$tmp ; rm -f $tmp ; fml_1_72 & PIDS[72]=$!
  exec 173>$tmp ; rm -f $tmp ; fml_1_73 & PIDS[73]=$!
  exec 174>$tmp ; rm -f $tmp ; fml_1_74 & PIDS[74]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 75 ]]; do
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
  ((SKP+=5))
else
  # launch stage 2.0
  exec 100>$tmp ; rm -f $tmp ; fml_2_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_4 & PIDS[4]=$!

  # harvest stage 2.0
  C=0
  while [[ $C != 5 ]]; do
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
  ((SKP+=20))
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
  exec 109>$tmp ; rm -f $tmp ; fml_3_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_3_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_3_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_3_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_3_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_3_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_3_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_3_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_3_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_3_18 & PIDS[18]=$!
  exec 119>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[19]=$!

  # harvest stage 3.0
  C=0
  while [[ $C != 20 ]]; do
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
  ((SKP+=36))
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
  exec 109>$tmp ; rm -f $tmp ; fml_4_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_4_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_4_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_4_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_4_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_4_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_4_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_4_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_4_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_4_18 & PIDS[18]=$!
  exec 119>$tmp ; rm -f $tmp ; fml_4_19 & PIDS[19]=$!
  exec 120>$tmp ; rm -f $tmp ; fml_4_20 & PIDS[20]=$!
  exec 121>$tmp ; rm -f $tmp ; fml_4_21 & PIDS[21]=$!
  exec 122>$tmp ; rm -f $tmp ; fml_4_22 & PIDS[22]=$!
  exec 123>$tmp ; rm -f $tmp ; fml_4_23 & PIDS[23]=$!
  exec 124>$tmp ; rm -f $tmp ; fml_4_24 & PIDS[24]=$!
  exec 125>$tmp ; rm -f $tmp ; fml_4_25 & PIDS[25]=$!
  exec 126>$tmp ; rm -f $tmp ; fml_4_26 & PIDS[26]=$!
  exec 127>$tmp ; rm -f $tmp ; fml_4_27 & PIDS[27]=$!
  exec 128>$tmp ; rm -f $tmp ; fml_4_28 & PIDS[28]=$!
  exec 129>$tmp ; rm -f $tmp ; fml_4_29 & PIDS[29]=$!
  exec 130>$tmp ; rm -f $tmp ; fml_4_30 & PIDS[30]=$!
  exec 131>$tmp ; rm -f $tmp ; fml_4_31 & PIDS[31]=$!
  exec 132>$tmp ; rm -f $tmp ; fml_4_32 & PIDS[32]=$!
  exec 133>$tmp ; rm -f $tmp ; fml_4_33 & PIDS[33]=$!
  exec 134>$tmp ; rm -f $tmp ; fml_4_34 & PIDS[34]=$!
  exec 135>$tmp ; rm -f $tmp ; fml_4_35 & PIDS[35]=$!

  # harvest stage 4.0
  C=0
  while [[ $C != 36 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((100+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
