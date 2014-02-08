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
NAMES[0]='common/canon/canon.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/canon/canon.c -o common/canon/canon.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/canon/canon.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/canon/canon.c -o common/canon/canon.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/cksum/cksum.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/cksum/cksum.c -o common/cksum/cksum.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/coll/coll.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/coll/coll.c -o common/coll/coll.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/coll/coll.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/coll/coll.c -o common/coll/coll.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/color/color.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/color/color.c -o common/color/color.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/parseint/parseint.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/parseint/parseint.c -o common/parseint/parseint.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/parseint/parseint.pic.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/pstring/pstring.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/pstring/pstring.c -o common/pstring/pstring.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='common/unitstring/unitstring.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/unitstring/unitstring.c -o common/unitstring/unitstring.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[10]='common/wstdlib/wstdlib.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='common/wstdlib/wstdlib.pic.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='common/xstring/xstring.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/xstring/xstring.c -o common/xstring/xstring.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='common/xstring/xstring.pic.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/xstring/xstring.c -o common/xstring/xstring.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='common/yyutil/yyutil.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -c common/yyutil/yyutil.c -o common/yyutil/yyutil.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='common/yyutil/yyutil.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='{fab/FAB.errtab.h, fab/FAB.errtab.c}'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	hfl=fab/FAB.errtab.h
	hin=FAB.errtab.h
	cfl=fab/FAB.errtab.c
	pfx=FAB
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRORS_H"
echo "#define _${pfx}_ERRORS_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat fab/FAB.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat fab/FAB.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat fab/FAB.errtab | \
perl -e '$max=-0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $max = [$max,$1]->[$max < $1] } } print $max, "\n"'

echo "extern etable * perrtab_${pfx};"
echo "#endif"

