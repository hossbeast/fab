#!/bin/bash

# Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>
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
NAMES[0]='common/cksum/cksum.devel.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/color/color.devel.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/parseint/parseint.devel.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint/parseint.devel.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/strutil/strutil.devel.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/strutil/strutil.devel.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring/unitstring.devel.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.devel.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/wstdlib/wstdlib.devel.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/wstdlib/wstdlib.devel.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[9]='{fab/error/FAB.errtab.h, fab/error/FAB.errtab.c}'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	hfl=fab/error/FAB.errtab.h
	hin=FAB.errtab.h
	cfl=fab/error/FAB.errtab.c
	pfx=FAB
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat fab/error/FAB.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat fab/error/FAB.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat fab/error/FAB.errtab | \
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
  echo 9 1>&99
  exit $X
}

NAMES[10]='{fab/ff/ff.dsc.tab.h, fab/ff/ff.dsc.tab.c}'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	bison --warnings=error -o fab/ff/ff.dsc.tab.c -d	fab/ff/ff.dsc.y


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[11]='{fab/ff/ff.lex.h, fab/ff/ff.lex.c}'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	  flex  -o fab/ff/ff.lex.c  --header-file=fab/ff/ff.lex.h  fab/ff/ff.l


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[12]='{fab/ff/ff.list.tab.h, fab/ff/ff.list.tab.c}'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	bison --warnings=error -o fab/ff/ff.list.tab.c -d	fab/ff/ff.list.y


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='{fab/ff/ff.tab.h, fab/ff/ff.tab.c}'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	bison --warnings=error -o fab/ff/ff.tab.c -d	fab/ff/ff.y


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='{fab/ff/ff.var.tab.h, fab/ff/ff.var.tab.c}'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	bison --warnings=error -o fab/ff/ff.var.tab.c -d	fab/ff/ff.var.y


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[15]='libpstring/exports'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libpstring/pstring/pstring.c )
echo "local: *; };"
	  ) 1>libpstring/exports


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[16]='libxapi/error/SYS.errtab'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	exec > libxapi/error/SYS.errtab

	cat /usr/include/asm-generic/errno-base.h /usr/include/asm-generic/errno.h  | \
sed -e 's/#define\s\+\(E[A-Z]\+\)\s\+\([0-9]\+\)\s\+\/\*\s*\(.*\)\s\+\*\//\2\t\1\t\3/p; d'


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='{libxapi/error/XAPI.errtab.h, libxapi/error/XAPI.errtab.c}'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	hfl=libxapi/error/XAPI.errtab.h
	hin=XAPI.errtab.h
	cfl=libxapi/error/XAPI.errtab.c
	pfx=XAPI
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat libxapi/error/XAPI.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat libxapi/error/XAPI.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat libxapi/error/XAPI.errtab | \
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
  echo 17 1>&99
  exit $X
}

NAMES[18]='libxapi/test.devel.pic.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/test.c -o libxapi/test.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='{libxlinux/error/XLINUX.errtab.h, libxlinux/error/XLINUX.errtab.c}'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	hfl=libxlinux/error/XLINUX.errtab.h
	hin=XLINUX.errtab.h
	cfl=libxlinux/error/XLINUX.errtab.c
	pfx=XLINUX
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat libxlinux/error/XLINUX.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat libxlinux/error/XLINUX.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat libxlinux/error/XLINUX.errtab | \
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
  echo 0 1>&99
  exit $X
}

NAMES[20]='{listwise/liblistwise/error/LW.errtab.h, listwise/liblistwise/error/LW.errtab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	hfl=listwise/liblistwise/error/LW.errtab.h
	hin=LW.errtab.h
	cfl=listwise/liblistwise/error/LW.errtab.c
	pfx=LW
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat listwise/liblistwise/error/LW.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat listwise/liblistwise/error/LW.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat listwise/liblistwise/error/LW.errtab | \
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

NAMES[21]='listwise/liblistwise/error/PCRE.errtab'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	exec 1>listwise/liblistwise/error/PCRE.errtab
sed -e 's/#define \(PCRE_ERROR_[A-Z_]\+\) \+(\([-0-9]\+\)).*/\2\t\1/p; d' <\
	/usr/include/pcre.h


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[22]='{listwise/liblistwise/generator/generator.lex.h, listwise/liblistwise/generator/generator.lex.c}'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	  flex  -o listwise/liblistwise/generator/generator.lex.c  --header-file=listwise/liblistwise/generator/generator.lex.h  listwise/liblistwise/generator/generator.l


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[23]='{listwise/liblistwise/generator/generator.tab.h, listwise/liblistwise/generator/generator.tab.c}'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	bison --warnings=error -o listwise/liblistwise/generator/generator.tab.c -d	listwise/liblistwise/generator/generator.y


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[24]='{listwise/listwise/error/LISTWISE.errtab.h, listwise/listwise/error/LISTWISE.errtab.c}'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	hfl=listwise/listwise/error/LISTWISE.errtab.h
	hin=LISTWISE.errtab.h
	cfl=listwise/listwise/error/LISTWISE.errtab.c
	pfx=LISTWISE
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat listwise/listwise/error/LISTWISE.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat listwise/listwise/error/LISTWISE.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat listwise/listwise/error/LISTWISE.errtab | \
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
  echo 5 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[25]='common/canon/canon.devel.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[26]='common/canon/canon.devel.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[27]='common/coll/coll.devel.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[28]='common/coll/coll.devel.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[29]='common/logger/logger.devel.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[30]='common/map/map.devel.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/map/map.c -o common/map/map.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[31]='common/map/map.devel.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.devel.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[33]='common/yyutil/yyutil.devel.pic.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[34]='fab/depblock/depblock.devel.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/depblock/depblock.c -o fab/depblock/depblock.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[35]='fab/dirutil/dirutil.devel.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.devel.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[36]='fab/error/FAB.errtab.devel.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/error/FAB.errtab.c -o fab/error/FAB.errtab.devel.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[37]='{fab/ff/ff.states.h, fab/ff/ff.states.c}'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  
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
  echo 12 1>&99
  exit $X
}

