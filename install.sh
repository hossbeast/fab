#!/bin/bash

# Copyright (c) 2012-2013 Todd Freed <todd.freed\@gmail.com>
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

# along with fab.  If not, see <http://www.gnu.org/licenses/>. */

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

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/cksum.c -o ./common/cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='./common/coll.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/coll.c -o ./common/coll.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='./common/coll.pic.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -fPIC -g -m64 -c ./common/coll.c -o ./common/coll.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='./common/parseint.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/parseint.c -o ./common/parseint.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='./common/parseint.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -fPIC -g -m64 -c ./common/parseint.c -o ./common/parseint.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='./common/pstring.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/pstring.c -o ./common/pstring.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='./common/unitstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/unitstring.c -o ./common/unitstring.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='./common/xmem.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/xmem.c -o ./common/xmem.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='./common/xmem.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -fPIC -g -m64 -c ./common/xmem.c -o ./common/xmem.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='./common/xstring.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -g -m64 -c ./common/xstring.c -o ./common/xstring.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[10]='./common/xstring.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./common -O3 -Wall -Werror -fPIC -g -m64 -c ./common/xstring.c -o ./common/xstring.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='./fabdev/args/args.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/args/args.c -o ./fabdev/args/args.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='./fabdev/canon/canon.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/canon/canon.c -o ./fabdev/canon/canon.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='./fabdev/depblock/depblock.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/depblock/depblock.c -o ./fabdev/depblock/depblock.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='./fabdev/dirutil/dirutil.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/dirutil/dirutil.c -o ./fabdev/dirutil/dirutil.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='./fabdev/fablw/op/fx/fx.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/fablw/op/fx/fx.c -o ./fabdev/fablw/op/fx/fx.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='./fabdev/fablw/op/fxc/fxc.pic.o'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/fablw/op/fxc/fxc.c -o ./fabdev/fablw/op/fxc/fxc.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='./fabdev/fablw/op/fxw/fxw.pic.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/fablw/op/fxw/fxw.c -o ./fabdev/fablw/op/fxw/fxw.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='{./fabdev/ff/ff.dsc.tab.h, ./fabdev/ff/ff.dsc.tab.c}'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  # command  #!/bin/bash


	bison --warnings=error -o ./fabdev/ff/ff.dsc.tab.c -d	./fabdev/ff/ff.dsc.y


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='{./fabdev/ff/ff.lex.h, ./fabdev/ff/ff.lex.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&119

  # command  #!/bin/bash


	  flex  -o ./fabdev/ff/ff.lex.c  --header-file=./fabdev/ff/ff.lex.h  ./fabdev/ff/ff.l


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[20]='{./fabdev/ff/ff.tab.h, ./fabdev/ff/ff.tab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&120

  # command  #!/bin/bash


	bison --warnings=error -o ./fabdev/ff/ff.tab.c -d	./fabdev/ff/ff.y


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[21]='./fabdev/gn/gnlw.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&121

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/gn/gnlw.c -o ./fabdev/gn/gnlw.o


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[22]='./fabdev/hashblock/hashblock.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&122

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/hashblock/hashblock.c -o ./fabdev/hashblock/hashblock.o


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[23]='./fabdev/identity/identity.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&123

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/identity/identity.c -o ./fabdev/identity/identity.o


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[24]='./fabdev/log/log.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&124

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/log/log.c -o ./fabdev/log/log.o


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[25]='./fabdev/map/map.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&125

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/map/map.c -o ./fabdev/map/map.o


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[26]='./fabdev/path/path.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&126

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/path/path.c -o ./fabdev/path/path.o


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[27]='./fabdev/strstack/strstack.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&127

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/strstack/strstack.c -o ./fabdev/strstack/strstack.o


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[28]='./fabdev/tmp/tmp.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&128

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/tmp/tmp.c -o ./fabdev/tmp/tmp.o


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[29]='./fabdev/traverse/traverse.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&129

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/traverse/traverse.c -o ./fabdev/traverse/traverse.o


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[30]='./liblistwise/apidata/apidata.pic.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&130

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/apidata/apidata.c -o ./liblistwise/apidata/apidata.pic.o


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[31]='./liblistwise/exec/exec.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&131

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/exec/exec.c -o ./liblistwise/exec/exec.pic.o


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[32]='{./liblistwise/generator/generator.lex.h, ./liblistwise/generator/generator.lex.c}'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&132

  # command  #!/bin/bash


	  flex  -o ./liblistwise/generator/generator.lex.c  --header-file=./liblistwise/generator/generator.lex.h  ./liblistwise/generator/generator.l


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[33]='{./liblistwise/generator/generator.tab.h, ./liblistwise/generator/generator.tab.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&133

  # command  #!/bin/bash


	bison --warnings=error -o ./liblistwise/generator/generator.tab.c -d	./liblistwise/generator/generator.y


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[34]='./liblistwise/idx/idx.pic.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&134

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/idx/idx.c -o ./liblistwise/idx/idx.pic.o


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[35]='./liblistwise/last/last.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&135

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/last/last.c -o ./liblistwise/last/last.pic.o


  X=$?
  echo 35 1>&99
  exit $X
}

