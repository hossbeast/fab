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
NAMES[0]='common/cksum/cksum.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/cksum/cksum.c -o common/cksum/cksum.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/color/color.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/color/color.c -o common/color/color.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/parseint/parseint.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/parseint/parseint.c -o common/parseint/parseint.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint/parseint.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/strutil/strutil.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/strutil/strutil.c -o common/strutil/strutil.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/strutil/strutil.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring/unitstring.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/unitstring/unitstring.c -o common/unitstring/unitstring.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/wstdlib/wstdlib.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/wstdlib/wstdlib.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.pic.o


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

NAMES[10]='fab/ff/ff.dsc.y'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	cat fab/ff/ff.dsc.y.header fab/ff/ff.dsc.y.rules.1 fab/ff/ff.dsc.y.rules.2 fab/ff/ff.dsc.y.rules.3 fab/ff/ff.dsc.y.rules.4 > fab/ff/ff.dsc.y


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

NAMES[12]='fab/ff/ff.list.y'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	cat fab/ff/ff.list.y.header fab/ff/ff.dsc.y.rules.1 fab/ff/ff.dsc.y.rules.4 fab/ff/ff.list.y.rules.2 fab/ff/ff.list.y.rules.4 > fab/ff/ff.list.y


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[13]='fab/ff/ff.var.y'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	cat fab/ff/ff.var.y.header fab/ff/ff.dsc.y.rules.1 fab/ff/ff.dsc.y.rules.4 fab/ff/ff.var.y.rules.2 fab/ff/ff.var.y.rules.4 > fab/ff/ff.var.y


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[14]='fab/ff/ff.y'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	cat fab/ff/ff.y.header fab/ff/ff.dsc.y.rules.1 fab/ff/ff.dsc.y.rules.3 fab/ff/ff.dsc.y.rules.4 fab/ff/ff.y.rules.2 > fab/ff/ff.y


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

NAMES[21]='libxapi/test.pic.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/test.c -o libxapi/test.pic.o


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
NAMES[24]='common/canon/canon.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/canon/canon.c -o common/canon/canon.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[25]='common/canon/canon.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/canon/canon.c -o common/canon/canon.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[26]='common/coll/coll.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/coll/coll.c -o common/coll/coll.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[27]='common/coll/coll.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/coll/coll.c -o common/coll/coll.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[28]='common/logger/logger.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/logger/logger.c -o common/logger/logger.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[29]='common/map/map.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/map/map.c -o common/map/map.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[30]='common/map/map.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/map/map.c -o common/map/map.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[31]='common/yyutil/yyutil.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c common/yyutil/yyutil.c -o common/yyutil/yyutil.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.pic.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[33]='fab/dirutil/dirutil.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[34]='fab/error/FAB.errtab.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/error/FAB.errtab.c -o fab/error/FAB.errtab.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[35]='{fab/ff/ff.dsc.tab.h, fab/ff/ff.dsc.tab.c}'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	bison --warnings=error -o fab/ff/ff.dsc.tab.c -d	fab/ff/ff.dsc.y


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[36]='{fab/ff/ff.list.tab.h, fab/ff/ff.list.tab.c}'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	bison --warnings=error -o fab/ff/ff.list.tab.c -d	fab/ff/ff.list.y


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[37]='{fab/ff/ff.states.h, fab/ff/ff.states.c}'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&113

  
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
  echo 13 1>&99
  exit $X
}

NAMES[38]='{fab/ff/ff.tab.h, fab/ff/ff.tab.c}'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	bison --warnings=error -o fab/ff/ff.tab.c -d	fab/ff/ff.y


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[39]='{fab/ff/ff.var.tab.h, fab/ff/ff.var.tab.c}'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	bison --warnings=error -o fab/ff/ff.var.tab.c -d	fab/ff/ff.var.y


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[40]='fab/logs/logs.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/logs/logs.c -o fab/logs/logs.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[41]='fab/path/path.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/path/path.c -o fab/path/path.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[42]='fab/strstack/strstack.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/strstack/strstack.c -o fab/strstack/strstack.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[43]='liblistwise/error/LW.errtab.pic.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/error/LW.errtab.c -o liblistwise/error/LW.errtab.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[44]='{liblistwise/error/PCRE.errtab.h, liblistwise/error/PCRE.errtab.c}'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[45]='{liblistwise/generator/generator.states.h, liblistwise/generator/generator.states.c}'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&102

  
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
  echo 2 1>&99
  exit $X
}

NAMES[46]='{liblistwise/generator/generator.tokens.h, liblistwise/generator/generator.tokens.c}'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&103

  
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
  echo 3 1>&99
  exit $X
}

NAMES[47]='libpstring/pstring/pstring.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibpstring/../common/../libxapi -Ilibpstring/../common/../libxlinux -Ilibpstring/../common/../liblistwise -Ilibpstring/../common/../libpstring -Ilibpstring/../common/unitstring -Ilibpstring/../common/coll -Ilibpstring/../common/wstdlib -Ilibpstring/../common/parseint -Ilibpstring/../common/logger -Ilibpstring/../common/map -Ilibpstring/../common/canon -Ilibpstring/../common/yyutil -Ilibpstring/../common/cksum -Ilibpstring/../common/strutil -Ilibpstring/../common/color -Ilibpstring/../common/wstring -Ilibpstring/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibpstring/../common/../libpstring/pstring -Ilibpstring/../common/../libpstring/internal -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[48]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
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

