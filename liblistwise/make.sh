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
NAMES[0]='./../common/coll.pic.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -fPIC -g -m64 -c ./../common/coll.c -o ./../common/coll.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='./../common/parseint.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -fPIC -g -m64 -c ./../common/parseint.c -o ./../common/parseint.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='./../common/xmem.pic.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -fPIC -g -m64 -c ./../common/xmem.c -o ./../common/xmem.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='./../common/xstring.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -D_GNU_SOURCE -I./../common -O3 -Wall -Werror -fPIC -g -m64 -c ./../common/xstring.c -o ./../common/xstring.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='./apidata/apidata.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./apidata/apidata.c -o ./apidata/apidata.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='./exec/exec.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./exec/exec.c -o ./exec/exec.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='{./generator/generator.lex.h, ./generator/generator.lex.c}'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	  flex  -o ./generator/generator.lex.c  --header-file=./generator/generator.lex.h  ./generator/generator.l


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='{./generator/generator.tab.h, ./generator/generator.tab.c}'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	bison --warnings=error -o ./generator/generator.tab.c -d	./generator/generator.y


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='./idx/idx.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./idx/idx.c -o ./idx/idx.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='./last/last.pic.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./last/last.c -o ./last/last.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[10]='./object/object.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./object/object.c -o ./object/object.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='./op/-d/-d.pic.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/-d/-d.c -o ./op/-d/-d.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='./op/-f/-f.pic.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/-f/-f.c -o ./op/-f/-f.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='./op/-l/-l.pic.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/-l/-l.c -o ./op/-l/-l.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='./op/bn/bn.pic.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/bn/bn.c -o ./op/bn/bn.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='./op/c/c.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/c/c.c -o ./op/c/c.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='./op/cp/cp.pic.o'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/cp/cp.c -o ./op/cp/cp.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='./op/d/d.pic.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/d/d.c -o ./op/d/d.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='./op/dj/dj.pic.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/dj/dj.c -o ./op/dj/dj.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='./op/dn/dn.pic.o'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&119

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/dn/dn.c -o ./op/dn/dn.pic.o


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[20]='./op/fn/fn.pic.o'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&120

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/fn/fn.c -o ./op/fn/fn.pic.o


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[21]='./op/h/h.pic.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&121

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/h/h.c -o ./op/h/h.pic.o


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[22]='./op/j/j.pic.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&122

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/j/j.c -o ./op/j/j.pic.o


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[23]='./op/ls/ls.pic.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&123

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/ls/ls.c -o ./op/ls/ls.pic.o


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[24]='./op/lsr/lsr.pic.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&124

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/lsr/lsr.c -o ./op/lsr/lsr.pic.o


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[25]='./op/m/m.pic.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&125

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/m/m.c -o ./op/m/m.pic.o


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[26]='./op/o/o.pic.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&126

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/o/o.c -o ./op/o/o.pic.o


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[27]='./op/op.pic.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&127

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/op.c -o ./op/op.pic.o


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[28]='./op/pop/pop.pic.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&128

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/pop/pop.c -o ./op/pop/pop.pic.o


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[29]='./op/r/r.pic.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&129

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/r/r.c -o ./op/r/r.pic.o


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[30]='./op/rp/rp.pic.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&130

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/rp/rp.c -o ./op/rp/rp.pic.o


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[31]='./op/rx/rx.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&131

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/rx/rx.c -o ./op/rx/rx.pic.o


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[32]='./op/s/s.pic.o'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&132

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/s/s.c -o ./op/s/s.pic.o


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[33]='./op/shift/shift.pic.o'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&133

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/shift/shift.c -o ./op/shift/shift.pic.o


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[34]='./op/sn/sn.pic.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&134

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/sn/sn.c -o ./op/sn/sn.pic.o


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[35]='./op/ss/ss.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&135

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/ss/ss.c -o ./op/ss/ss.pic.o


  X=$?
  echo 35 1>&99
  exit $X
}

NAMES[36]='./op/u/u.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&136

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/u/u.c -o ./op/u/u.pic.o


  X=$?
  echo 36 1>&99
  exit $X
}

NAMES[37]='./op/v/v.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&137

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/v/v.c -o ./op/v/v.pic.o


  X=$?
  echo 37 1>&99
  exit $X
}

NAMES[38]='./op/w/w.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&138

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/w/w.c -o ./op/w/w.pic.o


  X=$?
  echo 38 1>&99
  exit $X
}

NAMES[39]='./op/xch/xch.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&139

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/xch/xch.c -o ./op/xch/xch.pic.o


  X=$?
  echo 39 1>&99
  exit $X
}

