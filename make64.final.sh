#!/bin/bash

# Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>
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

# fab --bslic-fab -k make64.final.sh @final

# re-exec under time
if [[ $1 != "timed" ]]; then
  time $0 timed
  exit $?
fi

# formulas and names for stage 0
NAMES[0]='listwise/liblistwise/error/PCRE.errtab'
fml_0_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	exec 1>listwise/liblistwise/error/PCRE.errtab
sed -e 's/#define \(PCRE_ERROR_[A-Z_]\+\) \+(\([-0-9]\+\)).*/\2\t\1/p; d' <\
	`pcre-config --prefix`/include/pcre.h


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 1
NAMES[1]='common/canon/canon.final.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[2]='common/canon/canon.final.pic.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[3]='common/cksum/cksum.final.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[4]='common/coll/coll.final.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.final.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[5]='common/coll/coll.final.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[6]='common/color/color.final.pic.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[7]='common/logger/logger.final.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[8]='common/map/map.final.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/map/map.c -o common/map/map.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[9]='common/map/map.final.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[10]='common/memblk/memblk.def.final.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/memblk/memblk.def.c -o common/memblk/memblk.def.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[11]='common/memblk/memblk.def.final.pic.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/memblk/memblk.def.c -o common/memblk/memblk.def.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[12]='common/parseint/parseint.final.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[13]='common/parseint/parseint.final.pic.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[14]='common/strutil/strutil.final.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.final.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[15]='common/strutil/strutil.final.pic.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[16]='common/unitstring/unitstring.final.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[17]='common/wstdlib/wstdlib.final.o'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[18]='common/wstdlib/wstdlib.final.pic.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[19]='common/yyutil/yyutil.final.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[20]='common/yyutil/yyutil.final.pic.o'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[21]='fab/common/dirutil/dirutil.final.o'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/dirutil/dirutil.c -o fab/common/dirutil/dirutil.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[22]='{fab/common/error/FAB.errtab.h, fab/common/error/FAB.errtab.c}'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	hfl=fab/common/error/FAB.errtab.h
	hin=FAB.errtab.h
	cfl=fab/common/error/FAB.errtab.c
	pfx=FAB
	api=

exec >${hfl}
echo "#ifndef _${pfx}_ERRTAB_H"
echo "#define _${pfx}_ERRTAB_H"
echo "#include \"xapi.h\""
echo "#define ETABLE_${pfx} \\"

	cat fab/common/error/FAB.errtab | \
sed -e 's/^\([-0-9]\+\)[\t ]\+\([^\t ]\+\)\([\t ]\+\(.*\)\)\?/_E(\1, \2, "\4") \\/p; d'

echo
echo "enum {"
echo "#define _E(a, b, c) ${pfx}_ ## b = a,"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "};"

echo -n "#define ERRMIN_${pfx} "
	cat fab/common/error/FAB.errtab | \
perl -e '$min=0xFFFF;while(<>){ if(/^([^\t ]+)/ && int($1)) { $min = [$min,$1]->[$min > $1] } } print $min, "\n"'

echo -n "#define ERRMAX_${pfx} "
	cat fab/common/error/FAB.errtab | \
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[23]='fab/common/logs/logs.final.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/logs/logs.c -o fab/common/logs/logs.final.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[24]='{fab/fabd/ff/ff.dsc.tab.h, fab/fabd/ff/ff.dsc.tab.c}'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	bison --warnings=error -o fab/fabd/ff/ff.dsc.tab.c -d	fab/fabd/ff/ff.dsc.y


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[25]='{fab/fabd/ff/ff.lex.h, fab/fabd/ff/ff.lex.c}'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	  flex  -o fab/fabd/ff/ff.lex.c  --header-file=fab/fabd/ff/ff.lex.h  fab/fabd/ff/ff.l


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[26]='{fab/fabd/ff/ff.list.tab.h, fab/fabd/ff/ff.list.tab.c}'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	bison --warnings=error -o fab/fabd/ff/ff.list.tab.c -d	fab/fabd/ff/ff.list.y


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[27]='{fab/fabd/ff/ff.tab.h, fab/fabd/ff/ff.tab.c}'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	bison --warnings=error -o fab/fabd/ff/ff.tab.c -d	fab/fabd/ff/ff.y


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[28]='{fab/fabd/ff/ff.var.tab.h, fab/fabd/ff/ff.var.tab.c}'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o fab/fabd/ff/ff.var.tab.c -d	fab/fabd/ff/ff.var.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[29]='libpstring/exports'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libpstring/pstring/pstring.c libpstring/write/write.c )
echo "local: *; };"
	  ) 1>libpstring/exports


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[30]='libpstring/pstring/pstring.final.pic.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[31]='libxapi/error/SYS.errtab'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	exec > libxapi/error/SYS.errtab

	cat /usr/include/asm-generic/errno-base.h /usr/include/asm-generic/errno.h  | \
sed -e 's/#define\s\+\(E[A-Z]\+\)\s\+\([0-9]\+\)\s\+\/\*\s*\(.*\)\s\+\*\//\2\t\1\t\3/p; d'


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[32]='{libxapi/error/XAPI.errtab.h, libxapi/error/XAPI.errtab.c}'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&104

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[33]='{libxlinux/error/XLINUX.errtab.h, libxlinux/error/XLINUX.errtab.c}'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&105

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[34]='{listwise/liblistwise/error/LW.errtab.h, listwise/liblistwise/error/LW.errtab.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&106

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[35]='{listwise/liblistwise/error/PCRE.errtab.h, listwise/liblistwise/error/PCRE.errtab.c}'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&107

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[36]='{listwise/liblistwise/transform/transform.lex.h, listwise/liblistwise/transform/transform.lex.c}'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	  flex  -o listwise/liblistwise/transform/transform.lex.c  --header-file=listwise/liblistwise/transform/transform.lex.h  listwise/liblistwise/transform/transform.l


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[37]='{listwise/liblistwise/transform/transform.tab.h, listwise/liblistwise/transform/transform.tab.c}'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o listwise/liblistwise/transform/transform.tab.c -d	listwise/liblistwise/transform/transform.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[38]='{listwise/listwise/error/LISTWISE.errtab.h, listwise/listwise/error/LISTWISE.errtab.c}'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&101

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[39]='listwise/listwise/logs/logs.final.o'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./listwise/listwise/logs -I./listwise/listwise/error -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/logs/logs.c -o listwise/listwise/logs/logs.final.o


  X=$?
  echo 2 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[40]='fab/common/error/FAB.errtab.final.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/error/FAB.errtab.c -o fab/common/error/FAB.errtab.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[41]='fab/common/params/params.final.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/params/params.c -o fab/common/params/params.final.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[42]='fab/fabd/ff/ff.dsc.tab.final.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.dsc.tab.c -o fab/fabd/ff/ff.dsc.tab.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[43]='fab/fabd/ff/ff.list.tab.final.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.list.tab.c -o fab/fabd/ff/ff.list.tab.final.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[44]='{fab/fabd/ff/ff.states.h, fab/fabd/ff/ff.states.c}'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	src=fab/fabd/ff/ff.lex.h
	states_h=fab/fabd/ff/ff.states.h
	states_c=fab/fabd/ff/ff.states.c
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
  echo 4 1>&99
  exit $X
}