NAMES[49]='libxapi/error/XAPI.errtab.pic.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[50]='libxapi/error/error.pic.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/error/error.c -o libxapi/error/error.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[51]='libxlinux/error/XLINUX.errtab.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[52]='libxlinux/error/error.pic.o'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/error/error.c -o libxlinux/error/error.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[53]='libxlinux/xdirent/xdirent.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[54]='libxlinux/xdlfcn/xdlfcn.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[55]='libxlinux/xfcntl/xfcntl.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[56]='libxlinux/xftw/xftw.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[57]='libxlinux/xgrp/xgrp.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[58]='libxlinux/xmman/xmman.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[59]='libxlinux/xpwd/xpwd.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[60]='libxlinux/xstat/xstat.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[61]='libxlinux/xstdlib/xstdlib.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[62]='libxlinux/xstring/xstring.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[63]='libxlinux/xtime/xtime.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[64]='libxlinux/xunistd/xunistd.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -fPIC -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/coll -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/parseint -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/canon -Ilibxlinux/../common/yyutil -Ilibxlinux/../common/cksum -Ilibxlinux/../common/strutil -Ilibxlinux/../common/color -Ilibxlinux/../common/wstring -Ilibxlinux/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[65]='listwise/error/LISTWISE.errtab.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../common/../libxapi -Ilistwise/../common/../libxlinux -Ilistwise/../common/../liblistwise -Ilistwise/../common/../libpstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/logger -Ilistwise/../common/map -Ilistwise/../common/canon -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/strutil -Ilistwise/../common/color -Ilistwise/../common/wstring -Ilistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ilistwise/../common/../listwise/error -Ilistwise/../common/../listwise/logs -Ilistwise/../common/../listwise/man -Ilistwise/../common/../listwise/args -Ilistwise/../common/../listwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DBUILDSTAMP="`date`" -c listwise/error/LISTWISE.errtab.c -o listwise/error/LISTWISE.errtab.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[66]='listwise/logs/logs.o'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../common/../libxapi -Ilistwise/../common/../libxlinux -Ilistwise/../common/../liblistwise -Ilistwise/../common/../libpstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/logger -Ilistwise/../common/map -Ilistwise/../common/canon -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/strutil -Ilistwise/../common/color -Ilistwise/../common/wstring -Ilistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ilistwise/../common/../listwise/error -Ilistwise/../common/../listwise/logs -Ilistwise/../common/../listwise/man -Ilistwise/../common/../listwise/args -Ilistwise/../common/../listwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DBUILDSTAMP="`date`" -c listwise/logs/logs.c -o listwise/logs/logs.o


  X=$?
  echo 4 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[67]='fab/depblock/depblock.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/depblock/depblock.c -o fab/depblock/depblock.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[68]='fab/error/error.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/error/error.c -o fab/error/error.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[69]='fab/ff/ff.dsc.tab.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[70]='fab/ff/ff.list.tab.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[71]='fab/ff/ff.states.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.states.c -o fab/ff/ff.states.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[72]='fab/ff/ff.tab.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.tab.c -o fab/ff/ff.tab.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[73]='{fab/ff/ff.tokens.h, fab/ff/ff.tokens.c}'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
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
  echo 6 1>&99
  exit $X
}

