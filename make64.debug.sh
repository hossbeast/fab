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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/color/color.debug.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/parseint/parseint.debug.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint/parseint.debug.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/strutil/strutil.debug.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/strutil/strutil.debug.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring/unitstring.debug.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.debug.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/wstdlib/wstdlib.debug.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/wstdlib/wstdlib.debug.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.debug.pic.o


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

NAMES[15]='{liblistwise/error/LW.errtab.h, liblistwise/error/LW.errtab.c}'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	hfl=liblistwise/error/LW.errtab.h
	hin=LW.errtab.h
	cfl=liblistwise/error/LW.errtab.c
	pfx=LW
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat liblistwise/error/LW.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat liblistwise/error/LW.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat liblistwise/error/LW.errtab | \
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

NAMES[16]='liblistwise/error/PCRE.errtab'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	exec 1>liblistwise/error/PCRE.errtab
sed -e 's/#define \(PCRE_ERROR_[A-Z_]\+\) \+(\([-0-9]\+\)).*/\2\t\1/p; d' <\
	/usr/include/pcre.h


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[17]='{liblistwise/generator/generator.lex.h, liblistwise/generator/generator.lex.c}'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	  flex  -o liblistwise/generator/generator.lex.c  --header-file=liblistwise/generator/generator.lex.h  liblistwise/generator/generator.l


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[18]='{liblistwise/generator/generator.tab.h, liblistwise/generator/generator.tab.c}'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	bison --warnings=error -o liblistwise/generator/generator.tab.c -d	liblistwise/generator/generator.y


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[19]='libxapi/error/SYS.errtab'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	exec > libxapi/error/SYS.errtab

	cat /usr/include/asm-generic/errno-base.h /usr/include/asm-generic/errno.h  | \
sed -e 's/#define\s\+\(E[A-Z]\+\)\s\+\([0-9]\+\)\s\+\/\*\s*\(.*\)\s\+\*\//\2\t\1\t\3/p; d'


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[20]='{libxapi/error/XAPI.errtab.h, libxapi/error/XAPI.errtab.c}'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[21]='libxapi/test.debug.pic.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/test.c -o libxapi/test.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[22]='{libxlinux/error/XLINUX.errtab.h, libxlinux/error/XLINUX.errtab.c}'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&103

  
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
  echo 3 1>&99
  exit $X
}

NAMES[23]='{listwise/error/LISTWISE.errtab.h, listwise/error/LISTWISE.errtab.c}'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	hfl=listwise/error/LISTWISE.errtab.h
	hin=LISTWISE.errtab.h
	cfl=listwise/error/LISTWISE.errtab.c
	pfx=LISTWISE
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat listwise/error/LISTWISE.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat listwise/error/LISTWISE.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat listwise/error/LISTWISE.errtab | \
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


# formulas and names for stage 2
NAMES[24]='common/canon/canon.debug.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[25]='common/canon/canon.debug.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[26]='common/coll/coll.debug.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[27]='common/coll/coll.debug.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[28]='common/logger/logger.debug.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[29]='common/map/map.debug.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/map/map.c -o common/map/map.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[30]='common/map/map.debug.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[31]='common/yyutil/yyutil.debug.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.debug.pic.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[33]='fab/depblock/depblock.debug.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/depblock/depblock.c -o fab/depblock/depblock.debug.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[34]='fab/dirutil/dirutil.debug.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.debug.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[35]='fab/error/FAB.errtab.debug.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/error/FAB.errtab.c -o fab/error/FAB.errtab.debug.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[36]='fab/ff/ff.dsc.tab.debug.o'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.debug.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[37]='fab/ff/ff.lex.debug.o'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.lex.c -o fab/ff/ff.lex.debug.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[38]='fab/ff/ff.list.tab.debug.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.debug.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[39]='{fab/ff/ff.states.h, fab/ff/ff.states.c}'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
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
  echo 15 1>&99
  exit $X
}

NAMES[40]='fab/ff/ff.tab.debug.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.tab.c -o fab/ff/ff.tab.debug.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[41]='{fab/ff/ff.tokens.h, fab/ff/ff.tokens.c}'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
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
  echo 17 1>&99
  exit $X
}

