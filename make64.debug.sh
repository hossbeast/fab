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
NAMES[0]='common/cksum/cksum.debug.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/color/color.debug.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/parseint/parseint.debug.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint/parseint.debug.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/strutil/strutil.debug.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/strutil/strutil.debug.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring/unitstring.debug.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.debug.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/wstdlib/wstdlib.debug.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/wstdlib/wstdlib.debug.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.debug.pic.o


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

NAMES[18]='libxapi/test.debug.pic.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/test.c -o libxapi/test.debug.pic.o


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
NAMES[25]='common/canon/canon.debug.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[26]='common/canon/canon.debug.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[27]='common/coll/coll.debug.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[28]='common/coll/coll.debug.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[29]='common/logger/logger.debug.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[30]='common/map/map.debug.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/map/map.c -o common/map/map.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[31]='common/map/map.debug.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.debug.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[33]='common/yyutil/yyutil.debug.pic.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[34]='fab/depblock/depblock.debug.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/depblock/depblock.c -o fab/depblock/depblock.debug.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[35]='fab/dirutil/dirutil.debug.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.debug.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[36]='fab/error/FAB.errtab.debug.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/error/FAB.errtab.c -o fab/error/FAB.errtab.debug.o


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

NAMES[39]='fab/gn/gnlw.debug.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/gn/gnlw.c -o fab/gn/gnlw.debug.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[40]='fab/hashblock/hashblock.debug.o'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.debug.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[41]='fab/identity/identity.debug.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/identity/identity.c -o fab/identity/identity.debug.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[42]='fab/logs/logs.debug.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/logs/logs.c -o fab/logs/logs.debug.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[43]='fab/lwutil/lwutil.debug.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.debug.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[44]='fab/params/params.debug.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/params/params.c -o fab/params/params.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[45]='fab/path/path.debug.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/path/path.c -o fab/path/path.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[46]='fab/strstack/strstack.debug.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/strstack/strstack.c -o fab/strstack/strstack.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[47]='fab/traverse/traverse.debug.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/traverse/traverse.c -o fab/traverse/traverse.debug.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[48]='libpstring/pstring/pstring.debug.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.debug.pic.o


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

NAMES[50]='libxapi/error/XAPI.errtab.debug.pic.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[51]='libxlinux/error/XLINUX.errtab.debug.pic.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[52]='libxlinux/error/error.debug.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/error/error.c -o libxlinux/error/error.debug.pic.o


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

NAMES[54]='libxlinux/xdirent/xdirent.debug.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[55]='libxlinux/xdlfcn/xdlfcn.debug.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[56]='libxlinux/xfcntl/xfcntl.debug.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[57]='libxlinux/xftw/xftw.debug.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[58]='libxlinux/xgrp/xgrp.debug.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[59]='libxlinux/xmman/xmman.debug.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[60]='libxlinux/xpwd/xpwd.debug.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[61]='libxlinux/xstat/xstat.debug.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[62]='libxlinux/xstdlib/xstdlib.debug.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[63]='libxlinux/xstring/xstring.debug.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[64]='libxlinux/xtime/xtime.debug.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[65]='libxlinux/xunistd/xunistd.debug.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[66]='listwise/liblistwise/error/LW.errtab.debug.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/LW.errtab.c -o listwise/liblistwise/error/LW.errtab.debug.pic.o


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