NAMES[74]='fab/ff/ff.var.tab.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[75]='fab/gn/gnlw.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/gn/gnlw.c -o fab/gn/gnlw.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[76]='fab/hashblock/hashblock.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[77]='fab/identity/identity.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/identity/identity.c -o fab/identity/identity.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[78]='fab/lwutil/lwutil.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[79]='fab/params/params.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/params/params.c -o fab/params/params.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[80]='fab/traverse/traverse.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/traverse/traverse.c -o fab/traverse/traverse.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[81]='liblistwise/describe/describe.pic.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/describe/describe.c -o liblistwise/describe/describe.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[82]='liblistwise/error/PCRE.errtab.pic.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/error/PCRE.errtab.c -o liblistwise/error/PCRE.errtab.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[83]='liblistwise/error/error.pic.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/error/error.c -o liblistwise/error/error.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[84]='liblistwise/exec/exec.pic.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/exec/exec.c -o liblistwise/exec/exec.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[85]='liblistwise/fs/fs.pic.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/fs/fs.c -o liblistwise/fs/fs.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[86]='liblistwise/generator/generator.lex.pic.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/generator.lex.c -o liblistwise/generator/generator.lex.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[87]='liblistwise/generator/generator.pic.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/generator.c -o liblistwise/generator/generator.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[88]='liblistwise/generator/generator.states.pic.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/generator.states.c -o liblistwise/generator/generator.states.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[89]='liblistwise/generator/generator.tab.pic.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/generator.tab.c -o liblistwise/generator/generator.tab.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[90]='liblistwise/generator/generator.tokens.pic.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/generator.tokens.c -o liblistwise/generator/generator.tokens.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[91]='liblistwise/generator/genscan.pic.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/generator/genscan.c -o liblistwise/generator/genscan.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[92]='liblistwise/iterate/iterate.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/iterate/iterate.c -o liblistwise/iterate/iterate.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[93]='liblistwise/logging/logging.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/logging/logging.c -o liblistwise/logging/logging.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[94]='liblistwise/lstack/lstack.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/lstack/lstack.c -o liblistwise/lstack/lstack.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[95]='liblistwise/lwx/lwx.pic.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/lwx/lwx.c -o liblistwise/lwx/lwx.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[96]='liblistwise/object/object.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/object/object.c -o liblistwise/object/object.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[97]='liblistwise/op/-X/-X.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/-X/-X.c -o liblistwise/op/-X/-X.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[98]='liblistwise/op/C/C.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/C/C.c -o liblistwise/op/C/C.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[99]='liblistwise/op/b/b.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/b/b.c -o liblistwise/op/b/b.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[100]='liblistwise/op/bn/bn.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/bn/bn.c -o liblistwise/op/bn/bn.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[101]='liblistwise/op/c/c.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/c/c.c -o liblistwise/op/c/c.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[102]='liblistwise/op/can/can.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/can/can.c -o liblistwise/op/can/can.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[103]='liblistwise/op/cp/cp.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/cp/cp.c -o liblistwise/op/cp/cp.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[104]='liblistwise/op/d/d.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/d/d.c -o liblistwise/op/d/d.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[105]='liblistwise/op/datef/datef.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/datef/datef.c -o liblistwise/op/datef/datef.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[106]='liblistwise/op/dj/dj.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/dj/dj.c -o liblistwise/op/dj/dj.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[107]='liblistwise/op/dn/dn.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/dn/dn.c -o liblistwise/op/dn/dn.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[108]='liblistwise/op/f/f.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/f/f.c -o liblistwise/op/f/f.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[109]='liblistwise/op/fn/fn.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/fn/fn.c -o liblistwise/op/fn/fn.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[110]='liblistwise/op/g/g.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/g/g.c -o liblistwise/op/g/g.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[111]='liblistwise/op/hl/hl.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/hl/hl.c -o liblistwise/op/hl/hl.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[112]='liblistwise/op/j/j.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/j/j.c -o liblistwise/op/j/j.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[113]='liblistwise/op/l/l.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/l/l.c -o liblistwise/op/l/l.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[114]='liblistwise/op/ls/ls.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/ls/ls.c -o liblistwise/op/ls/ls.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[115]='liblistwise/op/lx/lx.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/lx/lx.c -o liblistwise/op/lx/lx.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[116]='liblistwise/op/o/o.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/o/o.c -o liblistwise/op/o/o.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[117]='liblistwise/op/p/p.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/p/p.c -o liblistwise/op/p/p.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[118]='liblistwise/op/pop/pop.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/pop/pop.c -o liblistwise/op/pop/pop.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[119]='liblistwise/op/r/r.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/r/r.c -o liblistwise/op/r/r.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[120]='liblistwise/op/rb/rb.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/rb/rb.c -o liblistwise/op/rb/rb.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[121]='liblistwise/op/re/re.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/re/re.c -o liblistwise/op/re/re.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[122]='liblistwise/op/rp/rp.pic.o'
fml_3_55()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/rp/rp.c -o liblistwise/op/rp/rp.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[123]='liblistwise/op/rx/rx.pic.o'
fml_3_56()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/rx/rx.c -o liblistwise/op/rx/rx.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[124]='liblistwise/op/s/s.pic.o'
fml_3_57()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/s/s.c -o liblistwise/op/s/s.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[125]='liblistwise/op/shift/shift.pic.o'
fml_3_58()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/shift/shift.c -o liblistwise/op/shift/shift.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[126]='liblistwise/op/sort/sort.pic.o'
fml_3_59()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/sort/sort.c -o liblistwise/op/sort/sort.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[127]='liblistwise/op/stat/stat.pic.o'
fml_3_60()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/stat/stat.c -o liblistwise/op/stat/stat.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[128]='liblistwise/op/sx/sx.pic.o'
fml_3_61()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/sx/sx.c -o liblistwise/op/sx/sx.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[129]='liblistwise/op/t/t.pic.o'
fml_3_62()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/t/t.c -o liblistwise/op/t/t.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[130]='liblistwise/op/u/u.pic.o'
fml_3_63()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/u/u.c -o liblistwise/op/u/u.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[131]='liblistwise/op/up/up.pic.o'
fml_3_64()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/up/up.c -o liblistwise/op/up/up.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[132]='liblistwise/op/uu/uu.pic.o'
fml_3_65()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/uu/uu.c -o liblistwise/op/uu/uu.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[133]='liblistwise/op/v/v.pic.o'
fml_3_66()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/v/v.c -o liblistwise/op/v/v.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[134]='liblistwise/op/vf/vf.pic.o'
fml_3_67()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/vf/vf.c -o liblistwise/op/vf/vf.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[135]='liblistwise/op/vp/vp.pic.o'
fml_3_68()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/vp/vp.c -o liblistwise/op/vp/vp.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[136]='liblistwise/op/w/w.pic.o'
fml_3_69()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/w/w.c -o liblistwise/op/w/w.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[137]='liblistwise/op/wv/wv.pic.o'
fml_3_70()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/wv/wv.c -o liblistwise/op/wv/wv.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[138]='liblistwise/op/wvf/wvf.pic.o'
fml_3_71()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/wvf/wvf.c -o liblistwise/op/wvf/wvf.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[139]='liblistwise/op/wvp/wvp.pic.o'
fml_3_72()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/wvp/wvp.c -o liblistwise/op/wvp/wvp.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[140]='liblistwise/op/wz/wz.pic.o'
fml_3_73()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/wz/wz.c -o liblistwise/op/wz/wz.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[141]='liblistwise/op/x/x.pic.o'
fml_3_74()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/x/x.c -o liblistwise/op/x/x.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[142]='liblistwise/op/xch/xch.pic.o'
fml_3_75()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/xch/xch.c -o liblistwise/op/xch/xch.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[143]='liblistwise/op/y/y.pic.o'
fml_3_76()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/y/y.c -o liblistwise/op/y/y.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[144]='liblistwise/op/z/z.pic.o'
fml_3_77()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/op/z/z.c -o liblistwise/op/z/z.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[145]='liblistwise/operators/operators.pic.o'
fml_3_78()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/operators/operators.c -o liblistwise/operators/operators.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[146]='liblistwise/re/re.pic.o'
fml_3_79()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/re/re.c -o liblistwise/re/re.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[147]='liblistwise/sanity/sanity.pic.o'
fml_3_80()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/sanity/sanity.c -o liblistwise/sanity/sanity.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[148]='liblistwise/selection/selection.pic.o'
fml_3_81()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/selection/selection.c -o liblistwise/selection/selection.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[149]='liblistwise/tune/tune.pic.o'
fml_3_82()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/tune/tune.c -o liblistwise/tune/tune.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[150]='liblistwise/window/window.pic.o'
fml_3_83()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -c liblistwise/window/window.c -o liblistwise/window/window.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[151]='libxapi/callstack/callstack.pic.o'
fml_3_84()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[152]='libxapi/error/SYS.errtab.pic.o'
fml_3_85()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[153]='libxapi/frame/frame.pic.o'
fml_3_86()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/frame/frame.c -o libxapi/frame/frame.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[154]='libxapi/trace/trace.pic.o'
fml_3_87()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/unitstring -Ilibxapi/../common/coll -Ilibxapi/../common/wstdlib -Ilibxapi/../common/parseint -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/canon -Ilibxapi/../common/yyutil -Ilibxapi/../common/cksum -Ilibxapi/../common/strutil -Ilibxapi/../common/color -Ilibxapi/../common/wstring -Ilibxapi/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxapi/frame -c libxapi/trace/trace.c -o libxapi/trace/trace.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[155]='listwise/args/args.o'
fml_3_88()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../common/../libxapi -Ilistwise/../common/../libxlinux -Ilistwise/../common/../liblistwise -Ilistwise/../common/../libpstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/logger -Ilistwise/../common/map -Ilistwise/../common/canon -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/strutil -Ilistwise/../common/color -Ilistwise/../common/wstring -Ilistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ilistwise/../common/../listwise/error -Ilistwise/../common/../listwise/logs -Ilistwise/../common/../listwise/man -Ilistwise/../common/../listwise/args -Ilistwise/../common/../listwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DBUILDSTAMP="`date`" -c listwise/args/args.c -o listwise/args/args.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[156]='listwise/main.o'
fml_3_89()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ilistwise/../common/../libxapi -Ilistwise/../common/../libxlinux -Ilistwise/../common/../liblistwise -Ilistwise/../common/../libpstring -Ilistwise/../common/unitstring -Ilistwise/../common/coll -Ilistwise/../common/wstdlib -Ilistwise/../common/parseint -Ilistwise/../common/logger -Ilistwise/../common/map -Ilistwise/../common/canon -Ilistwise/../common/yyutil -Ilistwise/../common/cksum -Ilistwise/../common/strutil -Ilistwise/../common/color -Ilistwise/../common/wstring -Ilistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ilistwise/../common/../listwise/error -Ilistwise/../common/../listwise/logs -Ilistwise/../common/../listwise/man -Ilistwise/../common/../listwise/args -Ilistwise/../common/../listwise -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DBUILDSTAMP="`date`" -c listwise/main.c -o listwise/main.o


  X=$?
  echo 13 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[157]='fab/args/args.o'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/args/args.c -o fab/args/args.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[158]='fab/bake/bake.o'