exec >${cfl}
echo "#include \"${hin}\""
echo "#define APIDATA"
echo "etable errtab_${pfx} = {"
echo "   .tag = \"${pfx}\""
echo " , .min = ERRMIN_${pfx}"
echo " , .max = ERRMAX_${pfx}"
echo " , .v =  (typeof(((etable*)0)->v[0])[]) {"
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='fab/fablw/op/fg/fg.pic.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='fab/fablw/op/fi/fi.pic.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='{fab/ff/ff.dsc.tab.h, fab/ff/ff.dsc.tab.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o fab/ff/ff.dsc.tab.c -d	fab/ff/ff.dsc.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[20]='{fab/ff/ff.lex.h, fab/ff/ff.lex.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	  flex  -o fab/ff/ff.lex.c  --header-file=fab/ff/ff.lex.h  fab/ff/ff.l


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[21]='{fab/ff/ff.list.tab.h, fab/ff/ff.list.tab.c}'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	bison --warnings=error -o fab/ff/ff.list.tab.c -d	fab/ff/ff.list.y


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[22]='{fab/ff/ff.tab.h, fab/ff/ff.tab.c}'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	bison --warnings=error -o fab/ff/ff.tab.c -d	fab/ff/ff.y


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[23]='{fab/ff/ff.var.tab.h, fab/ff/ff.var.tab.c}'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	bison --warnings=error -o fab/ff/ff.var.tab.c -d	fab/ff/ff.var.y


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[24]='fab/log/log.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/log/log.c -o fab/log/log.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[25]='{liblistwise/LW.errtab.h, liblistwise/LW.errtab.c}'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	hfl=liblistwise/LW.errtab.h
	hin=LW.errtab.h
	cfl=liblistwise/LW.errtab.c
	pfx=LW
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRORS_H"
echo "#define _${pfx}_ERRORS_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat liblistwise/LW.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat liblistwise/LW.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat liblistwise/LW.errtab | \
perl -e '$max=-0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $max = [$max,$1]->[$max < $1] } } print $max, "\n"'

echo "extern etable * perrtab_${pfx};"
echo "#endif"

exec >${cfl}
echo "#include \"${hin}\""
echo "#define APIDATA"
echo "etable errtab_${pfx} = {"
echo "   .tag = \"${pfx}\""
echo " , .min = ERRMIN_${pfx}"
echo " , .max = ERRMAX_${pfx}"
echo " , .v =  (typeof(((etable*)0)->v[0])[]) {"
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[26]='liblistwise/PCRE.errtab'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	exec 1>liblistwise/PCRE.errtab
sed -e 's/#define \(PCRE_ERROR_[A-Z_]\+\) \+(\([-0-9]\+\)).*/\2\t\1/p; d' <\
	../../../usr/include/pcre.h


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[27]='{liblistwise/generator/generator.lex.h, liblistwise/generator/generator.lex.c}'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	  flex  -o liblistwise/generator/generator.lex.c  --header-file=liblistwise/generator/generator.lex.h  liblistwise/generator/generator.l


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[28]='{liblistwise/generator/generator.tab.h, liblistwise/generator/generator.tab.c}'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	bison --warnings=error -o liblistwise/generator/generator.tab.c -d	liblistwise/generator/generator.y


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[29]='liblistwise/idx/idx.pic.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/idx/idx.c -o liblistwise/idx/idx.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[30]='libxlinux/xdirent/xdirent.pic.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[31]='libxlinux/xdlfcn/xdlfcn.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[32]='libxlinux/xfcntl/xfcntl.pic.o'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[33]='libxlinux/xftw/xftw.pic.o'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[34]='libxlinux/xgrp/xgrp.pic.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[35]='libxlinux/xmman/xmman.pic.o'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[36]='libxlinux/xpwd/xpwd.pic.o'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[37]='libxlinux/xstat/xstat.pic.o'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[38]='libxlinux/xstdlib/xstdlib.pic.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[39]='libxlinux/xtime/xtime.pic.o'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[40]='libxlinux/xunistd/xunistd.pic.o'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -Ilibxlinux -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[41]='{listwise/LW.errtab.h, listwise/LW.errtab.c}'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	hfl=listwise/LW.errtab.h
	hin=LW.errtab.h
	cfl=listwise/LW.errtab.c
	pfx=LW
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRORS_H"
echo "#define _${pfx}_ERRORS_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat listwise/LW.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat listwise/LW.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat listwise/LW.errtab | \
perl -e '$max=-0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $max = [$max,$1]->[$max < $1] } } print $max, "\n"'

echo "extern etable * perrtab_${pfx};"
echo "#endif"

exec >${cfl}
echo "#include \"${hin}\""
echo "#define APIDATA"
echo "etable errtab_${pfx} = {"
echo "   .tag = \"${pfx}\""
echo " , .min = ERRMIN_${pfx}"
echo " , .max = ERRMAX_${pfx}"
echo " , .v =  (typeof(((etable*)0)->v[0])[]) {"
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[42]='listwise/args.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../xapi -Ilistwise/../xapi/libxapi -Ilistwise/../libxlinux -Ilistwise/../liblistwise -Ilistwise/../common -Ilistwise/../common/pstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/canon -Ilistwise/../common/xstring -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/color -DXAPI_UNWIND -O3 -Ilistwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -c listwise/args.c -o listwise/args.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[43]='xapi/libxapi/SYS.errtab'
fml_1_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	exec > xapi/libxapi/SYS.errtab

	cat ../../../usr/include/asm-generic/errno-base.h ../../../usr/include/asm-generic/errno.h | \
sed -e 's/#define\s\+\(E[A-Z]\+\)\s\+\([0-9]\+\)\s\+\/\*\s*\(.*\)\s\+\*\//\2\t\1\t\3/p; d'


  X=$?
  echo 5 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[44]='fab/FAB.errtab.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/FAB.errtab.c -o fab/FAB.errtab.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[45]='fab/depblock/depblock.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/depblock/depblock.c -o fab/depblock/depblock.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[46]='fab/dirutil/dirutil.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[47]='{fab/ff/ff.states.h, fab/ff/ff.states.c}'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	src=fab/ff/ff.lex.h
	states_h=fab/ff/ff.states.h
	states_c=fab/ff/ff.states.c
	pfx=ff

exec 1>${states_h}
echo "#ifndef ${pfx}_STATES_H"
echo "#define ${pfx}_STATES_H"
echo "extern int ${pfx}_numstates;"
echo "extern int * ${pfx}_statenumbers;"
echo "extern char ** ${pfx}_statenames;"
echo "enum ${pfx}_statenames {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/${pfx}_\1 = \2,/p"
echo "};"	
echo "#endif"

exec 1>${states_c}
echo "char ** ${pfx}_statenames = (char *[]) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne 's/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/[\2] = "\1",/p'
echo "};"	
echo "int * ${pfx}_statenumbers = (int []) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/\2,/p"
echo "};"	
echo "int ${pfx}_numstates = sizeof((int []) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/\2,/p"
echo "}) / sizeof(int);"	


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[48]='{fab/ff/ff.tokens.h, fab/ff/ff.tokens.c}'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	src=fab/ff/ff.tab.c
	tokens_h=fab/ff/ff.tokens.h
	tokens_c=fab/ff/ff.tokens.c
	pfx=ff

exec 1>${tokens_h}
echo "#ifndef ${pfx}_TOKENS_H"
echo "#define ${pfx}_TOKENS_H"
echo "#define YY_NULL 0"
echo "extern int ${pfx}_numtokens;"
echo "extern int * ${pfx}_tokennumbers;"
echo "extern char ** ${pfx}_tokennames;"
echo "enum ${pfx}_tokennames {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne "s/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/${pfx}_\1 = \2,/p"
echo "};"	
echo "#endif"

exec 1>${tokens_c}
echo "#define YY_NULL 0"
echo "char ** ${pfx}_tokennames = (char *[]) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/[\2] = "\1",/p'
echo "};"	
echo "int * ${pfx}_tokennumbers = (int []) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2,/p'
echo "};"	
echo "int ${pfx}_numtokens = sizeof((int []) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2,/p'
echo "}) / sizeof(int);"	


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[49]='fab/gn/gnlw.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/gn/gnlw.c -o fab/gn/gnlw.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[50]='fab/hashblock/hashblock.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[51]='fab/identity/identity.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/identity/identity.c -o fab/identity/identity.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[52]='fab/lwutil/lwutil.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[53]='fab/map/map.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/map/map.c -o fab/map/map.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[54]='fab/params/params.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/params/params.c -o fab/params/params.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[55]='fab/path/path.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/path/path.c -o fab/path/path.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[56]='fab/strstack/strstack.o'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/strstack/strstack.c -o fab/strstack/strstack.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[57]='fab/traverse/traverse.o'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/traverse/traverse.c -o fab/traverse/traverse.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[58]='liblistwise/LW.errtab.pic.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/LW.errtab.c -o liblistwise/LW.errtab.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[59]='{liblistwise/PCRE.errtab.h, liblistwise/PCRE.errtab.c}'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	hfl=liblistwise/PCRE.errtab.h
	hin=PCRE.errtab.h
	cfl=liblistwise/PCRE.errtab.c
	pfx=PCRE
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRORS_H"
echo "#define _${pfx}_ERRORS_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat liblistwise/PCRE.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat liblistwise/PCRE.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat liblistwise/PCRE.errtab | \
perl -e '$max=-0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $max = [$max,$1]->[$max < $1] } } print $max, "\n"'

echo "extern etable * perrtab_${pfx};"
echo "#endif"

exec >${cfl}
echo "#include \"${hin}\""
echo "#define APIDATA"
echo "etable errtab_${pfx} = {"
echo "   .tag = \"${pfx}\""
echo " , .min = ERRMIN_${pfx}"
echo " , .max = ERRMAX_${pfx}"
echo " , .v =  (typeof(((etable*)0)->v[0])[]) {"
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[60]='{liblistwise/generator/generator.states.h, liblistwise/generator/generator.states.c}'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	src=liblistwise/generator/generator.lex.h
	states_h=liblistwise/generator/generator.states.h
	states_c=liblistwise/generator/generator.states.c
	pfx=generator

exec 1>${states_h}
echo "#ifndef ${pfx}_STATES_H"
echo "#define ${pfx}_STATES_H"
echo "extern int ${pfx}_numstates;"
echo "extern int * ${pfx}_statenumbers;"
echo "extern char ** ${pfx}_statenames;"
echo "enum ${pfx}_statenames {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/${pfx}_\1 = \2,/p"
echo "};"	
echo "#endif"

exec 1>${states_c}
echo "char ** ${pfx}_statenames = (char *[]) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne 's/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/[\2] = "\1",/p'
echo "};"	
echo "int * ${pfx}_statenumbers = (int []) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/\2,/p"
echo "};"	
echo "int ${pfx}_numstates = sizeof((int []) {"
sed   -ne '/#ifdef.*YY_HEADER_EXPORT_START_CONDITIONS/,/#.*endif/ p' < ${src} \
| sed -ne "s/#define[ \t]\+\([^ \t]\+\)[ \t]\+\([^ \t]\+\)/\2,/p"
echo "}) / sizeof(int);"	


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[61]='{liblistwise/generator/generator.tokens.h, liblistwise/generator/generator.tokens.c}'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	src=liblistwise/generator/generator.tab.c
	tokens_h=liblistwise/generator/generator.tokens.h
	tokens_c=liblistwise/generator/generator.tokens.c
	pfx=generator

exec 1>${tokens_h}
echo "#ifndef ${pfx}_TOKENS_H"
echo "#define ${pfx}_TOKENS_H"
echo "#define YY_NULL 0"
echo "extern int ${pfx}_numtokens;"
echo "extern int * ${pfx}_tokennumbers;"
echo "extern char ** ${pfx}_tokennames;"
echo "enum ${pfx}_tokennames {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne "s/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/${pfx}_\1 = \2,/p"
echo "};"	
echo "#endif"

exec 1>${tokens_c}
echo "#define YY_NULL 0"
echo "char ** ${pfx}_tokennames = (char *[]) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/[\2] = "\1",/p'
echo "};"	
echo "int * ${pfx}_tokennumbers = (int []) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2,/p'
echo "};"	
echo "int ${pfx}_numtokens = sizeof((int []) {"
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2,/p'
echo "}) / sizeof(int);"	


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[62]='listwise/LW.errtab.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../xapi -Ilistwise/../xapi/libxapi -Ilistwise/../libxlinux -Ilistwise/../liblistwise -Ilistwise/../common -Ilistwise/../common/pstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/canon -Ilistwise/../common/xstring -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/color -DXAPI_UNWIND -O3 -Ilistwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -c listwise/LW.errtab.c -o listwise/LW.errtab.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[63]='listwise/main.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../xapi -Ilistwise/../xapi/libxapi -Ilistwise/../libxlinux -Ilistwise/../liblistwise -Ilistwise/../common -Ilistwise/../common/pstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/canon -Ilistwise/../common/xstring -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/color -DXAPI_UNWIND -O3 -Ilistwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -c listwise/main.c -o listwise/main.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[64]='{xapi/libxapi/SYS.errtab.h, xapi/libxapi/SYS.errtab.c}'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	hfl=xapi/libxapi/SYS.errtab.h
	hin=SYS.errtab.h
	cfl=xapi/libxapi/SYS.errtab.c
	pfx=SYS
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRORS_H"
echo "#define _${pfx}_ERRORS_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat xapi/libxapi/SYS.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat xapi/libxapi/SYS.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat xapi/libxapi/SYS.errtab | \
perl -e '$max=-0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $max = [$max,$1]->[$max < $1] } } print $max, "\n"'

echo "extern etable * perrtab_${pfx};"
echo "#endif"

exec >${cfl}
echo "#include \"${hin}\""
echo "#define APIDATA"
echo "etable errtab_${pfx} = {"
echo "   .tag = \"${pfx}\""
echo " , .min = ERRMIN_${pfx}"
echo " , .max = ERRMAX_${pfx}"
echo " , .v =  (typeof(((etable*)0)->v[0])[]) {"
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[65]='fab/args/args.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/args/args.c -o fab/args/args.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[66]='fab/bake/bake.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/bake/bake.c -o fab/bake/bake.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[67]='fab/bp/bp.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/bp/bp.c -o fab/bp/bp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[68]='fab/dep/dep.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dep/dep.c -o fab/dep/dep.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[69]='fab/dsc/dsc.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/dsc/dsc.c -o fab/dsc/dsc.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[70]='fab/enclose/enclose.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/enclose/enclose.c -o fab/enclose/enclose.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[71]='fab/ff/ff.dsc.tab.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[72]='fab/ff/ff.lex.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.lex.c -o fab/ff/ff.lex.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[73]='fab/ff/ff.list.tab.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[74]='fab/ff/ff.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.c -o fab/ff/ff.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[75]='fab/ff/ff.states.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.states.c -o fab/ff/ff.states.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[76]='fab/ff/ff.tab.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.tab.c -o fab/ff/ff.tab.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[77]='fab/ff/ff.tokens.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[78]='fab/ff/ff.var.tab.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[79]='fab/ffn/ffn.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ffn/ffn.c -o fab/ffn/ffn.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[80]='fab/ffproc/ffproc.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[81]='fab/fml/fml.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/fml/fml.c -o fab/fml/fml.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[82]='fab/gn/gn.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/gn/gn.c -o fab/gn/gn.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[83]='fab/list/list.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/list/list.c -o fab/list/list.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[84]='fab/main.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/main.c -o fab/main.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[85]='fab/selector/selector.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/selector/selector.c -o fab/selector/selector.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[86]='fab/tmp/tmp.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/tmp/tmp.c -o fab/tmp/tmp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[87]='fab/ts/ts.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/ts/ts.c -o fab/ts/ts.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[88]='fab/var/var.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -c fab/var/var.c -o fab/var/var.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[89]='liblistwise/PCRE.errtab.pic.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/PCRE.errtab.c -o liblistwise/PCRE.errtab.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[90]='liblistwise/apidata/apidata.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/apidata/apidata.c -o liblistwise/apidata/apidata.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[91]='liblistwise/error/error.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/error/error.c -o liblistwise/error/error.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[92]='liblistwise/exec/exec.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/exec/exec.c -o liblistwise/exec/exec.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[93]='liblistwise/fs/fs.pic.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/fs/fs.c -o liblistwise/fs/fs.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[94]='liblistwise/generator/generator.lex.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.lex.c -o liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[95]='liblistwise/generator/generator.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.c -o liblistwise/generator/generator.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[96]='liblistwise/generator/generator.states.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.states.c -o liblistwise/generator/generator.states.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[97]='liblistwise/generator/generator.tab.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.tab.c -o liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[98]='liblistwise/generator/generator.tokens.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/generator.tokens.c -o liblistwise/generator/generator.tokens.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[99]='liblistwise/generator/genscan.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/generator/genscan.c -o liblistwise/generator/genscan.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[100]='liblistwise/lstack/lstack.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/lstack/lstack.c -o liblistwise/lstack/lstack.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[101]='liblistwise/lwx/lwx.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/lwx/lwx.c -o liblistwise/lwx/lwx.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[102]='liblistwise/object/object.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/object/object.c -o liblistwise/object/object.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[103]='liblistwise/op/-X/-X.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/-X/-X.c -o liblistwise/op/-X/-X.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[104]='liblistwise/op/C/C.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/C/C.c -o liblistwise/op/C/C.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[105]='liblistwise/op/b/b.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/b/b.c -o liblistwise/op/b/b.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[106]='liblistwise/op/bn/bn.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/bn/bn.c -o liblistwise/op/bn/bn.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[107]='liblistwise/op/c/c.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/c/c.c -o liblistwise/op/c/c.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[108]='liblistwise/op/can/can.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/can/can.c -o liblistwise/op/can/can.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[109]='liblistwise/op/cp/cp.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/cp/cp.c -o liblistwise/op/cp/cp.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[110]='liblistwise/op/d/d.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/d/d.c -o liblistwise/op/d/d.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[111]='liblistwise/op/dj/dj.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/dj/dj.c -o liblistwise/op/dj/dj.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[112]='liblistwise/op/dn/dn.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/dn/dn.c -o liblistwise/op/dn/dn.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[113]='liblistwise/op/f/f.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/f/f.c -o liblistwise/op/f/f.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[114]='liblistwise/op/fn/fn.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/fn/fn.c -o liblistwise/op/fn/fn.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[115]='liblistwise/op/g/g.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/g/g.c -o liblistwise/op/g/g.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[116]='liblistwise/op/hl/hl.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/hl/hl.c -o liblistwise/op/hl/hl.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[117]='liblistwise/op/j/j.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/j/j.c -o liblistwise/op/j/j.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[118]='liblistwise/op/l/l.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/l/l.c -o liblistwise/op/l/l.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[119]='liblistwise/op/ls/ls.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/ls/ls.c -o liblistwise/op/ls/ls.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[120]='liblistwise/op/lx/lx.pic.o'
fml_3_55()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/lx/lx.c -o liblistwise/op/lx/lx.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[121]='liblistwise/op/o/o.pic.o'
fml_3_56()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/o/o.c -o liblistwise/op/o/o.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[122]='liblistwise/op/op.pic.o'
fml_3_57()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/op.c -o liblistwise/op/op.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[123]='liblistwise/op/p/p.pic.o'
fml_3_58()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/p/p.c -o liblistwise/op/p/p.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[124]='liblistwise/op/pop/pop.pic.o'
fml_3_59()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/pop/pop.c -o liblistwise/op/pop/pop.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[125]='liblistwise/op/r/r.pic.o'
fml_3_60()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/r/r.c -o liblistwise/op/r/r.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[126]='liblistwise/op/rb/rb.pic.o'
fml_3_61()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/rb/rb.c -o liblistwise/op/rb/rb.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[127]='liblistwise/op/re/re.pic.o'
fml_3_62()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/re/re.c -o liblistwise/op/re/re.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[128]='liblistwise/op/rp/rp.pic.o'
fml_3_63()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/rp/rp.c -o liblistwise/op/rp/rp.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[129]='liblistwise/op/rx/rx.pic.o'
fml_3_64()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/rx/rx.c -o liblistwise/op/rx/rx.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[130]='liblistwise/op/s/s.pic.o'
fml_3_65()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/s/s.c -o liblistwise/op/s/s.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[131]='liblistwise/op/shift/shift.pic.o'
fml_3_66()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/shift/shift.c -o liblistwise/op/shift/shift.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[132]='liblistwise/op/sort/sort.pic.o'
fml_3_67()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/sort/sort.c -o liblistwise/op/sort/sort.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[133]='liblistwise/op/stat/stat.pic.o'
fml_3_68()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/stat/stat.c -o liblistwise/op/stat/stat.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[134]='liblistwise/op/sx/sx.pic.o'
fml_3_69()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/sx/sx.c -o liblistwise/op/sx/sx.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[135]='liblistwise/op/t/t.pic.o'
fml_3_70()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/t/t.c -o liblistwise/op/t/t.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[136]='liblistwise/op/u/u.pic.o'
fml_3_71()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/u/u.c -o liblistwise/op/u/u.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[137]='liblistwise/op/up/up.pic.o'
fml_3_72()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/up/up.c -o liblistwise/op/up/up.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[138]='liblistwise/op/uu/uu.pic.o'
fml_3_73()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/uu/uu.c -o liblistwise/op/uu/uu.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[139]='liblistwise/op/v/v.pic.o'
fml_3_74()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/v/v.c -o liblistwise/op/v/v.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[140]='liblistwise/op/vf/vf.pic.o'
fml_3_75()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/vf/vf.c -o liblistwise/op/vf/vf.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[141]='liblistwise/op/vp/vp.pic.o'
fml_3_76()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/vp/vp.c -o liblistwise/op/vp/vp.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[142]='liblistwise/op/w/w.pic.o'
fml_3_77()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/w/w.c -o liblistwise/op/w/w.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[143]='liblistwise/op/wv/wv.pic.o'
fml_3_78()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/wv/wv.c -o liblistwise/op/wv/wv.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[144]='liblistwise/op/wvf/wvf.pic.o'
fml_3_79()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/wvf/wvf.c -o liblistwise/op/wvf/wvf.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[145]='liblistwise/op/wvp/wvp.pic.o'
fml_3_80()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/wvp/wvp.c -o liblistwise/op/wvp/wvp.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[146]='liblistwise/op/wz/wz.pic.o'
fml_3_81()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/wz/wz.c -o liblistwise/op/wz/wz.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[147]='liblistwise/op/x/x.pic.o'
fml_3_82()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/x/x.c -o liblistwise/op/x/x.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[148]='liblistwise/op/xch/xch.pic.o'
fml_3_83()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/xch/xch.c -o liblistwise/op/xch/xch.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[149]='liblistwise/op/y/y.pic.o'
fml_3_84()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/y/y.c -o liblistwise/op/y/y.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[150]='liblistwise/op/z/z.pic.o'
fml_3_85()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/op/z/z.c -o liblistwise/op/z/z.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[151]='liblistwise/re/re.pic.o'
fml_3_86()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/re/re.c -o liblistwise/re/re.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[152]='liblistwise/sanity/sanity.pic.o'
fml_3_87()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/sanity/sanity.c -o liblistwise/sanity/sanity.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[153]='liblistwise/sel/sel.pic.o'
fml_3_88()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/sel/sel.c -o liblistwise/sel/sel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[154]='liblistwise/window/window.pic.o'
fml_3_89()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Icommon/../xapi -Icommon/../xapi/libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise -Icommon -Icommon/pstring -Icommon/unitstring -Icommon/coll -Icommon/wstdlib -Icommon/parseint -Icommon/canon -Icommon/xstring -Icommon/yyutil -Icommon/cksum -Icommon/color -DXAPI_UNWIND -O3 -fPIC -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -DLWOPDIR="/usr/lib/listwise" -c liblistwise/window/window.c -o liblistwise/window/window.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[155]='xapi/libxapi/SYS.errtab.pic.o'
fml_3_90()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -I./common/../xapi/libxapi/.. -c xapi/libxapi/SYS.errtab.c -o xapi/libxapi/SYS.errtab.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[156]='xapi/libxapi/callstack/callstack.pic.o'
fml_3_91()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -I./common/../xapi/libxapi/.. -c xapi/libxapi/callstack/callstack.c -o xapi/libxapi/callstack/callstack.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[157]='xapi/libxapi/errstr/errstr.pic.o'
fml_3_92()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -I./common/../xapi/libxapi/.. -c xapi/libxapi/errstr/errstr.c -o xapi/libxapi/errstr/errstr.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[158]='xapi/libxapi/frame/frame.pic.o'
fml_3_93()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -I./common/../xapi/libxapi/.. -c xapi/libxapi/frame/frame.c -o xapi/libxapi/frame/frame.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[159]='xapi/libxapi/trace/trace.pic.o'
fml_3_94()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -I./xapi -I./common/../xapi/libxapi -I./common -I./common/pstring -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/canon -I./common/xstring -I./common/yyutil -I./common/cksum -I./common/color -DXAPI_UNWIND -O3 -fPIC -I./common/../xapi/libxapi/.. -c xapi/libxapi/trace/trace.c -o xapi/libxapi/trace/trace.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[160]='xapi/libxapi/libxapi.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  exec 99>&1 1>./common/../xapi/libxapi/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- common/wstdlib/wstdlib.c common/xstring/xstring.c xapi/libxapi/SYS.errtab.c xapi/libxapi/callstack/callstack.c xapi/libxapi/errstr/errstr.c xapi/libxapi/frame/frame.c xapi/libxapi/trace/trace.c )
echo "local: *; };"
		exec 1>&99

	gcc -DXAPI_UNWIND -D_GNU_SOURCE -I./common -I./common/../xapi/libxapi -I./common/../xapi/libxapi/.. -I./common/canon -I./common/cksum -I./common/coll -I./common/color -I./common/parseint -I./common/pstring -I./common/unitstring -I./common/wstdlib -I./common/xstring -I./common/yyutil -I./xapi -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o xapi/libxapi/libxapi.so common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o xapi/libxapi/SYS.errtab.pic.o xapi/libxapi/callstack/callstack.pic.o xapi/libxapi/errstr/errstr.pic.o xapi/libxapi/frame/frame.pic.o xapi/libxapi/trace/trace.pic.o -shared -Wl,-soname,libxapi.so -Wl,--version-script=./common/../xapi/libxapi/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[161]='libxlinux/libxlinux.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  exec 99>&1 1>libxlinux/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxlinux/xdirent/xdirent.c libxlinux/xdlfcn/xdlfcn.c libxlinux/xfcntl/xfcntl.c libxlinux/xftw/xftw.c libxlinux/xgrp/xgrp.c libxlinux/xmman/xmman.c libxlinux/xpwd/xpwd.c libxlinux/xstat/xstat.c libxlinux/xstdlib/xstdlib.c libxlinux/xtime/xtime.c libxlinux/xunistd/xunistd.c )
echo "local: *; };"
		exec 1>&99

	gcc -DXAPI_UNWIND -D_GNU_SOURCE -I./common -I./common/../xapi/libxapi -I./common/canon -I./common/cksum -I./common/coll -I./common/color -I./common/parseint -I./common/pstring -I./common/unitstring -I./common/wstdlib -I./common/xstring -I./common/yyutil -I./xapi -Ilibxlinux -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o libxlinux/libxlinux.so libxlinux/xdirent/xdirent.pic.o libxlinux/xdlfcn/xdlfcn.pic.o libxlinux/xfcntl/xfcntl.pic.o libxlinux/xftw/xftw.pic.o libxlinux/xgrp/xgrp.pic.o libxlinux/xmman/xmman.pic.o libxlinux/xpwd/xpwd.pic.o libxlinux/xstat/xstat.pic.o libxlinux/xstdlib/xstdlib.pic.o libxlinux/xtime/xtime.pic.o libxlinux/xunistd/xunistd.pic.o -shared -Wl,-soname,libxlinux.so -Wl,--version-script=libxlinux/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[162]='liblistwise/liblistwise.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  exec 99>&1 1>liblistwise/exports
	  echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- common/canon/canon.c common/coll/coll.c common/color/color.c common/parseint/parseint.c common/wstdlib/wstdlib.c common/xstring/xstring.c common/yyutil/yyutil.c liblistwise/LW.errtab.c liblistwise/PCRE.errtab.c liblistwise/apidata/apidata.c liblistwise/error/error.c liblistwise/exec/exec.c liblistwise/fs/fs.c liblistwise/generator/generator.c liblistwise/generator/generator.lex.c liblistwise/generator/generator.states.c liblistwise/generator/generator.tab.c liblistwise/generator/generator.tokens.c liblistwise/generator/genscan.c liblistwise/idx/idx.c liblistwise/lstack/lstack.c liblistwise/lwx/lwx.c liblistwise/object/object.c liblistwise/op/op.c liblistwise/re/re.c liblistwise/sanity/sanity.c liblistwise/sel/sel.c liblistwise/window/window.c )