NAMES[45]='fab/fabd/ff/ff.tab.final.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.tab.c -o fab/fabd/ff/ff.tab.final.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[46]='{fab/fabd/ff/ff.tokens.h, fab/fabd/ff/ff.tokens.c}'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	src=fab/fabd/ff/ff.tab.c
	tokens_h=fab/fabd/ff/ff.tokens.h
	tokens_c=fab/fabd/ff/ff.tokens.c
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
echo -n "#define ${pfx}_MINTOKEN "
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2/p' \
| sort -n \
| head -n 1
echo -n "#define ${pfx}_MAXTOKEN "
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2/p' \
| sort -nr \
| head -n 1
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

NAMES[47]='fab/fabd/ff/ff.var.tab.final.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.var.tab.c -o fab/fabd/ff/ff.var.tab.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[48]='fab/fabw/main.final.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabw -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabw/main.c -o fab/fabw/main.final.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[49]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&100

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c, .str = #b \" : \" c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[50]='libxapi/error/XAPI.errtab.final.pic.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[51]='libxlinux/error/XLINUX.errtab.final.pic.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[52]='libxlinux/error/error.final.pic.o'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/error/error.c -o libxlinux/error/error.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[53]='libxlinux/exports'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	  (echo "{ global: "
perl -ne 'if(/API(?:DATA)?.*? ([a-zA-Z_][a-zA-Z0-9_]+) *(?:\(|=)/) { print "$1;\n" }' <(\
	  cat -- libxlinux/xwait/xwait.c libxlinux/xfcntl/xfcntl.c libxlinux/xdirent/xdirent.c libxlinux/xprctl/xprctl.c libxlinux/error/error.c libxlinux/error/XLINUX.errtab.c libxlinux/xunistd/xunistd.c libxlinux/xtime/xtime.c libxlinux/xuio/xuio.c libxlinux/xgrp/xgrp.c libxlinux/xfile/xfile.c libxlinux/xstring/xstring.c libxlinux/xpwd/xpwd.c libxlinux/xsignal/xsignal.c libxlinux/xftw/xftw.c libxlinux/xstat/xstat.c libxlinux/mempolicy/mempolicy.c libxlinux/xmman/xmman.c libxlinux/xstdlib/xstdlib.c libxlinux/xdlfcn/xdlfcn.c )
echo "local: *; };"
	  ) 1>libxlinux/exports


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[54]='libxlinux/mempolicy/mempolicy.final.pic.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/mempolicy/mempolicy.c -o libxlinux/mempolicy/mempolicy.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[55]='libxlinux/xdirent/xdirent.final.pic.o'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[56]='libxlinux/xdlfcn/xdlfcn.final.pic.o'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[57]='libxlinux/xfcntl/xfcntl.final.pic.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[58]='libxlinux/xfile/xfile.final.pic.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xfile/xfile.c -o libxlinux/xfile/xfile.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[59]='libxlinux/xftw/xftw.final.pic.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[60]='libxlinux/xgrp/xgrp.final.pic.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[61]='libxlinux/xmman/xmman.final.pic.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[62]='libxlinux/xprctl/xprctl.final.pic.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xprctl/xprctl.c -o libxlinux/xprctl/xprctl.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[63]='libxlinux/xpwd/xpwd.final.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[64]='libxlinux/xsignal/xsignal.final.pic.o'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xsignal/xsignal.c -o libxlinux/xsignal/xsignal.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[65]='libxlinux/xstat/xstat.final.pic.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[66]='libxlinux/xstdlib/xstdlib.final.pic.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[67]='libxlinux/xstring/xstring.final.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[68]='libxlinux/xtime/xtime.final.pic.o'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[69]='libxlinux/xuio/xuio.final.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xuio/xuio.c -o libxlinux/xuio/xuio.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[70]='libxlinux/xunistd/xunistd.final.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[71]='libxlinux/xwait/xwait.final.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -c libxlinux/xwait/xwait.c -o libxlinux/xwait/xwait.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[72]='listwise/liblistwise/error/LW.errtab.final.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/LW.errtab.c -o listwise/liblistwise/error/LW.errtab.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[73]='listwise/liblistwise/error/PCRE.errtab.final.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/PCRE.errtab.c -o listwise/liblistwise/error/PCRE.errtab.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[74]='listwise/liblistwise/lstack/lstack.final.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lstack/lstack.c -o listwise/liblistwise/lstack/lstack.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[75]='listwise/liblistwise/lwx/lwx.final.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lwx/lwx.c -o listwise/liblistwise/lwx/lwx.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[76]='listwise/liblistwise/operator/operator.final.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operator/operator.c -o listwise/liblistwise/operator/operator.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[77]='listwise/liblistwise/operators/operators.final.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operators/operators.c -o listwise/liblistwise/operators/operators.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[78]='listwise/liblistwise/re/re.final.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/re/re.c -o listwise/liblistwise/re/re.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[79]='{listwise/liblistwise/transform/transform.states.h, listwise/liblistwise/transform/transform.states.c}'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	src=listwise/liblistwise/transform/transform.lex.h
	states_h=listwise/liblistwise/transform/transform.states.h
	states_c=listwise/liblistwise/transform/transform.states.c
	pfx=transform

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

NAMES[80]='{listwise/liblistwise/transform/transform.tokens.h, listwise/liblistwise/transform/transform.tokens.c}'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&104

  
	src=listwise/liblistwise/transform/transform.tab.c
	tokens_h=listwise/liblistwise/transform/transform.tokens.h
	tokens_c=listwise/liblistwise/transform/transform.tokens.c
	pfx=transform

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
echo -n "#define ${pfx}_MINTOKEN "
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2/p' \
| sort -n \
| head -n 1
echo -n "#define ${pfx}_MAXTOKEN "
sed -ne '/enum[ \t]\+yytokentype/,/};/ p' < ${src} \
| sed -ne 's/\([^ \t]\+\)[ \t]\+=[ \t]\+\([^ \t,]\+\),\?/\2/p' \
| sort -nr \
| head -n 1
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