fml_4_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/bake/bake.c -o fab/bake/bake.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[159]='fab/bp/bp.o'
fml_4_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/bp/bp.c -o fab/bp/bp.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[160]='fab/dep/dep.o'
fml_4_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/dep/dep.c -o fab/dep/dep.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[161]='fab/dsc/dsc.o'
fml_4_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/dsc/dsc.c -o fab/dsc/dsc.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[162]='fab/enclose/enclose.o'
fml_4_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/enclose/enclose.c -o fab/enclose/enclose.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[163]='fab/ff/ff.lex.o'
fml_4_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.lex.c -o fab/ff/ff.lex.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[164]='fab/ff/ff.o'
fml_4_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.c -o fab/ff/ff.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[165]='fab/ff/ff.tokens.o'
fml_4_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[166]='fab/ffn/ffn.o'
fml_4_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ffn/ffn.c -o fab/ffn/ffn.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[167]='fab/ffproc/ffproc.o'
fml_4_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[168]='fab/fml/fml.o'
fml_4_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/fml/fml.c -o fab/fml/fml.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[169]='fab/gn/gn.o'
fml_4_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/gn/gn.c -o fab/gn/gn.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[170]='fab/list/list.o'
fml_4_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/list/list.c -o fab/list/list.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[171]='fab/main.o'
fml_4_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/main.c -o fab/main.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[172]='fab/selector/selector.o'
fml_4_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/selector/selector.c -o fab/selector/selector.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[173]='fab/tmp/tmp.o'
fml_4_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/tmp/tmp.c -o fab/tmp/tmp.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[174]='fab/ts/ts.o'
fml_4_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/ts/ts.c -o fab/ts/ts.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[175]='fab/var/var.o'
fml_4_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -c fab/var/var.c -o fab/var/var.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[176]='libxapi/libxapi.so'
fml_4_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- common/strutil/strutil.c common/wstdlib/wstdlib.c libxapi/callstack/callstack.c libxapi/error/SYS.errtab.c libxapi/error/XAPI.errtab.c libxapi/error/error.c libxapi/frame/frame.c libxapi/test.c libxapi/trace/trace.c )
	  echo "callstack;"
echo "local: *; };"
	) 1>libxapi/../common/../libxapi/exports

	gcc -DDEVEL -DSANITY -DXAPI_PROVIDE_BACKTRACE -DXAPI_RUNTIME_CHECKS -D_GNU_SOURCE -Ilibxapi/../common -Ilibxapi/../common/../liblistwise -Ilibxapi/../common/../libpstring -Ilibxapi/../common/../libxapi -Ilibxapi/../common/../libxapi/callstack -Ilibxapi/../common/../libxapi/error -Ilibxapi/../common/../libxapi/frame -Ilibxapi/../common/../libxapi/internal -Ilibxapi/../common/../libxapi/trace -Ilibxapi/../common/../libxapi/xapi -Ilibxapi/../common/../libxlinux -Ilibxapi/../common/canon -Ilibxapi/../common/cksum -Ilibxapi/../common/coll -Ilibxapi/../common/color -Ilibxapi/../common/logger -Ilibxapi/../common/map -Ilibxapi/../common/parseint -Ilibxapi/../common/strutil -Ilibxapi/../common/unitstring -Ilibxapi/../common/wstdlib -Ilibxapi/../common/wstring -Ilibxapi/../common/yyutil -O0 -Wall -Werror -fPIC -fplan9-extensions -g -m64 -o libxapi/libxapi.so common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o libxapi/callstack/callstack.pic.o libxapi/error/SYS.errtab.pic.o libxapi/error/XAPI.errtab.pic.o libxapi/error/error.pic.o libxapi/frame/frame.pic.o libxapi/test.pic.o libxapi/trace/trace.pic.o -shared -Wl,-soname,libxapi.so -Wl,--version-script=libxapi/../common/../libxapi/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[177]='libxlinux/libxlinux.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxlinux/error/XLINUX.errtab.c libxlinux/error/error.c libxlinux/xdirent/xdirent.c libxlinux/xdlfcn/xdlfcn.c libxlinux/xfcntl/xfcntl.c libxlinux/xftw/xftw.c libxlinux/xgrp/xgrp.c libxlinux/xmman/xmman.c libxlinux/xpwd/xpwd.c libxlinux/xstat/xstat.c libxlinux/xstdlib/xstdlib.c libxlinux/xstring/xstring.c libxlinux/xtime/xtime.c libxlinux/xunistd/xunistd.c )