echo "local: *; };"
		exec 1>&99

	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/liblistwise.so common/canon/canon.pic.o common/coll/coll.pic.o common/color/color.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o common/yyutil/yyutil.pic.o liblistwise/LW.errtab.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/apidata/apidata.pic.o liblistwise/error/error.pic.o liblistwise/exec/exec.pic.o liblistwise/fs/fs.pic.o liblistwise/generator/generator.lex.pic.o liblistwise/generator/generator.pic.o liblistwise/generator/generator.states.pic.o liblistwise/generator/generator.tab.pic.o liblistwise/generator/generator.tokens.pic.o liblistwise/generator/genscan.pic.o liblistwise/idx/idx.pic.o liblistwise/lstack/lstack.pic.o liblistwise/lwx/lwx.pic.o liblistwise/object/object.pic.o liblistwise/op/op.pic.o liblistwise/re/re.pic.o liblistwise/sanity/sanity.pic.o liblistwise/sel/sel.pic.o liblistwise/window/window.pic.o -lpcre -ldl libxlinux/libxlinux.so -shared -Wl,-soname,liblistwise.so -Wl,--version-script=liblistwise/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[163]='fab/fab'
fml_7_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -export-dynamic -o fab/fab common/wstdlib/wstdlib.o common/xstring/xstring.o common/cksum/cksum.o fab/log/log.o fab/FAB.errtab.o common/unitstring/unitstring.o common/canon/canon.o common/coll/coll.o fab/path/path.o fab/params/params.o fab/identity/identity.o fab/dirutil/dirutil.o fab/hashblock/hashblock.o fab/map/map.o common/yyutil/yyutil.o fab/ff/ff.tokens.o fab/ffn/ffn.o fab/ff/ff.tab.o fab/ff/ff.dsc.tab.o fab/ff/ff.var.tab.o fab/ff/ff.list.tab.o fab/ff/ff.lex.o fab/ff/ff.states.o fab/depblock/depblock.o fab/strstack/strstack.o common/pstring/pstring.o fab/ts/ts.o fab/traverse/traverse.o fab/gn/gnlw.o fab/lwutil/lwutil.o fab/var/var.o fab/list/list.o fab/enclose/enclose.o fab/fml/fml.o common/parseint/parseint.o fab/gn/gn.o fab/ff/ff.o fab/selector/selector.o fab/args/args.o fab/bp/bp.o fab/dep/dep.o fab/dsc/dsc.o fab/tmp/tmp.o fab/bake/bake.o fab/ffproc/ffproc.o fab/main.o ./common/../xapi/libxapi/libxapi.so liblistwise/liblistwise.so libxlinux/libxlinux.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[164]='fab/fablw/op/fg/fg.so'
fml_7_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o fab/fablw/op/fg/fg.so	fab/fablw/op/fg/fg.pic.o  liblistwise/liblistwise.so -shared -Wl,-soname,fg.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[165]='fab/fablw/op/fi/fi.so'
fml_7_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../xapi -Ifab/../libxlinux -Ifab/../liblistwise -Ifab/../common -Ifab/../common/pstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/canon -Ifab/../common/xstring -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/color -DXAPI_UNWIND -O3 -fno-builtin-log -Ifab/list -Ifab/bp -Ifab/dep -Ifab/ff -Ifab/ts -Ifab/depblock -Ifab/tmp -Ifab/doc -Ifab/strstack -Ifab/path -Ifab/ffproc -Ifab/dirutil -Ifab/lwutil -Ifab/fablib -Ifab/fablib/std -Ifab/fablib/std/bison -Ifab/fablib/std/flex -Ifab/fablib/std/xapi -Ifab/gn -Ifab/map -Ifab/dsc -Ifab/hashblock -Ifab/fml -Ifab/fablw -Ifab/fablw/op -Ifab/fablw/op/fi -Ifab/fablw/op/fg -Ifab/bake -Ifab/selector -Ifab/enclose -Ifab/log -Ifab/args -Ifab/ffn -Ifab/traverse -Ifab/identity -Ifab/params -Ifab/var -Ifab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/usr/lib/fab/lib" -DFABINVOKEDIR="/usr/lib/fab/listwise" -o fab/fablw/op/fi/fi.so	fab/fablw/op/fi/fi.pic.o  liblistwise/liblistwise.so -shared -Wl,-soname,fi.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[166]='liblistwise/op/-X/-X.so'
fml_7_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/-X/-X.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/-X/-X.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,-X.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[167]='liblistwise/op/C/C.so'
fml_7_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/C/C.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/C/C.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,C.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[168]='liblistwise/op/b/b.so'
fml_7_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/b/b.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/b/b.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,b.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[169]='liblistwise/op/bn/bn.so'
fml_7_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/bn/bn.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/bn/bn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,bn.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[170]='liblistwise/op/c/c.so'
fml_7_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/c/c.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/c/c.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,c.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[171]='liblistwise/op/can/can.so'
fml_7_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/can/can.so			\
	common/canon/canon.pic.o common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/can/can.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,can.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[172]='liblistwise/op/cp/cp.so'