NAMES[81]='listwise/liblistwise/tune/tune.final.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/tune/tune.c -o listwise/liblistwise/tune/tune.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[82]='listwise/listwise/args/args.final.o'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./listwise/listwise/logs -I./listwise/listwise/error -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/args/args.c -o listwise/listwise/args/args.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[83]='listwise/listwise/error/LISTWISE.errtab.final.o'
fml_2_43()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./listwise/listwise/logs -I./listwise/listwise/error -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/error/LISTWISE.errtab.c -o listwise/listwise/error/LISTWISE.errtab.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[84]='listwise/listwise/main.final.o'
fml_2_44()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./listwise/listwise/logs -I./listwise/listwise/error -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c listwise/listwise/main.c -o listwise/listwise/main.final.o


  X=$?
  echo 8 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[85]='common/memblk/memblk.final.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/memblk/memblk.c -o common/memblk/memblk.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[86]='common/memblk/memblk.final.pic.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/memblk/memblk.c -o common/memblk/memblk.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[87]='fab/common/args/args.final.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/args/args.c -o fab/common/args/args.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[88]='fab/common/identity/identity.final.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/identity/identity.c -o fab/common/identity/identity.final.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[89]='fab/common/path/path.final.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/path/path.c -o fab/common/path/path.final.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[90]='fab/fab/main.final.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fab -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fab/main.c -o fab/fab/main.final.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[91]='fab/fabd/bp/bp.final.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/bp/bp.c -o fab/fabd/bp/bp.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[92]='fab/fabd/bs/bs.final.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/bs/bs.c -o fab/fabd/bs/bs.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[93]='fab/fabd/dep/dep.final.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/dep/dep.c -o fab/fabd/dep/dep.final.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[94]='fab/fabd/dsc/dsc.final.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/dsc/dsc.c -o fab/fabd/dsc/dsc.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[95]='fab/fabd/enclose/enclose.final.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/enclose/enclose.c -o fab/fabd/enclose/enclose.final.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[96]='fab/fabd/ff/ff.final.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.c -o fab/fabd/ff/ff.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[97]='fab/fabd/ff/ff.lex.final.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.lex.c -o fab/fabd/ff/ff.lex.final.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[98]='fab/fabd/ff/ff.states.final.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.states.c -o fab/fabd/ff/ff.states.final.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[99]='fab/fabd/ff/ff.tokens.final.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.tokens.c -o fab/fabd/ff/ff.tokens.final.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[100]='fab/fabd/ffn/ffn.final.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ffn/ffn.c -o fab/fabd/ffn/ffn.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[101]='fab/fabd/ffproc/ffproc.final.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ffproc/ffproc.c -o fab/fabd/ffproc/ffproc.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[102]='fab/fabd/fml/fml.final.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/fml/fml.c -o fab/fabd/fml/fml.final.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[103]='fab/fabd/gn/gn.final.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/gn/gn.c -o fab/fabd/gn/gn.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[104]='fab/fabd/gn/gnlw.final.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/gn/gnlw.c -o fab/fabd/gn/gnlw.final.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[105]='fab/fabd/hashblock/hashblock.final.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/hashblock/hashblock.c -o fab/fabd/hashblock/hashblock.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[106]='fab/fabd/list/list.final.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/list/list.c -o fab/fabd/list/list.final.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[107]='fab/fabd/lwutil/lwutil.final.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/lwutil/lwutil.c -o fab/fabd/lwutil/lwutil.final.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[108]='fab/fabd/main.final.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/main.c -o fab/fabd/main.final.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[109]='fab/fabd/selector/selector.final.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/selector/selector.c -o fab/fabd/selector/selector.final.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[110]='fab/fabd/strstack/strstack.final.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/strstack/strstack.c -o fab/fabd/strstack/strstack.final.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[111]='fab/fabd/tmp/tmp.final.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/tmp/tmp.c -o fab/fabd/tmp/tmp.final.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[112]='fab/fabd/traverse/traverse.final.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/traverse/traverse.c -o fab/fabd/traverse/traverse.final.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[113]='fab/fabd/ts/ts.final.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ts/ts.c -o fab/fabd/ts/ts.final.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[114]='fab/fabd/var/var.final.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/var/var.c -o fab/fabd/var/var.final.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[115]='libxapi/callstack/callstack.final.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[116]='libxapi/error/SYS.errtab.final.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[117]='libxapi/error/error.final.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/error/error.c -o libxapi/error/error.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[118]='libxapi/exports'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	  (echo "{ global: "
perl -ne 'if(/API(?:DATA)?.*? ([a-zA-Z_][a-zA-Z0-9_]+) *(?:\(|=)/) { print "$1;\n" }' <(\
	  cat -- libxapi/error/error.c libxapi/error/SYS.errtab.c libxapi/error/XAPI.errtab.c libxapi/callstack/callstack.c libxapi/trace/trace.c libxapi/frame/frame.c libxapi/test.c )
	  echo "callstack;"