echo "local: *; };"
	  ) 1>libxlinux/../common/../libxlinux/exports

	gcc -DDEVEL -DSANITY -DXAPI_PROVIDE_BACKTRACE -DXAPI_RUNTIME_CHECKS -D_GNU_SOURCE -Ilibxlinux/../common -Ilibxlinux/../common/../liblistwise -Ilibxlinux/../common/../libpstring -Ilibxlinux/../common/../libxapi -Ilibxlinux/../common/../libxlinux -Ilibxlinux/../common/../libxlinux/error -Ilibxlinux/../common/../libxlinux/internal -Ilibxlinux/../common/../libxlinux/xdirent -Ilibxlinux/../common/../libxlinux/xdlfcn -Ilibxlinux/../common/../libxlinux/xfcntl -Ilibxlinux/../common/../libxlinux/xftw -Ilibxlinux/../common/../libxlinux/xgrp -Ilibxlinux/../common/../libxlinux/xlinux -Ilibxlinux/../common/../libxlinux/xmman -Ilibxlinux/../common/../libxlinux/xpwd -Ilibxlinux/../common/../libxlinux/xstat -Ilibxlinux/../common/../libxlinux/xstdlib -Ilibxlinux/../common/../libxlinux/xstring -Ilibxlinux/../common/../libxlinux/xtime -Ilibxlinux/../common/../libxlinux/xunistd -Ilibxlinux/../common/canon -Ilibxlinux/../common/cksum -Ilibxlinux/../common/coll -Ilibxlinux/../common/color -Ilibxlinux/../common/logger -Ilibxlinux/../common/map -Ilibxlinux/../common/parseint -Ilibxlinux/../common/strutil -Ilibxlinux/../common/unitstring -Ilibxlinux/../common/wstdlib -Ilibxlinux/../common/wstring -Ilibxlinux/../common/yyutil -O0 -Wall -Werror -fPIC -fplan9-extensions -g -m64 -o libxlinux/libxlinux.so libxlinux/error/XLINUX.errtab.pic.o libxlinux/error/error.pic.o libxlinux/xdirent/xdirent.pic.o libxlinux/xdlfcn/xdlfcn.pic.o libxlinux/xfcntl/xfcntl.pic.o libxlinux/xftw/xftw.pic.o libxlinux/xgrp/xgrp.pic.o libxlinux/xmman/xmman.pic.o libxlinux/xpwd/xpwd.pic.o libxlinux/xstat/xstat.pic.o libxlinux/xstdlib/xstdlib.pic.o libxlinux/xstring/xstring.pic.o libxlinux/xtime/xtime.pic.o libxlinux/xunistd/xunistd.pic.o libxapi/../common/../libxapi/libxapi.so -shared -Wl,-soname,libxlinux.so -Wl,--version-script=libxlinux/../common/../libxlinux/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[178]='libpstring/libpstring.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libpstring/pstring/pstring.c )
echo "local: *; };"
	  ) 1>libpstring/../common/../libpstring/exports

	gcc -DDEVEL -DSANITY -DXAPI_PROVIDE_BACKTRACE -DXAPI_RUNTIME_CHECKS -D_GNU_SOURCE -Ilibpstring/../common -Ilibpstring/../common/../liblistwise -Ilibpstring/../common/../libpstring -Ilibpstring/../common/../libpstring/internal -Ilibpstring/../common/../libpstring/pstring -Ilibpstring/../common/../libxapi -Ilibpstring/../common/../libxlinux -Ilibpstring/../common/canon -Ilibpstring/../common/cksum -Ilibpstring/../common/coll -Ilibpstring/../common/color -Ilibpstring/../common/logger -Ilibpstring/../common/map -Ilibpstring/../common/parseint -Ilibpstring/../common/strutil -Ilibpstring/../common/unitstring -Ilibpstring/../common/wstdlib -Ilibpstring/../common/wstring -Ilibpstring/../common/yyutil -O0 -Wall -Werror -fPIC -fplan9-extensions -g -m64 -o libpstring/libpstring.so libpstring/pstring/pstring.pic.o libxapi/../common/../libxapi/libxapi.so libxlinux/../common/../libxlinux/libxlinux.so -shared -Wl,-soname,libpstring.so -Wl,--version-script=libpstring/../common/../libpstring/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[179]='liblistwise/liblistwise.so'