NAMES[38]='{fab/ff/ff.tokens.h, fab/ff/ff.tokens.c}'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  
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
  echo 13 1>&99
  exit $X
}

NAMES[39]='fab/gn/gnlw.devel.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/gn/gnlw.c -o fab/gn/gnlw.devel.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[40]='fab/hashblock/hashblock.devel.o'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.devel.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[41]='fab/identity/identity.devel.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/identity/identity.c -o fab/identity/identity.devel.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[42]='fab/logs/logs.devel.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/logs/logs.c -o fab/logs/logs.devel.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[43]='fab/lwutil/lwutil.devel.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.devel.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[44]='fab/params/params.devel.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/params/params.c -o fab/params/params.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[45]='fab/path/path.devel.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/path/path.c -o fab/path/path.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[46]='fab/strstack/strstack.devel.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/strstack/strstack.c -o fab/strstack/strstack.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[47]='fab/traverse/traverse.devel.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/traverse/traverse.c -o fab/traverse/traverse.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[48]='libpstring/pstring/pstring.devel.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[49]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	hfl=libxapi/error/SYS.errtab.h
	hin=SYS.errtab.h
	cfl=libxapi/error/SYS.errtab.c
	pfx=SYS
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat libxapi/error/SYS.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat libxapi/error/SYS.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat libxapi/error/SYS.errtab | \
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
  echo 5 1>&99
  exit $X
}

NAMES[50]='libxapi/error/XAPI.errtab.devel.pic.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[51]='libxlinux/error/XLINUX.errtab.devel.pic.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[52]='libxlinux/error/error.devel.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/error/error.c -o libxlinux/error/error.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[53]='libxlinux/exports'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxlinux/xgrp/xgrp.c libxlinux/error/error.c libxlinux/error/XLINUX.errtab.c libxlinux/xstat/xstat.c libxlinux/xunistd/xunistd.c libxlinux/xpwd/xpwd.c libxlinux/xmman/xmman.c libxlinux/xdirent/xdirent.c libxlinux/xfcntl/xfcntl.c libxlinux/xftw/xftw.c libxlinux/xstdlib/xstdlib.c libxlinux/xdlfcn/xdlfcn.c libxlinux/xstring/xstring.c libxlinux/xtime/xtime.c )
echo "local: *; };"
	  ) 1>libxlinux/exports


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[54]='libxlinux/xdirent/xdirent.devel.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[55]='libxlinux/xdlfcn/xdlfcn.devel.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[56]='libxlinux/xfcntl/xfcntl.devel.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[57]='libxlinux/xftw/xftw.devel.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[58]='libxlinux/xgrp/xgrp.devel.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[59]='libxlinux/xmman/xmman.devel.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[60]='libxlinux/xpwd/xpwd.devel.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[61]='libxlinux/xstat/xstat.devel.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[62]='libxlinux/xstdlib/xstdlib.devel.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[63]='libxlinux/xstring/xstring.devel.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[64]='libxlinux/xtime/xtime.devel.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[65]='libxlinux/xunistd/xunistd.devel.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[66]='listwise/liblistwise/error/LW.errtab.devel.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/error/LW.errtab.c -o listwise/liblistwise/error/LW.errtab.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[67]='{listwise/liblistwise/error/PCRE.errtab.h, listwise/liblistwise/error/PCRE.errtab.c}'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	hfl=listwise/liblistwise/error/PCRE.errtab.h
	hin=PCRE.errtab.h
	cfl=listwise/liblistwise/error/PCRE.errtab.c
	pfx=PCRE
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat listwise/liblistwise/error/PCRE.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat listwise/liblistwise/error/PCRE.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat listwise/liblistwise/error/PCRE.errtab | \
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
  echo 4 1>&99
  exit $X
}

NAMES[68]='{listwise/liblistwise/generator/generator.states.h, listwise/liblistwise/generator/generator.states.c}'
fml_2_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	src=listwise/liblistwise/generator/generator.lex.h
	states_h=listwise/liblistwise/generator/generator.states.h
	states_c=listwise/liblistwise/generator/generator.states.c
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
  echo 5 1>&99
  exit $X
}

NAMES[69]='{listwise/liblistwise/generator/generator.tokens.h, listwise/liblistwise/generator/generator.tokens.c}'
fml_2_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	src=listwise/liblistwise/generator/generator.tab.c
	tokens_h=listwise/liblistwise/generator/generator.tokens.h
	tokens_c=listwise/liblistwise/generator/generator.tokens.c
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
  echo 6 1>&99
  exit $X
}