NAMES[40]='./op/xm/xm.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&140

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/xm/xm.c -o ./op/xm/xm.pic.o


  X=$?
  echo 40 1>&99
  exit $X
}

NAMES[41]='./op/xmf/xmf.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&141

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/xmf/xmf.c -o ./op/xmf/xmf.pic.o


  X=$?
  echo 41 1>&99
  exit $X
}

NAMES[42]='./op/xs/xs.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&142

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/xs/xs.c -o ./op/xs/xs.pic.o


  X=$?
  echo 42 1>&99
  exit $X
}

NAMES[43]='./op/xsf/xsf.pic.o'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&143

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/xsf/xsf.c -o ./op/xsf/xsf.pic.o


  X=$?
  echo 43 1>&99
  exit $X
}

NAMES[44]='./op/y/y.pic.o'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&144

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/y/y.c -o ./op/y/y.pic.o


  X=$?
  echo 44 1>&99
  exit $X
}

NAMES[45]='./op/z/z.pic.o'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&145

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./op/z/z.c -o ./op/z/z.pic.o


  X=$?
  echo 45 1>&99
  exit $X
}

NAMES[46]='./re/re.pic.o'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&146

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./re/re.c -o ./re/re.pic.o


  X=$?
  echo 46 1>&99
  exit $X
}

NAMES[47]='./sel/sel.pic.o'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&147

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./sel/sel.c -o ./sel/sel.pic.o


  X=$?
  echo 47 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[48]='./generator/generator.lex.pic.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./generator/generator.lex.c -o ./generator/generator.lex.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[49]='./generator/generator.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./generator/generator.c -o ./generator/generator.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[50]='./generator/generator.tab.pic.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./generator/generator.tab.c -o ./generator/generator.tab.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[51]='./lstack/lstack.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -c ./lstack/lstack.c -o ./lstack/lstack.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[52]='./op/-d/-d.so'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/-d/-d.so			\
	./op/-d/-d.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,-d.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[53]='./op/-f/-f.so'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/-f/-f.so			\
	./../common/parseint.pic.o ./op/-f/-f.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,-f.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[54]='./op/-l/-l.so'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/-l/-l.so			\
	./../common/parseint.pic.o ./op/-l/-l.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,-l.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[55]='./op/bn/bn.so'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/bn/bn.so			\
	./op/bn/bn.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,bn.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[56]='./op/c/c.so'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/c/c.so			\
	./op/c/c.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,c.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[57]='./op/cp/cp.so'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/cp/cp.so			\
	./../common/xmem.pic.o ./op/cp/cp.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,cp.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[58]='./op/d/d.so'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/d/d.so			\
	./op/d/d.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,d.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[59]='./op/dj/dj.so'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/dj/dj.so			\
	./op/dj/dj.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,dj.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[60]='./op/dn/dn.so'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/dn/dn.so			\
	./op/dn/dn.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,dn.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[61]='./op/fn/fn.so'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/fn/fn.so			\
	./op/fn/fn.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,fn.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[62]='./op/h/h.so'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/h/h.so			\
	./op/h/h.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,h.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[63]='./op/j/j.so'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/j/j.so			\
	./op/j/j.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,j.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[64]='./op/ls/ls.so'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/ls/ls.so			\
	./../common/parseint.pic.o ./op/ls/ls.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,ls.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[65]='./op/lsr/lsr.so'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/lsr/lsr.so			\
	./../common/parseint.pic.o ./op/lsr/lsr.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,lsr.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[66]='./op/m/m.so'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/m/m.so			\
	./op/m/m.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,m.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[67]='./op/o/o.so'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&119

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/o/o.so			\
	./op/o/o.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,o.so


  X=$?
  echo 19 1>&99
  exit $X
}

NAMES[68]='./op/pop/pop.so'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&120

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/pop/pop.so			\
	./op/pop/pop.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,pop.so


  X=$?
  echo 20 1>&99
  exit $X
}

NAMES[69]='./op/r/r.so'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&121

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/r/r.so			\
	./op/r/r.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,r.so


  X=$?
  echo 21 1>&99
  exit $X
}

NAMES[70]='./op/rp/rp.so'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&122

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/rp/rp.so			\
	./../common/xmem.pic.o ./op/rp/rp.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,rp.so


  X=$?
  echo 22 1>&99
  exit $X
}

NAMES[71]='./op/rx/rx.so'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&123

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/rx/rx.so			\
	./../common/xmem.pic.o ./op/rx/rx.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,rx.so


  X=$?
  echo 23 1>&99
  exit $X
}

