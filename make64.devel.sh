#!/bin/bash

# A build script made by fab v0.5.0.0
#  fab is free software released under the GNU General Public License.
#
#  As a special exception, build scripts made by fab v0.5.0.0 (including this
#  build script) are excluded from the license covering fab itself, even
#  if substantial portions of the fab source code are copied verbatim into
#  the build script. You may create a larger work that contains part or all
#  of the build script and distribute that work under terms of your choice

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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[1]='common/color/color.devel.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[2]='common/parseint/parseint.devel.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[3]='common/parseint/parseint.devel.pic.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[4]='common/strutil/strutil.devel.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[5]='common/strutil/strutil.devel.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[6]='common/unitstring/unitstring.devel.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.devel.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[7]='common/wstdlib/wstdlib.devel.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[8]='common/wstdlib/wstdlib.devel.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.devel.pic.o


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

NAMES[19]='libpstring/exports'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libpstring/pstring/pstring.c )
echo "local: *; };"
	  ) 1>libpstring/exports


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[20]='libxapi/error/SYS.errtab'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	exec > libxapi/error/SYS.errtab

	cat /usr/include/asm-generic/errno-base.h /usr/include/asm-generic/errno.h  | \
sed -e 's/#define\s\+\(E[A-Z]\+\)\s\+\([0-9]\+\)\s\+\/\*\s*\(.*\)\s\+\*\//\2\t\1\t\3/p; d'


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[21]='{libxapi/error/XAPI.errtab.h, libxapi/error/XAPI.errtab.c}'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&102

  
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
  echo 2 1>&99
  exit $X
}

NAMES[22]='libxapi/test.devel.pic.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/test.c -o libxapi/test.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[23]='{libxlinux/error/XLINUX.errtab.h, libxlinux/error/XLINUX.errtab.c}'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&104

  
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
  echo 4 1>&99
  exit $X
}

NAMES[24]='{listwise/error/LISTWISE.errtab.h, listwise/error/LISTWISE.errtab.c}'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&105

  
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
  echo 5 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[25]='common/canon/canon.devel.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[26]='common/canon/canon.devel.pic.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[27]='common/coll/coll.devel.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[28]='common/coll/coll.devel.pic.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[29]='common/logger/logger.devel.o'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[30]='common/map/map.devel.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/map/map.c -o common/map/map.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[31]='common/map/map.devel.pic.o'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.devel.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[33]='common/yyutil/yyutil.devel.pic.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[34]='fab/depblock/depblock.devel.o'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/depblock/depblock.c -o fab/depblock/depblock.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[35]='fab/dirutil/dirutil.devel.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dirutil/dirutil.c -o fab/dirutil/dirutil.devel.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[36]='fab/error/FAB.errtab.devel.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/error/FAB.errtab.c -o fab/error/FAB.errtab.devel.o


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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/gn/gnlw.c -o fab/gn/gnlw.devel.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[40]='fab/hashblock/hashblock.devel.o'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/hashblock/hashblock.c -o fab/hashblock/hashblock.devel.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[41]='fab/identity/identity.devel.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/identity/identity.c -o fab/identity/identity.devel.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[42]='fab/logs/logs.devel.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/logs/logs.c -o fab/logs/logs.devel.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[43]='fab/lwutil/lwutil.devel.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/lwutil/lwutil.c -o fab/lwutil/lwutil.devel.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[44]='fab/params/params.devel.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/params/params.c -o fab/params/params.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[45]='fab/path/path.devel.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/path/path.c -o fab/path/path.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[46]='fab/strstack/strstack.devel.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/strstack/strstack.c -o fab/strstack/strstack.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[47]='fab/traverse/traverse.devel.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/traverse/traverse.c -o fab/traverse/traverse.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[48]='liblistwise/error/LW.errtab.devel.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/error/LW.errtab.c -o liblistwise/error/LW.errtab.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[49]='{liblistwise/error/PCRE.errtab.h, liblistwise/error/PCRE.errtab.c}'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&105

  
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
  echo 5 1>&99
  exit $X
}

NAMES[50]='{liblistwise/generator/generator.states.h, liblistwise/generator/generator.states.c}'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&106

  
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
  echo 6 1>&99
  exit $X
}

NAMES[51]='{liblistwise/generator/generator.tokens.h, liblistwise/generator/generator.tokens.c}'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&107

  
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
  echo 7 1>&99
  exit $X
}

NAMES[52]='libpstring/pstring/pstring.devel.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libpstring/pstring -I./libpstring/internal -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[53]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&109

  
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
  echo 9 1>&99
  exit $X
}