NAMES[36]='./liblistwise/object/object.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&136

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/object/object.c -o ./liblistwise/object/object.pic.o


  X=$?
  echo 36 1>&99
  exit $X
}

NAMES[37]='./liblistwise/op/-d/-d.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&137

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/-d/-d.c -o ./liblistwise/op/-d/-d.pic.o


  X=$?
  echo 37 1>&99
  exit $X
}

NAMES[38]='./liblistwise/op/-f/-f.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&138

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/-f/-f.c -o ./liblistwise/op/-f/-f.pic.o


  X=$?
  echo 38 1>&99
  exit $X
}

NAMES[39]='./liblistwise/op/-l/-l.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&139

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/-l/-l.c -o ./liblistwise/op/-l/-l.pic.o


  X=$?
  echo 39 1>&99
  exit $X
}

NAMES[40]='./liblistwise/op/bn/bn.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&140

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/bn/bn.c -o ./liblistwise/op/bn/bn.pic.o


  X=$?
  echo 40 1>&99
  exit $X
}

NAMES[41]='./liblistwise/op/c/c.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&141

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/c/c.c -o ./liblistwise/op/c/c.pic.o


  X=$?
  echo 41 1>&99
  exit $X
}

NAMES[42]='./liblistwise/op/cp/cp.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&142

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/cp/cp.c -o ./liblistwise/op/cp/cp.pic.o


  X=$?
  echo 42 1>&99
  exit $X
}

NAMES[43]='./liblistwise/op/d/d.pic.o'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&143

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/d/d.c -o ./liblistwise/op/d/d.pic.o


  X=$?
  echo 43 1>&99
  exit $X
}

NAMES[44]='./liblistwise/op/dj/dj.pic.o'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&144

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/dj/dj.c -o ./liblistwise/op/dj/dj.pic.o


  X=$?
  echo 44 1>&99
  exit $X
}

NAMES[45]='./liblistwise/op/dn/dn.pic.o'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&145

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/dn/dn.c -o ./liblistwise/op/dn/dn.pic.o


  X=$?
  echo 45 1>&99
  exit $X
}

NAMES[46]='./liblistwise/op/fn/fn.pic.o'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&146

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/fn/fn.c -o ./liblistwise/op/fn/fn.pic.o


  X=$?
  echo 46 1>&99
  exit $X
}

NAMES[47]='./liblistwise/op/h/h.pic.o'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&147

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/h/h.c -o ./liblistwise/op/h/h.pic.o


  X=$?
  echo 47 1>&99
  exit $X
}

NAMES[48]='./liblistwise/op/j/j.pic.o'
fml_1_48()
{
  exec 1>/dev/null
  exec 2>&148

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/j/j.c -o ./liblistwise/op/j/j.pic.o


  X=$?
  echo 48 1>&99
  exit $X
}

NAMES[49]='./liblistwise/op/ls/ls.pic.o'
fml_1_49()
{
  exec 1>/dev/null
  exec 2>&149

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/ls/ls.c -o ./liblistwise/op/ls/ls.pic.o


  X=$?
  echo 49 1>&99
  exit $X
}

NAMES[50]='./liblistwise/op/lsr/lsr.pic.o'
fml_1_50()
{
  exec 1>/dev/null
  exec 2>&150

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/lsr/lsr.c -o ./liblistwise/op/lsr/lsr.pic.o


  X=$?
  echo 50 1>&99
  exit $X
}

NAMES[51]='./liblistwise/op/m/m.pic.o'
fml_1_51()
{
  exec 1>/dev/null
  exec 2>&151

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/m/m.c -o ./liblistwise/op/m/m.pic.o


  X=$?
  echo 51 1>&99
  exit $X
}