echo "local: *; };"
	) 1>libxapi/exports


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[119]='libxapi/frame/frame.final.pic.o'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/frame/frame.c -o libxapi/frame/frame.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[120]='libxapi/test.final.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/test.c -o libxapi/test.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[121]='libxapi/trace/trace.final.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -c libxapi/trace/trace.c -o libxapi/trace/trace.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[122]='listwise/liblistwise/describe/describe.final.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/describe/describe.c -o listwise/liblistwise/describe/describe.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[123]='listwise/liblistwise/error/error.final.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/error.c -o listwise/liblistwise/error/error.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[124]='listwise/liblistwise/exec/exec.final.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/exec/exec.c -o listwise/liblistwise/exec/exec.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[125]='listwise/liblistwise/exports'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&104

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- listwise/liblistwise/object/object.c listwise/liblistwise/transform/transform.c listwise/liblistwise/transform/transform.states.c listwise/liblistwise/transform/transform.tab.c listwise/liblistwise/transform/transform.lex.c listwise/liblistwise/transform/transform.tokens.c listwise/liblistwise/transform/genscan.c listwise/liblistwise/operators/operators.c listwise/liblistwise/error/PCRE.errtab.c listwise/liblistwise/error/error.c listwise/liblistwise/error/LW.errtab.c listwise/liblistwise/tune/tune.c listwise/liblistwise/operator/operator.c listwise/liblistwise/fs/fs.c listwise/liblistwise/lwx/lwx.c listwise/liblistwise/describe/describe.c listwise/liblistwise/exec/exec.c listwise/liblistwise/re/re.c listwise/liblistwise/lstack/lstack.c listwise/liblistwise/op/datef/datef.c listwise/liblistwise/op/can/can.c listwise/liblistwise/op/x/x.c listwise/liblistwise/op/f/f.c listwise/liblistwise/op/f/test.c listwise/liblistwise/op/y/y.c listwise/liblistwise/op/y/test.c listwise/liblistwise/op/r/r.c listwise/liblistwise/op/j/j.c listwise/liblistwise/op/sx/test.c listwise/liblistwise/op/sx/sx.c listwise/liblistwise/op/uu/uu.c listwise/liblistwise/op/uu/test.c listwise/liblistwise/op/t/t.c listwise/liblistwise/op/ls/ls.c listwise/liblistwise/op/xch/xch.c listwise/liblistwise/op/u/u.c listwise/liblistwise/op/u/test.c listwise/liblistwise/op/rp/rp.c listwise/liblistwise/op/vf/vf.c listwise/liblistwise/op/z/test.c listwise/liblistwise/op/z/z.c listwise/liblistwise/op/w/w.c listwise/liblistwise/op/v/v.c listwise/liblistwise/op/v/test.c listwise/liblistwise/op/bn/bn.c listwise/liblistwise/op/sort/sort.c listwise/liblistwise/op/sort/test.c listwise/liblistwise/op/g/g.c listwise/liblistwise/op/wvf/wvf.c listwise/liblistwise/op/wvf/test.c listwise/liblistwise/op/rx/rx.c listwise/liblistwise/op/s/test.c listwise/liblistwise/op/s/s.c listwise/liblistwise/op/re/re.c listwise/liblistwise/op/d/d.c listwise/liblistwise/op/up/up.c listwise/liblistwise/op/-X/-X.c listwise/liblistwise/op/vp/vp.c listwise/liblistwise/op/fn/fn.c listwise/liblistwise/op/shift/shift.c listwise/liblistwise/op/hl/hl.c listwise/liblistwise/op/rb/rb.c listwise/liblistwise/op/cons/cons.c listwise/liblistwise/op/p/p.c listwise/liblistwise/op/p/test.c listwise/liblistwise/op/b/b.c listwise/liblistwise/op/stat/stat.c listwise/liblistwise/op/l/l.c listwise/liblistwise/op/l/test.c listwise/liblistwise/op/wvp/wvp.c listwise/liblistwise/op/wvp/test.c listwise/liblistwise/op/ln/ln.c listwise/liblistwise/op/dj/dj.c listwise/liblistwise/op/lx/lx.c listwise/liblistwise/op/lx/test.c listwise/liblistwise/op/c/c.c listwise/liblistwise/op/pop/pop.c listwise/liblistwise/op/o/test.c listwise/liblistwise/op/o/o.c listwise/liblistwise/op/cp/cp.c listwise/liblistwise/op/dn/dn.c listwise/liblistwise/op/C/C.c listwise/liblistwise/test/fab_test/test.c listwise/liblistwise/test/listwise_test/listwise_test.c listwise/liblistwise/sanity/sanity.c listwise/liblistwise/selection/selection.c listwise/liblistwise/window/window.c listwise/liblistwise/logging/logging.c listwise/liblistwise/iterate/iterate.c )
echo "local: *; };"
	 ) 1>listwise/liblistwise/exports


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[126]='listwise/liblistwise/fs/fs.final.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/fs/fs.c -o listwise/liblistwise/fs/fs.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[127]='listwise/liblistwise/iterate/iterate.final.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/iterate/iterate.c -o listwise/liblistwise/iterate/iterate.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[128]='listwise/liblistwise/logging/logging.final.pic.o'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/logging/logging.c -o listwise/liblistwise/logging/logging.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[129]='listwise/liblistwise/object/object.final.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/object/object.c -o listwise/liblistwise/object/object.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[130]='listwise/liblistwise/sanity/sanity.final.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/sanity/sanity.c -o listwise/liblistwise/sanity/sanity.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[131]='listwise/liblistwise/selection/selection.final.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/selection/selection.c -o listwise/liblistwise/selection/selection.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[132]='listwise/liblistwise/transform/genscan.final.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/genscan.c -o listwise/liblistwise/transform/genscan.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[133]='listwise/liblistwise/transform/transform.final.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.c -o listwise/liblistwise/transform/transform.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[134]='listwise/liblistwise/transform/transform.lex.final.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.lex.c -o listwise/liblistwise/transform/transform.lex.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[135]='listwise/liblistwise/transform/transform.states.final.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.states.c -o listwise/liblistwise/transform/transform.states.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[136]='listwise/liblistwise/transform/transform.tab.final.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tab.c -o listwise/liblistwise/transform/transform.tab.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[137]='listwise/liblistwise/transform/transform.tokens.final.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tokens.c -o listwise/liblistwise/transform/transform.tokens.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[138]='listwise/liblistwise/window/window.final.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/window/window.c -o listwise/liblistwise/window/window.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[139]='libxapi/libxapi.final.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi/internal -I./libxapi/error -I./libxapi/callstack -I./libxapi/trace -I./libxapi/xapi -I./libxapi/frame -I./libxapi -o libxapi/libxapi.final.so common/memblk/memblk.def.final.pic.o common/memblk/memblk.final.pic.o common/strutil/strutil.final.pic.o common/wstdlib/wstdlib.final.pic.o libxapi/callstack/callstack.final.pic.o libxapi/error/SYS.errtab.final.pic.o libxapi/error/XAPI.errtab.final.pic.o libxapi/error/error.final.pic.o libxapi/frame/frame.final.pic.o libxapi/test.final.pic.o libxapi/trace/trace.final.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.so 


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[140]='libxlinux/libxlinux.final.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxlinux/xwait -I./libxlinux/xfcntl -I./libxlinux/internal -I./libxlinux/xdirent -I./libxlinux/xprctl -I./libxlinux/xlinux -I./libxlinux/error -I./libxlinux/xunistd -I./libxlinux/xtime -I./libxlinux/xuio -I./libxlinux/xgrp -I./libxlinux/xfile -I./libxlinux/xstring -I./libxlinux/xpwd -I./libxlinux/xsignal -I./libxlinux/xftw -I./libxlinux/xstat -I./libxlinux/mempolicy -I./libxlinux/xmman -I./libxlinux/xstdlib -I./libxlinux/xdlfcn -I./libxlinux -I./libxapi -I./listwise/liblistwise -o libxlinux/libxlinux.final.so libxlinux/error/XLINUX.errtab.final.pic.o libxlinux/error/error.final.pic.o libxlinux/mempolicy/mempolicy.final.pic.o libxlinux/xdirent/xdirent.final.pic.o libxlinux/xdlfcn/xdlfcn.final.pic.o libxlinux/xfcntl/xfcntl.final.pic.o libxlinux/xfile/xfile.final.pic.o libxlinux/xftw/xftw.final.pic.o libxlinux/xgrp/xgrp.final.pic.o libxlinux/xmman/xmman.final.pic.o libxlinux/xprctl/xprctl.final.pic.o libxlinux/xpwd/xpwd.final.pic.o libxlinux/xsignal/xsignal.final.pic.o libxlinux/xstat/xstat.final.pic.o libxlinux/xstdlib/xstdlib.final.pic.o libxlinux/xstring/xstring.final.pic.o libxlinux/xtime/xtime.final.pic.o libxlinux/xuio/xuio.final.pic.o libxlinux/xunistd/xunistd.final.pic.o libxlinux/xwait/xwait.final.pic.o ./libxapi/libxapi.final.so -ldl -lrt -shared -Wl,--version-script=libxlinux/exports -Wl,-soname,libxlinux.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[141]='libpstring/libpstring.final.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring.final.so libpstring/pstring/pstring.final.pic.o ./libxapi/libxapi.final.so -shared -Wl,--version-script=libpstring/exports ./libxlinux/libxlinux.final.so -Wl,-soname,libpstring.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[142]='listwise/liblistwise/liblistwise.final.so'
fml_7_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fms-extensions \
-DXAPI_PROVIDE_BACKTRACE \
-I./common/cksum \
-I./common/wstdlib \
-I./common/parseint \
-I./common/memblk \
-I./common/logger \
-I./common/map \
-I./common/yyutil \
-I./common/canon \
-I./common/color \
-I./common/wstring \
-I./common/strutil \
-I./common/unitstring \
-I./common/coll \
-I./common \
-DBUILDSTAMP="`date`" \
-O3 \
-fPIC \
-I./libxapi \
-I./libxlinux \
-I./libpstring \
-I./xunit/libxunit \
-I./listwise/liblistwise/object \
-I./listwise/liblistwise/transform \
-I./listwise/liblistwise/internal \
-I./listwise/liblistwise/operators \
-I./listwise/liblistwise/error \
-I./listwise/liblistwise/tune \
-I./listwise/liblistwise/operator \
-I./listwise/liblistwise/fs \
-I./listwise/liblistwise/lwx \
-I./listwise/liblistwise/describe \
-I./listwise/liblistwise/exec \
-I./listwise/liblistwise/re \
-I./listwise/liblistwise/lstack \
-I./listwise/liblistwise/op \
-I./listwise/liblistwise/test \
-I./listwise/liblistwise/sanity \
-I./listwise/liblistwise/listwise \
-I./listwise/liblistwise/selection \
-I./listwise/liblistwise/window \
-I./listwise/liblistwise/logging \
-I./listwise/liblistwise/iterate \
-I./listwise/liblistwise \
-DLWOPDIR=/usr/lib/listwise \
-DLWOPEXT=so \
-o \
listwise/liblistwise/liblistwise.final.so \
listwise/liblistwise/error/LW.errtab.final.pic.o \
listwise/liblistwise/tune/tune.final.pic.o \
listwise/liblistwise/re/re.final.pic.o \
listwise/liblistwise/operators/operators.final.pic.o \
listwise/liblistwise/operator/operator.final.pic.o \
common/wstdlib/wstdlib.final.pic.o \
listwise/liblistwise/lwx/lwx.final.pic.o \
listwise/liblistwise/lstack/lstack.final.pic.o \
listwise/liblistwise/transform/transform.states.final.pic.o \
listwise/liblistwise/transform/genscan.final.pic.o \
common/strutil/strutil.final.pic.o \
common/yyutil/yyutil.final.pic.o \
listwise/liblistwise/transform/transform.tab.final.pic.o \
listwise/liblistwise/transform/transform.tokens.final.pic.o \
listwise/liblistwise/transform/transform.lex.final.pic.o \
listwise/liblistwise/transform/transform.final.pic.o \
common/canon/canon.final.pic.o \
common/parseint/parseint.final.pic.o \
common/color/color.final.pic.o \
listwise/liblistwise/fs/fs.final.pic.o \
listwise/liblistwise/exec/exec.final.pic.o \
listwise/liblistwise/describe/describe.final.pic.o \
common/coll/coll.final.pic.o \
common/map/map.final.pic.o \
listwise/liblistwise/object/object.final.pic.o \
listwise/liblistwise/error/PCRE.errtab.final.pic.o \
listwise/liblistwise/error/error.final.pic.o \
listwise/liblistwise/sanity/sanity.final.pic.o \
listwise/liblistwise/selection/selection.final.pic.o \
listwise/liblistwise/window/window.final.pic.o \
listwise/liblistwise/logging/logging.final.pic.o \
listwise/liblistwise/iterate/iterate.final.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=listwise/liblistwise/exports \
./libxapi/libxapi.final.so \
./libxlinux/libxlinux.final.so \
./libpstring/libpstring.final.so \
-Wl,-soname,liblistwise.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[143]='fab/fab/fab.final'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fab -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -export-dynamic -o fab/fab/fab.final fab/common/error/FAB.errtab.final.o common/wstdlib/wstdlib.final.o common/strutil/strutil.final.o common/logger/logger.final.o fab/common/logs/logs.final.o common/canon/canon.final.o fab/common/params/params.final.o common/cksum/cksum.final.o common/memblk/memblk.def.final.o common/memblk/memblk.final.o fab/common/path/path.final.o common/unitstring/unitstring.final.o fab/common/args/args.final.o fab/common/dirutil/dirutil.final.o fab/common/identity/identity.final.o common/parseint/parseint.final.o fab/fab/main.final.o -lrt ./libxapi/libxapi.final.so ./libxlinux/libxlinux.final.so ./libpstring/libpstring.final.so ./listwise/liblistwise/liblistwise.final.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[144]='fab/fabd/fabd.final'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/fabd -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -export-dynamic -o fab/fabd/fabd.final fab/common/error/FAB.errtab.final.o common/wstdlib/wstdlib.final.o common/strutil/strutil.final.o common/logger/logger.final.o fab/common/logs/logs.final.o common/canon/canon.final.o fab/common/params/params.final.o common/cksum/cksum.final.o common/memblk/memblk.def.final.o common/memblk/memblk.final.o fab/common/path/path.final.o common/unitstring/unitstring.final.o fab/common/args/args.final.o fab/common/identity/identity.final.o common/yyutil/yyutil.final.o fab/fabd/ff/ff.tokens.final.o common/coll/coll.final.o common/map/map.final.o fab/common/dirutil/dirutil.final.o fab/fabd/bs/bs.final.o fab/fabd/dsc/dsc.final.o fab/fabd/selector/selector.final.o fab/fabd/dep/dep.final.o fab/fabd/ffproc/ffproc.final.o fab/fabd/list/list.final.o fab/fabd/var/var.final.o fab/fabd/tmp/tmp.final.o fab/fabd/lwutil/lwutil.final.o fab/fabd/gn/gnlw.final.o fab/fabd/enclose/enclose.final.o fab/fabd/traverse/traverse.final.o common/parseint/parseint.final.o fab/fabd/gn/gn.final.o fab/fabd/bp/bp.final.o fab/fabd/strstack/strstack.final.o fab/fabd/hashblock/hashblock.final.o fab/fabd/ff/ff.tab.final.o fab/fabd/ff/ff.dsc.tab.final.o fab/fabd/ff/ff.var.tab.final.o fab/fabd/ff/ff.list.tab.final.o fab/fabd/ff/ff.lex.final.o fab/fabd/ff/ff.states.final.o fab/fabd/ff/ff.final.o fab/fabd/ts/ts.final.o fab/fabd/ffn/ffn.final.o fab/fabd/fml/fml.final.o fab/fabd/main.final.o -lrt ./libxapi/libxapi.final.so ./libxlinux/libxlinux.final.so ./libpstring/libpstring.final.so ./listwise/liblistwise/liblistwise.final.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[145]='fab/fabw/fabw.final'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabw -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fabw/fabw.final fab/common/error/FAB.errtab.final.o common/wstdlib/wstdlib.final.o common/strutil/strutil.final.o common/logger/logger.final.o fab/common/logs/logs.final.o common/canon/canon.final.o fab/common/params/params.final.o common/parseint/parseint.final.o fab/fabw/main.final.o ./libxapi/libxapi.final.so ./libxlinux/libxlinux.final.so ./libpstring/libpstring.final.so ./listwise/liblistwise/liblistwise.final.so


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[146]='listwise/liblistwise/op/-X/-X.final.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/-X/-X.c -o listwise/liblistwise/op/-X/-X.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[147]='listwise/liblistwise/op/C/C.final.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/C/C.c -o listwise/liblistwise/op/C/C.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[148]='listwise/liblistwise/op/b/b.final.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/b/b.c -o listwise/liblistwise/op/b/b.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[149]='listwise/liblistwise/op/bn/bn.final.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/bn/bn.c -o listwise/liblistwise/op/bn/bn.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[150]='listwise/liblistwise/op/c/c.final.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/c/c.c -o listwise/liblistwise/op/c/c.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[151]='listwise/liblistwise/op/can/can.final.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/can/can.c -o listwise/liblistwise/op/can/can.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[152]='listwise/liblistwise/op/cons/cons.final.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cons/cons.c -o listwise/liblistwise/op/cons/cons.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[153]='listwise/liblistwise/op/cp/cp.final.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cp/cp.c -o listwise/liblistwise/op/cp/cp.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[154]='listwise/liblistwise/op/d/d.final.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/d/d.c -o listwise/liblistwise/op/d/d.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[155]='listwise/liblistwise/op/datef/datef.final.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/datef/datef.c -o listwise/liblistwise/op/datef/datef.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[156]='listwise/liblistwise/op/dj/dj.final.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dj/dj.c -o listwise/liblistwise/op/dj/dj.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[157]='listwise/liblistwise/op/dn/dn.final.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dn/dn.c -o listwise/liblistwise/op/dn/dn.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[158]='listwise/liblistwise/op/f/f.final.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/f/f.c -o listwise/liblistwise/op/f/f.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[159]='listwise/liblistwise/op/fn/fn.final.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/fn/fn.c -o listwise/liblistwise/op/fn/fn.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[160]='listwise/liblistwise/op/g/g.final.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/g/g.c -o listwise/liblistwise/op/g/g.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[161]='listwise/liblistwise/op/hl/hl.final.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/hl/hl.c -o listwise/liblistwise/op/hl/hl.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[162]='listwise/liblistwise/op/j/j.final.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/j/j.c -o listwise/liblistwise/op/j/j.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[163]='listwise/liblistwise/op/l/l.final.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/l/l.c -o listwise/liblistwise/op/l/l.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[164]='listwise/liblistwise/op/ln/ln.final.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/ln/ln.c -o listwise/liblistwise/op/ln/ln.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[165]='listwise/liblistwise/op/ls/ls.final.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/ls/ls.c -o listwise/liblistwise/op/ls/ls.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[166]='listwise/liblistwise/op/lx/lx.final.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/lx/lx.c -o listwise/liblistwise/op/lx/lx.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[167]='listwise/liblistwise/op/o/o.final.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/o/o.c -o listwise/liblistwise/op/o/o.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[168]='listwise/liblistwise/op/p/p.final.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/p/p.c -o listwise/liblistwise/op/p/p.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[169]='listwise/liblistwise/op/pop/pop.final.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/pop/pop.c -o listwise/liblistwise/op/pop/pop.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[170]='listwise/liblistwise/op/r/r.final.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/r/r.c -o listwise/liblistwise/op/r/r.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[171]='listwise/liblistwise/op/rb/rb.final.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rb/rb.c -o listwise/liblistwise/op/rb/rb.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[172]='listwise/liblistwise/op/re/re.final.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/re/re.c -o listwise/liblistwise/op/re/re.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[173]='listwise/liblistwise/op/rp/rp.final.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rp/rp.c -o listwise/liblistwise/op/rp/rp.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[174]='listwise/liblistwise/op/rx/rx.final.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rx/rx.c -o listwise/liblistwise/op/rx/rx.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[175]='listwise/liblistwise/op/s/s.final.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/s/s.c -o listwise/liblistwise/op/s/s.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[176]='listwise/liblistwise/op/shift/shift.final.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/shift/shift.c -o listwise/liblistwise/op/shift/shift.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[177]='listwise/liblistwise/op/sort/sort.final.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sort/sort.c -o listwise/liblistwise/op/sort/sort.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[178]='listwise/liblistwise/op/stat/stat.final.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/stat/stat.c -o listwise/liblistwise/op/stat/stat.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[179]='listwise/liblistwise/op/sx/sx.final.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sx/sx.c -o listwise/liblistwise/op/sx/sx.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[180]='listwise/liblistwise/op/t/t.final.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/t/t.c -o listwise/liblistwise/op/t/t.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[181]='listwise/liblistwise/op/u/u.final.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/u/u.c -o listwise/liblistwise/op/u/u.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[182]='listwise/liblistwise/op/up/up.final.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/up/up.c -o listwise/liblistwise/op/up/up.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[183]='listwise/liblistwise/op/uu/uu.final.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/uu/uu.c -o listwise/liblistwise/op/uu/uu.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[184]='listwise/liblistwise/op/v/v.final.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/v/v.c -o listwise/liblistwise/op/v/v.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[185]='listwise/liblistwise/op/vf/vf.final.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vf/vf.c -o listwise/liblistwise/op/vf/vf.final.pic.o


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[186]='listwise/liblistwise/op/vp/vp.final.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vp/vp.c -o listwise/liblistwise/op/vp/vp.final.pic.o


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[187]='listwise/liblistwise/op/w/w.final.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/w/w.c -o listwise/liblistwise/op/w/w.final.pic.o


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[188]='listwise/liblistwise/op/wvf/wvf.final.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvf/wvf.c -o listwise/liblistwise/op/wvf/wvf.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[189]='listwise/liblistwise/op/wvp/wvp.final.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvp/wvp.c -o listwise/liblistwise/op/wvp/wvp.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[190]='listwise/liblistwise/op/x/x.final.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/x/x.c -o listwise/liblistwise/op/x/x.final.pic.o


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[191]='listwise/liblistwise/op/xch/xch.final.pic.o'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/xch/xch.c -o listwise/liblistwise/op/xch/xch.final.pic.o


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[192]='listwise/liblistwise/op/y/y.final.pic.o'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/y/y.c -o listwise/liblistwise/op/y/y.final.pic.o


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[193]='listwise/liblistwise/op/z/z.final.pic.o'
fml_8_50()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/z/z.c -o listwise/liblistwise/op/z/z.final.pic.o


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[194]='listwise/listwise/listwise.final'
fml_8_51()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./listwise/listwise/logs -I./listwise/listwise/error -I./listwise/listwise/man -I./listwise/listwise/args -I./listwise/listwise -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -o listwise/listwise/listwise.final listwise/listwise/error/LISTWISE.errtab.final.o common/wstdlib/wstdlib.final.o common/strutil/strutil.final.o common/logger/logger.final.o listwise/listwise/logs/logs.final.o common/parseint/parseint.final.o listwise/listwise/args/args.final.o listwise/listwise/main.final.o ./libxapi/libxapi.final.so ./libxlinux/libxlinux.final.so ./libpstring/libpstring.final.so ./listwise/liblistwise/liblistwise.final.so


  X=$?
  echo 6 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[195]='fab/fablw/op/fg/fg.final.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.final.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[196]='fab/fablw/op/fi/fi.final.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.final.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[197]='listwise/liblistwise/op/-X/-X.final.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/-X/-X.final.so listwise/liblistwise/op/-X/-X.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[198]='listwise/liblistwise/op/C/C.final.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/C/C.final.so listwise/liblistwise/op/C/C.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[199]='listwise/liblistwise/op/b/b.final.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/b/b.final.so listwise/liblistwise/op/b/b.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[200]='listwise/liblistwise/op/bn/bn.final.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/bn/bn.final.so listwise/liblistwise/op/bn/bn.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[201]='listwise/liblistwise/op/c/c.final.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/c/c.final.so listwise/liblistwise/op/c/c.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[202]='listwise/liblistwise/op/can/can.final.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/can/can.final.so common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o common/canon/canon.final.pic.o listwise/liblistwise/op/can/can.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[203]='listwise/liblistwise/op/cons/cons.final.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cons/cons.final.so common/parseint/parseint.final.pic.o listwise/liblistwise/op/cons/cons.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[204]='listwise/liblistwise/op/cp/cp.final.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cp/cp.final.so listwise/liblistwise/op/cp/cp.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[205]='listwise/liblistwise/op/d/d.final.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/d/d.final.so listwise/liblistwise/op/d/d.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[206]='listwise/liblistwise/op/datef/datef.final.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/datef/datef.final.so common/parseint/parseint.final.pic.o listwise/liblistwise/op/datef/datef.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[207]='listwise/liblistwise/op/dj/dj.final.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dj/dj.final.so listwise/liblistwise/op/dj/dj.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[208]='listwise/liblistwise/op/dn/dn.final.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dn/dn.final.so listwise/liblistwise/op/dn/dn.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[209]='listwise/liblistwise/op/f/f.final.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/f/f.final.so listwise/liblistwise/op/f/f.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[210]='listwise/liblistwise/op/fn/fn.final.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/fn/fn.final.so listwise/liblistwise/op/fn/fn.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[211]='listwise/liblistwise/op/g/g.final.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/g/g.final.so listwise/liblistwise/op/g/g.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[212]='listwise/liblistwise/op/hl/hl.final.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/hl/hl.final.so common/color/color.final.pic.o listwise/liblistwise/op/hl/hl.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[213]='listwise/liblistwise/op/j/j.final.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/j/j.final.so listwise/liblistwise/op/j/j.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[214]='listwise/liblistwise/op/l/l.final.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/l/l.final.so common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o listwise/liblistwise/op/l/l.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[215]='listwise/liblistwise/op/ln/ln.final.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/ln/ln.final.so listwise/liblistwise/op/ln/ln.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[216]='listwise/liblistwise/op/ls/ls.final.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/ls/ls.final.so common/parseint/parseint.final.pic.o listwise/liblistwise/op/ls/ls.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[217]='listwise/liblistwise/op/lx/lx.final.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/lx/lx.final.so listwise/liblistwise/op/lx/lx.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[218]='listwise/liblistwise/op/o/o.final.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/o/o.final.so listwise/liblistwise/op/o/o.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[219]='listwise/liblistwise/op/p/p.final.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/p/p.final.so listwise/liblistwise/op/p/p.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[220]='listwise/liblistwise/op/pop/pop.final.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/pop/pop.final.so listwise/liblistwise/op/pop/pop.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[221]='listwise/liblistwise/op/r/r.final.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/r/r.final.so listwise/liblistwise/op/r/r.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[222]='listwise/liblistwise/op/rb/rb.final.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rb/rb.final.so common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o common/canon/canon.final.pic.o listwise/liblistwise/op/rb/rb.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[223]='listwise/liblistwise/op/re/re.final.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/re/re.final.so listwise/liblistwise/op/re/re.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[224]='listwise/liblistwise/op/rp/rp.final.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rp/rp.final.so listwise/liblistwise/op/rp/rp.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[225]='listwise/liblistwise/op/rx/rx.final.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rx/rx.final.so listwise/liblistwise/op/rx/rx.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[226]='listwise/liblistwise/op/s/s.final.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/s/s.final.so common/parseint/parseint.final.pic.o listwise/liblistwise/op/s/s.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[227]='listwise/liblistwise/op/shift/shift.final.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/shift/shift.final.so listwise/liblistwise/op/shift/shift.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[228]='listwise/liblistwise/op/sort/sort.final.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sort/sort.final.so common/parseint/parseint.final.pic.o common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o listwise/liblistwise/op/sort/sort.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[229]='listwise/liblistwise/op/stat/stat.final.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/stat/stat.final.so listwise/liblistwise/op/stat/stat.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[230]='listwise/liblistwise/op/sx/sx.final.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sx/sx.final.so listwise/liblistwise/op/sx/sx.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[231]='listwise/liblistwise/op/t/t.final.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/t/t.final.so listwise/liblistwise/op/t/t.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[232]='listwise/liblistwise/op/u/u.final.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/u/u.final.so common/parseint/parseint.final.pic.o common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o listwise/liblistwise/op/u/u.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[233]='listwise/liblistwise/op/up/up.final.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/up/up.final.so listwise/liblistwise/op/up/up.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[234]='listwise/liblistwise/op/uu/uu.final.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/uu/uu.final.so common/parseint/parseint.final.pic.o common/wstdlib/wstdlib.final.pic.o common/strutil/strutil.final.pic.o listwise/liblistwise/op/uu/uu.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[235]='listwise/liblistwise/op/v/v.final.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/v/v.final.so listwise/liblistwise/op/v/v.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}