NAMES[70]='listwise/listwise/args/args.devel.o'
fml_2_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/args/args.c -o listwise/listwise/args/args.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[71]='listwise/listwise/error/LISTWISE.errtab.devel.o'
fml_2_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/error/LISTWISE.errtab.c -o listwise/listwise/error/LISTWISE.errtab.devel.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[72]='listwise/listwise/logs/logs.devel.o'
fml_2_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/logs/logs.c -o listwise/listwise/logs/logs.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[73]='listwise/listwise/main.devel.o'
fml_2_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/main.c -o listwise/listwise/main.devel.o


  X=$?
  echo 10 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[74]='fab/args/args.devel.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/args/args.c -o fab/args/args.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[75]='fab/bake/bake.devel.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/bake/bake.c -o fab/bake/bake.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[76]='fab/bp/bp.devel.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/bp/bp.c -o fab/bp/bp.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[77]='fab/dep/dep.devel.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dep/dep.c -o fab/dep/dep.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[78]='fab/dsc/dsc.devel.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dsc/dsc.c -o fab/dsc/dsc.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[79]='fab/enclose/enclose.devel.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/enclose/enclose.c -o fab/enclose/enclose.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[80]='fab/error/error.devel.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/error/error.c -o fab/error/error.devel.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[81]='fab/ff/ff.devel.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.c -o fab/ff/ff.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[82]='fab/ff/ff.dsc.tab.devel.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.devel.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[83]='fab/ff/ff.lex.devel.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.lex.c -o fab/ff/ff.lex.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[84]='fab/ff/ff.list.tab.devel.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.devel.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[85]='fab/ff/ff.states.devel.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.states.c -o fab/ff/ff.states.devel.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[86]='fab/ff/ff.tab.devel.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.tab.c -o fab/ff/ff.tab.devel.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[87]='fab/ff/ff.tokens.devel.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.devel.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[88]='fab/ff/ff.var.tab.devel.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.devel.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[89]='fab/ffn/ffn.devel.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ffn/ffn.c -o fab/ffn/ffn.devel.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[90]='fab/ffproc/ffproc.devel.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.devel.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[91]='fab/fml/fml.devel.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/fml/fml.c -o fab/fml/fml.devel.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[92]='fab/gn/gn.devel.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/gn/gn.c -o fab/gn/gn.devel.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[93]='fab/list/list.devel.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/list/list.c -o fab/list/list.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[94]='fab/main.devel.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/main.c -o fab/main.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[95]='fab/selector/selector.devel.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/selector/selector.c -o fab/selector/selector.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[96]='fab/tmp/tmp.devel.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/tmp/tmp.c -o fab/tmp/tmp.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[97]='fab/ts/ts.devel.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ts/ts.c -o fab/ts/ts.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[98]='fab/var/var.devel.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/var/var.c -o fab/var/var.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[99]='libxapi/callstack/callstack.devel.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[100]='libxapi/error/SYS.errtab.devel.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[101]='libxapi/error/error.devel.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/error.c -o libxapi/error/error.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[102]='libxapi/exports'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxapi/error/error.c libxapi/error/SYS.errtab.c libxapi/error/XAPI.errtab.c libxapi/trace/trace.c libxapi/callstack/callstack.c libxapi/test.c libxapi/frame/frame.c )
	  echo "callstack;"