fml_7_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/cp/cp.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/cp/cp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,cp.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[173]='liblistwise/op/d/d.so'
fml_7_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/d/d.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/d/d.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,d.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[174]='liblistwise/op/dj/dj.so'
fml_7_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/dj/dj.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/dj/dj.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dj.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[175]='liblistwise/op/dn/dn.so'
fml_7_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/dn/dn.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/dn/dn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,dn.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[176]='liblistwise/op/f/f.so'
fml_7_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/f/f.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/f/f.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,f.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[177]='liblistwise/op/fn/fn.so'
fml_7_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/fn/fn.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/fn/fn.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,fn.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[178]='liblistwise/op/g/g.so'
fml_7_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/g/g.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/g/g.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,g.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[179]='liblistwise/op/hl/hl.so'
fml_7_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/hl/hl.so			\
	common/coll/coll.pic.o common/color/color.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/hl/hl.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,hl.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[180]='liblistwise/op/j/j.so'
fml_7_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/j/j.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/j/j.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,j.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[181]='liblistwise/op/l/l.so'
fml_7_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/l/l.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/l/l.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,l.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[182]='liblistwise/op/ls/ls.so'
fml_7_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/ls/ls.so			\
	common/coll/coll.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/ls/ls.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,ls.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[183]='liblistwise/op/lx/lx.so'