NAMES[52]='./liblistwise/op/o/o.pic.o'
fml_1_52()
{
  exec 1>/dev/null
  exec 2>&152

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/o/o.c -o ./liblistwise/op/o/o.pic.o


  X=$?
  echo 52 1>&99
  exit $X
}

NAMES[53]='./liblistwise/op/op.pic.o'
fml_1_53()
{
  exec 1>/dev/null
  exec 2>&153

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/op.c -o ./liblistwise/op/op.pic.o


  X=$?
  echo 53 1>&99
  exit $X
}

NAMES[54]='./liblistwise/op/pop/pop.pic.o'
fml_1_54()
{
  exec 1>/dev/null
  exec 2>&154

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/pop/pop.c -o ./liblistwise/op/pop/pop.pic.o


  X=$?
  echo 54 1>&99
  exit $X
}

NAMES[55]='./liblistwise/op/r/r.pic.o'
fml_1_55()
{
  exec 1>/dev/null
  exec 2>&155

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/r/r.c -o ./liblistwise/op/r/r.pic.o


  X=$?
  echo 55 1>&99
  exit $X
}

NAMES[56]='./liblistwise/op/rp/rp.pic.o'
fml_1_56()
{
  exec 1>/dev/null
  exec 2>&156

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/rp/rp.c -o ./liblistwise/op/rp/rp.pic.o


  X=$?
  echo 56 1>&99
  exit $X
}

NAMES[57]='./liblistwise/op/rx/rx.pic.o'
fml_1_57()
{
  exec 1>/dev/null
  exec 2>&157

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/rx/rx.c -o ./liblistwise/op/rx/rx.pic.o


  X=$?
  echo 57 1>&99
  exit $X
}

NAMES[58]='./liblistwise/op/s/s.pic.o'
fml_1_58()
{
  exec 1>/dev/null
  exec 2>&158

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/s/s.c -o ./liblistwise/op/s/s.pic.o


  X=$?
  echo 58 1>&99
  exit $X
}

NAMES[59]='./liblistwise/op/shift/shift.pic.o'
fml_1_59()
{
  exec 1>/dev/null
  exec 2>&159

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/shift/shift.c -o ./liblistwise/op/shift/shift.pic.o


  X=$?
  echo 59 1>&99
  exit $X
}

NAMES[60]='./liblistwise/op/sn/sn.pic.o'
fml_1_60()
{
  exec 1>/dev/null
  exec 2>&160

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/sn/sn.c -o ./liblistwise/op/sn/sn.pic.o


  X=$?
  echo 60 1>&99
  exit $X
}

NAMES[61]='./liblistwise/op/ss/ss.pic.o'
fml_1_61()
{
  exec 1>/dev/null
  exec 2>&161

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/ss/ss.c -o ./liblistwise/op/ss/ss.pic.o


  X=$?
  echo 61 1>&99
  exit $X
}

NAMES[62]='./liblistwise/op/u/u.pic.o'
fml_1_62()
{
  exec 1>/dev/null
  exec 2>&162

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/u/u.c -o ./liblistwise/op/u/u.pic.o


  X=$?
  echo 62 1>&99
  exit $X
}

NAMES[63]='./liblistwise/op/v/v.pic.o'
fml_1_63()
{
  exec 1>/dev/null
  exec 2>&163

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/v/v.c -o ./liblistwise/op/v/v.pic.o


  X=$?
  echo 63 1>&99
  exit $X
}

NAMES[64]='./liblistwise/op/w/w.pic.o'
fml_1_64()
{
  exec 1>/dev/null
  exec 2>&164

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/w/w.c -o ./liblistwise/op/w/w.pic.o


  X=$?
  echo 64 1>&99
  exit $X
}

NAMES[65]='./liblistwise/op/xch/xch.pic.o'
fml_1_65()
{
  exec 1>/dev/null
  exec 2>&165

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/xch/xch.c -o ./liblistwise/op/xch/xch.pic.o


  X=$?
  echo 65 1>&99
  exit $X
}

NAMES[66]='./liblistwise/op/xm/xm.pic.o'
fml_1_66()
{
  exec 1>/dev/null
  exec 2>&166

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/xm/xm.c -o ./liblistwise/op/xm/xm.pic.o


  X=$?
  echo 66 1>&99
  exit $X
}