echo "local: *; };"
	) 1>libxapi/exports


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[103]='libxapi/frame/frame.devel.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/frame/frame.c -o libxapi/frame/frame.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[104]='libxapi/trace/trace.devel.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/trace/trace.c -o libxapi/trace/trace.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[105]='listwise/liblistwise/describe/describe.devel.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/describe/describe.c -o listwise/liblistwise/describe/describe.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[106]='listwise/liblistwise/error/PCRE.errtab.devel.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/error/PCRE.errtab.c -o listwise/liblistwise/error/PCRE.errtab.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[107]='listwise/liblistwise/error/error.devel.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/error/error.c -o listwise/liblistwise/error/error.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[108]='listwise/liblistwise/exec/exec.devel.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/exec/exec.c -o listwise/liblistwise/exec/exec.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[109]='listwise/liblistwise/exports'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- listwise/liblistwise/exec/exec.c listwise/liblistwise/logging/logging.c listwise/liblistwise/error/LW.errtab.c listwise/liblistwise/error/PCRE.errtab.c listwise/liblistwise/error/error.c listwise/liblistwise/iterate/iterate.c listwise/liblistwise/object/object.c listwise/liblistwise/op/z/test.c listwise/liblistwise/op/z/z.c listwise/liblistwise/op/o/o.c listwise/liblistwise/op/o/test.c listwise/liblistwise/op/vf/vf.c listwise/liblistwise/op/wvp/wvp.c listwise/liblistwise/op/wvp/test.c listwise/liblistwise/op/sort/sort.c listwise/liblistwise/op/sort/test.c listwise/liblistwise/op/w/w.c listwise/liblistwise/op/shift/shift.c listwise/liblistwise/op/dj/dj.c listwise/liblistwise/op/uu/uu.c listwise/liblistwise/op/uu/test.c listwise/liblistwise/op/u/test.c listwise/liblistwise/op/u/u.c listwise/liblistwise/op/bn/bn.c listwise/liblistwise/op/re/re.c listwise/liblistwise/op/rp/rp.c listwise/liblistwise/op/f/f.c listwise/liblistwise/op/f/test.c listwise/liblistwise/op/ls/ls.c listwise/liblistwise/op/lx/lx.c listwise/liblistwise/op/lx/test.c listwise/liblistwise/op/hl/hl.c listwise/liblistwise/op/rb/rb.c listwise/liblistwise/op/j/j.c listwise/liblistwise/op/-X/-X.c listwise/liblistwise/op/up/up.c listwise/liblistwise/op/cp/cp.c listwise/liblistwise/op/p/p.c listwise/liblistwise/op/p/test.c listwise/liblistwise/op/c/c.c listwise/liblistwise/op/r/r.c listwise/liblistwise/op/xch/xch.c listwise/liblistwise/op/t/t.c listwise/liblistwise/op/s/s.c listwise/liblistwise/op/cons/cons.c listwise/liblistwise/op/wvf/wvf.c listwise/liblistwise/op/wvf/test.c listwise/liblistwise/op/dn/dn.c listwise/liblistwise/op/stat/stat.c listwise/liblistwise/op/g/g.c listwise/liblistwise/op/d/d.c listwise/liblistwise/op/vp/vp.c listwise/liblistwise/op/can/can.c listwise/liblistwise/op/datef/datef.c listwise/liblistwise/op/x/x.c listwise/liblistwise/op/b/b.c listwise/liblistwise/op/C/C.c listwise/liblistwise/op/rx/rx.c listwise/liblistwise/op/y/y.c listwise/liblistwise/op/y/test.c listwise/liblistwise/op/fn/fn.c listwise/liblistwise/op/pop/pop.c listwise/liblistwise/op/sx/sx.c listwise/liblistwise/op/sx/test.c listwise/liblistwise/op/l/l.c listwise/liblistwise/op/l/test.c listwise/liblistwise/op/v/test.c listwise/liblistwise/op/v/v.c listwise/liblistwise/describe/describe.c listwise/liblistwise/test/fab_test/test.c listwise/liblistwise/test/listwise_test/listwise_test.c listwise/liblistwise/lwx/lwx.c listwise/liblistwise/re/re.c listwise/liblistwise/lstack/lstack.c listwise/liblistwise/fs/fs.c listwise/liblistwise/selection/selection.c listwise/liblistwise/operators/operators.c listwise/liblistwise/operator/operator.c listwise/liblistwise/sanity/sanity.c listwise/liblistwise/generator/generator.lex.c listwise/liblistwise/generator/generator.c listwise/liblistwise/generator/generator.states.c listwise/liblistwise/generator/generator.tab.c listwise/liblistwise/generator/generator.tokens.c listwise/liblistwise/generator/genscan.c listwise/liblistwise/window/window.c listwise/liblistwise/tune/tune.c )
echo "local: *; };"
	 ) 1>listwise/liblistwise/exports


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[110]='listwise/liblistwise/fs/fs.devel.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/fs/fs.c -o listwise/liblistwise/fs/fs.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[111]='listwise/liblistwise/generator/generator.devel.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/generator.c -o listwise/liblistwise/generator/generator.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[112]='listwise/liblistwise/generator/generator.lex.devel.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/generator.lex.c -o listwise/liblistwise/generator/generator.lex.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[113]='listwise/liblistwise/generator/generator.states.devel.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/generator.states.c -o listwise/liblistwise/generator/generator.states.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[114]='listwise/liblistwise/generator/generator.tab.devel.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/generator.tab.c -o listwise/liblistwise/generator/generator.tab.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[115]='listwise/liblistwise/generator/generator.tokens.devel.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/generator.tokens.c -o listwise/liblistwise/generator/generator.tokens.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[116]='listwise/liblistwise/generator/genscan.devel.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/generator/genscan.c -o listwise/liblistwise/generator/genscan.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[117]='listwise/liblistwise/iterate/iterate.devel.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/iterate/iterate.c -o listwise/liblistwise/iterate/iterate.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[118]='listwise/liblistwise/logging/logging.devel.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/logging/logging.c -o listwise/liblistwise/logging/logging.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[119]='listwise/liblistwise/lstack/lstack.devel.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/lstack/lstack.c -o listwise/liblistwise/lstack/lstack.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[120]='listwise/liblistwise/lwx/lwx.devel.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/lwx/lwx.c -o listwise/liblistwise/lwx/lwx.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[121]='listwise/liblistwise/object/object.devel.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/object/object.c -o listwise/liblistwise/object/object.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[122]='listwise/liblistwise/operator/operator.devel.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/operator/operator.c -o listwise/liblistwise/operator/operator.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[123]='listwise/liblistwise/operators/operators.devel.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/operators/operators.c -o listwise/liblistwise/operators/operators.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[124]='listwise/liblistwise/re/re.devel.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/re/re.c -o listwise/liblistwise/re/re.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[125]='listwise/liblistwise/sanity/sanity.devel.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/sanity/sanity.c -o listwise/liblistwise/sanity/sanity.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[126]='listwise/liblistwise/selection/selection.devel.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/selection/selection.c -o listwise/liblistwise/selection/selection.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[127]='listwise/liblistwise/tune/tune.devel.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/tune/tune.c -o listwise/liblistwise/tune/tune.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[128]='listwise/liblistwise/window/window.devel.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/window/window.c -o listwise/liblistwise/window/window.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[129]='libxapi/libxapi.devel.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -o libxapi/libxapi.devel.so common/strutil/strutil.devel.pic.o common/wstdlib/wstdlib.devel.pic.o libxapi/callstack/callstack.devel.pic.o libxapi/error/SYS.errtab.devel.pic.o libxapi/error/XAPI.errtab.devel.pic.o libxapi/error/error.devel.pic.o libxapi/frame/frame.devel.pic.o libxapi/test.devel.pic.o libxapi/trace/trace.devel.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.devel.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[130]='libxlinux/libxlinux.devel.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -o libxlinux/libxlinux.devel.so libxlinux/error/XLINUX.errtab.devel.pic.o libxlinux/error/error.devel.pic.o libxlinux/xdirent/xdirent.devel.pic.o libxlinux/xdlfcn/xdlfcn.devel.pic.o libxlinux/xfcntl/xfcntl.devel.pic.o libxlinux/xftw/xftw.devel.pic.o libxlinux/xgrp/xgrp.devel.pic.o libxlinux/xmman/xmman.devel.pic.o libxlinux/xpwd/xpwd.devel.pic.o libxlinux/xstat/xstat.devel.pic.o libxlinux/xstdlib/xstdlib.devel.pic.o libxlinux/xstring/xstring.devel.pic.o libxlinux/xtime/xtime.devel.pic.o libxlinux/xunistd/xunistd.devel.pic.o ./libxapi/libxapi.devel.so -ldl -shared -Wl,--version-script=libxlinux/exports -Wl,-soname,libxlinux.devel.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[131]='libpstring/libpstring.devel.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring.devel.so libpstring/pstring/pstring.devel.pic.o ./libxapi/libxapi.devel.so -shared -Wl,--version-script=libpstring/exports ./libxlinux/libxlinux.devel.so -Wl,-soname,libpstring.devel.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[132]='listwise/liblistwise/liblistwise.devel.so'
fml_7_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fplan9-extensions \
-DXAPI_PROVIDE_BACKTRACE \
-I./common/unitstring \
-I./common/coll \
-I./common/wstdlib \
-I./common/parseint \
-I./common/logger \
-I./common/map \
-I./common/canon \
-I./common/yyutil \
-I./common/cksum \
-I./common/strutil \
-I./common/color \
-I./common/wstring \
-I./common \
-DBUILDSTAMP="`date`" \
-O0 \
-g \
-ggdb3 \
-DDEVEL \
-DXAPI_RUNTIME_CHECKS \
-DSANITY \
-DBUILDROOT=/home/todd/fab \
-fPIC \
-I./libxapi \
-I./libxlinux \
-I./libpstring \
-I./xunit/libxunit \
-I./listwise/liblistwise/exec \
-I./listwise/liblistwise/logging \
-I./listwise/liblistwise/error \
-I./listwise/liblistwise/iterate \
-I./listwise/liblistwise/object \
-I./listwise/liblistwise/op \
-I./listwise/liblistwise/describe \
-I./listwise/liblistwise/test \
-I./listwise/liblistwise/lwx \
-I./listwise/liblistwise/re \
-I./listwise/liblistwise/internal \
-I./listwise/liblistwise/lstack \
-I./listwise/liblistwise/listwise \
-I./listwise/liblistwise/fs \
-I./listwise/liblistwise/selection \
-I./listwise/liblistwise/operators \
-I./listwise/liblistwise/operator \
-I./listwise/liblistwise/sanity \
-I./listwise/liblistwise/generator \
-I./listwise/liblistwise/window \
-I./listwise/liblistwise/tune \
-I./listwise/liblistwise \
-DLWOPDIR=/home/todd/fab/listwise/liblistwise/op \
-DLWOPEXT=devel.so \
-o \
listwise/liblistwise/liblistwise.devel.so \
listwise/liblistwise/error/LW.errtab.devel.pic.o \
listwise/liblistwise/re/re.devel.pic.o \
common/coll/coll.devel.pic.o \
common/map/map.devel.pic.o \
listwise/liblistwise/object/object.devel.pic.o \
common/wstdlib/wstdlib.devel.pic.o \
listwise/liblistwise/lwx/lwx.devel.pic.o \
listwise/liblistwise/lstack/lstack.devel.pic.o \
listwise/liblistwise/logging/logging.devel.pic.o \
listwise/liblistwise/iterate/iterate.devel.pic.o \
listwise/liblistwise/generator/generator.states.devel.pic.o \
listwise/liblistwise/generator/genscan.devel.pic.o \
listwise/liblistwise/describe/describe.devel.pic.o \
listwise/liblistwise/sanity/sanity.devel.pic.o \
listwise/liblistwise/exec/exec.devel.pic.o \
listwise/liblistwise/error/PCRE.errtab.devel.pic.o \
listwise/liblistwise/error/error.devel.pic.o \
common/strutil/strutil.devel.pic.o \
common/canon/canon.devel.pic.o \
common/parseint/parseint.devel.pic.o \
common/color/color.devel.pic.o \
listwise/liblistwise/fs/fs.devel.pic.o \
listwise/liblistwise/selection/selection.devel.pic.o \
listwise/liblistwise/operators/operators.devel.pic.o \
listwise/liblistwise/operator/operator.devel.pic.o \
common/yyutil/yyutil.devel.pic.o \
listwise/liblistwise/generator/generator.tab.devel.pic.o \
listwise/liblistwise/generator/generator.tokens.devel.pic.o \
listwise/liblistwise/generator/generator.lex.devel.pic.o \
listwise/liblistwise/generator/generator.devel.pic.o \
listwise/liblistwise/window/window.devel.pic.o \
listwise/liblistwise/tune/tune.devel.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=listwise/liblistwise/exports \
./libxapi/libxapi.devel.so \
./libxlinux/libxlinux.devel.so \
./libpstring/libpstring.devel.so \
-Wl,-soname,liblistwise.devel.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[133]='fab/fab.devel'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -export-dynamic -o fab/fab.devel common/wstdlib/wstdlib.devel.o common/strutil/strutil.devel.o common/logger/logger.devel.o fab/logs/logs.devel.o fab/error/FAB.errtab.devel.o common/unitstring/unitstring.devel.o common/canon/canon.devel.o common/coll/coll.devel.o common/cksum/cksum.devel.o fab/path/path.devel.o fab/params/params.devel.o fab/identity/identity.devel.o fab/dirutil/dirutil.devel.o fab/hashblock/hashblock.devel.o common/map/map.devel.o common/yyutil/yyutil.devel.o fab/ff/ff.tokens.devel.o fab/ffn/ffn.devel.o fab/ff/ff.tab.devel.o fab/ff/ff.dsc.tab.devel.o fab/ff/ff.var.tab.devel.o fab/ff/ff.list.tab.devel.o fab/ff/ff.lex.devel.o fab/ff/ff.states.devel.o fab/depblock/depblock.devel.o fab/strstack/strstack.devel.o fab/ts/ts.devel.o fab/traverse/traverse.devel.o fab/gn/gnlw.devel.o fab/lwutil/lwutil.devel.o fab/var/var.devel.o fab/list/list.devel.o fab/enclose/enclose.devel.o fab/fml/fml.devel.o common/parseint/parseint.devel.o fab/gn/gn.devel.o fab/ff/ff.devel.o fab/selector/selector.devel.o fab/args/args.devel.o fab/bp/bp.devel.o fab/dep/dep.devel.o fab/dsc/dsc.devel.o fab/tmp/tmp.devel.o fab/bake/bake.devel.o fab/ffproc/ffproc.devel.o fab/error/error.devel.o fab/main.devel.o ./libxapi/libxapi.devel.so ./libxlinux/libxlinux.devel.so ./libpstring/libpstring.devel.so ./listwise/liblistwise/liblistwise.devel.so '-Wl,-rpath,$ORIGIN/.././libxapi' '-Wl,-rpath,$ORIGIN/.././libxlinux' '-Wl,-rpath,$ORIGIN/.././xunit/libxunit' '-Wl,-rpath,$ORIGIN/.././libpstring' '-Wl,-rpath,$ORIGIN/.././listwise/liblistwise'


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[134]='listwise/liblistwise/op/-X/-X.devel.pic.o'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/-X/-X.c -o listwise/liblistwise/op/-X/-X.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[135]='listwise/liblistwise/op/C/C.devel.pic.o'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/C/C.c -o listwise/liblistwise/op/C/C.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[136]='listwise/liblistwise/op/b/b.devel.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/b/b.c -o listwise/liblistwise/op/b/b.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[137]='listwise/liblistwise/op/bn/bn.devel.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/bn/bn.c -o listwise/liblistwise/op/bn/bn.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[138]='listwise/liblistwise/op/c/c.devel.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/c/c.c -o listwise/liblistwise/op/c/c.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[139]='listwise/liblistwise/op/can/can.devel.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/can/can.c -o listwise/liblistwise/op/can/can.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[140]='listwise/liblistwise/op/cons/cons.devel.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/cons/cons.c -o listwise/liblistwise/op/cons/cons.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[141]='listwise/liblistwise/op/cp/cp.devel.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/cp/cp.c -o listwise/liblistwise/op/cp/cp.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[142]='listwise/liblistwise/op/d/d.devel.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/d/d.c -o listwise/liblistwise/op/d/d.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[143]='listwise/liblistwise/op/datef/datef.devel.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/datef/datef.c -o listwise/liblistwise/op/datef/datef.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[144]='listwise/liblistwise/op/dj/dj.devel.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/dj/dj.c -o listwise/liblistwise/op/dj/dj.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[145]='listwise/liblistwise/op/dn/dn.devel.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/dn/dn.c -o listwise/liblistwise/op/dn/dn.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[146]='listwise/liblistwise/op/f/f.devel.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/f/f.c -o listwise/liblistwise/op/f/f.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[147]='listwise/liblistwise/op/fn/fn.devel.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/fn/fn.c -o listwise/liblistwise/op/fn/fn.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[148]='listwise/liblistwise/op/g/g.devel.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/g/g.c -o listwise/liblistwise/op/g/g.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[149]='listwise/liblistwise/op/hl/hl.devel.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/hl/hl.c -o listwise/liblistwise/op/hl/hl.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[150]='listwise/liblistwise/op/j/j.devel.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/j/j.c -o listwise/liblistwise/op/j/j.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[151]='listwise/liblistwise/op/l/l.devel.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/l/l.c -o listwise/liblistwise/op/l/l.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[152]='listwise/liblistwise/op/ls/ls.devel.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/ls/ls.c -o listwise/liblistwise/op/ls/ls.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[153]='listwise/liblistwise/op/lx/lx.devel.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/lx/lx.c -o listwise/liblistwise/op/lx/lx.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[154]='listwise/liblistwise/op/o/o.devel.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/o/o.c -o listwise/liblistwise/op/o/o.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[155]='listwise/liblistwise/op/p/p.devel.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/p/p.c -o listwise/liblistwise/op/p/p.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[156]='listwise/liblistwise/op/pop/pop.devel.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/pop/pop.c -o listwise/liblistwise/op/pop/pop.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[157]='listwise/liblistwise/op/r/r.devel.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/r/r.c -o listwise/liblistwise/op/r/r.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[158]='listwise/liblistwise/op/rb/rb.devel.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/rb/rb.c -o listwise/liblistwise/op/rb/rb.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[159]='listwise/liblistwise/op/re/re.devel.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/re/re.c -o listwise/liblistwise/op/re/re.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[160]='listwise/liblistwise/op/rp/rp.devel.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/rp/rp.c -o listwise/liblistwise/op/rp/rp.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[161]='listwise/liblistwise/op/rx/rx.devel.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/rx/rx.c -o listwise/liblistwise/op/rx/rx.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[162]='listwise/liblistwise/op/s/s.devel.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/s/s.c -o listwise/liblistwise/op/s/s.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[163]='listwise/liblistwise/op/shift/shift.devel.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/shift/shift.c -o listwise/liblistwise/op/shift/shift.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[164]='listwise/liblistwise/op/sort/sort.devel.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/sort/sort.c -o listwise/liblistwise/op/sort/sort.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[165]='listwise/liblistwise/op/stat/stat.devel.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/stat/stat.c -o listwise/liblistwise/op/stat/stat.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[166]='listwise/liblistwise/op/sx/sx.devel.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/sx/sx.c -o listwise/liblistwise/op/sx/sx.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[167]='listwise/liblistwise/op/t/t.devel.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/t/t.c -o listwise/liblistwise/op/t/t.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[168]='listwise/liblistwise/op/u/u.devel.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/u/u.c -o listwise/liblistwise/op/u/u.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[169]='listwise/liblistwise/op/up/up.devel.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/up/up.c -o listwise/liblistwise/op/up/up.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[170]='listwise/liblistwise/op/uu/uu.devel.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/uu/uu.c -o listwise/liblistwise/op/uu/uu.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[171]='listwise/liblistwise/op/v/v.devel.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/v/v.c -o listwise/liblistwise/op/v/v.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[172]='listwise/liblistwise/op/vf/vf.devel.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/vf/vf.c -o listwise/liblistwise/op/vf/vf.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[173]='listwise/liblistwise/op/vp/vp.devel.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/vp/vp.c -o listwise/liblistwise/op/vp/vp.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[174]='listwise/liblistwise/op/w/w.devel.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/w/w.c -o listwise/liblistwise/op/w/w.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[175]='listwise/liblistwise/op/wvf/wvf.devel.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/wvf/wvf.c -o listwise/liblistwise/op/wvf/wvf.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[176]='listwise/liblistwise/op/wvp/wvp.devel.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/wvp/wvp.c -o listwise/liblistwise/op/wvp/wvp.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[177]='listwise/liblistwise/op/x/x.devel.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/x/x.c -o listwise/liblistwise/op/x/x.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[178]='listwise/liblistwise/op/xch/xch.devel.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/xch/xch.c -o listwise/liblistwise/op/xch/xch.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[179]='listwise/liblistwise/op/y/y.devel.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/y/y.c -o listwise/liblistwise/op/y/y.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[180]='listwise/liblistwise/op/z/z.devel.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -c listwise/liblistwise/op/z/z.c -o listwise/liblistwise/op/z/z.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[181]='listwise/listwise/listwise.devel'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -o listwise/listwise/listwise.devel listwise/listwise/error/LISTWISE.errtab.devel.o common/wstdlib/wstdlib.devel.o common/strutil/strutil.devel.o common/logger/logger.devel.o listwise/listwise/logs/logs.devel.o listwise/listwise/args/args.devel.o listwise/listwise/main.devel.o ./libxapi/libxapi.devel.so ./libxlinux/libxlinux.devel.so ./libpstring/libpstring.devel.so ./listwise/liblistwise/liblistwise.devel.so '-Wl,-rpath,$ORIGIN/../.././libxapi' '-Wl,-rpath,$ORIGIN/../.././libxlinux' '-Wl,-rpath,$ORIGIN/../.././xunit/libxunit' '-Wl,-rpath,$ORIGIN/../.././libpstring' '-Wl,-rpath,$ORIGIN/../.././listwise/liblistwise'


  X=$?
  echo 10 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[182]='fab/fablw/op/fg/fg.devel.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[183]='fab/fablw/op/fi/fi.devel.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[184]='listwise/liblistwise/op/-X/-X.devel.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/-X/-X.devel.so listwise/liblistwise/op/-X/-X.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[185]='listwise/liblistwise/op/C/C.devel.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/C/C.devel.so listwise/liblistwise/op/C/C.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[186]='listwise/liblistwise/op/b/b.devel.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/b/b.devel.so listwise/liblistwise/op/b/b.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[187]='listwise/liblistwise/op/bn/bn.devel.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/bn/bn.devel.so listwise/liblistwise/op/bn/bn.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[188]='listwise/liblistwise/op/c/c.devel.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/c/c.devel.so listwise/liblistwise/op/c/c.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[189]='listwise/liblistwise/op/can/can.devel.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/can/can.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o common/canon/canon.devel.pic.o listwise/liblistwise/op/can/can.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[190]='listwise/liblistwise/op/cons/cons.devel.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/cons/cons.devel.so common/parseint/parseint.devel.pic.o listwise/liblistwise/op/cons/cons.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[191]='listwise/liblistwise/op/cp/cp.devel.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/cp/cp.devel.so listwise/liblistwise/op/cp/cp.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[192]='listwise/liblistwise/op/d/d.devel.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/d/d.devel.so listwise/liblistwise/op/d/d.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[193]='listwise/liblistwise/op/datef/datef.devel.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/datef/datef.devel.so common/parseint/parseint.devel.pic.o listwise/liblistwise/op/datef/datef.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[194]='listwise/liblistwise/op/dj/dj.devel.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/dj/dj.devel.so listwise/liblistwise/op/dj/dj.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[195]='listwise/liblistwise/op/dn/dn.devel.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/dn/dn.devel.so listwise/liblistwise/op/dn/dn.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[196]='listwise/liblistwise/op/f/f.devel.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/f/f.devel.so listwise/liblistwise/op/f/f.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[197]='listwise/liblistwise/op/fn/fn.devel.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/fn/fn.devel.so listwise/liblistwise/op/fn/fn.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[198]='listwise/liblistwise/op/g/g.devel.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/g/g.devel.so listwise/liblistwise/op/g/g.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[199]='listwise/liblistwise/op/hl/hl.devel.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/hl/hl.devel.so common/color/color.devel.pic.o listwise/liblistwise/op/hl/hl.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[200]='listwise/liblistwise/op/j/j.devel.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/j/j.devel.so listwise/liblistwise/op/j/j.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[201]='listwise/liblistwise/op/l/l.devel.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/l/l.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o listwise/liblistwise/op/l/l.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[202]='listwise/liblistwise/op/ls/ls.devel.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/ls/ls.devel.so common/parseint/parseint.devel.pic.o listwise/liblistwise/op/ls/ls.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[203]='listwise/liblistwise/op/lx/lx.devel.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/lx/lx.devel.so listwise/liblistwise/op/lx/lx.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[204]='listwise/liblistwise/op/o/o.devel.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/o/o.devel.so listwise/liblistwise/op/o/o.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[205]='listwise/liblistwise/op/p/p.devel.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/p/p.devel.so listwise/liblistwise/op/p/p.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[206]='listwise/liblistwise/op/pop/pop.devel.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/pop/pop.devel.so listwise/liblistwise/op/pop/pop.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[207]='listwise/liblistwise/op/r/r.devel.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/r/r.devel.so listwise/liblistwise/op/r/r.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[208]='listwise/liblistwise/op/rb/rb.devel.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/rb/rb.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o common/canon/canon.devel.pic.o listwise/liblistwise/op/rb/rb.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[209]='listwise/liblistwise/op/re/re.devel.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/re/re.devel.so listwise/liblistwise/op/re/re.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[210]='listwise/liblistwise/op/rp/rp.devel.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/rp/rp.devel.so listwise/liblistwise/op/rp/rp.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[211]='listwise/liblistwise/op/rx/rx.devel.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/rx/rx.devel.so listwise/liblistwise/op/rx/rx.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[212]='listwise/liblistwise/op/s/s.devel.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/s/s.devel.so common/parseint/parseint.devel.pic.o listwise/liblistwise/op/s/s.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[213]='listwise/liblistwise/op/shift/shift.devel.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/shift/shift.devel.so listwise/liblistwise/op/shift/shift.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[214]='listwise/liblistwise/op/sort/sort.devel.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/sort/sort.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o listwise/liblistwise/op/sort/sort.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[215]='listwise/liblistwise/op/stat/stat.devel.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/stat/stat.devel.so listwise/liblistwise/op/stat/stat.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[216]='listwise/liblistwise/op/sx/sx.devel.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/sx/sx.devel.so listwise/liblistwise/op/sx/sx.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[217]='listwise/liblistwise/op/t/t.devel.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/t/t.devel.so listwise/liblistwise/op/t/t.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[218]='listwise/liblistwise/op/u/u.devel.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/u/u.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o listwise/liblistwise/op/u/u.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[219]='listwise/liblistwise/op/up/up.devel.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/up/up.devel.so listwise/liblistwise/op/up/up.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[220]='listwise/liblistwise/op/uu/uu.devel.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/uu/uu.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o listwise/liblistwise/op/uu/uu.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[221]='listwise/liblistwise/op/v/v.devel.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/v/v.devel.so listwise/liblistwise/op/v/v.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[222]='listwise/liblistwise/op/vf/vf.devel.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/vf/vf.devel.so listwise/liblistwise/op/vf/vf.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[223]='listwise/liblistwise/op/vp/vp.devel.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/vp/vp.devel.so listwise/liblistwise/op/vp/vp.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[224]='listwise/liblistwise/op/w/w.devel.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/w/w.devel.so listwise/liblistwise/op/w/w.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[225]='listwise/liblistwise/op/wvf/wvf.devel.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/wvf/wvf.devel.so listwise/liblistwise/op/wvf/wvf.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[226]='listwise/liblistwise/op/wvp/wvp.devel.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/wvp/wvp.devel.so listwise/liblistwise/op/wvp/wvp.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[227]='listwise/liblistwise/op/x/x.devel.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/x/x.devel.so listwise/liblistwise/op/x/x.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[228]='listwise/liblistwise/op/xch/xch.devel.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/xch/xch.devel.so listwise/liblistwise/op/xch/xch.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[229]='listwise/liblistwise/op/y/y.devel.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/y/y.devel.so listwise/liblistwise/op/y/y.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[230]='listwise/liblistwise/op/z/z.devel.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/test -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/home/todd/fab/listwise/liblistwise/op -DLWOPEXT=devel.so -o listwise/liblistwise/op/z/z.devel.so listwise/liblistwise/op/z/z.devel.pic.o ./listwise/liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[231]='fab/fablw/op/fg/fg.devel.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -o fab/fablw/op/fg/fg.devel.so	fab/fablw/op/fg/fg.devel.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[232]='fab/fablw/op/fi/fi.devel.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -o fab/fablw/op/fi/fi.devel.so	fab/fablw/op/fi/fi.devel.pic.o -shared


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
  ((SKP+=6))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[5]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 6 ]]; do
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
    I=$((25+$idx))
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
  # launch stage 2.1
  exec 100>$tmp ; rm -f $tmp ; fml_2_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_2_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_2_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_2_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_2_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_2_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_2_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_2_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_2_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_2_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_2_37 & PIDS[18]=$!

  # harvest stage 2.1
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
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=11))
else
  # launch stage 2.2
  exec 100>$tmp ; rm -f $tmp ; fml_2_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_43 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_44 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_45 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_46 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_2_47 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_2_48 & PIDS[10]=$!

  # harvest stage 2.2
  C=0
  while [[ $C != 11 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((63+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+38)) $X "$N"
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
    I=$((74+$idx))
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
    I=$((93+$idx))
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
  ((SKP+=17))
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

  # harvest stage 3.2
  C=0
  while [[ $C != 17 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((112+$idx))
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
    I=$((129+$idx))
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
    I=$((130+$idx))
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
    I=$((131+$idx))
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
  ((SKP+=1))
else
  # launch stage 7.0
  exec 100>$tmp ; rm -f $tmp ; fml_7_0 & PIDS[0]=$!

  # harvest stage 7.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((132+$idx))
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
  # launch stage 8.0
  exec 100>$tmp ; rm -f $tmp ; fml_8_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_8 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_8_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_8_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_8_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_8_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_8_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_8_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_8_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_8_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_8_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_8_18 & PIDS[18]=$!

  # harvest stage 8.0
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((133+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 8.1
  exec 100>$tmp ; rm -f $tmp ; fml_8_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_8_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_8_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_8_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_8_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_8_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_8_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_8_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_8_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_8_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_8_37 & PIDS[18]=$!

  # harvest stage 8.1
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((152+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=11))
else
  # launch stage 8.2
  exec 100>$tmp ; rm -f $tmp ; fml_8_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_43 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_44 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_45 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_46 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_8_47 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_8_48 & PIDS[10]=$!

  # harvest stage 8.2
  C=0
  while [[ $C != 11 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((171+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+38)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 9.0
  exec 100>$tmp ; rm -f $tmp ; fml_9_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_1 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_2 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_3 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_4 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_5 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_6 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_7 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_8 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_9_9 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_9_10 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_9_11 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_9_12 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_9_13 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_9_14 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_9_15 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_9_16 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_9_17 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_9_18 & PIDS[18]=$!

  # harvest stage 9.0
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
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=19))
else
  # launch stage 9.1
  exec 100>$tmp ; rm -f $tmp ; fml_9_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_23 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_24 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_25 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_26 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_27 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_9_28 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_9_29 & PIDS[10]=$!
  exec 111>$tmp ; rm -f $tmp ; fml_9_30 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_9_31 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_9_32 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_9_33 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_9_34 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_9_35 & PIDS[16]=$!
  exec 117>$tmp ; rm -f $tmp ; fml_9_36 & PIDS[17]=$!
  exec 118>$tmp ; rm -f $tmp ; fml_9_37 & PIDS[18]=$!

  # harvest stage 9.1
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((201+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+19)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=11))
else
  # launch stage 9.2
  exec 100>$tmp ; rm -f $tmp ; fml_9_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_42 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_43 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_44 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_45 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_46 & PIDS[8]=$!
  exec 109>$tmp ; rm -f $tmp ; fml_9_47 & PIDS[9]=$!
  exec 110>$tmp ; rm -f $tmp ; fml_9_48 & PIDS[10]=$!

  # harvest stage 9.2
  C=0
  while [[ $C != 11 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((220+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+38)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 10.0
  exec 100>$tmp ; rm -f $tmp ; fml_10_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_10_1 & PIDS[1]=$!

  # harvest stage 10.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((231+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 10 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

printf '%15s %d\n' succeeded $WIN
printf '%15s %d\n' failed $DIE
printf '%15s %d\n' skipped $SKP
# no failures=0, and 1 otherwise
exit $((!(DIE==0)))