fml_7_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- common/canon/canon.c common/coll/coll.c common/color/color.c common/map/map.c common/parseint/parseint.c common/strutil/strutil.c common/wstdlib/wstdlib.c common/yyutil/yyutil.c liblistwise/describe/describe.c liblistwise/error/LW.errtab.c liblistwise/error/PCRE.errtab.c liblistwise/error/error.c liblistwise/exec/exec.c liblistwise/fs/fs.c liblistwise/generator/generator.c liblistwise/generator/generator.lex.c liblistwise/generator/generator.states.c liblistwise/generator/generator.tab.c liblistwise/generator/generator.tokens.c liblistwise/generator/genscan.c liblistwise/iterate/iterate.c liblistwise/logging/logging.c liblistwise/lstack/lstack.c liblistwise/lwx/lwx.c liblistwise/object/object.c liblistwise/operators/operators.c liblistwise/re/re.c liblistwise/sanity/sanity.c liblistwise/selection/selection.c liblistwise/tune/tune.c liblistwise/window/window.c )
echo "local: *; };"
	 ) 1>liblistwise/../common/../liblistwise/exports

	gcc -DDEVEL -DLWOPDIR="/home/todd/fab/liblistwise/op" -DSANITY -DXAPI_PROVIDE_BACKTRACE -DXAPI_RUNTIME_CHECKS -D_GNU_SOURCE -Iliblistwise/../common -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/tune -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../libpstring -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/canon -Iliblistwise/../common/cksum -Iliblistwise/../common/coll -Iliblistwise/../common/color -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/parseint -Iliblistwise/../common/strutil -Iliblistwise/../common/unitstring -Iliblistwise/../common/wstdlib -Iliblistwise/../common/wstring -Iliblistwise/../common/yyutil -O0 -Wall -Werror -fPIC -fplan9-extensions -g -m64 -o liblistwise/liblistwise.so common/canon/canon.pic.o common/coll/coll.pic.o common/color/color.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o common/yyutil/yyutil.pic.o liblistwise/describe/describe.pic.o liblistwise/error/LW.errtab.pic.o liblistwise/error/PCRE.errtab.pic.o liblistwise/error/error.pic.o liblistwise/exec/exec.pic.o liblistwise/fs/fs.pic.o liblistwise/generator/generator.lex.pic.o liblistwise/generator/generator.pic.o liblistwise/generator/generator.states.pic.o liblistwise/generator/generator.tab.pic.o liblistwise/generator/generator.tokens.pic.o liblistwise/generator/genscan.pic.o liblistwise/iterate/iterate.pic.o liblistwise/logging/logging.pic.o liblistwise/lstack/lstack.pic.o liblistwise/lwx/lwx.pic.o liblistwise/object/object.pic.o liblistwise/operators/operators.pic.o liblistwise/re/re.pic.o liblistwise/sanity/sanity.pic.o liblistwise/selection/selection.pic.o liblistwise/tune/tune.pic.o liblistwise/window/window.pic.o -lpcre -ldl libxapi/../common/../libxapi/libxapi.so libxlinux/../common/../libxlinux/libxlinux.so libpstring/../common/../libpstring/libpstring.so -shared -Wl,-soname,liblistwise.so -Wl,--version-script=liblistwise/../common/../liblistwise/exports


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[180]='fab/fab'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -export-dynamic -o fab/fab common/wstdlib/wstdlib.o common/strutil/strutil.o common/logger/logger.o fab/logs/logs.o fab/error/FAB.errtab.o common/unitstring/unitstring.o common/canon/canon.o common/coll/coll.o common/cksum/cksum.o fab/path/path.o fab/params/params.o fab/identity/identity.o fab/dirutil/dirutil.o fab/hashblock/hashblock.o common/map/map.o common/yyutil/yyutil.o fab/ff/ff.tab.o fab/ff/ff.dsc.tab.o fab/ff/ff.var.tab.o fab/ff/ff.list.tab.o fab/ff/ff.tokens.o fab/ffn/ffn.o fab/ff/ff.lex.o fab/ff/ff.states.o fab/depblock/depblock.o fab/strstack/strstack.o fab/ts/ts.o fab/traverse/traverse.o fab/gn/gnlw.o fab/lwutil/lwutil.o fab/var/var.o fab/list/list.o fab/enclose/enclose.o fab/fml/fml.o common/parseint/parseint.o fab/gn/gn.o fab/ff/ff.o fab/selector/selector.o fab/args/args.o fab/bp/bp.o fab/dep/dep.o fab/dsc/dsc.o fab/tmp/tmp.o fab/bake/bake.o fab/ffproc/ffproc.o fab/error/error.o fab/main.o -Wl,-rpath,/home/todd/fab/libxapi -Wl,-rpath,/home/todd/fab/libxlinux -Wl,-rpath,/home/todd/fab/libpstring -Wl,-rpath,/home/todd/fab/liblistwise libxapi/../common/../libxapi/libxapi.so libxlinux/../common/../libxlinux/libxlinux.so libpstring/../common/../libpstring/libpstring.so liblistwise/../common/../liblistwise/liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[181]='liblistwise/op/-X/-X.so'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/-X/-X.so liblistwise/op/-X/-X.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,-X.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[182]='liblistwise/op/C/C.so'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/C/C.so liblistwise/op/C/C.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,C.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[183]='liblistwise/op/b/b.so'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/b/b.so liblistwise/op/b/b.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,b.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[184]='liblistwise/op/bn/bn.so'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/bn/bn.so liblistwise/op/bn/bn.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,bn.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[185]='liblistwise/op/c/c.so'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/c/c.so liblistwise/op/c/c.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,c.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[186]='liblistwise/op/can/can.so'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/can/can.so liblistwise/op/can/can.pic.o	common/canon/canon.pic.o common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,can.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[187]='liblistwise/op/cp/cp.so'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/cp/cp.so liblistwise/op/cp/cp.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,cp.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[188]='liblistwise/op/d/d.so'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/d/d.so liblistwise/op/d/d.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,d.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[189]='liblistwise/op/datef/datef.so'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/datef/datef.so liblistwise/op/datef/datef.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,datef.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[190]='liblistwise/op/dj/dj.so'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/dj/dj.so liblistwise/op/dj/dj.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,dj.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[191]='liblistwise/op/dn/dn.so'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/dn/dn.so liblistwise/op/dn/dn.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,dn.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[192]='liblistwise/op/f/f.so'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/f/f.so liblistwise/op/f/f.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,f.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[193]='liblistwise/op/fn/fn.so'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/fn/fn.so liblistwise/op/fn/fn.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,fn.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[194]='liblistwise/op/g/g.so'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/g/g.so liblistwise/op/g/g.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,g.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[195]='liblistwise/op/hl/hl.so'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/hl/hl.so liblistwise/op/hl/hl.pic.o	common/coll/coll.pic.o common/color/color.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,hl.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[196]='liblistwise/op/j/j.so'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/j/j.so liblistwise/op/j/j.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,j.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[197]='liblistwise/op/l/l.so'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/l/l.so liblistwise/op/l/l.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,l.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[198]='liblistwise/op/ls/ls.so'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/ls/ls.so liblistwise/op/ls/ls.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,ls.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[199]='liblistwise/op/lx/lx.so'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/lx/lx.so liblistwise/op/lx/lx.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,lx.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[200]='liblistwise/op/o/o.so'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/o/o.so liblistwise/op/o/o.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,o.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[201]='liblistwise/op/p/p.so'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/p/p.so liblistwise/op/p/p.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,p.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[202]='liblistwise/op/pop/pop.so'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/pop/pop.so liblistwise/op/pop/pop.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,pop.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[203]='liblistwise/op/r/r.so'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/r/r.so liblistwise/op/r/r.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,r.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[204]='liblistwise/op/rb/rb.so'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/rb/rb.so liblistwise/op/rb/rb.pic.o	common/canon/canon.pic.o common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,rb.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[205]='liblistwise/op/re/re.so'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/re/re.so liblistwise/op/re/re.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,re.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[206]='liblistwise/op/rp/rp.so'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/rp/rp.so liblistwise/op/rp/rp.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,rp.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[207]='liblistwise/op/rx/rx.so'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/rx/rx.so liblistwise/op/rx/rx.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,rx.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[208]='liblistwise/op/s/s.so'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/s/s.so liblistwise/op/s/s.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,s.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[209]='liblistwise/op/shift/shift.so'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/shift/shift.so liblistwise/op/shift/shift.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,shift.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[210]='liblistwise/op/sort/sort.so'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/sort/sort.so liblistwise/op/sort/sort.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,sort.so


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[211]='liblistwise/op/stat/stat.so'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/stat/stat.so liblistwise/op/stat/stat.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,stat.so


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[212]='liblistwise/op/sx/sx.so'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/sx/sx.so liblistwise/op/sx/sx.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,sx.so


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[213]='liblistwise/op/t/t.so'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/t/t.so liblistwise/op/t/t.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,t.so


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[214]='liblistwise/op/u/u.so'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/u/u.so liblistwise/op/u/u.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,u.so


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[215]='liblistwise/op/up/up.so'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/up/up.so liblistwise/op/up/up.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,up.so


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[216]='liblistwise/op/uu/uu.so'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/uu/uu.so liblistwise/op/uu/uu.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/parseint/parseint.pic.o common/strutil/strutil.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,uu.so


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[217]='liblistwise/op/v/v.so'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/v/v.so liblistwise/op/v/v.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,v.so


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[218]='liblistwise/op/vf/vf.so'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/vf/vf.so liblistwise/op/vf/vf.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,vf.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[219]='liblistwise/op/vp/vp.so'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/vp/vp.so liblistwise/op/vp/vp.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,vp.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[220]='liblistwise/op/w/w.so'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/w/w.so liblistwise/op/w/w.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,w.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[221]='liblistwise/op/wv/wv.so'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/wv/wv.so liblistwise/op/wv/wv.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,wv.so


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[222]='liblistwise/op/wvf/wvf.so'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/wvf/wvf.so liblistwise/op/wvf/wvf.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,wvf.so


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[223]='liblistwise/op/wvp/wvp.so'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/wvp/wvp.so liblistwise/op/wvp/wvp.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,wvp.so


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[224]='liblistwise/op/wz/wz.so'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/wz/wz.so liblistwise/op/wz/wz.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,wz.so


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[225]='liblistwise/op/x/x.so'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/x/x.so liblistwise/op/x/x.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,x.so


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[226]='liblistwise/op/xch/xch.so'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/xch/xch.so liblistwise/op/xch/xch.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,xch.so


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[227]='liblistwise/op/y/y.so'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/y/y.so liblistwise/op/y/y.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,y.so


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[228]='liblistwise/op/z/z.so'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Iliblistwise/../common/../libxapi -Iliblistwise/../common/../libxlinux -Iliblistwise/../common/../liblistwise -Iliblistwise/../common/../libpstring -Iliblistwise/../common/unitstring -Iliblistwise/../common/coll -Iliblistwise/../common/wstdlib -Iliblistwise/../common/parseint -Iliblistwise/../common/logger -Iliblistwise/../common/map -Iliblistwise/../common/canon -Iliblistwise/../common/yyutil -Iliblistwise/../common/cksum -Iliblistwise/../common/strutil -Iliblistwise/../common/color -Iliblistwise/../common/wstring -Iliblistwise/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fPIC -Iliblistwise/../common/../liblistwise/exec -Iliblistwise/../common/../liblistwise/logging -Iliblistwise/../common/../liblistwise/error -Iliblistwise/../common/../liblistwise/iterate -Iliblistwise/../common/../liblistwise/object -Iliblistwise/../common/../liblistwise/op -Iliblistwise/../common/../liblistwise/describe -Iliblistwise/../common/../liblistwise/lwx -Iliblistwise/../common/../liblistwise/re -Iliblistwise/../common/../liblistwise/internal -Iliblistwise/../common/../liblistwise/lstack -Iliblistwise/../common/../liblistwise/listwise -Iliblistwise/../common/../liblistwise/fs -Iliblistwise/../common/../liblistwise/selection -Iliblistwise/../common/../liblistwise/operators -Iliblistwise/../common/../liblistwise/operator -Iliblistwise/../common/../liblistwise/sanity -Iliblistwise/../common/../liblistwise/generator -Iliblistwise/../common/../liblistwise/window -Iliblistwise/../common/../liblistwise/tune -DLWOPDIR="/home/todd/fab/liblistwise/op" -o liblistwise/op/z/z.so liblistwise/op/z/z.pic.o	common/coll/coll.pic.o common/map/map.pic.o common/wstdlib/wstdlib.pic.o liblistwise/../common/../liblistwise/liblistwise.so -shared -Wl,-soname,z.so


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[229]='listwise/listwise'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -DBUILDSTAMP="`date`" -DDEVEL -DFABVERSIONN="500" -DFABVERSIONS="v0.5.0.0" -DSANITY -DXAPI_PROVIDE_BACKTRACE -DXAPI_RUNTIME_CHECKS -D_GNU_SOURCE -Ilistwise/../common -Ilistwise/../common/../liblistwise -Ilistwise/../common/../libpstring -Ilistwise/../common/../libxapi -Ilistwise/../common/../libxlinux -Ilistwise/../common/../listwise -Ilistwise/../common/../listwise/args -Ilistwise/../common/../listwise/error -Ilistwise/../common/../listwise/logs -Ilistwise/../common/../listwise/man -Ilistwise/../common/canon -Ilistwise/../common/cksum -Ilistwise/../common/coll -Ilistwise/../common/color -Ilistwise/../common/logger -Ilistwise/../common/map -Ilistwise/../common/parseint -Ilistwise/../common/strutil -Ilistwise/../common/unitstring -Ilistwise/../common/wstdlib -Ilistwise/../common/wstring -Ilistwise/../common/yyutil -O0 -Wall -Werror -fno-builtin-log -fplan9-extensions -g -m64 -o listwise/listwise listwise/error/LISTWISE.errtab.o common/wstdlib/wstdlib.o common/strutil/strutil.o common/logger/logger.o listwise/logs/logs.o listwise/args/args.o listwise/main.o -Wl,-rpath,/home/todd/fab/libxapi -Wl,-rpath,/home/todd/fab/libxlinux -Wl,-rpath,/home/todd/fab/libpstring -Wl,-rpath,/home/todd/fab/liblistwise libpstring/../common/../libpstring/libpstring.so liblistwise/../common/../liblistwise/liblistwise.so libxapi/../common/../libxapi/libxapi.so libxlinux/../common/../libxlinux/libxlinux.so


  X=$?
  echo 11 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[230]='fab/fablw/op/fg/fg.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[231]='fab/fablw/op/fi/fi.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[232]='fab/fablw/op/fg/fg.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -o fab/fablw/op/fg/fg.so	fab/fablw/op/fg/fg.pic.o  -shared -Wl,-soname,fg.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[233]='fab/fablw/op/fi/fi.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	  gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -Ifab/../common/../libxapi -Ifab/../common/../libxlinux -Ifab/../common/../liblistwise -Ifab/../common/../libpstring -Ifab/../common/unitstring -Ifab/../common/coll -Ifab/../common/wstdlib -Ifab/../common/parseint -Ifab/../common/logger -Ifab/../common/map -Ifab/../common/canon -Ifab/../common/yyutil -Ifab/../common/cksum -Ifab/../common/strutil -Ifab/../common/color -Ifab/../common/wstring -Ifab/../common -DXAPI_PROVIDE_BACKTRACE -O0 -g -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -fno-builtin-log -Ifab/../common/../fab/list -Ifab/../common/../fab/bp -Ifab/../common/../fab/dep -Ifab/../common/../fab/error -Ifab/../common/../fab/ff -Ifab/../common/../fab/ff/foo -Ifab/../common/../fab/ts -Ifab/../common/../fab/logs -Ifab/../common/../fab/depblock -Ifab/../common/../fab/tmp -Ifab/../common/../fab/doc -Ifab/../common/../fab/strstack -Ifab/../common/../fab/path -Ifab/../common/../fab/ffproc -Ifab/../common/../fab/dirutil -Ifab/../common/../fab/lwutil -Ifab/../common/../fab/man -Ifab/../common/../fab/fablib -Ifab/../common/../fab/fablib/std -Ifab/../common/../fab/fablib/std/bison -Ifab/../common/../fab/fablib/std/flex -Ifab/../common/../fab/fablib/std/xapi -Ifab/../common/../fab/gn -Ifab/../common/../fab/dsc -Ifab/../common/../fab/hashblock -Ifab/../common/../fab/fml -Ifab/../common/../fab/fablw -Ifab/../common/../fab/fablw/op -Ifab/../common/../fab/fablw/op/fi -Ifab/../common/../fab/fablw/op/fg -Ifab/../common/../fab/bake -Ifab/../common/../fab/selector -Ifab/../common/../fab/enclose -Ifab/../common/../fab/args -Ifab/../common/../fab/ffn -Ifab/../common/../fab/traverse -Ifab/../common/../fab/identity -Ifab/../common/../fab/params -Ifab/../common/../fab/var -Ifab/../common/../fab -DFABVERSIONS="v0.5.0.0" -DFABVERSIONN="500" -DFABCACHEDIR="/var/cache/fab" -DFABTMPDIR="/var/tmp/fab" -DFABLWOPDIR="/home/todd/fab/fab/fablw/op" -DFABINVOKEDIR="/home/todd/fab/fab/fablib" -DBUILDSTAMP="`date`" -o fab/fablw/op/fi/fi.so	fab/fablw/op/fi/fi.pic.o  -shared -Wl,-soname,fi.so


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
  ((SKP+=5))