NAMES[67]='./liblistwise/op/xmf/xmf.pic.o'
fml_1_67()
{
  exec 1>/dev/null
  exec 2>&167

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/xmf/xmf.c -o ./liblistwise/op/xmf/xmf.pic.o


  X=$?
  echo 67 1>&99
  exit $X
}

NAMES[68]='./liblistwise/op/xs/xs.pic.o'
fml_1_68()
{
  exec 1>/dev/null
  exec 2>&168

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/xs/xs.c -o ./liblistwise/op/xs/xs.pic.o


  X=$?
  echo 68 1>&99
  exit $X
}

NAMES[69]='./liblistwise/op/xsf/xsf.pic.o'
fml_1_69()
{
  exec 1>/dev/null
  exec 2>&169

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/xsf/xsf.c -o ./liblistwise/op/xsf/xsf.pic.o


  X=$?
  echo 69 1>&99
  exit $X
}

NAMES[70]='./liblistwise/op/y/y.pic.o'
fml_1_70()
{
  exec 1>/dev/null
  exec 2>&170

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/y/y.c -o ./liblistwise/op/y/y.pic.o


  X=$?
  echo 70 1>&99
  exit $X
}

NAMES[71]='./liblistwise/op/z/z.pic.o'
fml_1_71()
{
  exec 1>/dev/null
  exec 2>&171

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/op/z/z.c -o ./liblistwise/op/z/z.pic.o


  X=$?
  echo 71 1>&99
  exit $X
}

NAMES[72]='./liblistwise/re/re.pic.o'
fml_1_72()
{
  exec 1>/dev/null
  exec 2>&172

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/re/re.c -o ./liblistwise/re/re.pic.o


  X=$?
  echo 72 1>&99
  exit $X
}

NAMES[73]='./liblistwise/sel/sel.pic.o'
fml_1_73()
{
  exec 1>/dev/null
  exec 2>&173

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/sel/sel.c -o ./liblistwise/sel/sel.pic.o


  X=$?
  echo 73 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[74]='./fabdev/fablw/op/fx/fx.so'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fabdev/fablw/op/fx/fx.so	./fabdev/fablw/op/fx/fx.pic.o  -llistwise -shared -Wl,-soname,fx.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[75]='./fabdev/fablw/op/fxc/fxc.so'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fabdev/fablw/op/fxc/fxc.so	./fabdev/fablw/op/fxc/fxc.pic.o  -llistwise -shared -Wl,-soname,fxc.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[76]='./fabdev/fablw/op/fxw/fxw.so'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fabdev/fablw/op/fxw/fxw.so	./fabdev/fablw/op/fxw/fxw.pic.o  -llistwise -shared -Wl,-soname,fxw.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[77]='{./fabdev/ff/ff.tokens.c, ./fabdev/ff/ff.tokens.h}'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


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
  echo 3 1>&99
  exit $X
}