NAMES[72]='./op/s/s.so'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&124

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/s/s.so			\
	./op/s/s.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,s.so


  X=$?
  echo 24 1>&99
  exit $X
}

NAMES[73]='./op/shift/shift.so'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&125

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/shift/shift.so			\
	./op/shift/shift.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,shift.so


  X=$?
  echo 25 1>&99
  exit $X
}

NAMES[74]='./op/sn/sn.so'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&126

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/sn/sn.so			\
	./../common/parseint.pic.o ./op/sn/sn.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,sn.so


  X=$?
  echo 26 1>&99
  exit $X
}

NAMES[75]='./op/ss/ss.so'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&127

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/ss/ss.so			\
	./../common/xmem.pic.o ./../common/xstring.pic.o ./op/ss/ss.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,ss.so


  X=$?
  echo 27 1>&99
  exit $X
}

NAMES[76]='./op/u/u.so'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&128

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/u/u.so			\
	./../common/parseint.pic.o ./../common/xmem.pic.o ./../common/xstring.pic.o ./op/u/u.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,u.so


  X=$?
  echo 28 1>&99
  exit $X
}

NAMES[77]='./op/v/v.so'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&129

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/v/v.so			\
	./op/v/v.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,v.so


  X=$?
  echo 29 1>&99
  exit $X
}

NAMES[78]='./op/w/w.so'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&130

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/w/w.so			\
	./op/w/w.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,w.so


  X=$?
  echo 30 1>&99
  exit $X
}

NAMES[79]='./op/xch/xch.so'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&131

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/xch/xch.so			\
	./op/xch/xch.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,xch.so


  X=$?
  echo 31 1>&99
  exit $X
}

NAMES[80]='./op/xm/xm.so'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&132

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/xm/xm.so			\
	./op/xm/xm.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,xm.so


  X=$?
  echo 32 1>&99
  exit $X
}

NAMES[81]='./op/xmf/xmf.so'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&133

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/xmf/xmf.so			\
	./op/xmf/xmf.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,xmf.so


  X=$?
  echo 33 1>&99
  exit $X
}

NAMES[82]='./op/xs/xs.so'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&134

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/xs/xs.so			\
	./op/xs/xs.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,xs.so


  X=$?
  echo 34 1>&99
  exit $X
}

NAMES[83]='./op/xsf/xsf.so'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&135

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/xsf/xsf.so			\
	./op/xsf/xsf.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,xsf.so


  X=$?
  echo 35 1>&99
  exit $X
}

NAMES[84]='./op/y/y.so'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&136

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/y/y.so			\
	./../common/xmem.pic.o ./op/y/y.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,y.so


  X=$?
  echo 36 1>&99
  exit $X
}

NAMES[85]='./op/z/z.so'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&137

  
	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./op/z/z.so			\
	./op/z/z.pic.o						\
	./liblistwise.so -shared										\
	-Wl,-soname,z.so


  X=$?
  echo 37 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[86]='./liblistwise.so'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  exec 1>./exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- ./../common/coll.c ./../common/xmem.c ./../common/xstring.c ./apidata/apidata.c ./exec/exec.c ./generator/generator.c ./generator/generator.lex.c ./generator/generator.tab.c ./idx/idx.c ./last/last.c ./lstack/lstack.c ./object/object.c ./op/op.c ./re/re.c ./sel/sel.c )
echo "local: *; };"

	gcc -D_GNU_SOURCE -I. -I./../common -I./generator -I./idx -I./listwise -I./re -O3 -Wall -Werror -fPIC -g -m64 -o ./liblistwise.so ./../common/coll.pic.o ./../common/xmem.pic.o ./../common/xstring.pic.o ./apidata/apidata.pic.o ./exec/exec.pic.o ./generator/generator.lex.pic.o ./generator/generator.pic.o ./generator/generator.tab.pic.o ./idx/idx.pic.o ./last/last.pic.o ./lstack/lstack.pic.o ./object/object.pic.o ./op/op.pic.o ./re/re.pic.o ./sel/sel.pic.o -lpcre -ldl -shared -Wl,--version-script=./exports -Wl,-soname,liblistwise.so


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
  ((SKP+=48))
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

  # harvest stage 1.0
  C=0
  while [[ $C != 48 ]]; do
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
  ((SKP+=38))
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

  # harvest stage 2.0
  C=0
  while [[ $C != 38 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((48+$idx))
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
    I=$((86+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