NAMES[54]='libxapi/error/XAPI.errtab.devel.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[55]='libxlinux/error/XLINUX.errtab.devel.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[56]='libxlinux/error/error.devel.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/error/error.c -o libxlinux/error/error.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[57]='libxlinux/exports'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxlinux/xgrp/xgrp.c libxlinux/error/error.c libxlinux/error/XLINUX.errtab.c libxlinux/xstat/xstat.c libxlinux/xunistd/xunistd.c libxlinux/xpwd/xpwd.c libxlinux/xmman/xmman.c libxlinux/xdirent/xdirent.c libxlinux/xfcntl/xfcntl.c libxlinux/xftw/xftw.c libxlinux/xstdlib/xstdlib.c libxlinux/xdlfcn/xdlfcn.c libxlinux/xstring/xstring.c libxlinux/xtime/xtime.c )
echo "local: *; };"
	  ) 1>libxlinux/exports


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[58]='libxlinux/xdirent/xdirent.devel.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[59]='libxlinux/xdlfcn/xdlfcn.devel.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[60]='libxlinux/xfcntl/xfcntl.devel.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[61]='libxlinux/xftw/xftw.devel.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[62]='libxlinux/xgrp/xgrp.devel.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[63]='libxlinux/xmman/xmman.devel.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[64]='libxlinux/xpwd/xpwd.devel.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[65]='libxlinux/xstat/xstat.devel.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[66]='libxlinux/xstdlib/xstdlib.devel.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[67]='libxlinux/xstring/xstring.devel.pic.o'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[68]='libxlinux/xtime/xtime.devel.pic.o'
fml_2_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[69]='libxlinux/xunistd/xunistd.devel.pic.o'
fml_2_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[70]='listwise/args/args.devel.o'
fml_2_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/args/args.c -o listwise/args/args.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[71]='listwise/error/LISTWISE.errtab.devel.o'
fml_2_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/error/LISTWISE.errtab.c -o listwise/error/LISTWISE.errtab.devel.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[72]='listwise/logs/logs.devel.o'
fml_2_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/logs/logs.c -o listwise/logs/logs.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[73]='listwise/main.devel.o'
fml_2_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c listwise/main.c -o listwise/main.devel.o


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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/args/args.c -o fab/args/args.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[75]='fab/bake/bake.devel.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/bake/bake.c -o fab/bake/bake.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[76]='fab/bp/bp.devel.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/bp/bp.c -o fab/bp/bp.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[77]='fab/dep/dep.devel.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dep/dep.c -o fab/dep/dep.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[78]='fab/dsc/dsc.devel.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/dsc/dsc.c -o fab/dsc/dsc.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[79]='fab/enclose/enclose.devel.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/enclose/enclose.c -o fab/enclose/enclose.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[80]='fab/error/error.devel.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/error/error.c -o fab/error/error.devel.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[81]='fab/ff/ff.devel.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.c -o fab/ff/ff.devel.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[82]='fab/ff/ff.dsc.tab.devel.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.dsc.tab.c -o fab/ff/ff.dsc.tab.devel.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[83]='fab/ff/ff.lex.devel.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.lex.c -o fab/ff/ff.lex.devel.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[84]='fab/ff/ff.list.tab.devel.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.list.tab.c -o fab/ff/ff.list.tab.devel.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[85]='fab/ff/ff.states.devel.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.states.c -o fab/ff/ff.states.devel.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[86]='fab/ff/ff.tab.devel.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.tab.c -o fab/ff/ff.tab.devel.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[87]='fab/ff/ff.tokens.devel.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.tokens.c -o fab/ff/ff.tokens.devel.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[88]='fab/ff/ff.var.tab.devel.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ff/ff.var.tab.c -o fab/ff/ff.var.tab.devel.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[89]='fab/ffn/ffn.devel.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ffn/ffn.c -o fab/ffn/ffn.devel.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[90]='fab/ffproc/ffproc.devel.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ffproc/ffproc.c -o fab/ffproc/ffproc.devel.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[91]='fab/fml/fml.devel.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/fml/fml.c -o fab/fml/fml.devel.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[92]='fab/gn/gn.devel.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/gn/gn.c -o fab/gn/gn.devel.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[93]='fab/list/list.devel.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/list/list.c -o fab/list/list.devel.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[94]='fab/main.devel.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/main.c -o fab/main.devel.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[95]='fab/selector/selector.devel.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/selector/selector.c -o fab/selector/selector.devel.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[96]='fab/tmp/tmp.devel.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/tmp/tmp.c -o fab/tmp/tmp.devel.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[97]='fab/ts/ts.devel.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/ts/ts.c -o fab/ts/ts.devel.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[98]='fab/var/var.devel.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -c fab/var/var.c -o fab/var/var.devel.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[99]='liblistwise/describe/describe.devel.pic.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/describe/describe.c -o liblistwise/describe/describe.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[100]='liblistwise/error/PCRE.errtab.devel.pic.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/error/PCRE.errtab.c -o liblistwise/error/PCRE.errtab.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[101]='liblistwise/error/error.devel.pic.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/error/error.c -o liblistwise/error/error.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[102]='liblistwise/exec/exec.devel.pic.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/exec/exec.c -o liblistwise/exec/exec.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[103]='liblistwise/exports'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- liblistwise/exec/exec.c liblistwise/logging/logging.c liblistwise/error/LW.errtab.c liblistwise/error/PCRE.errtab.c liblistwise/error/error.c liblistwise/iterate/iterate.c liblistwise/object/object.c liblistwise/op/wz/wz.c liblistwise/op/z/z.c liblistwise/op/o/o.c liblistwise/op/vf/vf.c liblistwise/op/wvp/wvp.c liblistwise/op/sort/sort.c liblistwise/op/sort/test.c liblistwise/op/w/w.c liblistwise/op/shift/shift.c liblistwise/op/dj/dj.c liblistwise/op/uu/uu.c liblistwise/op/u/u.c liblistwise/op/bn/bn.c liblistwise/op/re/re.c liblistwise/op/rp/rp.c liblistwise/op/f/f.c liblistwise/op/ls/ls.c liblistwise/op/lx/lx.c liblistwise/op/hl/hl.c liblistwise/op/rb/rb.c liblistwise/op/j/j.c liblistwise/op/wv/wv.c liblistwise/op/-X/-X.c liblistwise/op/up/up.c liblistwise/op/cp/cp.c liblistwise/op/p/p.c liblistwise/op/c/c.c liblistwise/op/r/r.c liblistwise/op/xch/xch.c liblistwise/op/t/t.c liblistwise/op/s/s.c liblistwise/op/cons/cons.c liblistwise/op/wvf/wvf.c liblistwise/op/dn/dn.c liblistwise/op/stat/stat.c liblistwise/op/g/g.c liblistwise/op/d/d.c liblistwise/op/vp/vp.c liblistwise/op/can/can.c liblistwise/op/datef/datef.c liblistwise/op/x/x.c liblistwise/op/b/b.c liblistwise/op/C/C.c liblistwise/op/rx/rx.c liblistwise/op/y/y.c liblistwise/op/fn/fn.c liblistwise/op/pop/pop.c liblistwise/op/sx/sx.c liblistwise/op/l/l.c liblistwise/op/l/test.c liblistwise/op/v/v.c liblistwise/describe/describe.c liblistwise/lwx/lwx.c liblistwise/re/re.c liblistwise/lstack/lstack.c liblistwise/fs/fs.c liblistwise/selection/selection.c liblistwise/operators/operators.c liblistwise/operator/operator.c liblistwise/sanity/sanity.c liblistwise/listwise_test/listwise_test.c liblistwise/generator/generator.lex.c liblistwise/generator/generator.c liblistwise/generator/generator.states.c liblistwise/generator/generator.tab.c liblistwise/generator/generator.tokens.c liblistwise/generator/genscan.c liblistwise/window/window.c liblistwise/tune/tune.c )
echo "local: *; };"
	 ) 1>liblistwise/exports


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[104]='liblistwise/fs/fs.devel.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/fs/fs.c -o liblistwise/fs/fs.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[105]='liblistwise/generator/generator.devel.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/generator.c -o liblistwise/generator/generator.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[106]='liblistwise/generator/generator.lex.devel.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/generator.lex.c -o liblistwise/generator/generator.lex.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[107]='liblistwise/generator/generator.states.devel.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/generator.states.c -o liblistwise/generator/generator.states.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[108]='liblistwise/generator/generator.tab.devel.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/generator.tab.c -o liblistwise/generator/generator.tab.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[109]='liblistwise/generator/generator.tokens.devel.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/generator.tokens.c -o liblistwise/generator/generator.tokens.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[110]='liblistwise/generator/genscan.devel.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/generator/genscan.c -o liblistwise/generator/genscan.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[111]='liblistwise/iterate/iterate.devel.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/iterate/iterate.c -o liblistwise/iterate/iterate.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[112]='liblistwise/logging/logging.devel.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/logging/logging.c -o liblistwise/logging/logging.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[113]='liblistwise/lstack/lstack.devel.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/lstack/lstack.c -o liblistwise/lstack/lstack.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[114]='liblistwise/lwx/lwx.devel.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/lwx/lwx.c -o liblistwise/lwx/lwx.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[115]='liblistwise/object/object.devel.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/object/object.c -o liblistwise/object/object.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[116]='liblistwise/operator/operator.devel.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/operator/operator.c -o liblistwise/operator/operator.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[117]='liblistwise/operators/operators.devel.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/operators/operators.c -o liblistwise/operators/operators.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[118]='liblistwise/re/re.devel.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/re/re.c -o liblistwise/re/re.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[119]='liblistwise/sanity/sanity.devel.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/sanity/sanity.c -o liblistwise/sanity/sanity.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[120]='liblistwise/selection/selection.devel.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/selection/selection.c -o liblistwise/selection/selection.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[121]='liblistwise/tune/tune.devel.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/tune/tune.c -o liblistwise/tune/tune.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[122]='liblistwise/window/window.devel.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/window/window.c -o liblistwise/window/window.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[123]='libxapi/callstack/callstack.devel.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[124]='libxapi/error/SYS.errtab.devel.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[125]='libxapi/error/error.devel.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/error/error.c -o libxapi/error/error.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[126]='libxapi/exports'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&114

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libxapi/error/error.c libxapi/error/SYS.errtab.c libxapi/error/XAPI.errtab.c libxapi/trace/trace.c libxapi/callstack/callstack.c libxapi/test.c libxapi/frame/frame.c )
	  echo "callstack;"