NAMES[78]='./liblistwise/generator/generator.lex.pic.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/generator/generator.lex.c -o ./liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[79]='./liblistwise/generator/generator.pic.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/generator/generator.c -o ./liblistwise/generator/generator.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[80]='./liblistwise/generator/generator.tab.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/generator/generator.tab.c -o ./liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[81]='./liblistwise/lstack/lstack.pic.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -c ./liblistwise/lstack/lstack.c -o ./liblistwise/lstack/lstack.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[82]='./liblistwise/op/-d/-d.so'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/-d/-d.so			\
	./liblistwise/op/-d/-d.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,-d.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[83]='./liblistwise/op/-f/-f.so'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/-f/-f.so			\
	./common/parseint.pic.o ./liblistwise/op/-f/-f.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,-f.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[84]='./liblistwise/op/-l/-l.so'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/-l/-l.so			\
	./common/parseint.pic.o ./liblistwise/op/-l/-l.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,-l.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[85]='./liblistwise/op/bn/bn.so'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/bn/bn.so			\
	./liblistwise/op/bn/bn.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,bn.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[86]='./liblistwise/op/c/c.so'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/c/c.so			\
	./liblistwise/op/c/c.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,c.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[87]='./liblistwise/op/cp/cp.so'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/cp/cp.so			\
	./common/xmem.pic.o ./liblistwise/op/cp/cp.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,cp.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[88]='./liblistwise/op/d/d.so'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/d/d.so			\
	./liblistwise/op/d/d.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,d.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[89]='./liblistwise/op/dj/dj.so'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/dj/dj.so			\
	./liblistwise/op/dj/dj.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,dj.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[90]='./liblistwise/op/dn/dn.so'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/dn/dn.so			\
	./liblistwise/op/dn/dn.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,dn.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[91]='./liblistwise/op/fn/fn.so'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/fn/fn.so			\
	./liblistwise/op/fn/fn.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,fn.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[92]='./liblistwise/op/h/h.so'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/h/h.so			\
	./liblistwise/op/h/h.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,h.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[93]='./liblistwise/op/j/j.so'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&119

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/j/j.so			\
	./liblistwise/op/j/j.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,j.so


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[94]='./liblistwise/op/ls/ls.so'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&120

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/ls/ls.so			\
	./common/parseint.pic.o ./liblistwise/op/ls/ls.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,ls.so


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[95]='./liblistwise/op/lsr/lsr.so'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&121

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/lsr/lsr.so			\
	./common/parseint.pic.o ./liblistwise/op/lsr/lsr.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,lsr.so


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[96]='./liblistwise/op/m/m.so'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&122

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/m/m.so			\
	./liblistwise/op/m/m.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,m.so


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[97]='./liblistwise/op/o/o.so'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&123

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/o/o.so			\
	./liblistwise/op/o/o.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,o.so


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[98]='./liblistwise/op/pop/pop.so'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&124

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/pop/pop.so			\
	./liblistwise/op/pop/pop.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,pop.so


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[99]='./liblistwise/op/r/r.so'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&125

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/r/r.so			\
	./liblistwise/op/r/r.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,r.so


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[100]='./liblistwise/op/rp/rp.so'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&126

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/rp/rp.so			\
	./common/xmem.pic.o ./liblistwise/op/rp/rp.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,rp.so


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[101]='./liblistwise/op/rx/rx.so'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&127

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/rx/rx.so			\
	./common/xmem.pic.o ./liblistwise/op/rx/rx.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,rx.so


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[102]='./liblistwise/op/s/s.so'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&128

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/s/s.so			\
	./liblistwise/op/s/s.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,s.so


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[103]='./liblistwise/op/shift/shift.so'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&129

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/shift/shift.so			\
	./liblistwise/op/shift/shift.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,shift.so


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[104]='./liblistwise/op/sn/sn.so'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&130

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/sn/sn.so			\
	./common/parseint.pic.o ./liblistwise/op/sn/sn.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,sn.so


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[105]='./liblistwise/op/ss/ss.so'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&131

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/ss/ss.so			\
	./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/ss/ss.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,ss.so


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[106]='./liblistwise/op/u/u.so'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&132

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/u/u.so			\
	./common/parseint.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/op/u/u.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,u.so


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[107]='./liblistwise/op/v/v.so'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&133

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/v/v.so			\
	./liblistwise/op/v/v.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,v.so


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[108]='./liblistwise/op/w/w.so'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&134

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/w/w.so			\
	./liblistwise/op/w/w.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,w.so


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[109]='./liblistwise/op/xch/xch.so'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&135

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/xch/xch.so			\
	./liblistwise/op/xch/xch.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,xch.so


  X=$?
  echo 35 1>&99
  exit $X
}

NAMES[110]='./liblistwise/op/xm/xm.so'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&136

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/xm/xm.so			\
	./liblistwise/op/xm/xm.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,xm.so


  X=$?
  echo 36 1>&99
  exit $X
}

NAMES[111]='./liblistwise/op/xmf/xmf.so'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&137

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/xmf/xmf.so			\
	./liblistwise/op/xmf/xmf.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,xmf.so


  X=$?
  echo 37 1>&99
  exit $X
}

NAMES[112]='./liblistwise/op/xs/xs.so'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&138

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/xs/xs.so			\
	./liblistwise/op/xs/xs.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,xs.so


  X=$?
  echo 38 1>&99
  exit $X
}

NAMES[113]='./liblistwise/op/xsf/xsf.so'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&139

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/xsf/xsf.so			\
	./liblistwise/op/xsf/xsf.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,xsf.so


  X=$?
  echo 39 1>&99
  exit $X
}

NAMES[114]='./liblistwise/op/y/y.so'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&140

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/y/y.so			\
	./common/xmem.pic.o ./liblistwise/op/y/y.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,y.so


  X=$?
  echo 40 1>&99
  exit $X
}