fml_7_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/lx/lx.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/lx/lx.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,lx.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[184]='liblistwise/op/o/o.so'
fml_7_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/o/o.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/o/o.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,o.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[185]='liblistwise/op/p/p.so'
fml_7_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/p/p.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/p/p.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,p.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[186]='liblistwise/op/pop/pop.so'
fml_7_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/pop/pop.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/pop/pop.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,pop.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[187]='liblistwise/op/r/r.so'
fml_7_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/r/r.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/r/r.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,r.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[188]='liblistwise/op/rb/rb.so'
fml_7_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/rb/rb.so			\
	common/canon/canon.pic.o common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/rb/rb.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rb.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[189]='liblistwise/op/re/re.so'
fml_7_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/re/re.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/re/re.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,re.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[190]='liblistwise/op/rp/rp.so'
fml_7_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/rp/rp.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/rp/rp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rp.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[191]='liblistwise/op/rx/rx.so'
fml_7_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/rx/rx.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/rx/rx.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,rx.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[192]='liblistwise/op/s/s.so'
fml_7_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/s/s.so			\
	common/coll/coll.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/s/s.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,s.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[193]='liblistwise/op/shift/shift.so'
fml_7_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/shift/shift.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/shift/shift.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,shift.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[194]='liblistwise/op/sort/sort.so'
fml_7_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/sort/sort.so			\
	common/coll/coll.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/sort/sort.pic.o xapi/libxapi/SYS.errtab.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,sort.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[195]='liblistwise/op/stat/stat.so'