NAMES[70]='listwise/listwise/args/args.debug.o'
fml_2_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/args/args.c -o listwise/listwise/args/args.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[71]='listwise/listwise/error/LISTWISE.errtab.debug.o'
fml_2_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/error/LISTWISE.errtab.c -o listwise/listwise/error/LISTWISE.errtab.debug.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[72]='listwise/listwise/logs/logs.debug.o'
fml_2_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/logs/logs.c -o listwise/listwise/logs/logs.debug.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[73]='listwise/listwise/main.debug.o'
fml_2_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/main.c -o listwise/listwise/main.debug.o


  X=$?
  echo 10 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[74]='fab/args/args.debug.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/args/args.c -o fab/args/args.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[75]='fab/bake/bake.debug.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/bake/bake.c -o fab/bake/bake.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[76]='fab/bp/bp.debug.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/bp/bp.c -o fab/bp/bp.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[77]='fab/dep/dep.debug.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/dep/dep.c -o fab/dep/dep.debug.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[78]='fab/dsc/dsc.debug.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/dsc/dsc.c -o fab/dsc/dsc.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[79]='fab/enclose/enclose.debug.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/enclose/enclose.c -o fab/enclose/enclose.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[80]='fab/error/error.debug.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/error/error.c -o fab/error/error.debug.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[81]='fab/ff/ff.debug.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.c -o fab/ff/ff.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[82]='fab/ff/ff.dsc.tab.debug.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.debug.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[83]='fab/ff/ff.lex.debug.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.lex.c -o fab/ff/ff.lex.debug.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[84]='fab/ff/ff.list.tab.debug.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.debug.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[85]='fab/ff/ff.states.debug.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.states.c -o fab/ff/ff.states.debug.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[86]='fab/ff/ff.tab.debug.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.tab.c -o fab/ff/ff.tab.debug.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[87]='fab/ff/ff.tokens.debug.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.debug.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[88]='fab/ff/ff.var.tab.debug.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.debug.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[89]='fab/ffn/ffn.debug.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ffn/ffn.c -o fab/ffn/ffn.debug.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[90]='fab/ffproc/ffproc.debug.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.debug.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[91]='fab/fml/fml.debug.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/fml/fml.c -o fab/fml/fml.debug.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[92]='fab/gn/gn.debug.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/gn/gn.c -o fab/gn/gn.debug.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[93]='fab/list/list.debug.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/list/list.c -o fab/list/list.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[94]='fab/main.debug.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/main.c -o fab/main.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[95]='fab/selector/selector.debug.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/selector/selector.c -o fab/selector/selector.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[96]='fab/tmp/tmp.debug.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/tmp/tmp.c -o fab/tmp/tmp.debug.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[97]='fab/ts/ts.debug.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/ts/ts.c -o fab/ts/ts.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[98]='fab/var/var.debug.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -c fab/var/var.c -o fab/var/var.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[99]='libxapi/callstack/callstack.debug.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[100]='libxapi/error/SYS.errtab.debug.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[101]='libxapi/error/error.debug.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/error/error.c -o libxapi/error/error.debug.pic.o


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

NAMES[103]='libxapi/frame/frame.debug.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/frame/frame.c -o libxapi/frame/frame.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[104]='libxapi/trace/trace.debug.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c libxapi/trace/trace.c -o libxapi/trace/trace.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[105]='listwise/liblistwise/describe/describe.debug.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/describe/describe.c -o listwise/liblistwise/describe/describe.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[106]='listwise/liblistwise/error/PCRE.errtab.debug.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/PCRE.errtab.c -o listwise/liblistwise/error/PCRE.errtab.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[107]='listwise/liblistwise/error/error.debug.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/error.c -o listwise/liblistwise/error/error.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[108]='listwise/liblistwise/exec/exec.debug.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/exec/exec.c -o listwise/liblistwise/exec/exec.debug.pic.o


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
	  cat -- listwise/liblistwise/exec/exec.c listwise/liblistwise/logging/logging.c listwise/liblistwise/error/LW.errtab.c listwise/liblistwise/error/PCRE.errtab.c listwise/liblistwise/error/error.c listwise/liblistwise/iterate/iterate.c listwise/liblistwise/object/object.c listwise/liblistwise/op/wz/wz.c listwise/liblistwise/op/z/z.c listwise/liblistwise/op/o/o.c listwise/liblistwise/op/vf/vf.c listwise/liblistwise/op/wvp/wvp.c listwise/liblistwise/op/sort/sort.c listwise/liblistwise/op/sort/test.c listwise/liblistwise/op/w/w.c listwise/liblistwise/op/shift/shift.c listwise/liblistwise/op/dj/dj.c listwise/liblistwise/op/uu/uu.c listwise/liblistwise/op/u/u.c listwise/liblistwise/op/bn/bn.c listwise/liblistwise/op/re/re.c listwise/liblistwise/op/rp/rp.c listwise/liblistwise/op/f/f.c listwise/liblistwise/op/ls/ls.c listwise/liblistwise/op/lx/lx.c listwise/liblistwise/op/hl/hl.c listwise/liblistwise/op/rb/rb.c listwise/liblistwise/op/j/j.c listwise/liblistwise/op/wv/wv.c listwise/liblistwise/op/-X/-X.c listwise/liblistwise/op/up/up.c listwise/liblistwise/op/cp/cp.c listwise/liblistwise/op/p/p.c listwise/liblistwise/op/c/c.c listwise/liblistwise/op/r/r.c listwise/liblistwise/op/xch/xch.c listwise/liblistwise/op/t/t.c listwise/liblistwise/op/s/s.c listwise/liblistwise/op/cons/cons.c listwise/liblistwise/op/wvf/wvf.c listwise/liblistwise/op/dn/dn.c listwise/liblistwise/op/stat/stat.c listwise/liblistwise/op/g/g.c listwise/liblistwise/op/d/d.c listwise/liblistwise/op/vp/vp.c listwise/liblistwise/op/can/can.c listwise/liblistwise/op/datef/datef.c listwise/liblistwise/op/x/x.c listwise/liblistwise/op/b/b.c listwise/liblistwise/op/C/C.c listwise/liblistwise/op/rx/rx.c listwise/liblistwise/op/y/y.c listwise/liblistwise/op/fn/fn.c listwise/liblistwise/op/pop/pop.c listwise/liblistwise/op/sx/sx.c listwise/liblistwise/op/l/l.c listwise/liblistwise/op/l/test.c listwise/liblistwise/op/v/v.c listwise/liblistwise/describe/describe.c listwise/liblistwise/lwx/lwx.c listwise/liblistwise/re/re.c listwise/liblistwise/lstack/lstack.c listwise/liblistwise/fs/fs.c listwise/liblistwise/selection/selection.c listwise/liblistwise/operators/operators.c listwise/liblistwise/operator/operator.c listwise/liblistwise/sanity/sanity.c listwise/liblistwise/listwise_test/listwise_test.c listwise/liblistwise/generator/generator.lex.c listwise/liblistwise/generator/generator.c listwise/liblistwise/generator/generator.states.c listwise/liblistwise/generator/generator.tab.c listwise/liblistwise/generator/generator.tokens.c listwise/liblistwise/generator/genscan.c listwise/liblistwise/window/window.c listwise/liblistwise/tune/tune.c )