echo "local: *; };"
	) 1>libxapi/exports


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[127]='libxapi/frame/frame.devel.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/frame/frame.c -o libxapi/frame/frame.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[128]='libxapi/trace/trace.devel.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -c libxapi/trace/trace.c -o libxapi/trace/trace.devel.pic.o


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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi/error -I./libxapi/trace -I./libxapi/callstack -I./libxapi/internal -I./libxapi/xapi -I./libxapi/frame -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -o libxapi/libxapi.devel.so common/strutil/strutil.devel.pic.o common/wstdlib/wstdlib.devel.pic.o libxapi/callstack/callstack.devel.pic.o libxapi/error/SYS.errtab.devel.pic.o libxapi/error/XAPI.errtab.devel.pic.o libxapi/error/error.devel.pic.o libxapi/frame/frame.devel.pic.o libxapi/test.devel.pic.o libxapi/trace/trace.devel.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.devel.so


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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxlinux/xgrp -I./libxlinux/error -I./libxlinux/xstat -I./libxlinux/xunistd -I./libxlinux/xpwd -I./libxlinux/xmman -I./libxlinux/xdirent -I./libxlinux/internal -I./libxlinux/xfcntl -I./libxlinux/xftw -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux/xstring -I./libxlinux/xlinux -I./libxlinux/xtime -I./libxlinux -I./libxapi -I./liblistwise -I./libpstring -o libxlinux/libxlinux.devel.so libxlinux/error/XLINUX.errtab.devel.pic.o libxlinux/error/error.devel.pic.o libxlinux/xdirent/xdirent.devel.pic.o libxlinux/xdlfcn/xdlfcn.devel.pic.o libxlinux/xfcntl/xfcntl.devel.pic.o libxlinux/xftw/xftw.devel.pic.o libxlinux/xgrp/xgrp.devel.pic.o libxlinux/xmman/xmman.devel.pic.o libxlinux/xpwd/xpwd.devel.pic.o libxlinux/xstat/xstat.devel.pic.o libxlinux/xstdlib/xstdlib.devel.pic.o libxlinux/xstring/xstring.devel.pic.o libxlinux/xtime/xtime.devel.pic.o libxlinux/xunistd/xunistd.devel.pic.o ./libxapi/libxapi.devel.so -ldl -shared -Wl,--version-script=libxlinux/exports -Wl,-soname,libxlinux.devel.so


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
NAMES[132]='liblistwise/liblistwise.devel.so'
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
-DLWOPDIR=/home/todd/fab/liblistwise/op \
-DLWOPEXT=devel.so \
-o \
liblistwise/liblistwise.devel.so \
common/coll/coll.devel.pic.o \
common/map/map.devel.pic.o \
liblistwise/error/LW.errtab.devel.pic.o \
liblistwise/error/PCRE.errtab.devel.pic.o \
liblistwise/re/re.devel.pic.o \
liblistwise/object/object.devel.pic.o \
common/wstdlib/wstdlib.devel.pic.o \
liblistwise/lwx/lwx.devel.pic.o \
liblistwise/lstack/lstack.devel.pic.o \
liblistwise/logging/logging.devel.pic.o \
liblistwise/iterate/iterate.devel.pic.o \
liblistwise/generator/generator.states.devel.pic.o \
liblistwise/generator/genscan.devel.pic.o \
liblistwise/describe/describe.devel.pic.o \
liblistwise/sanity/sanity.devel.pic.o \
liblistwise/exec/exec.devel.pic.o \
liblistwise/error/error.devel.pic.o \
common/strutil/strutil.devel.pic.o \
common/canon/canon.devel.pic.o \
common/parseint/parseint.devel.pic.o \
common/color/color.devel.pic.o \
liblistwise/fs/fs.devel.pic.o \
liblistwise/selection/selection.devel.pic.o \
liblistwise/operators/operators.devel.pic.o \
liblistwise/operator/operator.devel.pic.o \
common/yyutil/yyutil.devel.pic.o \
liblistwise/generator/generator.tab.devel.pic.o \
liblistwise/generator/generator.tokens.devel.pic.o \
liblistwise/generator/generator.lex.devel.pic.o \
liblistwise/generator/generator.devel.pic.o \
liblistwise/window/window.devel.pic.o \
liblistwise/tune/tune.devel.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=liblistwise/exports \
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

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -export-dynamic -o fab/fab.devel common/wstdlib/wstdlib.devel.o common/strutil/strutil.devel.o common/logger/logger.devel.o fab/logs/logs.devel.o fab/error/FAB.errtab.devel.o common/unitstring/unitstring.devel.o common/canon/canon.devel.o common/coll/coll.devel.o common/cksum/cksum.devel.o fab/path/path.devel.o fab/params/params.devel.o fab/identity/identity.devel.o fab/dirutil/dirutil.devel.o fab/hashblock/hashblock.devel.o common/map/map.devel.o common/yyutil/yyutil.devel.o fab/ff/ff.tokens.devel.o fab/ffn/ffn.devel.o fab/ff/ff.tab.devel.o fab/ff/ff.dsc.tab.devel.o fab/ff/ff.var.tab.devel.o fab/ff/ff.list.tab.devel.o fab/ff/ff.lex.devel.o fab/ff/ff.states.devel.o fab/depblock/depblock.devel.o fab/strstack/strstack.devel.o fab/ts/ts.devel.o fab/traverse/traverse.devel.o fab/gn/gnlw.devel.o fab/lwutil/lwutil.devel.o fab/var/var.devel.o fab/list/list.devel.o fab/enclose/enclose.devel.o fab/fml/fml.devel.o common/parseint/parseint.devel.o fab/gn/gn.devel.o fab/ff/ff.devel.o fab/selector/selector.devel.o fab/args/args.devel.o fab/bp/bp.devel.o fab/dep/dep.devel.o fab/dsc/dsc.devel.o fab/tmp/tmp.devel.o fab/bake/bake.devel.o fab/ffproc/ffproc.devel.o fab/error/error.devel.o fab/main.devel.o ./libxapi/libxapi.devel.so ./libxlinux/libxlinux.devel.so ./libpstring/libpstring.devel.so ./liblistwise/liblistwise.devel.so '-Wl,-rpath,$ORIGIN/.././libxapi' '-Wl,-rpath,$ORIGIN/.././libxlinux' '-Wl,-rpath,$ORIGIN/.././xunit/libxunit' '-Wl,-rpath,$ORIGIN/.././libpstring' '-Wl,-rpath,$ORIGIN/.././liblistwise'


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[134]='liblistwise/op/-X/-X.devel.pic.o'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/-X/-X.c -o liblistwise/op/-X/-X.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[135]='liblistwise/op/C/C.devel.pic.o'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/C/C.c -o liblistwise/op/C/C.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[136]='liblistwise/op/b/b.devel.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/b/b.c -o liblistwise/op/b/b.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[137]='liblistwise/op/bn/bn.devel.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/bn/bn.c -o liblistwise/op/bn/bn.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[138]='liblistwise/op/c/c.devel.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/c/c.c -o liblistwise/op/c/c.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[139]='liblistwise/op/can/can.devel.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/can/can.c -o liblistwise/op/can/can.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[140]='liblistwise/op/cons/cons.devel.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/cons/cons.c -o liblistwise/op/cons/cons.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[141]='liblistwise/op/cp/cp.devel.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/cp/cp.c -o liblistwise/op/cp/cp.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[142]='liblistwise/op/d/d.devel.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/d/d.c -o liblistwise/op/d/d.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[143]='liblistwise/op/datef/datef.devel.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/datef/datef.c -o liblistwise/op/datef/datef.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[144]='liblistwise/op/dj/dj.devel.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/dj/dj.c -o liblistwise/op/dj/dj.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[145]='liblistwise/op/dn/dn.devel.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/dn/dn.c -o liblistwise/op/dn/dn.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[146]='liblistwise/op/f/f.devel.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/f/f.c -o liblistwise/op/f/f.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[147]='liblistwise/op/fn/fn.devel.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/fn/fn.c -o liblistwise/op/fn/fn.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[148]='liblistwise/op/g/g.devel.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/g/g.c -o liblistwise/op/g/g.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[149]='liblistwise/op/hl/hl.devel.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/hl/hl.c -o liblistwise/op/hl/hl.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[150]='liblistwise/op/j/j.devel.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/j/j.c -o liblistwise/op/j/j.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[151]='liblistwise/op/l/l.devel.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/l/l.c -o liblistwise/op/l/l.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[152]='liblistwise/op/ls/ls.devel.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/ls/ls.c -o liblistwise/op/ls/ls.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[153]='liblistwise/op/lx/lx.devel.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/lx/lx.c -o liblistwise/op/lx/lx.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[154]='liblistwise/op/o/o.devel.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/o/o.c -o liblistwise/op/o/o.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[155]='liblistwise/op/p/p.devel.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/p/p.c -o liblistwise/op/p/p.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[156]='liblistwise/op/pop/pop.devel.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/pop/pop.c -o liblistwise/op/pop/pop.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[157]='liblistwise/op/r/r.devel.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/r/r.c -o liblistwise/op/r/r.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[158]='liblistwise/op/rb/rb.devel.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/rb/rb.c -o liblistwise/op/rb/rb.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[159]='liblistwise/op/re/re.devel.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/re/re.c -o liblistwise/op/re/re.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[160]='liblistwise/op/rp/rp.devel.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/rp/rp.c -o liblistwise/op/rp/rp.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[161]='liblistwise/op/rx/rx.devel.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/rx/rx.c -o liblistwise/op/rx/rx.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[162]='liblistwise/op/s/s.devel.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/s/s.c -o liblistwise/op/s/s.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[163]='liblistwise/op/shift/shift.devel.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/shift/shift.c -o liblistwise/op/shift/shift.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[164]='liblistwise/op/sort/sort.devel.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/sort/sort.c -o liblistwise/op/sort/sort.devel.pic.o


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[165]='liblistwise/op/stat/stat.devel.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/stat/stat.c -o liblistwise/op/stat/stat.devel.pic.o


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[166]='liblistwise/op/sx/sx.devel.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/sx/sx.c -o liblistwise/op/sx/sx.devel.pic.o


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[167]='liblistwise/op/t/t.devel.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/t/t.c -o liblistwise/op/t/t.devel.pic.o


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[168]='liblistwise/op/u/u.devel.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/u/u.c -o liblistwise/op/u/u.devel.pic.o


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[169]='liblistwise/op/up/up.devel.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/up/up.c -o liblistwise/op/up/up.devel.pic.o


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[170]='liblistwise/op/uu/uu.devel.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/uu/uu.c -o liblistwise/op/uu/uu.devel.pic.o


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[171]='liblistwise/op/v/v.devel.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/v/v.c -o liblistwise/op/v/v.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[172]='liblistwise/op/vf/vf.devel.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/vf/vf.c -o liblistwise/op/vf/vf.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[173]='liblistwise/op/vp/vp.devel.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/vp/vp.c -o liblistwise/op/vp/vp.devel.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[174]='liblistwise/op/w/w.devel.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/w/w.c -o liblistwise/op/w/w.devel.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[175]='liblistwise/op/wv/wv.devel.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/wv/wv.c -o liblistwise/op/wv/wv.devel.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[176]='liblistwise/op/wvf/wvf.devel.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/wvf/wvf.c -o liblistwise/op/wvf/wvf.devel.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[177]='liblistwise/op/wvp/wvp.devel.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/wvp/wvp.c -o liblistwise/op/wvp/wvp.devel.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[178]='liblistwise/op/wz/wz.devel.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/wz/wz.c -o liblistwise/op/wz/wz.devel.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[179]='liblistwise/op/x/x.devel.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/x/x.c -o liblistwise/op/x/x.devel.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[180]='liblistwise/op/xch/xch.devel.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/xch/xch.c -o liblistwise/op/xch/xch.devel.pic.o


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[181]='liblistwise/op/y/y.devel.pic.o'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/y/y.c -o liblistwise/op/y/y.devel.pic.o


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[182]='liblistwise/op/z/z.devel.pic.o'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -c liblistwise/op/z/z.c -o liblistwise/op/z/z.devel.pic.o


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[183]='listwise/listwise.devel'
fml_8_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./listwise/error -I./listwise/logs -I./listwise/man -I./listwise/args -I./listwise -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -o listwise/listwise.devel listwise/error/LISTWISE.errtab.devel.o common/wstdlib/wstdlib.devel.o common/strutil/strutil.devel.o common/logger/logger.devel.o listwise/logs/logs.devel.o listwise/args/args.devel.o listwise/main.devel.o ./libxapi/libxapi.devel.so ./libxlinux/libxlinux.devel.so ./libpstring/libpstring.devel.so ./liblistwise/liblistwise.devel.so '-Wl,-rpath,$ORIGIN/.././libxapi' '-Wl,-rpath,$ORIGIN/.././libxlinux' '-Wl,-rpath,$ORIGIN/.././xunit/libxunit' '-Wl,-rpath,$ORIGIN/.././libpstring' '-Wl,-rpath,$ORIGIN/.././liblistwise'


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[184]='fab/fablw/op/fg/fg.devel.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.devel.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[185]='fab/fablw/op/fi/fi.devel.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.devel.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[186]='liblistwise/op/-X/-X.devel.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/-X/-X.devel.so liblistwise/op/-X/-X.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[187]='liblistwise/op/C/C.devel.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/C/C.devel.so liblistwise/op/C/C.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[188]='liblistwise/op/b/b.devel.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/b/b.devel.so liblistwise/op/b/b.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[189]='liblistwise/op/bn/bn.devel.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/bn/bn.devel.so liblistwise/op/bn/bn.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[190]='liblistwise/op/c/c.devel.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/c/c.devel.so liblistwise/op/c/c.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[191]='liblistwise/op/can/can.devel.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/can/can.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o common/canon/canon.devel.pic.o liblistwise/op/can/can.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[192]='liblistwise/op/cons/cons.devel.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/cons/cons.devel.so common/parseint/parseint.devel.pic.o liblistwise/op/cons/cons.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[193]='liblistwise/op/cp/cp.devel.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/cp/cp.devel.so liblistwise/op/cp/cp.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[194]='liblistwise/op/d/d.devel.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/d/d.devel.so liblistwise/op/d/d.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[195]='liblistwise/op/datef/datef.devel.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/datef/datef.devel.so common/parseint/parseint.devel.pic.o liblistwise/op/datef/datef.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[196]='liblistwise/op/dj/dj.devel.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/dj/dj.devel.so liblistwise/op/dj/dj.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[197]='liblistwise/op/dn/dn.devel.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/dn/dn.devel.so liblistwise/op/dn/dn.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[198]='liblistwise/op/f/f.devel.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/f/f.devel.so liblistwise/op/f/f.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[199]='liblistwise/op/fn/fn.devel.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/fn/fn.devel.so liblistwise/op/fn/fn.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[200]='liblistwise/op/g/g.devel.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/g/g.devel.so liblistwise/op/g/g.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[201]='liblistwise/op/hl/hl.devel.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/hl/hl.devel.so common/color/color.devel.pic.o liblistwise/op/hl/hl.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[202]='liblistwise/op/j/j.devel.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/j/j.devel.so liblistwise/op/j/j.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[203]='liblistwise/op/l/l.devel.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/l/l.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o liblistwise/op/l/l.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[204]='liblistwise/op/ls/ls.devel.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/ls/ls.devel.so common/parseint/parseint.devel.pic.o liblistwise/op/ls/ls.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[205]='liblistwise/op/lx/lx.devel.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/lx/lx.devel.so liblistwise/op/lx/lx.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[206]='liblistwise/op/o/o.devel.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/o/o.devel.so liblistwise/op/o/o.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[207]='liblistwise/op/p/p.devel.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/p/p.devel.so liblistwise/op/p/p.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[208]='liblistwise/op/pop/pop.devel.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/pop/pop.devel.so liblistwise/op/pop/pop.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[209]='liblistwise/op/r/r.devel.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/r/r.devel.so liblistwise/op/r/r.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[210]='liblistwise/op/rb/rb.devel.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/rb/rb.devel.so common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o common/canon/canon.devel.pic.o liblistwise/op/rb/rb.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[211]='liblistwise/op/re/re.devel.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/re/re.devel.so liblistwise/op/re/re.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[212]='liblistwise/op/rp/rp.devel.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/rp/rp.devel.so liblistwise/op/rp/rp.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[213]='liblistwise/op/rx/rx.devel.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/rx/rx.devel.so liblistwise/op/rx/rx.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[214]='liblistwise/op/s/s.devel.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/s/s.devel.so common/parseint/parseint.devel.pic.o liblistwise/op/s/s.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[215]='liblistwise/op/shift/shift.devel.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/shift/shift.devel.so liblistwise/op/shift/shift.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}

