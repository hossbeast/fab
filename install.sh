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
# along with fab.  If not, see <http://www.gnu.org/licenses/>. */

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0

# formulas and names for stage 1
NAMES[0]='./../common/cksum.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/cksum.c -o ./../common/cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='./../common/coll.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/coll.c -o ./../common/coll.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='./../common/parseint.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/parseint.c -o ./../common/parseint.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='./../common/pstring.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/pstring.c -o ./../common/pstring.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='./../common/unitstring.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/unitstring.c -o ./../common/unitstring.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='./../common/xmem.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/xmem.c -o ./../common/xmem.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='./../common/xstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -g -m64 -c ./../common/xstring.c -o ./../common/xstring.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='./args/args.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./args/args.c -o ./args/args.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='./canon/canon.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./canon/canon.c -o ./canon/canon.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='./depblock/depblock.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./depblock/depblock.c -o ./depblock/depblock.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[10]='./dirutil/dirutil.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./dirutil/dirutil.c -o ./dirutil/dirutil.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='./fablw/op/fx/fx.pic.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fablw/op/fx/fx.c -o ./fablw/op/fx/fx.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='./fablw/op/fxc/fxc.pic.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fablw/op/fxc/fxc.c -o ./fablw/op/fxc/fxc.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='./fablw/op/fxw/fxw.pic.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fPIC -fms-extensions -fno-builtin-log -g -m64 -c ./fablw/op/fxw/fxw.c -o ./fablw/op/fxw/fxw.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='{./ff/ff.dsc.tab.h, ./ff/ff.dsc.tab.c}'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  # command  #!/bin/bash


	bison --warnings=error -o ./ff/ff.dsc.tab.c -d	./ff/ff.dsc.y


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='{./ff/ff.lex.h, ./ff/ff.lex.c}'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  # command  #!/bin/bash


	  flex  -o ./ff/ff.lex.c  --header-file=./ff/ff.lex.h  ./ff/ff.l


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='{./ff/ff.tab.h, ./ff/ff.tab.c}'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  # command  #!/bin/bash


	bison --warnings=error -o ./ff/ff.tab.c -d	./ff/ff.y


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='./gn/gnlw.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./gn/gnlw.c -o ./gn/gnlw.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='./hashblock/hashblock.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./hashblock/hashblock.c -o ./hashblock/hashblock.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='./identity/identity.o'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&119

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./identity/identity.c -o ./identity/identity.o


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[20]='./log/log.o'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&120

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./log/log.c -o ./log/log.o


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[21]='./map/map.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&121

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./map/map.c -o ./map/map.o


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[22]='./path/path.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&122

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./path/path.c -o ./path/path.o


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[23]='./strstack/strstack.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&123

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./strstack/strstack.c -o ./strstack/strstack.o


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[24]='./tmp/tmp.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&124

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./tmp/tmp.c -o ./tmp/tmp.o


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[25]='./traverse/traverse.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&125

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./traverse/traverse.c -o ./traverse/traverse.o


  X=$?
  echo 25 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[26]='./fablw/op/fx/fx.so'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fablw/op/fx/fx.so	./fablw/op/fx/fx.pic.o  -llistwise -shared -Wl,-soname,fx.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[27]='./fablw/op/fxc/fxc.so'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fablw/op/fxc/fxc.so	./fablw/op/fxc/fxc.pic.o  -llistwise -shared -Wl,-soname,fxc.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[28]='./fablw/op/fxw/fxw.so'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	  gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fablw/op/fxw/fxw.so	./fablw/op/fxw/fxw.pic.o  -llistwise -shared -Wl,-soname,fxw.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[29]='{./ff/ff.tokens.c, ./ff/ff.tokens.h}'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	exec 1>./ff/ff.tokens.h

echo "#ifndef ff_TOK_H"
echo "#define ff_TOK_H"
echo "const char* ff_tokname(int);"
sed   -ne '/yytokentype/,/;/ p' 					\
	  <./ff/ff.tab.c										\
| sed -e 's/\(yytokentype\)/ff_\1/'			\
| sed -e '/ *[A-Z]/s/\([A-Z]\)/ff_\1/'
echo "#endif"

	exec 1>./ff/ff.tokens.c

echo "typedef unsigned short yytype_uint16;"
sed -ne '/yytname\[\]/,/};/ p'						\
	  <./ff/ff.tab.c										\
| sed -e 's/\\"\|'"'"'//g'
sed -ne '/yytoknum\[\]/,/};/ p'						\
	  <./ff/ff.tab.c
grep 'define YYNTOKENS'										\
	  <./ff/ff.tab.c
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