NAMES[42]='fab/ff/ff.var.tab.debug.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.debug.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[43]='fab/gn/gnlw.debug.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/gn/gnlw.c -o fab/gn/gnlw.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[44]='fab/hashblock/hashblock.debug.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[45]='fab/identity/identity.debug.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/identity/identity.c -o fab/identity/identity.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[46]='fab/logs/logs.debug.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/logs/logs.c -o fab/logs/logs.debug.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[47]='fab/lwutil/lwutil.debug.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[48]='fab/params/params.debug.o'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/params/params.c -o fab/params/params.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[49]='fab/path/path.debug.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/path/path.c -o fab/path/path.debug.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[50]='fab/strstack/strstack.debug.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/strstack/strstack.c -o fab/strstack/strstack.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[51]='fab/traverse/traverse.debug.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/traverse/traverse.c -o fab/traverse/traverse.debug.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[52]='liblistwise/error/LW.errtab.debug.pic.o'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/error/LW.errtab.c -o liblistwise/error/LW.errtab.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[53]='{liblistwise/error/PCRE.errtab.h, liblistwise/error/PCRE.errtab.c}'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	hfl=liblistwise/error/PCRE.errtab.h
	hin=PCRE.errtab.h
	cfl=liblistwise/error/PCRE.errtab.c
	pfx=PCRE
	api=APIDATA

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat liblistwise/error/PCRE.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat liblistwise/error/PCRE.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat liblistwise/error/PCRE.errtab | \
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
  echo 10 1>&99
  exit $X
}

NAMES[54]='liblistwise/generator/generator.lex.debug.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/generator.lex.c -o liblistwise/generator/generator.lex.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[55]='{liblistwise/generator/generator.states.h, liblistwise/generator/generator.states.c}'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&112

  
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
  echo 12 1>&99
  exit $X
}

NAMES[56]='liblistwise/generator/generator.tab.debug.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/generator.tab.c -o liblistwise/generator/generator.tab.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[57]='{liblistwise/generator/generator.tokens.h, liblistwise/generator/generator.tokens.c}'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&114

  
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
  echo 14 1>&99
  exit $X
}

NAMES[58]='libpstring/pstring/pstring.debug.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[59]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&116

  
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
  echo 16 1>&99
  exit $X
}