NAMES[236]='listwise/liblistwise/op/vf/vf.final.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&105

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vf/vf.final.so listwise/liblistwise/op/vf/vf.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 5 1>&99
  exit $X
}

NAMES[237]='listwise/liblistwise/op/vp/vp.final.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&106

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vp/vp.final.so listwise/liblistwise/op/vp/vp.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 6 1>&99
  exit $X
}

NAMES[238]='listwise/liblistwise/op/w/w.final.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&107

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/w/w.final.so listwise/liblistwise/op/w/w.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 7 1>&99
  exit $X
}

NAMES[239]='listwise/liblistwise/op/wvf/wvf.final.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&108

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvf/wvf.final.so listwise/liblistwise/op/wvf/wvf.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 8 1>&99
  exit $X
}

NAMES[240]='listwise/liblistwise/op/wvp/wvp.final.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvp/wvp.final.so listwise/liblistwise/op/wvp/wvp.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[241]='listwise/liblistwise/op/x/x.final.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/x/x.final.so listwise/liblistwise/op/x/x.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[242]='listwise/liblistwise/op/xch/xch.final.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&102

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/xch/xch.final.so listwise/liblistwise/op/xch/xch.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 2 1>&99
  exit $X
}

NAMES[243]='listwise/liblistwise/op/y/y.final.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&103

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/y/y.final.so listwise/liblistwise/op/y/y.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 3 1>&99
  exit $X
}