NAMES[216]='liblistwise/op/sort/sort.devel.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&113

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/sort/sort.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o liblistwise/op/sort/sort.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 13 1>&99
  exit $X
}

NAMES[217]='liblistwise/op/stat/stat.devel.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&114

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/stat/stat.devel.so liblistwise/op/stat/stat.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 14 1>&99
  exit $X
}

NAMES[218]='liblistwise/op/sx/sx.devel.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&115

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/sx/sx.devel.so liblistwise/op/sx/sx.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 15 1>&99
  exit $X
}

NAMES[219]='liblistwise/op/t/t.devel.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&116

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/t/t.devel.so liblistwise/op/t/t.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 16 1>&99
  exit $X
}

NAMES[220]='liblistwise/op/u/u.devel.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&117

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/u/u.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o liblistwise/op/u/u.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 17 1>&99
  exit $X
}

NAMES[221]='liblistwise/op/up/up.devel.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&118

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/up/up.devel.so liblistwise/op/up/up.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 18 1>&99
  exit $X
}

NAMES[222]='liblistwise/op/uu/uu.devel.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/uu/uu.devel.so common/parseint/parseint.devel.pic.o common/wstdlib/wstdlib.devel.pic.o common/strutil/strutil.devel.pic.o liblistwise/op/uu/uu.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[223]='liblistwise/op/v/v.devel.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/v/v.devel.so liblistwise/op/v/v.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[224]='liblistwise/op/vf/vf.devel.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/vf/vf.devel.so liblistwise/op/vf/vf.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[225]='liblistwise/op/vp/vp.devel.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/vp/vp.devel.so liblistwise/op/vp/vp.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[226]='liblistwise/op/w/w.devel.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/w/w.devel.so liblistwise/op/w/w.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[227]='liblistwise/op/wv/wv.devel.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/wv/wv.devel.so liblistwise/op/wv/wv.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[228]='liblistwise/op/wvf/wvf.devel.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/wvf/wvf.devel.so liblistwise/op/wvf/wvf.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[229]='liblistwise/op/wvp/wvp.devel.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/wvp/wvp.devel.so liblistwise/op/wvp/wvp.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[230]='liblistwise/op/wz/wz.devel.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/wz/wz.devel.so liblistwise/op/wz/wz.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[231]='liblistwise/op/x/x.devel.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&109

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/x/x.devel.so liblistwise/op/x/x.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 9 1>&99
  exit $X
}