echo "local: *; };"
	 ) 1>listwise/liblistwise/exports


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[110]='listwise/liblistwise/fs/fs.debug.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/fs/fs.c -o listwise/liblistwise/fs/fs.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[111]='listwise/liblistwise/generator/generator.debug.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/generator.c -o listwise/liblistwise/generator/generator.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[112]='listwise/liblistwise/generator/generator.lex.debug.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/generator.lex.c -o listwise/liblistwise/generator/generator.lex.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[113]='listwise/liblistwise/generator/generator.states.debug.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/generator.states.c -o listwise/liblistwise/generator/generator.states.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[114]='listwise/liblistwise/generator/generator.tab.debug.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/generator.tab.c -o listwise/liblistwise/generator/generator.tab.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[115]='listwise/liblistwise/generator/generator.tokens.debug.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/generator.tokens.c -o listwise/liblistwise/generator/generator.tokens.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[116]='listwise/liblistwise/generator/genscan.debug.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/generator/genscan.c -o listwise/liblistwise/generator/genscan.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[117]='listwise/liblistwise/iterate/iterate.debug.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/iterate/iterate.c -o listwise/liblistwise/iterate/iterate.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[118]='listwise/liblistwise/logging/logging.debug.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/logging/logging.c -o listwise/liblistwise/logging/logging.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[119]='listwise/liblistwise/lstack/lstack.debug.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lstack/lstack.c -o listwise/liblistwise/lstack/lstack.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[120]='listwise/liblistwise/lwx/lwx.debug.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lwx/lwx.c -o listwise/liblistwise/lwx/lwx.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[121]='listwise/liblistwise/object/object.debug.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/object/object.c -o listwise/liblistwise/object/object.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[122]='listwise/liblistwise/operator/operator.debug.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operator/operator.c -o listwise/liblistwise/operator/operator.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[123]='listwise/liblistwise/operators/operators.debug.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operators/operators.c -o listwise/liblistwise/operators/operators.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[124]='listwise/liblistwise/re/re.debug.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/re/re.c -o listwise/liblistwise/re/re.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[125]='listwise/liblistwise/sanity/sanity.debug.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/sanity/sanity.c -o listwise/liblistwise/sanity/sanity.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[126]='listwise/liblistwise/selection/selection.debug.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/selection/selection.c -o listwise/liblistwise/selection/selection.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[127]='listwise/liblistwise/tune/tune.debug.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/tune/tune.c -o listwise/liblistwise/tune/tune.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[128]='listwise/liblistwise/window/window.debug.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/window/window.c -o listwise/liblistwise/window/window.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[129]='libxapi/libxapi.debug.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -o libxapi/libxapi.debug.so common/strutil/strutil.debug.pic.o common/wstdlib/wstdlib.debug.pic.o libxapi/callstack/callstack.debug.pic.o libxapi/error/SYS.errtab.debug.pic.o libxapi/error/XAPI.errtab.debug.pic.o libxapi/error/error.debug.pic.o libxapi/frame/frame.debug.pic.o libxapi/test.debug.pic.o libxapi/trace/trace.debug.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[130]='libxlinux/libxlinux.debug.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./listwise/liblistwise -I./libpstring -o libxlinux/libxlinux.debug.so libxlinux/error/XLINUX.errtab.debug.pic.o libxlinux/error/error.debug.pic.o libxlinux/xdirent/xdirent.debug.pic.o libxlinux/xdlfcn/xdlfcn.debug.pic.o libxlinux/xfcntl/xfcntl.debug.pic.o libxlinux/xftw/xftw.debug.pic.o libxlinux/xgrp/xgrp.debug.pic.o libxlinux/xmman/xmman.debug.pic.o libxlinux/xpwd/xpwd.debug.pic.o libxlinux/xstat/xstat.debug.pic.o libxlinux/xstdlib/xstdlib.debug.pic.o libxlinux/xstring/xstring.debug.pic.o libxlinux/xtime/xtime.debug.pic.o libxlinux/xunistd/xunistd.debug.pic.o ./libxapi/libxapi.debug.so -ldl -shared -Wl,--version-script=libxlinux/exports -Wl,-soname,libxlinux.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[131]='libpstring/libpstring.debug.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring.debug.so libpstring/pstring/pstring.debug.pic.o ./libxapi/libxapi.debug.so -shared -Wl,--version-script=libpstring/exports ./libxlinux/libxlinux.debug.so -Wl,-soname,libpstring.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[132]='listwise/liblistwise/liblistwise.debug.so'
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
-DDEBUG \
-DXAPI_RUNTIME_CHECKS \
-DSANITY \
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
-I./listwise/liblistwise/listwise_test \
-I./listwise/liblistwise/generator \
-I./listwise/liblistwise/window \
-I./listwise/liblistwise/tune \
-I./listwise/liblistwise \
-DLWOPDIR=/usr/lib/listwise \
-DLWOPEXT=so \
-o \
listwise/liblistwise/liblistwise.debug.so \
common/coll/coll.debug.pic.o \
common/map/map.debug.pic.o \
listwise/liblistwise/error/LW.errtab.debug.pic.o \
listwise/liblistwise/re/re.debug.pic.o \
listwise/liblistwise/object/object.debug.pic.o \
common/wstdlib/wstdlib.debug.pic.o \
listwise/liblistwise/lwx/lwx.debug.pic.o \
listwise/liblistwise/lstack/lstack.debug.pic.o \
listwise/liblistwise/logging/logging.debug.pic.o \
listwise/liblistwise/iterate/iterate.debug.pic.o \
listwise/liblistwise/generator/generator.states.debug.pic.o \
listwise/liblistwise/generator/genscan.debug.pic.o \
listwise/liblistwise/describe/describe.debug.pic.o \
listwise/liblistwise/sanity/sanity.debug.pic.o \
listwise/liblistwise/exec/exec.debug.pic.o \
listwise/liblistwise/error/PCRE.errtab.debug.pic.o \
listwise/liblistwise/error/error.debug.pic.o \
common/strutil/strutil.debug.pic.o \
common/canon/canon.debug.pic.o \
common/parseint/parseint.debug.pic.o \
common/color/color.debug.pic.o \
listwise/liblistwise/fs/fs.debug.pic.o \
listwise/liblistwise/selection/selection.debug.pic.o \
listwise/liblistwise/operators/operators.debug.pic.o \
listwise/liblistwise/operator/operator.debug.pic.o \
common/yyutil/yyutil.debug.pic.o \
listwise/liblistwise/generator/generator.tab.debug.pic.o \
listwise/liblistwise/generator/generator.tokens.debug.pic.o \
listwise/liblistwise/generator/generator.lex.debug.pic.o \
listwise/liblistwise/generator/generator.debug.pic.o \
listwise/liblistwise/window/window.debug.pic.o \
listwise/liblistwise/tune/tune.debug.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=listwise/liblistwise/exports \
./libxapi/libxapi.debug.so \
./libxlinux/libxlinux.debug.so \
./libpstring/libpstring.debug.so \
-Wl,-soname,liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[133]='fab/fab.debug'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -export-dynamic -o fab/fab.debug common/wstdlib/wstdlib.debug.o common/strutil/strutil.debug.o common/logger/logger.debug.o fab/logs/logs.debug.o fab/error/FAB.errtab.debug.o common/unitstring/unitstring.debug.o common/canon/canon.debug.o common/coll/coll.debug.o common/cksum/cksum.debug.o fab/path/path.debug.o fab/params/params.debug.o fab/identity/identity.debug.o fab/dirutil/dirutil.debug.o fab/hashblock/hashblock.debug.o common/map/map.debug.o common/yyutil/yyutil.debug.o fab/ff/ff.tokens.debug.o fab/ffn/ffn.debug.o fab/ff/ff.tab.debug.o fab/ff/ff.dsc.tab.debug.o fab/ff/ff.var.tab.debug.o fab/ff/ff.list.tab.debug.o fab/ff/ff.lex.debug.o fab/ff/ff.states.debug.o fab/depblock/depblock.debug.o fab/strstack/strstack.debug.o fab/ts/ts.debug.o fab/traverse/traverse.debug.o fab/gn/gnlw.debug.o fab/lwutil/lwutil.debug.o fab/var/var.debug.o fab/list/list.debug.o fab/enclose/enclose.debug.o fab/fml/fml.debug.o common/parseint/parseint.debug.o fab/gn/gn.debug.o fab/ff/ff.debug.o fab/selector/selector.debug.o fab/args/args.debug.o fab/bp/bp.debug.o fab/dep/dep.debug.o fab/dsc/dsc.debug.o fab/tmp/tmp.debug.o fab/bake/bake.debug.o fab/ffproc/ffproc.debug.o fab/error/error.debug.o fab/main.debug.o ./libxapi/libxapi.debug.so ./libxlinux/libxlinux.debug.so ./libpstring/libpstring.debug.so ./listwise/liblistwise/liblistwise.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[134]='listwise/liblistwise/op/-X/-X.debug.pic.o'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/-X/-X.c -o listwise/liblistwise/op/-X/-X.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[135]='listwise/liblistwise/op/C/C.debug.pic.o'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/C/C.c -o listwise/liblistwise/op/C/C.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[136]='listwise/liblistwise/op/b/b.debug.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/b/b.c -o listwise/liblistwise/op/b/b.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[137]='listwise/liblistwise/op/bn/bn.debug.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/bn/bn.c -o listwise/liblistwise/op/bn/bn.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[138]='listwise/liblistwise/op/c/c.debug.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/c/c.c -o listwise/liblistwise/op/c/c.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[139]='listwise/liblistwise/op/can/can.debug.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/can/can.c -o listwise/liblistwise/op/can/can.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[140]='listwise/liblistwise/op/cons/cons.debug.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cons/cons.c -o listwise/liblistwise/op/cons/cons.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[141]='listwise/liblistwise/op/cp/cp.debug.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cp/cp.c -o listwise/liblistwise/op/cp/cp.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[142]='listwise/liblistwise/op/d/d.debug.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/d/d.c -o listwise/liblistwise/op/d/d.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[143]='listwise/liblistwise/op/datef/datef.debug.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/datef/datef.c -o listwise/liblistwise/op/datef/datef.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[144]='listwise/liblistwise/op/dj/dj.debug.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dj/dj.c -o listwise/liblistwise/op/dj/dj.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[145]='listwise/liblistwise/op/dn/dn.debug.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dn/dn.c -o listwise/liblistwise/op/dn/dn.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[146]='listwise/liblistwise/op/f/f.debug.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/f/f.c -o listwise/liblistwise/op/f/f.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[147]='listwise/liblistwise/op/fn/fn.debug.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/fn/fn.c -o listwise/liblistwise/op/fn/fn.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[148]='listwise/liblistwise/op/g/g.debug.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/g/g.c -o listwise/liblistwise/op/g/g.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[149]='listwise/liblistwise/op/hl/hl.debug.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/hl/hl.c -o listwise/liblistwise/op/hl/hl.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[150]='listwise/liblistwise/op/j/j.debug.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/j/j.c -o listwise/liblistwise/op/j/j.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[151]='listwise/liblistwise/op/l/l.debug.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/l/l.c -o listwise/liblistwise/op/l/l.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[152]='listwise/liblistwise/op/ls/ls.debug.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/ls/ls.c -o listwise/liblistwise/op/ls/ls.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[153]='listwise/liblistwise/op/lx/lx.debug.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/lx/lx.c -o listwise/liblistwise/op/lx/lx.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[154]='listwise/liblistwise/op/o/o.debug.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/o/o.c -o listwise/liblistwise/op/o/o.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[155]='listwise/liblistwise/op/p/p.debug.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/p/p.c -o listwise/liblistwise/op/p/p.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[156]='listwise/liblistwise/op/pop/pop.debug.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/pop/pop.c -o listwise/liblistwise/op/pop/pop.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[157]='listwise/liblistwise/op/r/r.debug.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/r/r.c -o listwise/liblistwise/op/r/r.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[158]='listwise/liblistwise/op/rb/rb.debug.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rb/rb.c -o listwise/liblistwise/op/rb/rb.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[159]='listwise/liblistwise/op/re/re.debug.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/re/re.c -o listwise/liblistwise/op/re/re.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[160]='listwise/liblistwise/op/rp/rp.debug.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rp/rp.c -o listwise/liblistwise/op/rp/rp.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[161]='listwise/liblistwise/op/rx/rx.debug.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rx/rx.c -o listwise/liblistwise/op/rx/rx.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[162]='listwise/liblistwise/op/s/s.debug.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/s/s.c -o listwise/liblistwise/op/s/s.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[163]='listwise/liblistwise/op/shift/shift.debug.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/shift/shift.c -o listwise/liblistwise/op/shift/shift.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[164]='listwise/liblistwise/op/sort/sort.debug.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sort/sort.c -o listwise/liblistwise/op/sort/sort.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[165]='listwise/liblistwise/op/stat/stat.debug.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/stat/stat.c -o listwise/liblistwise/op/stat/stat.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[166]='listwise/liblistwise/op/sx/sx.debug.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sx/sx.c -o listwise/liblistwise/op/sx/sx.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[167]='listwise/liblistwise/op/t/t.debug.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/t/t.c -o listwise/liblistwise/op/t/t.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[168]='listwise/liblistwise/op/u/u.debug.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/u/u.c -o listwise/liblistwise/op/u/u.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[169]='listwise/liblistwise/op/up/up.debug.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/up/up.c -o listwise/liblistwise/op/up/up.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[170]='listwise/liblistwise/op/uu/uu.debug.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/uu/uu.c -o listwise/liblistwise/op/uu/uu.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[171]='listwise/liblistwise/op/v/v.debug.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/v/v.c -o listwise/liblistwise/op/v/v.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[172]='listwise/liblistwise/op/vf/vf.debug.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vf/vf.c -o listwise/liblistwise/op/vf/vf.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[173]='listwise/liblistwise/op/vp/vp.debug.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vp/vp.c -o listwise/liblistwise/op/vp/vp.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[174]='listwise/liblistwise/op/w/w.debug.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/w/w.c -o listwise/liblistwise/op/w/w.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[175]='listwise/liblistwise/op/wv/wv.debug.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wv/wv.c -o listwise/liblistwise/op/wv/wv.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[176]='listwise/liblistwise/op/wvf/wvf.debug.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvf/wvf.c -o listwise/liblistwise/op/wvf/wvf.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[177]='listwise/liblistwise/op/wvp/wvp.debug.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvp/wvp.c -o listwise/liblistwise/op/wvp/wvp.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[178]='listwise/liblistwise/op/wz/wz.debug.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wz/wz.c -o listwise/liblistwise/op/wz/wz.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[179]='listwise/liblistwise/op/x/x.debug.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/x/x.c -o listwise/liblistwise/op/x/x.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[180]='listwise/liblistwise/op/xch/xch.debug.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/xch/xch.c -o listwise/liblistwise/op/xch/xch.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[181]='listwise/liblistwise/op/y/y.debug.pic.o'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/y/y.c -o listwise/liblistwise/op/y/y.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[182]='listwise/liblistwise/op/z/z.debug.pic.o'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/z/z.c -o listwise/liblistwise/op/z/z.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[183]='listwise/listwise/listwise.debug'
fml_8_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/listwise/error -I./listwise/listwise/logs -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -o listwise/listwise/listwise.debug listwise/listwise/error/LISTWISE.errtab.debug.o common/wstdlib/wstdlib.debug.o common/strutil/strutil.debug.o common/logger/logger.debug.o listwise/listwise/logs/logs.debug.o listwise/listwise/args/args.debug.o listwise/listwise/main.debug.o ./libxapi/libxapi.debug.so ./libxlinux/libxlinux.debug.so ./libpstring/libpstring.debug.so ./listwise/liblistwise/liblistwise.debug.so


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[184]='fab/fablw/op/fg/fg.debug.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[185]='fab/fablw/op/fi/fi.debug.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[186]='listwise/liblistwise/op/-X/-X.debug.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/-X/-X.debug.so listwise/liblistwise/op/-X/-X.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[187]='listwise/liblistwise/op/C/C.debug.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/C/C.debug.so listwise/liblistwise/op/C/C.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[188]='listwise/liblistwise/op/b/b.debug.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/b/b.debug.so listwise/liblistwise/op/b/b.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[189]='listwise/liblistwise/op/bn/bn.debug.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/bn/bn.debug.so listwise/liblistwise/op/bn/bn.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[190]='listwise/liblistwise/op/c/c.debug.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/c/c.debug.so listwise/liblistwise/op/c/c.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[191]='listwise/liblistwise/op/can/can.debug.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/can/can.debug.so common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o common/canon/canon.debug.pic.o listwise/liblistwise/op/can/can.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[192]='listwise/liblistwise/op/cons/cons.debug.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cons/cons.debug.so common/parseint/parseint.debug.pic.o listwise/liblistwise/op/cons/cons.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[193]='listwise/liblistwise/op/cp/cp.debug.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cp/cp.debug.so listwise/liblistwise/op/cp/cp.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[194]='listwise/liblistwise/op/d/d.debug.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/d/d.debug.so listwise/liblistwise/op/d/d.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[195]='listwise/liblistwise/op/datef/datef.debug.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/datef/datef.debug.so common/parseint/parseint.debug.pic.o listwise/liblistwise/op/datef/datef.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[196]='listwise/liblistwise/op/dj/dj.debug.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dj/dj.debug.so listwise/liblistwise/op/dj/dj.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[197]='listwise/liblistwise/op/dn/dn.debug.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dn/dn.debug.so listwise/liblistwise/op/dn/dn.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[198]='listwise/liblistwise/op/f/f.debug.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/f/f.debug.so listwise/liblistwise/op/f/f.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[199]='listwise/liblistwise/op/fn/fn.debug.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/fn/fn.debug.so listwise/liblistwise/op/fn/fn.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[200]='listwise/liblistwise/op/g/g.debug.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/g/g.debug.so listwise/liblistwise/op/g/g.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[201]='listwise/liblistwise/op/hl/hl.debug.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/hl/hl.debug.so common/color/color.debug.pic.o listwise/liblistwise/op/hl/hl.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[202]='listwise/liblistwise/op/j/j.debug.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/j/j.debug.so listwise/liblistwise/op/j/j.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[203]='listwise/liblistwise/op/l/l.debug.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/l/l.debug.so common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o listwise/liblistwise/op/l/l.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[204]='listwise/liblistwise/op/ls/ls.debug.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/ls/ls.debug.so common/parseint/parseint.debug.pic.o listwise/liblistwise/op/ls/ls.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[205]='listwise/liblistwise/op/lx/lx.debug.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/lx/lx.debug.so listwise/liblistwise/op/lx/lx.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[206]='listwise/liblistwise/op/o/o.debug.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/o/o.debug.so listwise/liblistwise/op/o/o.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[207]='listwise/liblistwise/op/p/p.debug.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/p/p.debug.so listwise/liblistwise/op/p/p.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[208]='listwise/liblistwise/op/pop/pop.debug.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/pop/pop.debug.so listwise/liblistwise/op/pop/pop.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[209]='listwise/liblistwise/op/r/r.debug.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/r/r.debug.so listwise/liblistwise/op/r/r.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[210]='listwise/liblistwise/op/rb/rb.debug.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rb/rb.debug.so common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o common/canon/canon.debug.pic.o listwise/liblistwise/op/rb/rb.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[211]='listwise/liblistwise/op/re/re.debug.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/re/re.debug.so listwise/liblistwise/op/re/re.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[212]='listwise/liblistwise/op/rp/rp.debug.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rp/rp.debug.so listwise/liblistwise/op/rp/rp.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[213]='listwise/liblistwise/op/rx/rx.debug.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rx/rx.debug.so listwise/liblistwise/op/rx/rx.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[214]='listwise/liblistwise/op/s/s.debug.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/s/s.debug.so common/parseint/parseint.debug.pic.o listwise/liblistwise/op/s/s.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[215]='listwise/liblistwise/op/shift/shift.debug.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/shift/shift.debug.so listwise/liblistwise/op/shift/shift.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[216]='listwise/liblistwise/op/sort/sort.debug.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sort/sort.debug.so common/parseint/parseint.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o listwise/liblistwise/op/sort/sort.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[217]='listwise/liblistwise/op/stat/stat.debug.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/stat/stat.debug.so listwise/liblistwise/op/stat/stat.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[218]='listwise/liblistwise/op/sx/sx.debug.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sx/sx.debug.so listwise/liblistwise/op/sx/sx.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[219]='listwise/liblistwise/op/t/t.debug.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/t/t.debug.so listwise/liblistwise/op/t/t.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[220]='listwise/liblistwise/op/u/u.debug.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/u/u.debug.so common/parseint/parseint.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o listwise/liblistwise/op/u/u.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[221]='listwise/liblistwise/op/up/up.debug.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/up/up.debug.so listwise/liblistwise/op/up/up.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[222]='listwise/liblistwise/op/uu/uu.debug.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/uu/uu.debug.so common/parseint/parseint.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o listwise/liblistwise/op/uu/uu.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[223]='listwise/liblistwise/op/v/v.debug.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/v/v.debug.so listwise/liblistwise/op/v/v.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[224]='listwise/liblistwise/op/vf/vf.debug.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vf/vf.debug.so listwise/liblistwise/op/vf/vf.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[225]='listwise/liblistwise/op/vp/vp.debug.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vp/vp.debug.so listwise/liblistwise/op/vp/vp.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[226]='listwise/liblistwise/op/w/w.debug.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/w/w.debug.so listwise/liblistwise/op/w/w.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[227]='listwise/liblistwise/op/wv/wv.debug.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wv/wv.debug.so listwise/liblistwise/op/wv/wv.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[228]='listwise/liblistwise/op/wvf/wvf.debug.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvf/wvf.debug.so listwise/liblistwise/op/wvf/wvf.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[229]='listwise/liblistwise/op/wvp/wvp.debug.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvp/wvp.debug.so listwise/liblistwise/op/wvp/wvp.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[230]='listwise/liblistwise/op/wz/wz.debug.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wz/wz.debug.so listwise/liblistwise/op/wz/wz.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[231]='listwise/liblistwise/op/x/x.debug.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/x/x.debug.so listwise/liblistwise/op/x/x.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[232]='listwise/liblistwise/op/xch/xch.debug.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/xch/xch.debug.so listwise/liblistwise/op/xch/xch.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[233]='listwise/liblistwise/op/y/y.debug.so'
fml_9_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/y/y.debug.so listwise/liblistwise/op/y/y.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[234]='listwise/liblistwise/op/z/z.debug.so'
fml_9_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/exec -I./listwise/liblistwise/logging -I./listwise/liblistwise/error -I./listwise/liblistwise/iterate -I./listwise/liblistwise/object -I./listwise/liblistwise/op -I./listwise/liblistwise/describe -I./listwise/liblistwise/lwx -I./listwise/liblistwise/re -I./listwise/liblistwise/internal -I./listwise/liblistwise/lstack -I./listwise/liblistwise/listwise -I./listwise/liblistwise/fs -I./listwise/liblistwise/selection -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise_test -I./listwise/liblistwise/generator -I./listwise/liblistwise/window -I./listwise/liblistwise/tune -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/z/z.debug.so listwise/liblistwise/op/z/z.debug.pic.o ./listwise/liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[235]='fab/fablw/op/fg/fg.debug.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -o fab/fablw/op/fg/fg.debug.so	fab/fablw/op/fg/fg.debug.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[236]='fab/fablw/op/fi/fi.debug.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -o fab/fablw/op/fi/fi.debug.so	fab/fablw/op/fi/fi.debug.pic.o -shared


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
  ((SKP+=13))
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
  exec 111>$tmp ; rm -f $tmp ; fml_8_49 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_8_50 & PIDS[12]=$!

  # harvest stage 8.2
  C=0
  while [[ $C != 13 ]]; do
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
    I=$((184+$idx))
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
    I=$((203+$idx))
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
  ((SKP+=13))
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
  exec 111>$tmp ; rm -f $tmp ; fml_9_49 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_9_50 & PIDS[12]=$!

  # harvest stage 9.2
  C=0
  while [[ $C != 13 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((222+$idx))
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
    I=$((235+$idx))
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