NAMES[115]='./liblistwise/op/z/z.so'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&141

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/op/z/z.so			\
	./liblistwise/op/z/z.pic.o						\
	-L./liblistwise -llistwise -shared			\
	-Wl,-soname,z.so


  X=$?
  echo 41 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[116]='./fabdev/bake/bake.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/bake/bake.c -o ./fabdev/bake/bake.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[117]='./fabdev/bp/bp.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/bp/bp.c -o ./fabdev/bp/bp.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[118]='./fabdev/dep/dep.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/dep/dep.c -o ./fabdev/dep/dep.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[119]='./fabdev/dsc/dsc.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/dsc/dsc.c -o ./fabdev/dsc/dsc.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[120]='./fabdev/enclose/enclose.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/enclose/enclose.c -o ./fabdev/enclose/enclose.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[121]='./fabdev/ff/ff.dsc.tab.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ff/ff.dsc.tab.c -o ./fabdev/ff/ff.dsc.tab.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[122]='./fabdev/ff/ff.lex.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ff/ff.lex.c -o ./fabdev/ff/ff.lex.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[123]='./fabdev/ff/ff.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ff/ff.c -o ./fabdev/ff/ff.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[124]='./fabdev/ff/ff.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ff/ff.tab.c -o ./fabdev/ff/ff.tab.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[125]='./fabdev/ff/ff.tokens.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ff/ff.tokens.c -o ./fabdev/ff/ff.tokens.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[126]='./fabdev/ffn/ffn.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ffn/ffn.c -o ./fabdev/ffn/ffn.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[127]='./fabdev/ffproc/ffproc.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ffproc/ffproc.c -o ./fabdev/ffproc/ffproc.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[128]='./fabdev/fml/fml.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/fml/fml.c -o ./fabdev/fml/fml.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[129]='./fabdev/gn/gn.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/gn/gn.c -o ./fabdev/gn/gn.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[130]='./fabdev/list/list.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/list/list.c -o ./fabdev/list/list.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[131]='./fabdev/main.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/main.c -o ./fabdev/main.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[132]='./fabdev/ts/ts.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/ts/ts.c -o ./fabdev/ts/ts.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[133]='./fabdev/var/var.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fabdev/var/var.c -o ./fabdev/var/var.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[134]='./liblistwise/liblistwise.so'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  # command  #!/bin/bash


	  exec 1>./liblistwise/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- ./common/coll.c ./common/xmem.c ./common/xstring.c ./liblistwise/apidata/apidata.c ./liblistwise/exec/exec.c ./liblistwise/generator/generator.c ./liblistwise/generator/generator.lex.c ./liblistwise/generator/generator.tab.c ./liblistwise/idx/idx.c ./liblistwise/last/last.c ./liblistwise/lstack/lstack.c ./liblistwise/object/object.c ./liblistwise/op/op.c ./liblistwise/re/re.c ./liblistwise/sel/sel.c )