NAMES[232]='liblistwise/op/xch/xch.devel.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&110

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/xch/xch.devel.so liblistwise/op/xch/xch.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 10 1>&99
  exit $X
}

NAMES[233]='liblistwise/op/y/y.devel.so'
fml_9_49()
{
  exec 1>/dev/null
  exec 2>&111

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/y/y.devel.so liblistwise/op/y/y.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 11 1>&99
  exit $X
}

NAMES[234]='liblistwise/op/z/z.devel.so'
fml_9_50()
{
  exec 1>/dev/null
  exec 2>&112

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./liblistwise/exec -I./liblistwise/logging -I./liblistwise/error -I./liblistwise/iterate -I./liblistwise/object -I./liblistwise/op -I./liblistwise/describe -I./liblistwise/lwx -I./liblistwise/re -I./liblistwise/internal -I./liblistwise/lstack -I./liblistwise/listwise -I./liblistwise/fs -I./liblistwise/selection -I./liblistwise/operators -I./liblistwise/operator -I./liblistwise/sanity -I./liblistwise/listwise_test -I./liblistwise/generator -I./liblistwise/window -I./liblistwise/tune -I./liblistwise -DLWOPDIR=/home/todd/fab/liblistwise/op -DLWOPEXT=devel.so -o liblistwise/op/z/z.devel.so liblistwise/op/z/z.devel.pic.o ./liblistwise/liblistwise.devel.so -shared


  X=$?
  echo 12 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[235]='fab/fablw/op/fg/fg.devel.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -o fab/fablw/op/fg/fg.devel.so	fab/fablw/op/fg/fg.devel.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[236]='fab/fablw/op/fi/fi.devel.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/unitstring -I./common/coll -I./common/wstdlib -I./common/parseint -I./common/logger -I./common/map -I./common/canon -I./common/yyutil -I./common/cksum -I./common/strutil -I./common/color -I./common/wstring -I./common -DBUILDSTAMP="`date`" -O0 -g -ggdb3 -DDEVEL -DXAPI_RUNTIME_CHECKS -DSANITY -DBUILDROOT=/home/todd/fab -fno-builtin-log -I./fab/list -I./fab/bp -I./fab/dep -I./fab/error -I./fab/ff -I./fab/ts -I./fab/logs -I./fab/depblock -I./fab/tmp -I./fab/doc -I./fab/strstack -I./fab/path -I./fab/ffproc -I./fab/dirutil -I./fab/lwutil -I./fab/man -I./fab/fablib -I./fab/fablib/std -I./fab/fablib/std/xunit -I./fab/fablib/std/bison -I./fab/fablib/std/flex -I./fab/fablib/std/xapi -I./fab/gn -I./fab/dsc -I./fab/hashblock -I./fab/fml -I./fab/fablw -I./fab/fablw/op -I./fab/fablw/op/fi -I./fab/fablw/op/fg -I./fab/bake -I./fab/selector -I./fab/enclose -I./fab/args -I./fab/ffn -I./fab/traverse -I./fab/identity -I./fab/params -I./fab/var -I./fab -DFABVERSIONS=v0.5.0.0 -DFABVERSIONN=500 -DFABCACHEDIR=/var/cache/fab -DFABTMPDIR=/var/tmp/fab -I./libxapi -I./libxlinux -I./liblistwise -I./libpstring -DFABLWOPDIR=/home/todd/fab/fab/fablw/op -DFABINVOKEDIR=/home/todd/fab/fab/fablib -o fab/fablw/op/fi/fi.devel.so	fab/fablw/op/fi/fi.devel.pic.o -shared


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