fml_7_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/stat/stat.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/stat/stat.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,stat.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[196]='liblistwise/op/sx/sx.so'
fml_7_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/sx/sx.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/sx/sx.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,sx.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[197]='liblistwise/op/t/t.so'
fml_7_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/t/t.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/t/t.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,t.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[198]='liblistwise/op/u/u.so'
fml_7_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/u/u.so			\
	common/coll/coll.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/u/u.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,u.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[199]='liblistwise/op/up/up.so'
fml_7_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/up/up.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/up/up.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,up.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[200]='liblistwise/op/uu/uu.so'
fml_7_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/uu/uu.so			\
	common/coll/coll.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/uu/uu.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,uu.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[201]='liblistwise/op/v/v.so'
fml_7_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/v/v.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/v/v.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,v.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[202]='liblistwise/op/vf/vf.so'
fml_7_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/vf/vf.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/vf/vf.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,vf.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[203]='liblistwise/op/vp/vp.so'
fml_7_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/vp/vp.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/vp/vp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,vp.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[204]='liblistwise/op/w/w.so'
fml_7_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/w/w.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/w/w.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,w.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[205]='liblistwise/op/wv/wv.so'
fml_7_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/wv/wv.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/wv/wv.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,wv.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[206]='liblistwise/op/wvf/wvf.so'
fml_7_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/wvf/wvf.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/wvf/wvf.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,wvf.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[207]='liblistwise/op/wvp/wvp.so'
fml_7_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/wvp/wvp.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/wvp/wvp.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,wvp.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[208]='liblistwise/op/wz/wz.so'
fml_7_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/wz/wz.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/wz/wz.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,wz.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[209]='liblistwise/op/x/x.so'
fml_7_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/x/x.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/x/x.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,x.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[210]='liblistwise/op/xch/xch.so'
fml_7_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/xch/xch.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/xch/xch.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,xch.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[211]='liblistwise/op/y/y.so'
fml_7_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/y/y.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/y/y.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,y.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[212]='liblistwise/op/z/z.so'
fml_7_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -DLWOPDIR="/usr/lib/listwise" -DXAPI_UNWIND -D_GNU_SOURCE -Icommon -Icommon/../xapi -Icommon/../xapi/libxapi -Icommon/canon -Icommon/cksum -Icommon/coll -Icommon/color -Icommon/parseint -Icommon/pstring -Icommon/unitstring -Icommon/wstdlib -Icommon/xstring -Icommon/yyutil -Iliblistwise -Iliblistwise/../common/../libxlinux -Iliblistwise/generator -Iliblistwise/idx -Iliblistwise/listwise -Iliblistwise/sanity -O3 -Wall -Werror -fPIC -fplan9-extensions -m64 -o liblistwise/op/z/z.so			\
	common/coll/coll.pic.o common/wstdlib/wstdlib.pic.o common/xstring/xstring.pic.o liblistwise/PCRE.errtab.pic.o liblistwise/idx/idx.pic.o liblistwise/op/z/z.pic.o						\
	liblistwise/liblistwise.so -shared										\
	-Wl,-soname,z.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[213]='listwise/listwise'