echo "local: *; };"

	gcc -D_GNU_SOURCE -I./liblistwise -I./liblistwise/../common -I./liblistwise/generator -I./liblistwise/idx -I./liblistwise/listwise -I./liblistwise/re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise/liblistwise.so ./common/coll.pic.o ./common/xmem.pic.o ./common/xstring.pic.o ./liblistwise/apidata/apidata.pic.o ./liblistwise/exec/exec.pic.o ./liblistwise/generator/generator.lex.pic.o ./liblistwise/generator/generator.pic.o ./liblistwise/generator/generator.tab.pic.o ./liblistwise/idx/idx.pic.o ./liblistwise/last/last.pic.o ./liblistwise/lstack/lstack.pic.o ./liblistwise/object/object.pic.o ./liblistwise/op/op.pic.o ./liblistwise/re/re.pic.o ./liblistwise/sel/sel.pic.o -lpcre -ldl -shared -Wl,--version-script=exports -Wl,-soname,liblistwise.so


  X=$?
  echo 18 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[135]='./fabdev/fab'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./fabdev -I./fabdev/../common -I./fabdev/args -I./fabdev/bake -I./fabdev/bp -I./fabdev/canon -I./fabdev/dep -I./fabdev/depblock -I./fabdev/dirutil -I./fabdev/dsc -I./fabdev/enclose -I./fabdev/ff -I./fabdev/ffn -I./fabdev/ffproc -I./fabdev/fml -I./fabdev/gn -I./fabdev/hashblock -I./fabdev/identity -I./fabdev/list -I./fabdev/log -I./fabdev/map -I./fabdev/path -I./fabdev/strstack -I./fabdev/tmp -I./fabdev/traverse -I./fabdev/ts -I./fabdev/var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fabdev/fab ./common/cksum.o ./common/coll.o ./common/parseint.o ./common/pstring.o ./common/unitstring.o ./common/xmem.o ./common/xstring.o ./fabdev/args/args.o ./fabdev/bake/bake.o ./fabdev/bp/bp.o ./fabdev/canon/canon.o ./fabdev/dep/dep.o ./fabdev/depblock/depblock.o ./fabdev/dirutil/dirutil.o ./fabdev/dsc/dsc.o ./fabdev/enclose/enclose.o ./fabdev/ff/ff.dsc.tab.o ./fabdev/ff/ff.lex.o ./fabdev/ff/ff.o ./fabdev/ff/ff.tab.o ./fabdev/ff/ff.tokens.o ./fabdev/ffn/ffn.o ./fabdev/ffproc/ffproc.o ./fabdev/fml/fml.o ./fabdev/gn/gn.o ./fabdev/gn/gnlw.o ./fabdev/hashblock/hashblock.o ./fabdev/identity/identity.o ./fabdev/list/list.o ./fabdev/log/log.o ./fabdev/main.o ./fabdev/map/map.o ./fabdev/path/path.o ./fabdev/strstack/strstack.o ./fabdev/tmp/tmp.o ./fabdev/traverse/traverse.o ./fabdev/ts/ts.o ./fabdev/var/var.o -llistwise


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[136]='/../liblistwise/install'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	install -d																				//usr/lib64
	install	./liblistwise/liblistwise.so																	//usr/lib64/liblistwise.so.0.1
	ln -vfs liblistwise.so.0.1													//usr/lib64/liblistwise.so
	install -d																				//usr/include
	install -d																				//usr/include/listwise
	install ./liblistwise/listwise.h							//usr/include/listwise.h
	install ./liblistwise/listwise/operator.h			//usr/include/listwise/operator.h
	install ./liblistwise/listwise/generator.h 	//usr/include/listwise/generator.h
	install ./liblistwise/listwise/ops.h					//usr/include/listwise/ops.h
	install ./liblistwise/listwise/lstack.h 			//usr/include/listwise/lstack.h
	install ./liblistwise/listwise/object.h 			//usr/include/listwise/object.h

	install -d																				///listwise
	for x in ./liblistwise/op/bn/bn.so./liblistwise/op/v/v.so./liblistwise/op/xm/xm.so./liblistwise/op/rp/rp.so./liblistwise/op/rx/rx.so./liblistwise/op/m/m.so./liblistwise/op/-d/-d.so./liblistwise/op/c/c.so./liblistwise/op/xch/xch.so./liblistwise/op/xsf/xsf.so./liblistwise/op/-l/-l.so./liblistwise/op/s/s.so./liblistwise/op/xs/xs.so./liblistwise/op/h/h.so./liblistwise/op/pop/pop.so./liblistwise/op/w/w.so./liblistwise/op/o/o.so./liblistwise/op/dj/dj.so./liblistwise/op/dn/dn.so./liblistwise/op/-f/-f.so./liblistwise/op/cp/cp.so./liblistwise/op/xmf/xmf.so./liblistwise/op/u/u.so./liblistwise/op/y/y.so./liblistwise/op/ss/ss.so./liblistwise/op/j/j.so./liblistwise/op/ls/ls.so./liblistwise/op/shift/shift.so./liblistwise/op/lsr/lsr.so./liblistwise/op/r/r.so./liblistwise/op/sn/sn.so./liblistwise/op/d/d.so./liblistwise/op/z/z.so./liblistwise/op/fn/fn.so; do \
		install $x																			///listwise
	done


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[137]='/../fab/install'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	install -d															//usr/local/bin
	install ./fabdev/fab												//usr/local/bin/fab
	chown fabsys:fabsys											//usr/local/bin/fab
	chmod u+s 															//usr/local/bin/fab
	chmod g+s 															//usr/local/bin/fab
	install ./fabdev/gcc-dep								//usr/local/bin/gcc-dep
	install -d 															//var/cache/fab
	chown fabsys:fabsys											//var/cache/fab
	install -d 															//var/tmp/fab
	chown fabsys:fabsys											//var/tmp/fab
	install -d															//usr/lib/fab/lib/std
	install -d															//usr/lib/fab/lib/std/c
	install -d															//usr/lib/fab/lib/std/l
	install -d															//usr/lib/fab/lib/std/y
	install ./fabdev/fablib/std/c.fab			//usr/lib/fab/lib/std/c.fab
	install ./fabdev/fablib/std/l.fab			//usr/lib/fab/lib/std/l.fab
	install ./fabdev/fablib/std/y.fab			//usr/lib/fab/lib/std/y.fab
	install ./fabdev/fablib/std/c/ext.fab	//usr/lib/fab/lib/std/c/ext.fab
	install ./fabdev/fablib/std/l/ext.fab	//usr/lib/fab/lib/std/l/ext.fab
	install ./fabdev/fablib/std/y/ext.fab	//usr/lib/fab/lib/std/y/ext.fab
	install -d															//usr/lib/fab/listwise
	install ./fabdev/fablw/op/fx/fx.so			//usr/lib/fab/listwise/fx.so
	install ./fabdev/fablw/op/fxc/fxc.so		//usr/lib/fab/listwise/fxc.so
	install ./fabdev/fablw/op/fxw/fxw.so		//usr/lib/fab/listwise/fxw.so


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
  ((SKP+=74))
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

  # harvest stage 1.0
  C=0
  while [[ $C != 74 ]]; do
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
  ((SKP+=42))