NAMES[244]='listwise/liblistwise/op/z/z.final.so'
fml_9_49()
{
  exec 1>/dev/null
  exec 2>&104

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fPIC -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -I./listwise/liblistwise/object -I./listwise/liblistwise/transform -I./listwise/liblistwise/internal -I./listwise/liblistwise/operators -I./listwise/liblistwise/error -I./listwise/liblistwise/tune -I./listwise/liblistwise/operator -I./listwise/liblistwise/fs -I./listwise/liblistwise/lwx -I./listwise/liblistwise/describe -I./listwise/liblistwise/exec -I./listwise/liblistwise/re -I./listwise/liblistwise/lstack -I./listwise/liblistwise/op -I./listwise/liblistwise/test -I./listwise/liblistwise/sanity -I./listwise/liblistwise/listwise -I./listwise/liblistwise/selection -I./listwise/liblistwise/window -I./listwise/liblistwise/logging -I./listwise/liblistwise/iterate -I./listwise/liblistwise -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/z/z.final.so listwise/liblistwise/op/z/z.final.pic.o ./listwise/liblistwise/liblistwise.final.so -shared


  X=$?
  echo 4 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[245]='fab/fablw/op/fg/fg.final.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fablw/op/fg/fg.final.so	fab/fablw/op/fg/fg.final.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[246]='fab/fablw/op/fi/fi.final.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fms-extensions -DXAPI_PROVIDE_BACKTRACE -I./common/cksum -I./common/wstdlib -I./common/parseint -I./common/memblk -I./common/logger -I./common/map -I./common/yyutil -I./common/canon -I./common/color -I./common/wstring -I./common/strutil -I./common/unitstring -I./common/coll -I./common -DBUILDSTAMP="`date`" -O3 -fno-builtin-log -I./fab/fabd/dsc -I./fab/fabd/selector -I./fab/fabd/ts -I./fab/fabd/ff -I./fab/fabd/tmp -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/ffn -I./fab/fabd/bp -I./fab/fabd/traverse -I./fab/fabd/enclose -I./fab/fabd/dep -I./fab/fabd/var -I./fab/fabd/strstack -I./fab/fabd/ffproc -I./fab/fabd/gn -I./fab/fabd/hashblock -I./fab/fabd/list -I./fab/fabd/fml -I./fab/common/dirutil -I./fab/common/logs -I./fab/common/error -I./fab/common/path -I./fab/common/params -I./fab/common/args -I./fab/common/identity -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.5.3.0 -DFABVERSIONN=530 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fablw/op/fi/fi.final.so	fab/fablw/op/fi/fi.final.pic.o -shared


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
  ((SKP+=1))
else
  # launch stage 0.0
  exec 100>$tmp ; rm -f $tmp ; fml_0_0 & PIDS[0]=$!

  # harvest stage 0.0
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((0+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 0 $((idx+0)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
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

  # harvest stage 1.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((1+$idx))
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
  ((SKP+=9))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_17 & PIDS[8]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((10+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.2
  exec 100>$tmp ; rm -f $tmp ; fml_1_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[8]=$!

  # harvest stage 1.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((19+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 1.3
  exec 100>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[8]=$!

  # harvest stage 1.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((28+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=3))
else
  # launch stage 1.4
  exec 100>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[2]=$!

  # harvest stage 1.4
  C=0
  while [[ $C != 3 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((37+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
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

  # harvest stage 2.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((40+$idx))
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
  ((SKP+=9))
else
  # launch stage 2.1
  exec 100>$tmp ; rm -f $tmp ; fml_2_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_17 & PIDS[8]=$!

  # harvest stage 2.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((49+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 2.2
  exec 100>$tmp ; rm -f $tmp ; fml_2_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_26 & PIDS[8]=$!

  # harvest stage 2.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((58+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 2.3
  exec 100>$tmp ; rm -f $tmp ; fml_2_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_35 & PIDS[8]=$!

  # harvest stage 2.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((67+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 2.4
  exec 100>$tmp ; rm -f $tmp ; fml_2_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_2_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_2_39 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_2_40 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_2_41 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_2_42 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_2_43 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_2_44 & PIDS[8]=$!

  # harvest stage 2.4
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((76+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
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

  # harvest stage 3.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((85+$idx))
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
  ((SKP+=9))
else
  # launch stage 3.1
  exec 100>$tmp ; rm -f $tmp ; fml_3_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_17 & PIDS[8]=$!

  # harvest stage 3.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((94+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 3.2
  exec 100>$tmp ; rm -f $tmp ; fml_3_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_26 & PIDS[8]=$!

  # harvest stage 3.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((103+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 3.3
  exec 100>$tmp ; rm -f $tmp ; fml_3_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_35 & PIDS[8]=$!

  # harvest stage 3.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((112+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 3.4
  exec 100>$tmp ; rm -f $tmp ; fml_3_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_39 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_40 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_41 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_42 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_43 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_44 & PIDS[8]=$!

  # harvest stage 3.4
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((121+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 3.5
  exec 100>$tmp ; rm -f $tmp ; fml_3_45 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_46 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_3_47 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_3_48 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_3_49 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_3_50 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_3_51 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_3_52 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_3_53 & PIDS[8]=$!

  # harvest stage 3.5
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((130+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+45)) $X "$N"
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
    I=$((139+$idx))
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
    I=$((140+$idx))
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
    I=$((141+$idx))
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
    I=$((142+$idx))
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
  ((SKP+=9))
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

  # harvest stage 8.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((143+$idx))
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
  ((SKP+=9))
else
  # launch stage 8.1
  exec 100>$tmp ; rm -f $tmp ; fml_8_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_17 & PIDS[8]=$!

  # harvest stage 8.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((152+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 8.2
  exec 100>$tmp ; rm -f $tmp ; fml_8_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_26 & PIDS[8]=$!

  # harvest stage 8.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((161+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 8.3
  exec 100>$tmp ; rm -f $tmp ; fml_8_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_35 & PIDS[8]=$!

  # harvest stage 8.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((170+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 8.4
  exec 100>$tmp ; rm -f $tmp ; fml_8_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_39 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_40 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_41 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_42 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_8_43 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_8_44 & PIDS[8]=$!

  # harvest stage 8.4
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((179+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=7))
else
  # launch stage 8.5
  exec 100>$tmp ; rm -f $tmp ; fml_8_45 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_46 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_8_47 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_8_48 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_8_49 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_8_50 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_8_51 & PIDS[6]=$!

  # harvest stage 8.5
  C=0
  while [[ $C != 7 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((188+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+45)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
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

  # harvest stage 9.0
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((195+$idx))
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
  ((SKP+=9))
else
  # launch stage 9.1
  exec 100>$tmp ; rm -f $tmp ; fml_9_9 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_10 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_11 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_12 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_13 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_14 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_15 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_16 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_17 & PIDS[8]=$!

  # harvest stage 9.1
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((204+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+9)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 9.2
  exec 100>$tmp ; rm -f $tmp ; fml_9_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_19 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_20 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_21 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_22 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_23 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_24 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_25 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_26 & PIDS[8]=$!

  # harvest stage 9.2
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((213+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+18)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 9.3
  exec 100>$tmp ; rm -f $tmp ; fml_9_27 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_28 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_29 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_30 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_31 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_32 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_33 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_34 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_35 & PIDS[8]=$!

  # harvest stage 9.3
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((222+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+27)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=9))
else
  # launch stage 9.4
  exec 100>$tmp ; rm -f $tmp ; fml_9_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_37 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_38 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_39 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_40 & PIDS[4]=$!
  exec 105>$tmp ; rm -f $tmp ; fml_9_41 & PIDS[5]=$!
  exec 106>$tmp ; rm -f $tmp ; fml_9_42 & PIDS[6]=$!
  exec 107>$tmp ; rm -f $tmp ; fml_9_43 & PIDS[7]=$!
  exec 108>$tmp ; rm -f $tmp ; fml_9_44 & PIDS[8]=$!

  # harvest stage 9.4
  C=0
  while [[ $C != 9 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((231+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=5))
else
  # launch stage 9.5
  exec 100>$tmp ; rm -f $tmp ; fml_9_45 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_46 & PIDS[1]=$!
  exec 102>$tmp ; rm -f $tmp ; fml_9_47 & PIDS[2]=$!
  exec 103>$tmp ; rm -f $tmp ; fml_9_48 & PIDS[3]=$!
  exec 104>$tmp ; rm -f $tmp ; fml_9_49 & PIDS[4]=$!

  # harvest stage 9.5
  C=0
  while [[ $C != 5 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((240+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+45)) $X "$N"
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
    I=$((245+$idx))
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