else
  # launch stage 2.2
  exec 100>$tmp ; rm -f $tmp ; fml_2_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_39 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_40 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_41 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_42 & PIDS[4]=$!

  # harvest stage 2.2
  C=0
  while [[ $C != 5 ]]; do
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
    I=$((67+$idx))
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
    I=$((86+$idx))
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
    I=$((105+$idx))
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
    I=$((124+$idx))
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
  ((SKP+=14))
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

  # harvest stage 3.4
  C=0
  while [[ $C != 14 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((143+$idx))
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
  ((SKP+=19))
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

  # harvest stage 4.0
  C=0
  while [[ $C != 19 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((157+$idx))
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
  # launch stage 4.1
  exec 100>$tmp ; rm -f $tmp ; fml_4_19 & PIDS[0]=$!

  # harvest stage 4.1
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((176+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 4 $((idx+19)) $X "$N"
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
    I=$((177+$idx))
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
    I=$((178+$idx))
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
    I=$((179+$idx))
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
    I=$((180+$idx))
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
    I=$((199+$idx))
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
  ((SKP+=12))
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

  # harvest stage 8.2
  C=0
  while [[ $C != 12 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((218+$idx))
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
  ((SKP+=2))
else
  # launch stage 9.0
  exec 100>$tmp ; rm -f $tmp ; fml_9_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_1 & PIDS[1]=$!

  # harvest stage 9.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((230+$idx))
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
    I=$((232+$idx))
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