else
  # launch stage 2.0
  exec 100>$tmp ; rm -f $tmp ; fml_2_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_8 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_2_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_2_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_2_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_2_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_2_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_2_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_2_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_2_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_2_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_2_18 & PIDS[18]=$!
  exec 119>$tmp ; rm -f $tmp ; fml_2_19 & PIDS[19]=$!
  exec 120>$tmp ; rm -f $tmp ; fml_2_20 & PIDS[20]=$!
  exec 121>$tmp ; rm -f $tmp ; fml_2_21 & PIDS[21]=$!
  exec 122>$tmp ; rm -f $tmp ; fml_2_22 & PIDS[22]=$!
  exec 123>$tmp ; rm -f $tmp ; fml_2_23 & PIDS[23]=$!
  exec 124>$tmp ; rm -f $tmp ; fml_2_24 & PIDS[24]=$!
  exec 125>$tmp ; rm -f $tmp ; fml_2_25 & PIDS[25]=$!
  exec 126>$tmp ; rm -f $tmp ; fml_2_26 & PIDS[26]=$!
  exec 127>$tmp ; rm -f $tmp ; fml_2_27 & PIDS[27]=$!
  exec 128>$tmp ; rm -f $tmp ; fml_2_28 & PIDS[28]=$!
  exec 129>$tmp ; rm -f $tmp ; fml_2_29 & PIDS[29]=$!
  exec 130>$tmp ; rm -f $tmp ; fml_2_30 & PIDS[30]=$!
  exec 131>$tmp ; rm -f $tmp ; fml_2_31 & PIDS[31]=$!
  exec 132>$tmp ; rm -f $tmp ; fml_2_32 & PIDS[32]=$!
  exec 133>$tmp ; rm -f $tmp ; fml_2_33 & PIDS[33]=$!
  exec 134>$tmp ; rm -f $tmp ; fml_2_34 & PIDS[34]=$!
  exec 135>$tmp ; rm -f $tmp ; fml_2_35 & PIDS[35]=$!
  exec 136>$tmp ; rm -f $tmp ; fml_2_36 & PIDS[36]=$!
  exec 137>$tmp ; rm -f $tmp ; fml_2_37 & PIDS[37]=$!
  exec 138>$tmp ; rm -f $tmp ; fml_2_38 & PIDS[38]=$!
  exec 139>$tmp ; rm -f $tmp ; fml_2_39 & PIDS[39]=$!
  exec 140>$tmp ; rm -f $tmp ; fml_2_40 & PIDS[40]=$!
  exec 141>$tmp ; rm -f $tmp ; fml_2_41 & PIDS[41]=$!

  # harvest stage 2.0
  C=0
  while [[ $C != 42 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((74+$idx))
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
  ((SKP+=19))
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

  # harvest stage 3.0
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((116+$idx))
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
    I=$((135+$idx))
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
  ((SKP+=1))
else
  # launch stage 5.0
  exec 100>$tmp ; rm -f $tmp ; fml_5_0 & PIDS[0]=$!

  # harvest stage 5.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((136+$idx))
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
    I=$((137+$idx))
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