# formulas and names for stage 3
NAMES[30]='./bake/bake.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./bake/bake.c -o ./bake/bake.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[31]='./bp/bp.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./bp/bp.c -o ./bp/bp.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[32]='./dep/dep.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./dep/dep.c -o ./dep/dep.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[33]='./dsc/dsc.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./dsc/dsc.c -o ./dsc/dsc.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[34]='./enclose/enclose.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./enclose/enclose.c -o ./enclose/enclose.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[35]='./ff/ff.dsc.tab.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ff/ff.dsc.tab.c -o ./ff/ff.dsc.tab.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[36]='./ff/ff.lex.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ff/ff.lex.c -o ./ff/ff.lex.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[37]='./ff/ff.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ff/ff.c -o ./ff/ff.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[38]='./ff/ff.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ff/ff.tab.c -o ./ff/ff.tab.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[39]='./ff/ff.tokens.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ff/ff.tokens.c -o ./ff/ff.tokens.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[40]='./ffn/ffn.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ffn/ffn.c -o ./ffn/ffn.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[41]='./ffproc/ffproc.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ffproc/ffproc.c -o ./ffproc/ffproc.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[42]='./fml/fml.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./fml/fml.c -o ./fml/fml.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[43]='./gn/gn.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./gn/gn.c -o ./gn/gn.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[44]='./list/list.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./list/list.c -o ./list/list.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[45]='./main.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./main.c -o ./main.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[46]='./ts/ts.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./ts/ts.c -o ./ts/ts.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[47]='./var/var.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -c ./var/var.c -o ./var/var.o


  X=$?
  echo 17 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[48]='./fab'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	gcc -D_GNU_SOURCE -I. -I./../common -I./args -I./bake -I./bp -I./canon -I./dep -I./depblock -I./dirutil -I./dsc -I./enclose -I./ff -I./ffn -I./ffproc -I./fml -I./gn -I./hashblock -I./identity -I./list -I./log -I./map -I./path -I./strstack -I./tmp -I./traverse -I./ts -I./var -O3 -Wall -Werror -fms-extensions -fno-builtin-log -g -m64 -o ./fab ./../common/cksum.o ./../common/coll.o ./../common/parseint.o ./../common/pstring.o ./../common/unitstring.o ./../common/xmem.o ./../common/xstring.o ./args/args.o ./bake/bake.o ./bp/bp.o ./canon/canon.o ./dep/dep.o ./depblock/depblock.o ./dirutil/dirutil.o ./dsc/dsc.o ./enclose/enclose.o ./ff/ff.dsc.tab.o ./ff/ff.lex.o ./ff/ff.o ./ff/ff.tab.o ./ff/ff.tokens.o ./ffn/ffn.o ./ffproc/ffproc.o ./fml/fml.o ./gn/gn.o ./gn/gnlw.o ./hashblock/hashblock.o ./identity/identity.o ./list/list.o ./log/log.o ./main.o ./map/map.o ./path/path.o ./strstack/strstack.o ./tmp/tmp.o ./traverse/traverse.o ./ts/ts.o ./var/var.o -llistwise


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[49]='/../install'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  # command  #!/bin/bash


	install -d															//usr/local/bin
	install ./fab												//usr/local/bin/fab
	chown fabsys:fabsys											//usr/local/bin/fab
	chmod u+s 															//usr/local/bin/fab
	chmod g+s 															//usr/local/bin/fab
	install ./gcc-dep								//usr/local/bin/gcc-dep
	install -d 															//var/cache/fab
	chown fabsys:fabsys											//var/cache/fab
	install -d 															//var/tmp/fab
	chown fabsys:fabsys											//var/tmp/fab
	install -d															//usr/lib/fab/lib/std
	install -d															//usr/lib/fab/lib/std/c
	install -d															//usr/lib/fab/lib/std/l
	install -d															//usr/lib/fab/lib/std/y
	install ./fablib/std/c.fab			//usr/lib/fab/lib/std/c.fab
	install ./fablib/std/l.fab			//usr/lib/fab/lib/std/l.fab
	install ./fablib/std/y.fab			//usr/lib/fab/lib/std/y.fab
	install ./fablib/std/c/ext.fab	//usr/lib/fab/lib/std/c/ext.fab
	install ./fablib/std/l/ext.fab	//usr/lib/fab/lib/std/l/ext.fab
	install ./fablib/std/y/ext.fab	//usr/lib/fab/lib/std/y/ext.fab
	install -d															//usr/lib/fab/listwise
	install ./fablw/op/fx/fx.so			//usr/lib/fab/listwise/fx.so
	install ./fablw/op/fxc/fxc.so		//usr/lib/fab/listwise/fxc.so
	install ./fablw/op/fxw/fxw.so		//usr/lib/fab/listwise/fxw.so


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
  ((SKP+=26))
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

  # harvest stage 1.0
  C=0
  while [[ $C != 26 ]]; do
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
    I=$((26+$idx))
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
  ((SKP+=18))
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

  # harvest stage 3.0
  C=0
  while [[ $C != 18 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((30+$idx))
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
    I=$((48+$idx))
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
    I=$((49+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 5 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