fml_7_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -DFABVERSIONN="500" -DFABVERSIONS="v0.5.0.0" -DXAPI_UNWIND -D_GNU_SOURCE -Ilistwise -Ilistwise/../common -Ilistwise/../common/canon -Ilistwise/../common/cksum -Ilistwise/../common/coll -Ilistwise/../common/color -Ilistwise/../common/parseint -Ilistwise/../common/pstring -Ilistwise/../common/unitstring -Ilistwise/../common/wstdlib -Ilistwise/../common/xstring -Ilistwise/../common/yyutil -Ilistwise/../liblistwise -Ilistwise/../libxlinux -Ilistwise/../xapi -Ilistwise/../xapi/libxapi -O3 -Wall -Werror -fplan9-extensions -m64 -o listwise/listwise listwise/LW.errtab.o common/wstdlib/wstdlib.o common/xstring/xstring.o listwise/args.o listwise/main.o liblistwise/liblistwise.so ./common/../xapi/libxapi/libxapi.so libxlinux/libxlinux.so


  X=$?
  echo 12 1>&99
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
  ((SKP+=19))
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

  # harvest stage 1.0
  C=0
  while [[ $C != 19 ]]; do
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
  ((SKP+=19))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[18]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((19+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=6))
else
  # launch stage 1.2
  exec 100>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_43 & PIDS[5]=$!

  # harvest stage 1.2
  C=0
  while [[ $C != 6 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((38+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+38)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
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

  # harvest stage 2.0
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((44+$idx))
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
  ((SKP+=2))
else
  # launch stage 2.1
  exec 100>$tmp ; rm -f $tmp ; fml_2_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_20 & PIDS[1]=$!

  # harvest stage 2.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((63+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+19)) $X "$N"
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
    I=$((65+$idx))
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
  ((SKP+=19))
else
  # launch stage 3.1
  exec 100>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_3_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_3_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_3_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_3_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_3_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_3_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_3_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_3_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_3_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_3_37 & PIDS[18]=$!

  # harvest stage 3.1
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((84+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 3.2
  exec 100>$tmp ; rm -f $tmp ; fml_3_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_43 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_44 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_45 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_46 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_3_47 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_3_48 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_3_49 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_3_50 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_3_51 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_3_52 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_3_53 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_3_54 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_3_55 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_3_56 & PIDS[18]=$!

  # harvest stage 3.2
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((103+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+38)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 3.3
  exec 100>$tmp ; rm -f $tmp ; fml_3_57 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_58 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_59 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_60 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_61 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_62 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_63 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_64 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_65 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_3_66 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_3_67 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_3_68 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_3_69 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_3_70 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_3_71 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_3_72 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_3_73 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_3_74 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_3_75 & PIDS[18]=$!

  # harvest stage 3.3
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((122+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+57)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 3.4
  exec 100>$tmp ; rm -f $tmp ; fml_3_76 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_77 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_78 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_79 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_80 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_81 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_82 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_83 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_84 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_3_85 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_3_86 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_3_87 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_3_88 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_3_89 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_3_90 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_3_91 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_3_92 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_3_93 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_3_94 & PIDS[18]=$!

  # harvest stage 3.4
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((141+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+76)) $X "$N"
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
    I=$((160+$idx))
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
    I=$((161+$idx))
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
    I=$((162+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 6 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 7.0
  exec 100>$tmp ; rm -f $tmp ; fml_7_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_7_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_7_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_7_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_7_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_7_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_7_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_7_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_7_8 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_7_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_7_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_7_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_7_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_7_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_7_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_7_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_7_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_7_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_7_18 & PIDS[18]=$!

  # harvest stage 7.0
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((163+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 7 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 7.1
  exec 100>$tmp ; rm -f $tmp ; fml_7_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_7_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_7_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_7_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_7_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_7_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_7_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_7_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_7_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_7_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_7_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_7_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_7_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_7_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_7_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_7_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_7_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_7_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_7_37 & PIDS[18]=$!

  # harvest stage 7.1
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((182+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 7 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=13))
else
  # launch stage 7.2
  exec 100>$tmp ; rm -f $tmp ; fml_7_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_7_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_7_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_7_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_7_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_7_43 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_7_44 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_7_45 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_7_46 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_7_47 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_7_48 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_7_49 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_7_50 & PIDS[12]=$!

  # harvest stage 7.2
  C=0
  while [[ $C != 13 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((201+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 7 $((idx+38)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