NAMES[60]='libxapi/error/XAPI.errtab.debug.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[61]='libxlinux/error/XLINUX.errtab.debug.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[62]='libxlinux/error/error.debug.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/error/error.c -o libxlinux/error/error.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[63]='libxlinux/xdirent/xdirent.debug.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[64]='libxlinux/xdlfcn/xdlfcn.debug.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[65]='libxlinux/xfcntl/xfcntl.debug.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[66]='libxlinux/xftw/xftw.debug.pic.o'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[67]='libxlinux/xgrp/xgrp.debug.pic.o'
fml_2_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[68]='libxlinux/xmman/xmman.debug.pic.o'
fml_2_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[69]='libxlinux/xpwd/xpwd.debug.pic.o'
fml_2_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[70]='libxlinux/xstat/xstat.debug.pic.o'
fml_2_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[71]='libxlinux/xstdlib/xstdlib.debug.pic.o'
fml_2_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[72]='libxlinux/xstring/xstring.debug.pic.o'
fml_2_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[73]='libxlinux/xtime/xtime.debug.pic.o'
fml_2_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[74]='libxlinux/xunistd/xunistd.debug.pic.o'
fml_2_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[75]='listwise/args/args.debug.o'
fml_2_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/args/args.c -o listwise/args/args.debug.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[76]='listwise/error/LISTWISE.errtab.debug.o'
fml_2_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/error/LISTWISE.errtab.c -o listwise/error/LISTWISE.errtab.debug.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[77]='listwise/logs/logs.debug.o'
fml_2_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/logs/logs.c -o listwise/logs/logs.debug.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[78]='listwise/main.debug.o'
fml_2_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/main.c -o listwise/main.debug.o


  X=$?
  echo 16 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[79]='fab/args/args.debug.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/args/args.c -o fab/args/args.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[80]='fab/bake/bake.debug.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/bake/bake.c -o fab/bake/bake.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[81]='fab/bp/bp.debug.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/bp/bp.c -o fab/bp/bp.debug.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[82]='fab/dep/dep.debug.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/dep/dep.c -o fab/dep/dep.debug.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[83]='fab/dsc/dsc.debug.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/dsc/dsc.c -o fab/dsc/dsc.debug.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[84]='fab/enclose/enclose.debug.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/enclose/enclose.c -o fab/enclose/enclose.debug.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[85]='fab/error/error.debug.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/error/error.c -o fab/error/error.debug.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[86]='fab/ff/ff.debug.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.c -o fab/ff/ff.debug.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[87]='fab/ff/ff.states.debug.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.states.c -o fab/ff/ff.states.debug.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[88]='fab/ff/ff.tokens.debug.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.debug.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[89]='fab/ffn/ffn.debug.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ffn/ffn.c -o fab/ffn/ffn.debug.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[90]='fab/ffproc/ffproc.debug.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.debug.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[91]='fab/fml/fml.debug.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/fml/fml.c -o fab/fml/fml.debug.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[92]='fab/gn/gn.debug.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/gn/gn.c -o fab/gn/gn.debug.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[93]='fab/list/list.debug.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/list/list.c -o fab/list/list.debug.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[94]='fab/main.debug.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/main.c -o fab/main.debug.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[95]='fab/selector/selector.debug.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/selector/selector.c -o fab/selector/selector.debug.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[96]='fab/tmp/tmp.debug.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/tmp/tmp.c -o fab/tmp/tmp.debug.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[97]='fab/ts/ts.debug.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/ts/ts.c -o fab/ts/ts.debug.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[98]='fab/var/var.debug.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -c fab/var/var.c -o fab/var/var.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[99]='liblistwise/describe/describe.debug.pic.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/describe/describe.c -o liblistwise/describe/describe.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[100]='liblistwise/error/PCRE.errtab.debug.pic.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/error/PCRE.errtab.c -o liblistwise/error/PCRE.errtab.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[101]='liblistwise/error/error.debug.pic.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/error/error.c -o liblistwise/error/error.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[102]='liblistwise/exec/exec.debug.pic.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/exec/exec.c -o liblistwise/exec/exec.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[103]='liblistwise/fs/fs.debug.pic.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/fs/fs.c -o liblistwise/fs/fs.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[104]='liblistwise/generator/generator.debug.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/generator.c -o liblistwise/generator/generator.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[105]='liblistwise/generator/generator.states.debug.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/generator.states.c -o liblistwise/generator/generator.states.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[106]='liblistwise/generator/generator.tokens.debug.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/generator.tokens.c -o liblistwise/generator/generator.tokens.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[107]='liblistwise/generator/genscan.debug.pic.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/generator/genscan.c -o liblistwise/generator/genscan.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[108]='liblistwise/iterate/iterate.debug.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/iterate/iterate.c -o liblistwise/iterate/iterate.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[109]='liblistwise/logging/logging.debug.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/logging/logging.c -o liblistwise/logging/logging.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[110]='liblistwise/lstack/lstack.debug.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/lstack/lstack.c -o liblistwise/lstack/lstack.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[111]='liblistwise/lwx/lwx.debug.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/lwx/lwx.c -o liblistwise/lwx/lwx.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[112]='liblistwise/object/object.debug.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/object/object.c -o liblistwise/object/object.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[113]='liblistwise/operator/operator.debug.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/operator/operator.c -o liblistwise/operator/operator.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[114]='liblistwise/operators/operators.debug.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/operators/operators.c -o liblistwise/operators/operators.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[115]='liblistwise/re/re.debug.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/re/re.c -o liblistwise/re/re.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[116]='liblistwise/sanity/sanity.debug.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/sanity/sanity.c -o liblistwise/sanity/sanity.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[117]='liblistwise/selection/selection.debug.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/selection/selection.c -o liblistwise/selection/selection.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[118]='liblistwise/tune/tune.debug.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/tune/tune.c -o liblistwise/tune/tune.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[119]='liblistwise/window/window.debug.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/window/window.c -o liblistwise/window/window.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[120]='libxapi/callstack/callstack.debug.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[121]='libxapi/error/SYS.errtab.debug.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[122]='libxapi/error/error.debug.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/error.c -o libxapi/error/error.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[123]='libxapi/frame/frame.debug.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/frame/frame.c -o libxapi/frame/frame.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[124]='libxapi/trace/trace.debug.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/trace/trace.c -o libxapi/trace/trace.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[125]='libxapi/libxapi.debug.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -o libxapi/libxapi.debug.so common/strutil/strutil.debug.pic.o common/wstdlib/wstdlib.debug.pic.o libxapi/callstack/callstack.debug.pic.o libxapi/error/SYS.errtab.debug.pic.o libxapi/error/XAPI.errtab.debug.pic.o libxapi/error/error.debug.pic.o libxapi/frame/frame.debug.pic.o libxapi/test.debug.pic.o libxapi/trace/trace.debug.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[126]='libxlinux/libxlinux.debug.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -o libxlinux/libxlinux.debug.so libxlinux/error/XLINUX.errtab.debug.pic.o libxlinux/error/error.debug.pic.o libxlinux/xdirent/xdirent.debug.pic.o libxlinux/xdlfcn/xdlfcn.debug.pic.o libxlinux/xfcntl/xfcntl.debug.pic.o libxlinux/xftw/xftw.debug.pic.o libxlinux/xgrp/xgrp.debug.pic.o libxlinux/xmman/xmman.debug.pic.o libxlinux/xpwd/xpwd.debug.pic.o libxlinux/xstat/xstat.debug.pic.o libxlinux/xstdlib/xstdlib.debug.pic.o libxlinux/xstring/xstring.debug.pic.o libxlinux/xtime/xtime.debug.pic.o libxlinux/xunistd/xunistd.debug.pic.o ./libxapi/libxapi.debug.so -ldl -shared -Wl,--version-script=libxlinux/exports -Wl,-soname,libxlinux.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[127]='libpstring/libpstring.debug.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring.debug.so libpstring/pstring/pstring.debug.pic.o ./libxapi/libxapi.debug.so -shared -Wl,--version-script=libpstring/exports ./libxlinux/libxlinux.debug.so -Wl,-soname,libpstring.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[128]='liblistwise/liblistwise.debug.so'
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
-DDEBUG \
-DXAPI_RUNTIME_CHECKS \
-DSANITY \
-fPIC \
-I./libxapi \
-I./libxlinux \
-I./libpstring \
-I./xunit/libxunit \
-I./liblistwise/exec \
-I./liblistwise/logging \
-I./liblistwise/error \
-I./liblistwise/iterate \
-I./liblistwise/object \
-I./liblistwise/op \
-I./liblistwise/describe \
-I./liblistwise/lwx \
-I./liblistwise/re \
-I./liblistwise/internal \
-I./liblistwise/lstack \
-I./liblistwise/listwise \
-I./liblistwise/fs \
-I./liblistwise/selection \
-I./liblistwise/operators \
-I./liblistwise/operator \
-I./liblistwise/sanity \
-I./liblistwise/listwise_test \
-I./liblistwise/generator \
-I./liblistwise/window \
-I./liblistwise/tune \
-I./liblistwise \
-DLWOPDIR=/usr/lib/listwise \
-DLWOPEXT=so \
-o \
liblistwise/liblistwise.debug.so \
common/coll/coll.debug.pic.o \
common/map/map.debug.pic.o \
liblistwise/error/LW.errtab.debug.pic.o \
liblistwise/error/PCRE.errtab.debug.pic.o \
liblistwise/re/re.debug.pic.o \
liblistwise/object/object.debug.pic.o \
common/wstdlib/wstdlib.debug.pic.o \
liblistwise/lwx/lwx.debug.pic.o \
liblistwise/lstack/lstack.debug.pic.o \
liblistwise/logging/logging.debug.pic.o \
liblistwise/iterate/iterate.debug.pic.o \
liblistwise/generator/generator.states.debug.pic.o \
liblistwise/generator/genscan.debug.pic.o \
liblistwise/describe/describe.debug.pic.o \
liblistwise/sanity/sanity.debug.pic.o \
liblistwise/exec/exec.debug.pic.o \
liblistwise/error/error.debug.pic.o \
common/strutil/strutil.debug.pic.o \
common/canon/canon.debug.pic.o \
common/parseint/parseint.debug.pic.o \
common/color/color.debug.pic.o \
liblistwise/fs/fs.debug.pic.o \
liblistwise/selection/selection.debug.pic.o \
liblistwise/operators/operators.debug.pic.o \
liblistwise/operator/operator.debug.pic.o \
liblistwise/generator/generator.lex.debug.pic.o \
common/yyutil/yyutil.debug.pic.o \
liblistwise/generator/generator.tab.debug.pic.o \
liblistwise/generator/generator.tokens.debug.pic.o \
liblistwise/generator/generator.debug.pic.o \
liblistwise/window/window.debug.pic.o \
liblistwise/tune/tune.debug.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=liblistwise/exports \
./libxapi/libxapi.debug.so \
./libxlinux/libxlinux.debug.so \
./libpstring/libpstring.debug.so \
-Wl,-soname,liblistwise.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[129]='fab/fab.debug'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -export-dynamic -o fab/fab.debug common/wstdlib/wstdlib.debug.o common/strutil/strutil.debug.o common/logger/logger.debug.o fab/logs/logs.debug.o fab/error/FAB.errtab.debug.o common/unitstring/unitstring.debug.o common/canon/canon.debug.o common/coll/coll.debug.o common/cksum/cksum.debug.o fab/path/path.debug.o fab/params/params.debug.o fab/identity/identity.debug.o fab/dirutil/dirutil.debug.o fab/hashblock/hashblock.debug.o common/map/map.debug.o common/yyutil/yyutil.debug.o fab/ff/ff.tab.debug.o fab/ff/ff.dsc.tab.debug.o fab/ff/ff.var.tab.debug.o fab/ff/ff.list.tab.debug.o fab/ff/ff.lex.debug.o fab/ff/ff.tokens.debug.o fab/ff/ff.states.debug.o fab/depblock/depblock.debug.o fab/strstack/strstack.debug.o fab/ffn/ffn.debug.o fab/ts/ts.debug.o fab/traverse/traverse.debug.o fab/gn/gnlw.debug.o fab/lwutil/lwutil.debug.o fab/var/var.debug.o fab/list/list.debug.o fab/enclose/enclose.debug.o fab/fml/fml.debug.o common/parseint/parseint.debug.o fab/gn/gn.debug.o fab/ff/ff.debug.o fab/selector/selector.debug.o fab/args/args.debug.o fab/bp/bp.debug.o fab/dep/dep.debug.o fab/dsc/dsc.debug.o fab/tmp/tmp.debug.o fab/bake/bake.debug.o fab/ffproc/ffproc.debug.o fab/error/error.debug.o fab/main.debug.o ./libxapi/libxapi.debug.so ./libxlinux/libxlinux.debug.so ./libpstring/libpstring.debug.so ./liblistwise/liblistwise.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[130]='liblistwise/op/-X/-X.debug.pic.o'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/-X/-X.c -o liblistwise/op/-X/-X.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[131]='liblistwise/op/C/C.debug.pic.o'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/C/C.c -o liblistwise/op/C/C.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[132]='liblistwise/op/b/b.debug.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/b/b.c -o liblistwise/op/b/b.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[133]='liblistwise/op/bn/bn.debug.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/bn/bn.c -o liblistwise/op/bn/bn.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[134]='liblistwise/op/c/c.debug.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/c/c.c -o liblistwise/op/c/c.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[135]='liblistwise/op/can/can.debug.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/can/can.c -o liblistwise/op/can/can.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[136]='liblistwise/op/cons/cons.debug.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/cons/cons.c -o liblistwise/op/cons/cons.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[137]='liblistwise/op/cp/cp.debug.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/cp/cp.c -o liblistwise/op/cp/cp.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[138]='liblistwise/op/d/d.debug.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/d/d.c -o liblistwise/op/d/d.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[139]='liblistwise/op/datef/datef.debug.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/datef/datef.c -o liblistwise/op/datef/datef.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[140]='liblistwise/op/dj/dj.debug.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/dj/dj.c -o liblistwise/op/dj/dj.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[141]='liblistwise/op/dn/dn.debug.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/dn/dn.c -o liblistwise/op/dn/dn.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[142]='liblistwise/op/f/f.debug.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/f/f.c -o liblistwise/op/f/f.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[143]='liblistwise/op/fn/fn.debug.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/fn/fn.c -o liblistwise/op/fn/fn.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[144]='liblistwise/op/g/g.debug.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/g/g.c -o liblistwise/op/g/g.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[145]='liblistwise/op/hl/hl.debug.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/hl/hl.c -o liblistwise/op/hl/hl.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[146]='liblistwise/op/j/j.debug.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/j/j.c -o liblistwise/op/j/j.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[147]='liblistwise/op/l/l.debug.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/l/l.c -o liblistwise/op/l/l.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[148]='liblistwise/op/ls/ls.debug.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/ls/ls.c -o liblistwise/op/ls/ls.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[149]='liblistwise/op/lx/lx.debug.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/lx/lx.c -o liblistwise/op/lx/lx.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[150]='liblistwise/op/o/o.debug.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/o/o.c -o liblistwise/op/o/o.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[151]='liblistwise/op/p/p.debug.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/p/p.c -o liblistwise/op/p/p.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[152]='liblistwise/op/pop/pop.debug.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/pop/pop.c -o liblistwise/op/pop/pop.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[153]='liblistwise/op/r/r.debug.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/r/r.c -o liblistwise/op/r/r.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[154]='liblistwise/op/rb/rb.debug.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/rb/rb.c -o liblistwise/op/rb/rb.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[155]='liblistwise/op/re/re.debug.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/re/re.c -o liblistwise/op/re/re.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[156]='liblistwise/op/rp/rp.debug.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/rp/rp.c -o liblistwise/op/rp/rp.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[157]='liblistwise/op/rx/rx.debug.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/rx/rx.c -o liblistwise/op/rx/rx.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[158]='liblistwise/op/s/s.debug.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/s/s.c -o liblistwise/op/s/s.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[159]='liblistwise/op/shift/shift.debug.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/shift/shift.c -o liblistwise/op/shift/shift.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[160]='liblistwise/op/sort/sort.debug.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/sort/sort.c -o liblistwise/op/sort/sort.debug.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[161]='liblistwise/op/stat/stat.debug.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/stat/stat.c -o liblistwise/op/stat/stat.debug.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[162]='liblistwise/op/sx/sx.debug.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/sx/sx.c -o liblistwise/op/sx/sx.debug.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[163]='liblistwise/op/t/t.debug.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/t/t.c -o liblistwise/op/t/t.debug.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[164]='liblistwise/op/u/u.debug.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/u/u.c -o liblistwise/op/u/u.debug.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[165]='liblistwise/op/up/up.debug.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/up/up.c -o liblistwise/op/up/up.debug.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[166]='liblistwise/op/uu/uu.debug.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/uu/uu.c -o liblistwise/op/uu/uu.debug.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[167]='liblistwise/op/v/v.debug.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/v/v.c -o liblistwise/op/v/v.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[168]='liblistwise/op/vf/vf.debug.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/vf/vf.c -o liblistwise/op/vf/vf.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[169]='liblistwise/op/vp/vp.debug.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/vp/vp.c -o liblistwise/op/vp/vp.debug.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[170]='liblistwise/op/w/w.debug.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/w/w.c -o liblistwise/op/w/w.debug.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[171]='liblistwise/op/wv/wv.debug.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/wv/wv.c -o liblistwise/op/wv/wv.debug.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[172]='liblistwise/op/wvf/wvf.debug.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/wvf/wvf.c -o liblistwise/op/wvf/wvf.debug.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[173]='liblistwise/op/wvp/wvp.debug.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/wvp/wvp.c -o liblistwise/op/wvp/wvp.debug.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[174]='liblistwise/op/wz/wz.debug.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/wz/wz.c -o liblistwise/op/wz/wz.debug.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[175]='liblistwise/op/x/x.debug.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/x/x.c -o liblistwise/op/x/x.debug.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[176]='liblistwise/op/xch/xch.debug.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/xch/xch.c -o liblistwise/op/xch/xch.debug.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[177]='liblistwise/op/y/y.debug.pic.o'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/y/y.c -o liblistwise/op/y/y.debug.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[178]='liblistwise/op/z/z.debug.pic.o'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c liblistwise/op/z/z.c -o liblistwise/op/z/z.debug.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[179]='listwise/listwise.debug'
fml_8_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -o listwise/listwise.debug listwise/error/LISTWISE.errtab.debug.o common/wstdlib/wstdlib.debug.o common/strutil/strutil.debug.o common/logger/logger.debug.o listwise/logs/logs.debug.o listwise/args/args.debug.o listwise/main.debug.o ./libxapi/libxapi.debug.so ./libxlinux/libxlinux.debug.so ./libpstring/libpstring.debug.so ./liblistwise/liblistwise.debug.so


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[180]='fab/fablw/op/fg/fg.debug.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[181]='fab/fablw/op/fi/fi.debug.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[182]='liblistwise/op/-X/-X.debug.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/-X/-X.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/-X/-X.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[183]='liblistwise/op/C/C.debug.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/C/C.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/C/C.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[184]='liblistwise/op/b/b.debug.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/b/b.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/b/b.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[185]='liblistwise/op/bn/bn.debug.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/bn/bn.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/bn/bn.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[186]='liblistwise/op/c/c.debug.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/c/c.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/c/c.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[187]='liblistwise/op/can/can.debug.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/can/can.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o common/canon/canon.debug.pic.o liblistwise/op/can/can.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[188]='liblistwise/op/cons/cons.debug.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/cons/cons.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o liblistwise/op/cons/cons.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[189]='liblistwise/op/cp/cp.debug.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/cp/cp.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/cp/cp.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[190]='liblistwise/op/d/d.debug.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/d/d.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/d/d.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[191]='liblistwise/op/datef/datef.debug.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/datef/datef.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o liblistwise/op/datef/datef.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[192]='liblistwise/op/dj/dj.debug.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/dj/dj.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/dj/dj.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[193]='liblistwise/op/dn/dn.debug.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/dn/dn.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/dn/dn.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[194]='liblistwise/op/f/f.debug.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/f/f.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/f/f.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[195]='liblistwise/op/fn/fn.debug.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/fn/fn.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/fn/fn.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[196]='liblistwise/op/g/g.debug.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/g/g.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/g/g.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[197]='liblistwise/op/hl/hl.debug.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/hl/hl.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/color/color.debug.pic.o liblistwise/op/hl/hl.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[198]='liblistwise/op/j/j.debug.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/j/j.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/j/j.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[199]='liblistwise/op/l/l.debug.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/l/l.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o liblistwise/op/l/l.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[200]='liblistwise/op/ls/ls.debug.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/ls/ls.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o liblistwise/op/ls/ls.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[201]='liblistwise/op/lx/lx.debug.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/lx/lx.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/lx/lx.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[202]='liblistwise/op/o/o.debug.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/o/o.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/o/o.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[203]='liblistwise/op/p/p.debug.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/p/p.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/p/p.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[204]='liblistwise/op/pop/pop.debug.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/pop/pop.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/pop/pop.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[205]='liblistwise/op/r/r.debug.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/r/r.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/r/r.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[206]='liblistwise/op/rb/rb.debug.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/rb/rb.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/strutil/strutil.debug.pic.o common/canon/canon.debug.pic.o liblistwise/op/rb/rb.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[207]='liblistwise/op/re/re.debug.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/re/re.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/re/re.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[208]='liblistwise/op/rp/rp.debug.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/rp/rp.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/rp/rp.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[209]='liblistwise/op/rx/rx.debug.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/rx/rx.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/rx/rx.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[210]='liblistwise/op/s/s.debug.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/s/s.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o liblistwise/op/s/s.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[211]='liblistwise/op/shift/shift.debug.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/shift/shift.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/shift/shift.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[212]='liblistwise/op/sort/sort.debug.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/sort/sort.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o common/strutil/strutil.debug.pic.o liblistwise/op/sort/sort.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[213]='liblistwise/op/stat/stat.debug.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/stat/stat.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/stat/stat.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[214]='liblistwise/op/sx/sx.debug.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/sx/sx.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/sx/sx.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[215]='liblistwise/op/t/t.debug.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/t/t.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/t/t.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[216]='liblistwise/op/u/u.debug.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/u/u.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o common/strutil/strutil.debug.pic.o liblistwise/op/u/u.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[217]='liblistwise/op/up/up.debug.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/up/up.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/up/up.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[218]='liblistwise/op/uu/uu.debug.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/uu/uu.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o common/parseint/parseint.debug.pic.o common/strutil/strutil.debug.pic.o liblistwise/op/uu/uu.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[219]='liblistwise/op/v/v.debug.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/v/v.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/v/v.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[220]='liblistwise/op/vf/vf.debug.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/vf/vf.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/vf/vf.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[221]='liblistwise/op/vp/vp.debug.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/vp/vp.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/vp/vp.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[222]='liblistwise/op/w/w.debug.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/w/w.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/w/w.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[223]='liblistwise/op/wv/wv.debug.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/wv/wv.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/wv/wv.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[224]='liblistwise/op/wvf/wvf.debug.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/wvf/wvf.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/wvf/wvf.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[225]='liblistwise/op/wvp/wvp.debug.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/wvp/wvp.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/wvp/wvp.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[226]='liblistwise/op/wz/wz.debug.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/wz/wz.debug.so liblistwise/op/wz/wz.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[227]='liblistwise/op/x/x.debug.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/x/x.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/x/x.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[228]='liblistwise/op/xch/xch.debug.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/xch/xch.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/xch/xch.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[229]='liblistwise/op/y/y.debug.so'
fml_9_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/y/y.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/y/y.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[230]='liblistwise/op/z/z.debug.so'
fml_9_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o liblistwise/op/z/z.debug.so common/coll/coll.debug.pic.o common/map/map.debug.pic.o common/wstdlib/wstdlib.debug.pic.o liblistwise/op/z/z.debug.pic.o ./liblistwise/liblistwise.debug.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[231]='fab/fablw/op/fg/fg.debug.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -o fab/fablw/op/fg/fg.debug.so	fab/fablw/op/fg/fg.debug.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[232]='fab/fablw/op/fi/fi.debug.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -DDEBUG -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/lib -DFABINVOKEDIR=/usr/lib/fab/listwise -o fab/fablw/op/fi/fi.debug.so	fab/fablw/op/fi/fi.debug.pic.o -shared


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
  ((SKP+=5))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[4]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 5 ]]; do
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
    I=$((24+$idx))
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
    I=$((43+$idx))
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
  ((SKP+=17))
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
  exec 111>$tmp ; rm -f $tmp ; fml_2_49 & PIDS[11]=$!
  exec 112>$tmp ; rm -f $tmp ; fml_2_50 & PIDS[12]=$!
  exec 113>$tmp ; rm -f $tmp ; fml_2_51 & PIDS[13]=$!
  exec 114>$tmp ; rm -f $tmp ; fml_2_52 & PIDS[14]=$!
  exec 115>$tmp ; rm -f $tmp ; fml_2_53 & PIDS[15]=$!
  exec 116>$tmp ; rm -f $tmp ; fml_2_54 & PIDS[16]=$!

  # harvest stage 2.2
  C=0
  while [[ $C != 17 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((62+$idx))
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
    I=$((98+$idx))
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
  ((SKP+=8))
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

  # harvest stage 3.2
  C=0
  while [[ $C != 8 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((117+$idx))
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
    I=$((125+$idx))
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
    I=$((126+$idx))
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
    I=$((127+$idx))
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
    I=$((128+$idx))
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
    I=$((129+$idx))
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
    I=$((148+$idx))
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
    I=$((167+$idx))
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
    I=$((180+$idx))
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
    I=$((199+$idx))
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
    I=$((218+$idx))
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
