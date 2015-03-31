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

# fab --bslic-fab -k make64.debug.sh @debug

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
NAMES[1]='common/canon/canon.eapi.debug.pic.o'
fml_1_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[2]='common/canon/canon.xapi.debug.o'
fml_1_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/canon/canon.c -o common/canon/canon.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[3]='common/canon/canon.xapi.debug.pic.o'
fml_1_2()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/canon/canon.c -o common/canon/canon.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[4]='common/cksum/cksum.xapi.debug.o'
fml_1_3()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/cksum/cksum.c -o common/cksum/cksum.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[5]='common/coll/coll.eapi.debug.pic.o'
fml_1_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[6]='common/coll/coll.xapi.debug.o'
fml_1_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/coll/coll.c -o common/coll/coll.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[7]='common/coll/coll.xapi.debug.pic.o'
fml_1_6()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/coll/coll.c -o common/coll/coll.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[8]='common/color/color.eapi.debug.pic.o'
fml_1_7()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[9]='common/color/color.xapi.debug.pic.o'
fml_1_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/color/color.c -o common/color/color.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[10]='common/logger/logger.eapi.debug.o'
fml_1_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[11]='common/logger/logger.xapi.debug.o'
fml_1_10()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/logger/logger.c -o common/logger/logger.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[12]='common/map/map.eapi.debug.pic.o'
fml_1_11()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[13]='common/map/map.xapi.debug.o'
fml_1_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/map/map.c -o common/map/map.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[14]='common/map/map.xapi.debug.pic.o'
fml_1_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/map/map.c -o common/map/map.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[15]='common/memblk/memblk.def.xapi.debug.o'
fml_1_14()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/memblk/memblk.def.c -o common/memblk/memblk.def.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[16]='common/memblk/memblk.def.xapi.debug.pic.o'
fml_1_15()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/memblk/memblk.def.c -o common/memblk/memblk.def.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[17]='common/parseint/parseint.eapi.debug.o'
fml_1_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.eapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[18]='common/parseint/parseint.eapi.debug.pic.o'
fml_1_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[19]='common/parseint/parseint.xapi.debug.o'
fml_1_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/parseint/parseint.c -o common/parseint/parseint.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[20]='common/parseint/parseint.xapi.debug.pic.o'
fml_1_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/parseint/parseint.c -o common/parseint/parseint.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[21]='common/strutil/strutil.eapi.debug.o'
fml_1_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.eapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[22]='common/strutil/strutil.eapi.debug.pic.o'
fml_1_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[23]='common/strutil/strutil.xapi.debug.o'
fml_1_22()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/strutil/strutil.c -o common/strutil/strutil.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[24]='common/strutil/strutil.xapi.debug.pic.o'
fml_1_23()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/strutil/strutil.c -o common/strutil/strutil.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[25]='common/unitstring/unitstring.xapi.debug.o'
fml_1_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/unitstring/unitstring.c -o common/unitstring/unitstring.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[26]='common/wstdlib/wstdlib.eapi.debug.o'
fml_1_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[27]='common/wstdlib/wstdlib.eapi.debug.pic.o'
fml_1_26()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[28]='common/wstdlib/wstdlib.xapi.debug.o'
fml_1_27()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[29]='common/wstdlib/wstdlib.xapi.debug.pic.o'
fml_1_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/wstdlib/wstdlib.c -o common/wstdlib/wstdlib.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[30]='common/yyutil/yyutil.eapi.debug.pic.o'
fml_1_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[31]='common/yyutil/yyutil.xapi.debug.o'
fml_1_30()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/yyutil/yyutil.c -o common/yyutil/yyutil.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[32]='common/yyutil/yyutil.xapi.debug.pic.o'
fml_1_31()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -fPIC -c common/yyutil/yyutil.c -o common/yyutil/yyutil.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[33]='fab/common/dirutil/dirutil.xapi.debug.o'
fml_1_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/dirutil/dirutil.c -o fab/common/dirutil/dirutil.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[34]='{fab/common/error/FAB.errtab.h, fab/common/error/FAB.errtab.c}'
fml_1_33()
{
  exec 1>/dev/null
  exec 2>&101

  
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
echo "#define _E(a, b, c) [ a + (ERRMIN_${pfx} * -1) ] = { .name = #b, .desc = c },"
echo "ETABLE_${pfx}"
echo "#undef _E"
echo "}};"
echo "etable * ${api} perrtab_${pfx} = &errtab_${pfx};"


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[35]='fab/common/logs/logs.xapi.debug.o'
fml_1_34()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/logs/logs.c -o fab/common/logs/logs.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[36]='{fab/fabd/ff/ff.dsc.tab.h, fab/fabd/ff/ff.dsc.tab.c}'
fml_1_35()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o fab/fabd/ff/ff.dsc.tab.c -d	fab/fabd/ff/ff.dsc.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[37]='{fab/fabd/ff/ff.lex.h, fab/fabd/ff/ff.lex.c}'
fml_1_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	  flex  -o fab/fabd/ff/ff.lex.c  --header-file=fab/fabd/ff/ff.lex.h  fab/fabd/ff/ff.l


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[38]='{fab/fabd/ff/ff.list.tab.h, fab/fabd/ff/ff.list.tab.c}'
fml_1_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o fab/fabd/ff/ff.list.tab.c -d	fab/fabd/ff/ff.list.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[39]='{fab/fabd/ff/ff.tab.h, fab/fabd/ff/ff.tab.c}'
fml_1_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	bison --warnings=error -o fab/fabd/ff/ff.tab.c -d	fab/fabd/ff/ff.y


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[40]='{fab/fabd/ff/ff.var.tab.h, fab/fabd/ff/ff.var.tab.c}'
fml_1_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o fab/fabd/ff/ff.var.tab.c -d	fab/fabd/ff/ff.var.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[41]='libpstring/exports'
fml_1_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- libpstring/pstring/pstring.c libpstring/write/write.c )
echo "local: *; };"
	  ) 1>libpstring/exports


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[42]='libpstring/pstring/pstring.eapi.debug.pic.o'
fml_1_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[43]='libpstring/pstring/pstring.xapi.debug.pic.o'
fml_1_42()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -c libpstring/pstring/pstring.c -o libpstring/pstring/pstring.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[44]='libxapi/error/SYS.errtab'
fml_1_43()
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

NAMES[45]='{libxapi/error/XAPI.errtab.h, libxapi/error/XAPI.errtab.c}'
fml_1_44()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[46]='{libxlinux/error/XLINUX.errtab.h, libxlinux/error/XLINUX.errtab.c}'
fml_1_45()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[47]='{listwise/liblistwise/error/LW.errtab.h, listwise/liblistwise/error/LW.errtab.c}'
fml_1_46()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[48]='{listwise/liblistwise/error/PCRE.errtab.h, listwise/liblistwise/error/PCRE.errtab.c}'
fml_1_47()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[49]='{listwise/liblistwise/transform/transform.lex.h, listwise/liblistwise/transform/transform.lex.c}'
fml_1_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	  flex  -o listwise/liblistwise/transform/transform.lex.c  --header-file=listwise/liblistwise/transform/transform.lex.h  listwise/liblistwise/transform/transform.l


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[50]='{listwise/liblistwise/transform/transform.tab.h, listwise/liblistwise/transform/transform.tab.c}'
fml_1_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	bison --warnings=error -o listwise/liblistwise/transform/transform.tab.c -d	listwise/liblistwise/transform/transform.y


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[51]='{listwise/listwise/error/LISTWISE.errtab.h, listwise/listwise/error/LISTWISE.errtab.c}'
fml_1_50()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[52]='listwise/listwise/logs/logs.eapi.debug.o'
fml_1_51()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/logs/logs.c -o listwise/listwise/logs/logs.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[53]='listwise/listwise/logs/logs.xapi.debug.o'
fml_1_52()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/logs/logs.c -o listwise/listwise/logs/logs.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 2
NAMES[54]='fab/common/error/FAB.errtab.xapi.debug.o'
fml_2_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/error/FAB.errtab.c -o fab/common/error/FAB.errtab.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[55]='fab/common/params/params.xapi.debug.o'
fml_2_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/params/params.c -o fab/common/params/params.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[56]='fab/fabd/ff/ff.dsc.tab.xapi.debug.o'
fml_2_2()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.dsc.tab.c -o fab/fabd/ff/ff.dsc.tab.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[57]='fab/fabd/ff/ff.list.tab.xapi.debug.o'
fml_2_3()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.list.tab.c -o fab/fabd/ff/ff.list.tab.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[58]='{fab/fabd/ff/ff.states.h, fab/fabd/ff/ff.states.c}'
fml_2_4()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[59]='fab/fabd/ff/ff.tab.xapi.debug.o'
fml_2_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.tab.c -o fab/fabd/ff/ff.tab.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[60]='{fab/fabd/ff/ff.tokens.h, fab/fabd/ff/ff.tokens.c}'
fml_2_6()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[61]='fab/fabd/ff/ff.var.tab.xapi.debug.o'
fml_2_7()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.var.tab.c -o fab/fabd/ff/ff.var.tab.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[62]='fab/fabw/main.xapi.debug.o'
fml_2_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/fabw -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabw/main.c -o fab/fabw/main.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[63]='{libxapi/error/SYS.errtab.h, libxapi/error/SYS.errtab.c}'
fml_2_9()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[64]='libxapi/error/XAPI.errtab.eapi.debug.pic.o'
fml_2_10()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[65]='libxapi/error/XAPI.errtab.xapi.debug.pic.o'
fml_2_11()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/XAPI.errtab.c -o libxapi/error/XAPI.errtab.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[66]='libxlinux/error/XLINUX.errtab.eapi.debug.pic.o'
fml_2_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[67]='libxlinux/error/XLINUX.errtab.xapi.debug.pic.o'
fml_2_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/error/XLINUX.errtab.c -o libxlinux/error/XLINUX.errtab.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[68]='libxlinux/error/error.eapi.debug.pic.o'
fml_2_14()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/error/error.c -o libxlinux/error/error.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[69]='libxlinux/error/error.xapi.debug.pic.o'
fml_2_15()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/error/error.c -o libxlinux/error/error.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[70]='libxlinux/exports'
fml_2_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
perl -ne 'if(/API(?:DATA)?.*? ([a-zA-Z_][a-zA-Z0-9_]+) *(?:\(|=)/) { print "$1;\n" }' <(\
	  cat -- libxlinux/xftw/xftw.c libxlinux/xpwd/xpwd.c libxlinux/xdlfcn/xdlfcn.c libxlinux/xgrp/xgrp.c libxlinux/xsignal/xsignal.c libxlinux/xstring/xstring.c libxlinux/xfcntl/xfcntl.c libxlinux/xdirent/xdirent.c libxlinux/xstdlib/xstdlib.c libxlinux/xwait/xwait.c libxlinux/xtime/xtime.c libxlinux/xprctl/xprctl.c libxlinux/xmman/xmman.c libxlinux/xfile/xfile.c libxlinux/xstat/xstat.c libxlinux/error/XLINUX.errtab.c libxlinux/error/error.c libxlinux/mempolicy/mempolicy.c libxlinux/xuio/xuio.c libxlinux/xunistd/xunistd.c )
echo "local: *; };"
	  ) 1>libxlinux/exports


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[71]='libxlinux/mempolicy/mempolicy.eapi.debug.pic.o'
fml_2_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/mempolicy/mempolicy.c -o libxlinux/mempolicy/mempolicy.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[72]='libxlinux/mempolicy/mempolicy.xapi.debug.pic.o'
fml_2_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/mempolicy/mempolicy.c -o libxlinux/mempolicy/mempolicy.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[73]='libxlinux/xdirent/xdirent.eapi.debug.pic.o'
fml_2_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[74]='libxlinux/xdirent/xdirent.xapi.debug.pic.o'
fml_2_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xdirent/xdirent.c -o libxlinux/xdirent/xdirent.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[75]='libxlinux/xdlfcn/xdlfcn.eapi.debug.pic.o'
fml_2_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[76]='libxlinux/xdlfcn/xdlfcn.xapi.debug.pic.o'
fml_2_22()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xdlfcn/xdlfcn.c -o libxlinux/xdlfcn/xdlfcn.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[77]='libxlinux/xfcntl/xfcntl.eapi.debug.pic.o'
fml_2_23()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[78]='libxlinux/xfcntl/xfcntl.xapi.debug.pic.o'
fml_2_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xfcntl/xfcntl.c -o libxlinux/xfcntl/xfcntl.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[79]='libxlinux/xfile/xfile.eapi.debug.pic.o'
fml_2_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xfile/xfile.c -o libxlinux/xfile/xfile.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[80]='libxlinux/xfile/xfile.xapi.debug.pic.o'
fml_2_26()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xfile/xfile.c -o libxlinux/xfile/xfile.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[81]='libxlinux/xftw/xftw.eapi.debug.pic.o'
fml_2_27()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[82]='libxlinux/xftw/xftw.xapi.debug.pic.o'
fml_2_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xftw/xftw.c -o libxlinux/xftw/xftw.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[83]='libxlinux/xgrp/xgrp.eapi.debug.pic.o'
fml_2_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[84]='libxlinux/xgrp/xgrp.xapi.debug.pic.o'
fml_2_30()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xgrp/xgrp.c -o libxlinux/xgrp/xgrp.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[85]='libxlinux/xmman/xmman.eapi.debug.pic.o'
fml_2_31()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[86]='libxlinux/xmman/xmman.xapi.debug.pic.o'
fml_2_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xmman/xmman.c -o libxlinux/xmman/xmman.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[87]='libxlinux/xprctl/xprctl.eapi.debug.pic.o'
fml_2_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xprctl/xprctl.c -o libxlinux/xprctl/xprctl.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[88]='libxlinux/xprctl/xprctl.xapi.debug.pic.o'
fml_2_34()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xprctl/xprctl.c -o libxlinux/xprctl/xprctl.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[89]='libxlinux/xpwd/xpwd.eapi.debug.pic.o'
fml_2_35()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[90]='libxlinux/xpwd/xpwd.xapi.debug.pic.o'
fml_2_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xpwd/xpwd.c -o libxlinux/xpwd/xpwd.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[91]='libxlinux/xsignal/xsignal.eapi.debug.pic.o'
fml_2_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xsignal/xsignal.c -o libxlinux/xsignal/xsignal.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[92]='libxlinux/xsignal/xsignal.xapi.debug.pic.o'
fml_2_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xsignal/xsignal.c -o libxlinux/xsignal/xsignal.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[93]='libxlinux/xstat/xstat.eapi.debug.pic.o'
fml_2_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[94]='libxlinux/xstat/xstat.xapi.debug.pic.o'
fml_2_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstat/xstat.c -o libxlinux/xstat/xstat.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[95]='libxlinux/xstdlib/xstdlib.eapi.debug.pic.o'
fml_2_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[96]='libxlinux/xstdlib/xstdlib.xapi.debug.pic.o'
fml_2_42()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstdlib/xstdlib.c -o libxlinux/xstdlib/xstdlib.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[97]='libxlinux/xstring/xstring.eapi.debug.pic.o'
fml_2_43()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[98]='libxlinux/xstring/xstring.xapi.debug.pic.o'
fml_2_44()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xstring/xstring.c -o libxlinux/xstring/xstring.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[99]='libxlinux/xtime/xtime.eapi.debug.pic.o'
fml_2_45()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[100]='libxlinux/xtime/xtime.xapi.debug.pic.o'
fml_2_46()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xtime/xtime.c -o libxlinux/xtime/xtime.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[101]='libxlinux/xuio/xuio.eapi.debug.pic.o'
fml_2_47()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xuio/xuio.c -o libxlinux/xuio/xuio.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[102]='libxlinux/xuio/xuio.xapi.debug.pic.o'
fml_2_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xuio/xuio.c -o libxlinux/xuio/xuio.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[103]='libxlinux/xunistd/xunistd.eapi.debug.pic.o'
fml_2_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[104]='libxlinux/xunistd/xunistd.xapi.debug.pic.o'
fml_2_50()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xunistd/xunistd.c -o libxlinux/xunistd/xunistd.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[105]='libxlinux/xwait/xwait.eapi.debug.pic.o'
fml_2_51()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xwait/xwait.c -o libxlinux/xwait/xwait.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[106]='libxlinux/xwait/xwait.xapi.debug.pic.o'
fml_2_52()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -c libxlinux/xwait/xwait.c -o libxlinux/xwait/xwait.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[107]='listwise/liblistwise/error/LW.errtab.eapi.debug.pic.o'
fml_2_53()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/LW.errtab.c -o listwise/liblistwise/error/LW.errtab.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[108]='listwise/liblistwise/error/LW.errtab.xapi.debug.pic.o'
fml_2_54()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/LW.errtab.c -o listwise/liblistwise/error/LW.errtab.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[109]='listwise/liblistwise/error/PCRE.errtab.eapi.debug.pic.o'
fml_2_55()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/PCRE.errtab.c -o listwise/liblistwise/error/PCRE.errtab.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[110]='listwise/liblistwise/error/PCRE.errtab.xapi.debug.pic.o'
fml_2_56()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/PCRE.errtab.c -o listwise/liblistwise/error/PCRE.errtab.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[111]='{listwise/liblistwise/transform/transform.states.h, listwise/liblistwise/transform/transform.states.c}'
fml_2_57()
{
  exec 1>/dev/null
  exec 2>&101

  
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
  echo 1 1>&99
  exit $X
}

NAMES[112]='{listwise/liblistwise/transform/transform.tokens.h, listwise/liblistwise/transform/transform.tokens.c}'
fml_2_58()
{
  exec 1>/dev/null
  exec 2>&100

  
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
  echo 0 1>&99
  exit $X
}

NAMES[113]='listwise/listwise/args/args.eapi.debug.o'
fml_2_59()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/args/args.c -o listwise/listwise/args/args.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[114]='listwise/listwise/args/args.xapi.debug.o'
fml_2_60()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/args/args.c -o listwise/listwise/args/args.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[115]='listwise/listwise/error/LISTWISE.errtab.eapi.debug.o'
fml_2_61()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/error/LISTWISE.errtab.c -o listwise/listwise/error/LISTWISE.errtab.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[116]='listwise/listwise/error/LISTWISE.errtab.xapi.debug.o'
fml_2_62()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/error/LISTWISE.errtab.c -o listwise/listwise/error/LISTWISE.errtab.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[117]='listwise/listwise/main.eapi.debug.o'
fml_2_63()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/main.c -o listwise/listwise/main.eapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[118]='listwise/listwise/main.xapi.debug.o'
fml_2_64()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -Ilistwise/listwise/man -Ilistwise/listwise/args -Ilistwise/listwise/logs -Ilistwise/listwise/error -Ilistwise/listwise -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -c listwise/listwise/main.c -o listwise/listwise/main.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 3
NAMES[119]='common/memblk/memblk.xapi.debug.o'
fml_3_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -c common/memblk/memblk.c -o common/memblk/memblk.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[120]='fab/common/args/args.xapi.debug.o'
fml_3_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/args/args.c -o fab/common/args/args.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[121]='fab/common/identity/identity.xapi.debug.o'
fml_3_2()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/identity/identity.c -o fab/common/identity/identity.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[122]='fab/common/path/path.xapi.debug.o'
fml_3_3()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/common/path/path.c -o fab/common/path/path.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[123]='fab/fab/main.xapi.debug.o'
fml_3_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fab -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fab/main.c -o fab/fab/main.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[124]='fab/fabd/bp/bp.xapi.debug.o'
fml_3_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/bp/bp.c -o fab/fabd/bp/bp.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[125]='fab/fabd/bs/bs.xapi.debug.o'
fml_3_6()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/bs/bs.c -o fab/fabd/bs/bs.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[126]='fab/fabd/dep/dep.xapi.debug.o'
fml_3_7()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/dep/dep.c -o fab/fabd/dep/dep.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[127]='fab/fabd/dsc/dsc.xapi.debug.o'
fml_3_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/dsc/dsc.c -o fab/fabd/dsc/dsc.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[128]='fab/fabd/enclose/enclose.xapi.debug.o'
fml_3_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/enclose/enclose.c -o fab/fabd/enclose/enclose.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[129]='fab/fabd/ff/ff.lex.xapi.debug.o'
fml_3_10()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.lex.c -o fab/fabd/ff/ff.lex.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[130]='fab/fabd/ff/ff.states.xapi.debug.o'
fml_3_11()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.states.c -o fab/fabd/ff/ff.states.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[131]='fab/fabd/ff/ff.tokens.xapi.debug.o'
fml_3_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.tokens.c -o fab/fabd/ff/ff.tokens.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[132]='fab/fabd/ff/ff.xapi.debug.o'
fml_3_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ff/ff.c -o fab/fabd/ff/ff.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[133]='fab/fabd/ffn/ffn.xapi.debug.o'
fml_3_14()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ffn/ffn.c -o fab/fabd/ffn/ffn.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[134]='fab/fabd/ffproc/ffproc.xapi.debug.o'
fml_3_15()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ffproc/ffproc.c -o fab/fabd/ffproc/ffproc.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[135]='fab/fabd/fml/fml.xapi.debug.o'
fml_3_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/fml/fml.c -o fab/fabd/fml/fml.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[136]='fab/fabd/gn/gn.xapi.debug.o'
fml_3_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/gn/gn.c -o fab/fabd/gn/gn.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[137]='fab/fabd/gn/gnlw.xapi.debug.o'
fml_3_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/gn/gnlw.c -o fab/fabd/gn/gnlw.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[138]='fab/fabd/hashblock/hashblock.xapi.debug.o'
fml_3_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/hashblock/hashblock.c -o fab/fabd/hashblock/hashblock.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[139]='fab/fabd/list/list.xapi.debug.o'
fml_3_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/list/list.c -o fab/fabd/list/list.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[140]='fab/fabd/lwutil/lwutil.xapi.debug.o'
fml_3_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/lwutil/lwutil.c -o fab/fabd/lwutil/lwutil.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[141]='fab/fabd/main.xapi.debug.o'
fml_3_22()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/main.c -o fab/fabd/main.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[142]='fab/fabd/selector/selector.xapi.debug.o'
fml_3_23()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/selector/selector.c -o fab/fabd/selector/selector.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[143]='fab/fabd/strstack/strstack.xapi.debug.o'
fml_3_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/strstack/strstack.c -o fab/fabd/strstack/strstack.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[144]='fab/fabd/tmp/tmp.xapi.debug.o'
fml_3_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/tmp/tmp.c -o fab/fabd/tmp/tmp.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[145]='fab/fabd/traverse/traverse.xapi.debug.o'
fml_3_26()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/traverse/traverse.c -o fab/fabd/traverse/traverse.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[146]='fab/fabd/ts/ts.xapi.debug.o'
fml_3_27()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/ts/ts.c -o fab/fabd/ts/ts.xapi.debug.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[147]='fab/fabd/var/var.xapi.debug.o'
fml_3_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -c fab/fabd/var/var.c -o fab/fabd/var/var.xapi.debug.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[148]='libxapi/callstack/callstack.xapi.debug.pic.o'
fml_3_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/callstack/callstack.c -o libxapi/callstack/callstack.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[149]='libxapi/error/SYS.errtab.eapi.debug.pic.o'
fml_3_30()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[150]='libxapi/error/SYS.errtab.xapi.debug.pic.o'
fml_3_31()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/SYS.errtab.c -o libxapi/error/SYS.errtab.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[151]='libxapi/error/error.eapi.debug.pic.o'
fml_3_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/error.c -o libxapi/error/error.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[152]='libxapi/error/error.xapi.debug.pic.o'
fml_3_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/error/error.c -o libxapi/error/error.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[153]='libxapi/exports'
fml_3_34()
{
  exec 1>/dev/null
  exec 2>&100

  
	  (echo "{ global: "
perl -ne 'if(/API(?:DATA)?.*? ([a-zA-Z_][a-zA-Z0-9_]+) *(?:\(|=)/) { print "$1;\n" }' <(\
	  cat -- libxapi/callstack/callstack.c libxapi/frame/frame.c libxapi/test.c libxapi/trace/trace.c libxapi/error/XAPI.errtab.c libxapi/error/SYS.errtab.c libxapi/error/error.c )
	  echo "callstack;"
echo "local: *; };"
	) 1>libxapi/exports


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[154]='libxapi/frame/frame.xapi.debug.pic.o'
fml_3_35()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/frame/frame.c -o libxapi/frame/frame.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[155]='libxapi/trace/trace.xapi.debug.pic.o'
fml_3_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -c libxapi/trace/trace.c -o libxapi/trace/trace.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[156]='listwise/liblistwise/describe/describe.eapi.debug.pic.o'
fml_3_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/describe/describe.c -o listwise/liblistwise/describe/describe.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[157]='listwise/liblistwise/describe/describe.xapi.debug.pic.o'
fml_3_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/describe/describe.c -o listwise/liblistwise/describe/describe.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[158]='listwise/liblistwise/error/error.eapi.debug.pic.o'
fml_3_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/error.c -o listwise/liblistwise/error/error.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[159]='listwise/liblistwise/error/error.xapi.debug.pic.o'
fml_3_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/error/error.c -o listwise/liblistwise/error/error.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[160]='listwise/liblistwise/exec/exec.eapi.debug.pic.o'
fml_3_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/exec/exec.c -o listwise/liblistwise/exec/exec.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[161]='listwise/liblistwise/exec/exec.xapi.debug.pic.o'
fml_3_42()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/exec/exec.c -o listwise/liblistwise/exec/exec.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[162]='listwise/liblistwise/exports'
fml_3_43()
{
  exec 1>/dev/null
  exec 2>&101

  
	  (echo "{ global: "
sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' <(\
	  cat -- listwise/liblistwise/lwx/lwx.c listwise/liblistwise/object/object.c listwise/liblistwise/operators/operators.c listwise/liblistwise/operator/operator.c listwise/liblistwise/exec/exec.c listwise/liblistwise/describe/describe.c listwise/liblistwise/selection/selection.c listwise/liblistwise/tune/tune.c listwise/liblistwise/lstack/lstack.c listwise/liblistwise/sanity/sanity.c listwise/liblistwise/iterate/iterate.c listwise/liblistwise/logging/logging.c listwise/liblistwise/op/w/w.c listwise/liblistwise/op/rp/rp.c listwise/liblistwise/op/hl/hl.c listwise/liblistwise/op/cons/cons.c listwise/liblistwise/op/fn/fn.c listwise/liblistwise/op/p/p.c listwise/liblistwise/op/p/test.c listwise/liblistwise/op/v/v.c listwise/liblistwise/op/v/test.c listwise/liblistwise/op/rx/rx.c listwise/liblistwise/op/datef/datef.c listwise/liblistwise/op/dj/dj.c listwise/liblistwise/op/j/j.c listwise/liblistwise/op/uu/uu.c listwise/liblistwise/op/uu/test.c listwise/liblistwise/op/r/r.c listwise/liblistwise/op/wvp/wvp.c listwise/liblistwise/op/wvp/test.c listwise/liblistwise/op/vp/vp.c listwise/liblistwise/op/dn/dn.c listwise/liblistwise/op/can/can.c listwise/liblistwise/op/shift/shift.c listwise/liblistwise/op/z/z.c listwise/liblistwise/op/z/test.c listwise/liblistwise/op/lx/lx.c listwise/liblistwise/op/lx/test.c listwise/liblistwise/op/f/f.c listwise/liblistwise/op/f/test.c listwise/liblistwise/op/sort/test.c listwise/liblistwise/op/sort/sort.c listwise/liblistwise/op/-X/-X.c listwise/liblistwise/op/l/l.c listwise/liblistwise/op/l/test.c listwise/liblistwise/op/rb/rb.c listwise/liblistwise/op/bn/bn.c listwise/liblistwise/op/wvf/wvf.c listwise/liblistwise/op/wvf/test.c listwise/liblistwise/op/pop/pop.c listwise/liblistwise/op/y/y.c listwise/liblistwise/op/y/test.c listwise/liblistwise/op/up/up.c listwise/liblistwise/op/s/s.c listwise/liblistwise/op/s/test.c listwise/liblistwise/op/o/o.c listwise/liblistwise/op/o/test.c listwise/liblistwise/op/c/c.c listwise/liblistwise/op/stat/stat.c listwise/liblistwise/op/x/x.c listwise/liblistwise/op/t/t.c listwise/liblistwise/op/b/b.c listwise/liblistwise/op/C/C.c listwise/liblistwise/op/g/g.c listwise/liblistwise/op/u/test.c listwise/liblistwise/op/u/u.c listwise/liblistwise/op/xch/xch.c listwise/liblistwise/op/cp/cp.c listwise/liblistwise/op/ln/ln.c listwise/liblistwise/op/re/re.c listwise/liblistwise/op/vf/vf.c listwise/liblistwise/op/sx/sx.c listwise/liblistwise/op/sx/test.c listwise/liblistwise/op/d/d.c listwise/liblistwise/op/ls/ls.c listwise/liblistwise/transform/transform.tab.c listwise/liblistwise/transform/genscan.c listwise/liblistwise/transform/transform.states.c listwise/liblistwise/transform/transform.tokens.c listwise/liblistwise/transform/transform.c listwise/liblistwise/transform/transform.lex.c listwise/liblistwise/window/window.c listwise/liblistwise/fs/fs.c listwise/liblistwise/re/re.c listwise/liblistwise/error/error.c listwise/liblistwise/error/PCRE.errtab.c listwise/liblistwise/error/LW.errtab.c listwise/liblistwise/test/fab_test/test.c listwise/liblistwise/test/listwise_test/listwise_test.c )
echo "local: *; };"
	 ) 1>listwise/liblistwise/exports


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[163]='listwise/liblistwise/fs/fs.eapi.debug.pic.o'
fml_3_44()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/fs/fs.c -o listwise/liblistwise/fs/fs.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[164]='listwise/liblistwise/fs/fs.xapi.debug.pic.o'
fml_3_45()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/fs/fs.c -o listwise/liblistwise/fs/fs.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[165]='listwise/liblistwise/iterate/iterate.eapi.debug.pic.o'
fml_3_46()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/iterate/iterate.c -o listwise/liblistwise/iterate/iterate.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[166]='listwise/liblistwise/iterate/iterate.xapi.debug.pic.o'
fml_3_47()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/iterate/iterate.c -o listwise/liblistwise/iterate/iterate.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[167]='listwise/liblistwise/logging/logging.eapi.debug.pic.o'
fml_3_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/logging/logging.c -o listwise/liblistwise/logging/logging.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[168]='listwise/liblistwise/logging/logging.xapi.debug.pic.o'
fml_3_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/logging/logging.c -o listwise/liblistwise/logging/logging.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[169]='listwise/liblistwise/lstack/lstack.eapi.debug.pic.o'
fml_3_50()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lstack/lstack.c -o listwise/liblistwise/lstack/lstack.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[170]='listwise/liblistwise/lstack/lstack.xapi.debug.pic.o'
fml_3_51()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lstack/lstack.c -o listwise/liblistwise/lstack/lstack.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[171]='listwise/liblistwise/lwx/lwx.eapi.debug.pic.o'
fml_3_52()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lwx/lwx.c -o listwise/liblistwise/lwx/lwx.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[172]='listwise/liblistwise/lwx/lwx.xapi.debug.pic.o'
fml_3_53()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/lwx/lwx.c -o listwise/liblistwise/lwx/lwx.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[173]='listwise/liblistwise/object/object.eapi.debug.pic.o'
fml_3_54()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/object/object.c -o listwise/liblistwise/object/object.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[174]='listwise/liblistwise/object/object.xapi.debug.pic.o'
fml_3_55()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/object/object.c -o listwise/liblistwise/object/object.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[175]='listwise/liblistwise/operator/operator.eapi.debug.pic.o'
fml_3_56()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operator/operator.c -o listwise/liblistwise/operator/operator.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[176]='listwise/liblistwise/operator/operator.xapi.debug.pic.o'
fml_3_57()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operator/operator.c -o listwise/liblistwise/operator/operator.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[177]='listwise/liblistwise/operators/operators.eapi.debug.pic.o'
fml_3_58()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operators/operators.c -o listwise/liblistwise/operators/operators.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[178]='listwise/liblistwise/operators/operators.xapi.debug.pic.o'
fml_3_59()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/operators/operators.c -o listwise/liblistwise/operators/operators.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[179]='listwise/liblistwise/re/re.eapi.debug.pic.o'
fml_3_60()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/re/re.c -o listwise/liblistwise/re/re.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[180]='listwise/liblistwise/re/re.xapi.debug.pic.o'
fml_3_61()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/re/re.c -o listwise/liblistwise/re/re.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[181]='listwise/liblistwise/sanity/sanity.eapi.debug.pic.o'
fml_3_62()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/sanity/sanity.c -o listwise/liblistwise/sanity/sanity.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[182]='listwise/liblistwise/sanity/sanity.xapi.debug.pic.o'
fml_3_63()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/sanity/sanity.c -o listwise/liblistwise/sanity/sanity.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[183]='listwise/liblistwise/selection/selection.eapi.debug.pic.o'
fml_3_64()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/selection/selection.c -o listwise/liblistwise/selection/selection.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[184]='listwise/liblistwise/selection/selection.xapi.debug.pic.o'
fml_3_65()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/selection/selection.c -o listwise/liblistwise/selection/selection.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[185]='listwise/liblistwise/transform/genscan.eapi.debug.pic.o'
fml_3_66()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/genscan.c -o listwise/liblistwise/transform/genscan.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[186]='listwise/liblistwise/transform/genscan.xapi.debug.pic.o'
fml_3_67()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/genscan.c -o listwise/liblistwise/transform/genscan.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[187]='listwise/liblistwise/transform/transform.eapi.debug.pic.o'
fml_3_68()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.c -o listwise/liblistwise/transform/transform.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[188]='listwise/liblistwise/transform/transform.lex.eapi.debug.pic.o'
fml_3_69()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.lex.c -o listwise/liblistwise/transform/transform.lex.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[189]='listwise/liblistwise/transform/transform.lex.xapi.debug.pic.o'
fml_3_70()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.lex.c -o listwise/liblistwise/transform/transform.lex.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[190]='listwise/liblistwise/transform/transform.states.eapi.debug.pic.o'
fml_3_71()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.states.c -o listwise/liblistwise/transform/transform.states.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[191]='listwise/liblistwise/transform/transform.states.xapi.debug.pic.o'
fml_3_72()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.states.c -o listwise/liblistwise/transform/transform.states.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[192]='listwise/liblistwise/transform/transform.tab.eapi.debug.pic.o'
fml_3_73()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tab.c -o listwise/liblistwise/transform/transform.tab.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[193]='listwise/liblistwise/transform/transform.tab.xapi.debug.pic.o'
fml_3_74()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tab.c -o listwise/liblistwise/transform/transform.tab.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[194]='listwise/liblistwise/transform/transform.tokens.eapi.debug.pic.o'
fml_3_75()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tokens.c -o listwise/liblistwise/transform/transform.tokens.eapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[195]='listwise/liblistwise/transform/transform.tokens.xapi.debug.pic.o'
fml_3_76()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.tokens.c -o listwise/liblistwise/transform/transform.tokens.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[196]='listwise/liblistwise/transform/transform.xapi.debug.pic.o'
fml_3_77()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/transform/transform.c -o listwise/liblistwise/transform/transform.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[197]='listwise/liblistwise/tune/tune.eapi.debug.pic.o'
fml_3_78()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/tune/tune.c -o listwise/liblistwise/tune/tune.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[198]='listwise/liblistwise/tune/tune.xapi.debug.pic.o'
fml_3_79()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/tune/tune.c -o listwise/liblistwise/tune/tune.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[199]='listwise/liblistwise/window/window.eapi.debug.pic.o'
fml_3_80()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/window/window.c -o listwise/liblistwise/window/window.eapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[200]='listwise/liblistwise/window/window.xapi.debug.pic.o'
fml_3_81()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/window/window.c -o listwise/liblistwise/window/window.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 4
NAMES[201]='libxapi/libxapi-xapi.debug.so'
fml_4_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -o libxapi/libxapi-xapi.debug.so common/memblk/memblk.def.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o common/wstdlib/wstdlib.xapi.debug.pic.o libxapi/callstack/callstack.xapi.debug.pic.o libxapi/error/SYS.errtab.xapi.debug.pic.o libxapi/error/XAPI.errtab.xapi.debug.pic.o libxapi/error/error.xapi.debug.pic.o libxapi/frame/frame.xapi.debug.pic.o libxapi/trace/trace.xapi.debug.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[202]='libxapi/libxapi.debug.so'
fml_4_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxapi/callstack -I./libxapi/xapi -I./libxapi/internal -I./libxapi/frame -I./libxapi/trace -I./libxapi/error -I./libxapi -o libxapi/libxapi.debug.so libxapi/error/SYS.errtab.eapi.debug.pic.o libxapi/error/XAPI.errtab.eapi.debug.pic.o libxapi/error/error.eapi.debug.pic.o -shared -Wl,--version-script=libxapi/exports -Wl,-soname,libxapi.so


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 5
NAMES[203]='libxlinux/libxlinux-xapi.debug.so'
fml_5_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -o libxlinux/libxlinux-xapi.debug.so libxlinux/error/XLINUX.errtab.xapi.debug.pic.o libxlinux/error/error.xapi.debug.pic.o libxlinux/mempolicy/mempolicy.xapi.debug.pic.o libxlinux/xdirent/xdirent.xapi.debug.pic.o libxlinux/xdlfcn/xdlfcn.xapi.debug.pic.o libxlinux/xfcntl/xfcntl.xapi.debug.pic.o libxlinux/xfile/xfile.xapi.debug.pic.o libxlinux/xftw/xftw.xapi.debug.pic.o libxlinux/xgrp/xgrp.xapi.debug.pic.o libxlinux/xmman/xmman.xapi.debug.pic.o libxlinux/xprctl/xprctl.xapi.debug.pic.o libxlinux/xpwd/xpwd.xapi.debug.pic.o libxlinux/xsignal/xsignal.xapi.debug.pic.o libxlinux/xstat/xstat.xapi.debug.pic.o libxlinux/xstdlib/xstdlib.xapi.debug.pic.o libxlinux/xstring/xstring.xapi.debug.pic.o libxlinux/xtime/xtime.xapi.debug.pic.o libxlinux/xuio/xuio.xapi.debug.pic.o libxlinux/xunistd/xunistd.xapi.debug.pic.o libxlinux/xwait/xwait.xapi.debug.pic.o -ldl -lrt -shared -Wl,--version-script=libxlinux/exports ./libxapi/libxapi-xapi.debug.so -Wl,-soname,libxlinux-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[204]='libxlinux/libxlinux.debug.so'
fml_5_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libxlinux/xftw -I./libxlinux/xpwd -I./libxlinux/xdlfcn -I./libxlinux/xgrp -I./libxlinux/xsignal -I./libxlinux/xstring -I./libxlinux/xfcntl -I./libxlinux/xdirent -I./libxlinux/xstdlib -I./libxlinux/xwait -I./libxlinux/xtime -I./libxlinux/xprctl -I./libxlinux/internal -I./libxlinux/xmman -I./libxlinux/xlinux -I./libxlinux/xfile -I./libxlinux/xstat -I./libxlinux/error -I./libxlinux/mempolicy -I./libxlinux/xuio -I./libxlinux/xunistd -I./libxlinux -I./libxapi -o libxlinux/libxlinux.debug.so libxlinux/error/XLINUX.errtab.eapi.debug.pic.o libxlinux/error/error.eapi.debug.pic.o libxlinux/mempolicy/mempolicy.eapi.debug.pic.o libxlinux/xdirent/xdirent.eapi.debug.pic.o libxlinux/xdlfcn/xdlfcn.eapi.debug.pic.o libxlinux/xfcntl/xfcntl.eapi.debug.pic.o libxlinux/xfile/xfile.eapi.debug.pic.o libxlinux/xftw/xftw.eapi.debug.pic.o libxlinux/xgrp/xgrp.eapi.debug.pic.o libxlinux/xmman/xmman.eapi.debug.pic.o libxlinux/xprctl/xprctl.eapi.debug.pic.o libxlinux/xpwd/xpwd.eapi.debug.pic.o libxlinux/xsignal/xsignal.eapi.debug.pic.o libxlinux/xstat/xstat.eapi.debug.pic.o libxlinux/xstdlib/xstdlib.eapi.debug.pic.o libxlinux/xstring/xstring.eapi.debug.pic.o libxlinux/xtime/xtime.eapi.debug.pic.o libxlinux/xuio/xuio.eapi.debug.pic.o libxlinux/xunistd/xunistd.eapi.debug.pic.o libxlinux/xwait/xwait.eapi.debug.pic.o -ldl -lrt -shared -Wl,--version-script=libxlinux/exports ./libxapi/libxapi.debug.so -Wl,-soname,libxlinux.so


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 6
NAMES[205]='libpstring/libpstring-xapi.debug.so'
fml_6_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring-xapi.debug.so libpstring/pstring/pstring.xapi.debug.pic.o -shared -Wl,--version-script=libpstring/exports ./libxapi/libxapi-xapi.debug.so ./libxlinux/libxlinux-xapi.debug.so -Wl,-soname,libpstring-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[206]='libpstring/libpstring.debug.so'
fml_6_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_ERRCODE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -fPIC -I./libpstring/internal -I./libpstring/pstring -I./libpstring/write -I./libpstring -I./libxapi -I./libxlinux -o libpstring/libpstring.debug.so libpstring/pstring/pstring.eapi.debug.pic.o -shared -Wl,--version-script=libpstring/exports ./libxapi/libxapi.debug.so ./libxlinux/libxlinux.debug.so -Wl,-soname,libpstring.so


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 7
NAMES[207]='listwise/liblistwise/liblistwise-xapi.debug.so'
fml_7_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc \
-fPIC \
-I./listwise/liblistwise/lwx \
-I./listwise/liblistwise/object \
-I./listwise/liblistwise/listwise \
-I./listwise/liblistwise/operators \
-I./listwise/liblistwise/operator \
-I./listwise/liblistwise/exec \
-I./listwise/liblistwise/describe \
-I./listwise/liblistwise/selection \
-I./listwise/liblistwise/tune \
-I./listwise/liblistwise/lstack \
-I./listwise/liblistwise/internal \
-I./listwise/liblistwise/sanity \
-I./listwise/liblistwise/iterate \
-I./listwise/liblistwise/logging \
-I./listwise/liblistwise/op \
-I./listwise/liblistwise/transform \
-I./listwise/liblistwise/window \
-I./listwise/liblistwise/fs \
-I./listwise/liblistwise/re \
-I./listwise/liblistwise/error \
-I./listwise/liblistwise/test \
-I./listwise/liblistwise \
-I./libxapi \
-I./libxlinux \
-I./libpstring \
-I./xunit/libxunit \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fplan9-extensions \
-fno-builtin-log \
-I./common/canon \
-I./common/strutil \
-I./common/map \
-I./common/parseint \
-I./common/coll \
-I./common/memblk \
-I./common/narrate \
-I./common/wstdlib \
-I./common/unitstring \
-I./common/yyutil \
-I./common/wstring \
-I./common/cksum \
-I./common/logger \
-I./common/color \
-I./common \
-DBUILDSTAMP='Mon \
Mar \
30 \
2015 \
18:39:03' \
-DXAPI_PROVIDE_BACKTRACE \
-O0 \
-ggdb3 \
-DDEBUG \
-DXAPI_RUNTIME_SANITY_CHECKS \
-DSANITY \
-DLWOPDIR=/usr/lib/listwise \
-DLWOPEXT=so \
-o \
listwise/liblistwise/liblistwise-xapi.debug.so \
listwise/liblistwise/transform/transform.states.xapi.debug.pic.o \
listwise/liblistwise/transform/genscan.xapi.debug.pic.o \
common/wstdlib/wstdlib.xapi.debug.pic.o \
listwise/liblistwise/lwx/lwx.xapi.debug.pic.o \
common/coll/coll.xapi.debug.pic.o \
common/map/map.xapi.debug.pic.o \
listwise/liblistwise/object/object.xapi.debug.pic.o \
listwise/liblistwise/operators/operators.xapi.debug.pic.o \
listwise/liblistwise/operator/operator.xapi.debug.pic.o \
listwise/liblistwise/sanity/sanity.xapi.debug.pic.o \
listwise/liblistwise/exec/exec.xapi.debug.pic.o \
listwise/liblistwise/describe/describe.xapi.debug.pic.o \
listwise/liblistwise/selection/selection.xapi.debug.pic.o \
listwise/liblistwise/tune/tune.xapi.debug.pic.o \
listwise/liblistwise/lstack/lstack.xapi.debug.pic.o \
listwise/liblistwise/iterate/iterate.xapi.debug.pic.o \
listwise/liblistwise/logging/logging.xapi.debug.pic.o \
common/strutil/strutil.xapi.debug.pic.o \
common/yyutil/yyutil.xapi.debug.pic.o \
listwise/liblistwise/transform/transform.tab.xapi.debug.pic.o \
listwise/liblistwise/transform/transform.tokens.xapi.debug.pic.o \
listwise/liblistwise/transform/transform.lex.xapi.debug.pic.o \
listwise/liblistwise/transform/transform.xapi.debug.pic.o \
listwise/liblistwise/window/window.xapi.debug.pic.o \
common/canon/canon.xapi.debug.pic.o \
common/parseint/parseint.xapi.debug.pic.o \
common/color/color.xapi.debug.pic.o \
listwise/liblistwise/fs/fs.xapi.debug.pic.o \
listwise/liblistwise/re/re.xapi.debug.pic.o \
listwise/liblistwise/error/error.xapi.debug.pic.o \
listwise/liblistwise/error/PCRE.errtab.xapi.debug.pic.o \
listwise/liblistwise/error/LW.errtab.xapi.debug.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=listwise/liblistwise/exports \
./libxapi/libxapi-xapi.debug.so \
./libxlinux/libxlinux-xapi.debug.so \
./libpstring/libpstring-xapi.debug.so \
-Wl,-soname,liblistwise-xapi.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[208]='listwise/liblistwise/liblistwise.debug.so'
fml_7_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc \
-fPIC \
-I./listwise/liblistwise/lwx \
-I./listwise/liblistwise/object \
-I./listwise/liblistwise/listwise \
-I./listwise/liblistwise/operators \
-I./listwise/liblistwise/operator \
-I./listwise/liblistwise/exec \
-I./listwise/liblistwise/describe \
-I./listwise/liblistwise/selection \
-I./listwise/liblistwise/tune \
-I./listwise/liblistwise/lstack \
-I./listwise/liblistwise/internal \
-I./listwise/liblistwise/sanity \
-I./listwise/liblistwise/iterate \
-I./listwise/liblistwise/logging \
-I./listwise/liblistwise/op \
-I./listwise/liblistwise/transform \
-I./listwise/liblistwise/window \
-I./listwise/liblistwise/fs \
-I./listwise/liblistwise/re \
-I./listwise/liblistwise/error \
-I./listwise/liblistwise/test \
-I./listwise/liblistwise \
-I./libxapi \
-I./libxlinux \
-I./libpstring \
-I./xunit/libxunit \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fplan9-extensions \
-fno-builtin-log \
-I./common/canon \
-I./common/strutil \
-I./common/map \
-I./common/parseint \
-I./common/coll \
-I./common/memblk \
-I./common/narrate \
-I./common/wstdlib \
-I./common/unitstring \
-I./common/yyutil \
-I./common/wstring \
-I./common/cksum \
-I./common/logger \
-I./common/color \
-I./common \
-DBUILDSTAMP='Mon \
Mar \
30 \
2015 \
18:39:03' \
-DXAPI_PROVIDE_ERRCODE \
-O0 \
-ggdb3 \
-DDEBUG \
-DXAPI_RUNTIME_SANITY_CHECKS \
-DSANITY \
-DLWOPDIR=/usr/lib/listwise \
-DLWOPEXT=so \
-o \
listwise/liblistwise/liblistwise.debug.so \
listwise/liblistwise/transform/transform.states.eapi.debug.pic.o \
listwise/liblistwise/transform/genscan.eapi.debug.pic.o \
common/wstdlib/wstdlib.eapi.debug.pic.o \
listwise/liblistwise/lwx/lwx.eapi.debug.pic.o \
common/coll/coll.eapi.debug.pic.o \
common/map/map.eapi.debug.pic.o \
listwise/liblistwise/object/object.eapi.debug.pic.o \
listwise/liblistwise/operators/operators.eapi.debug.pic.o \
listwise/liblistwise/operator/operator.eapi.debug.pic.o \
listwise/liblistwise/sanity/sanity.eapi.debug.pic.o \
listwise/liblistwise/exec/exec.eapi.debug.pic.o \
listwise/liblistwise/describe/describe.eapi.debug.pic.o \
listwise/liblistwise/selection/selection.eapi.debug.pic.o \
listwise/liblistwise/tune/tune.eapi.debug.pic.o \
listwise/liblistwise/lstack/lstack.eapi.debug.pic.o \
listwise/liblistwise/iterate/iterate.eapi.debug.pic.o \
listwise/liblistwise/logging/logging.eapi.debug.pic.o \
common/strutil/strutil.eapi.debug.pic.o \
common/yyutil/yyutil.eapi.debug.pic.o \
listwise/liblistwise/transform/transform.tab.eapi.debug.pic.o \
listwise/liblistwise/transform/transform.tokens.eapi.debug.pic.o \
listwise/liblistwise/transform/transform.lex.eapi.debug.pic.o \
listwise/liblistwise/transform/transform.eapi.debug.pic.o \
listwise/liblistwise/window/window.eapi.debug.pic.o \
common/canon/canon.eapi.debug.pic.o \
common/parseint/parseint.eapi.debug.pic.o \
common/color/color.eapi.debug.pic.o \
listwise/liblistwise/fs/fs.eapi.debug.pic.o \
listwise/liblistwise/re/re.eapi.debug.pic.o \
listwise/liblistwise/error/error.eapi.debug.pic.o \
listwise/liblistwise/error/PCRE.errtab.eapi.debug.pic.o \
listwise/liblistwise/error/LW.errtab.eapi.debug.pic.o \
-lpcre \
-ldl \
-shared \
-Wl,--version-script=listwise/liblistwise/exports \
./libxapi/libxapi.debug.so \
./libxlinux/libxlinux.debug.so \
./libpstring/libpstring.debug.so \
-Wl,-soname,liblistwise.so


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 8
NAMES[209]='fab/fab/fab.debug'
fml_8_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fab -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -export-dynamic -o fab/fab/fab.debug fab/common/error/FAB.errtab.xapi.debug.o common/wstdlib/wstdlib.xapi.debug.o common/strutil/strutil.xapi.debug.o common/logger/logger.xapi.debug.o fab/common/logs/logs.xapi.debug.o common/canon/canon.xapi.debug.o fab/common/params/params.xapi.debug.o common/cksum/cksum.xapi.debug.o common/memblk/memblk.def.xapi.debug.o common/memblk/memblk.xapi.debug.o fab/common/path/path.xapi.debug.o common/unitstring/unitstring.xapi.debug.o fab/common/args/args.xapi.debug.o fab/common/dirutil/dirutil.xapi.debug.o fab/common/identity/identity.xapi.debug.o common/parseint/parseint.xapi.debug.o fab/fab/main.xapi.debug.o -lrt ./libxapi/libxapi-xapi.debug.so ./libxlinux/libxlinux-xapi.debug.so ./libpstring/libpstring-xapi.debug.so ./listwise/liblistwise/liblistwise-xapi.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[210]='fab/fabd/fabd.debug'
fml_8_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/fabd -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./listwise/liblistwise -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -export-dynamic -o fab/fabd/fabd.debug fab/common/error/FAB.errtab.xapi.debug.o common/wstdlib/wstdlib.xapi.debug.o common/strutil/strutil.xapi.debug.o common/logger/logger.xapi.debug.o fab/common/logs/logs.xapi.debug.o common/canon/canon.xapi.debug.o fab/common/params/params.xapi.debug.o common/cksum/cksum.xapi.debug.o common/memblk/memblk.def.xapi.debug.o common/memblk/memblk.xapi.debug.o fab/common/path/path.xapi.debug.o common/unitstring/unitstring.xapi.debug.o fab/common/args/args.xapi.debug.o fab/common/identity/identity.xapi.debug.o common/yyutil/yyutil.xapi.debug.o fab/fabd/ff/ff.tokens.xapi.debug.o common/coll/coll.xapi.debug.o common/map/map.xapi.debug.o fab/common/dirutil/dirutil.xapi.debug.o fab/fabd/bs/bs.xapi.debug.o fab/fabd/dsc/dsc.xapi.debug.o fab/fabd/selector/selector.xapi.debug.o fab/fabd/dep/dep.xapi.debug.o fab/fabd/ffproc/ffproc.xapi.debug.o fab/fabd/list/list.xapi.debug.o fab/fabd/var/var.xapi.debug.o fab/fabd/tmp/tmp.xapi.debug.o fab/fabd/lwutil/lwutil.xapi.debug.o fab/fabd/gn/gnlw.xapi.debug.o fab/fabd/enclose/enclose.xapi.debug.o fab/fabd/traverse/traverse.xapi.debug.o common/parseint/parseint.xapi.debug.o fab/fabd/gn/gn.xapi.debug.o fab/fabd/bp/bp.xapi.debug.o fab/fabd/strstack/strstack.xapi.debug.o fab/fabd/hashblock/hashblock.xapi.debug.o fab/fabd/ff/ff.tab.xapi.debug.o fab/fabd/ff/ff.dsc.tab.xapi.debug.o fab/fabd/ff/ff.var.tab.xapi.debug.o fab/fabd/ff/ff.list.tab.xapi.debug.o fab/fabd/ff/ff.lex.xapi.debug.o fab/fabd/ff/ff.states.xapi.debug.o fab/fabd/ff/ff.xapi.debug.o fab/fabd/ts/ts.xapi.debug.o fab/fabd/ffn/ffn.xapi.debug.o fab/fabd/fml/fml.xapi.debug.o fab/fabd/main.xapi.debug.o -lrt ./libxapi/libxapi-xapi.debug.so ./libxlinux/libxlinux-xapi.debug.so ./libpstring/libpstring-xapi.debug.so ./listwise/liblistwise/liblistwise-xapi.debug.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[211]='fab/fabw/fabw.debug'
fml_8_2()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./fab/fabw -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/common -I./libxapi -I./libxlinux -I./libpstring -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fabw/fabw.debug fab/common/error/FAB.errtab.xapi.debug.o common/wstdlib/wstdlib.xapi.debug.o common/strutil/strutil.xapi.debug.o common/logger/logger.xapi.debug.o fab/common/logs/logs.xapi.debug.o common/canon/canon.xapi.debug.o fab/common/params/params.xapi.debug.o common/parseint/parseint.xapi.debug.o fab/fabw/main.xapi.debug.o ./libxapi/libxapi-xapi.debug.so ./libxlinux/libxlinux-xapi.debug.so ./libpstring/libpstring-xapi.debug.so ./listwise/liblistwise/liblistwise-xapi.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[212]='listwise/liblistwise/op/-X/-X.xapi.debug.pic.o'
fml_8_3()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/-X/-X.c -o listwise/liblistwise/op/-X/-X.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[213]='listwise/liblistwise/op/C/C.xapi.debug.pic.o'
fml_8_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/C/C.c -o listwise/liblistwise/op/C/C.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[214]='listwise/liblistwise/op/b/b.xapi.debug.pic.o'
fml_8_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/b/b.c -o listwise/liblistwise/op/b/b.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[215]='listwise/liblistwise/op/bn/bn.xapi.debug.pic.o'
fml_8_6()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/bn/bn.c -o listwise/liblistwise/op/bn/bn.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[216]='listwise/liblistwise/op/c/c.xapi.debug.pic.o'
fml_8_7()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/c/c.c -o listwise/liblistwise/op/c/c.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[217]='listwise/liblistwise/op/can/can.xapi.debug.pic.o'
fml_8_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/can/can.c -o listwise/liblistwise/op/can/can.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[218]='listwise/liblistwise/op/cons/cons.xapi.debug.pic.o'
fml_8_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cons/cons.c -o listwise/liblistwise/op/cons/cons.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[219]='listwise/liblistwise/op/cp/cp.xapi.debug.pic.o'
fml_8_10()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/cp/cp.c -o listwise/liblistwise/op/cp/cp.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[220]='listwise/liblistwise/op/d/d.xapi.debug.pic.o'
fml_8_11()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/d/d.c -o listwise/liblistwise/op/d/d.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[221]='listwise/liblistwise/op/datef/datef.xapi.debug.pic.o'
fml_8_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/datef/datef.c -o listwise/liblistwise/op/datef/datef.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[222]='listwise/liblistwise/op/dj/dj.xapi.debug.pic.o'
fml_8_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dj/dj.c -o listwise/liblistwise/op/dj/dj.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[223]='listwise/liblistwise/op/dn/dn.xapi.debug.pic.o'
fml_8_14()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/dn/dn.c -o listwise/liblistwise/op/dn/dn.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[224]='listwise/liblistwise/op/f/f.xapi.debug.pic.o'
fml_8_15()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/f/f.c -o listwise/liblistwise/op/f/f.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[225]='listwise/liblistwise/op/fn/fn.xapi.debug.pic.o'
fml_8_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/fn/fn.c -o listwise/liblistwise/op/fn/fn.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[226]='listwise/liblistwise/op/g/g.xapi.debug.pic.o'
fml_8_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/g/g.c -o listwise/liblistwise/op/g/g.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[227]='listwise/liblistwise/op/hl/hl.xapi.debug.pic.o'
fml_8_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/hl/hl.c -o listwise/liblistwise/op/hl/hl.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[228]='listwise/liblistwise/op/j/j.xapi.debug.pic.o'
fml_8_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/j/j.c -o listwise/liblistwise/op/j/j.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[229]='listwise/liblistwise/op/l/l.xapi.debug.pic.o'
fml_8_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/l/l.c -o listwise/liblistwise/op/l/l.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[230]='listwise/liblistwise/op/ln/ln.xapi.debug.pic.o'
fml_8_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/ln/ln.c -o listwise/liblistwise/op/ln/ln.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[231]='listwise/liblistwise/op/ls/ls.xapi.debug.pic.o'
fml_8_22()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/ls/ls.c -o listwise/liblistwise/op/ls/ls.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[232]='listwise/liblistwise/op/lx/lx.xapi.debug.pic.o'
fml_8_23()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/lx/lx.c -o listwise/liblistwise/op/lx/lx.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[233]='listwise/liblistwise/op/o/o.xapi.debug.pic.o'
fml_8_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/o/o.c -o listwise/liblistwise/op/o/o.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[234]='listwise/liblistwise/op/p/p.xapi.debug.pic.o'
fml_8_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/p/p.c -o listwise/liblistwise/op/p/p.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[235]='listwise/liblistwise/op/pop/pop.xapi.debug.pic.o'
fml_8_26()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/pop/pop.c -o listwise/liblistwise/op/pop/pop.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[236]='listwise/liblistwise/op/r/r.xapi.debug.pic.o'
fml_8_27()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/r/r.c -o listwise/liblistwise/op/r/r.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[237]='listwise/liblistwise/op/rb/rb.xapi.debug.pic.o'
fml_8_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rb/rb.c -o listwise/liblistwise/op/rb/rb.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[238]='listwise/liblistwise/op/re/re.xapi.debug.pic.o'
fml_8_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/re/re.c -o listwise/liblistwise/op/re/re.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[239]='listwise/liblistwise/op/rp/rp.xapi.debug.pic.o'
fml_8_30()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rp/rp.c -o listwise/liblistwise/op/rp/rp.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[240]='listwise/liblistwise/op/rx/rx.xapi.debug.pic.o'
fml_8_31()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/rx/rx.c -o listwise/liblistwise/op/rx/rx.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[241]='listwise/liblistwise/op/s/s.xapi.debug.pic.o'
fml_8_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/s/s.c -o listwise/liblistwise/op/s/s.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[242]='listwise/liblistwise/op/shift/shift.xapi.debug.pic.o'
fml_8_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/shift/shift.c -o listwise/liblistwise/op/shift/shift.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[243]='listwise/liblistwise/op/sort/sort.xapi.debug.pic.o'
fml_8_34()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sort/sort.c -o listwise/liblistwise/op/sort/sort.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[244]='listwise/liblistwise/op/stat/stat.xapi.debug.pic.o'
fml_8_35()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/stat/stat.c -o listwise/liblistwise/op/stat/stat.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[245]='listwise/liblistwise/op/sx/sx.xapi.debug.pic.o'
fml_8_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/sx/sx.c -o listwise/liblistwise/op/sx/sx.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[246]='listwise/liblistwise/op/t/t.xapi.debug.pic.o'
fml_8_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/t/t.c -o listwise/liblistwise/op/t/t.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[247]='listwise/liblistwise/op/u/u.xapi.debug.pic.o'
fml_8_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/u/u.c -o listwise/liblistwise/op/u/u.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[248]='listwise/liblistwise/op/up/up.xapi.debug.pic.o'
fml_8_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/up/up.c -o listwise/liblistwise/op/up/up.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[249]='listwise/liblistwise/op/uu/uu.xapi.debug.pic.o'
fml_8_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/uu/uu.c -o listwise/liblistwise/op/uu/uu.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[250]='listwise/liblistwise/op/v/v.xapi.debug.pic.o'
fml_8_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/v/v.c -o listwise/liblistwise/op/v/v.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[251]='listwise/liblistwise/op/vf/vf.xapi.debug.pic.o'
fml_8_42()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vf/vf.c -o listwise/liblistwise/op/vf/vf.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[252]='listwise/liblistwise/op/vp/vp.xapi.debug.pic.o'
fml_8_43()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/vp/vp.c -o listwise/liblistwise/op/vp/vp.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[253]='listwise/liblistwise/op/w/w.xapi.debug.pic.o'
fml_8_44()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/w/w.c -o listwise/liblistwise/op/w/w.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[254]='listwise/liblistwise/op/wvf/wvf.xapi.debug.pic.o'
fml_8_45()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvf/wvf.c -o listwise/liblistwise/op/wvf/wvf.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[255]='listwise/liblistwise/op/wvp/wvp.xapi.debug.pic.o'
fml_8_46()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/wvp/wvp.c -o listwise/liblistwise/op/wvp/wvp.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[256]='listwise/liblistwise/op/x/x.xapi.debug.pic.o'
fml_8_47()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/x/x.c -o listwise/liblistwise/op/x/x.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[257]='listwise/liblistwise/op/xch/xch.xapi.debug.pic.o'
fml_8_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/xch/xch.c -o listwise/liblistwise/op/xch/xch.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[258]='listwise/liblistwise/op/y/y.xapi.debug.pic.o'
fml_8_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/y/y.c -o listwise/liblistwise/op/y/y.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[259]='listwise/liblistwise/op/z/z.xapi.debug.pic.o'
fml_8_50()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -c listwise/liblistwise/op/z/z.c -o listwise/liblistwise/op/z/z.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[260]='listwise/listwise/listwise.eapi.debug'
fml_8_51()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc \
-DFABVERSIONS=v0.6.0.0 \
-DFABVERSIONN=600 \
-Ilistwise/listwise/man \
-Ilistwise/listwise/args \
-Ilistwise/listwise/logs \
-Ilistwise/listwise/error \
-Ilistwise/listwise \
-I./libxapi \
-I./libxlinux \
-I./listwise/liblistwise \
-I./libpstring \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fplan9-extensions \
-fno-builtin-log \
-I./common/canon \
-I./common/strutil \
-I./common/map \
-I./common/parseint \
-I./common/coll \
-I./common/memblk \
-I./common/narrate \
-I./common/wstdlib \
-I./common/unitstring \
-I./common/yyutil \
-I./common/wstring \
-I./common/cksum \
-I./common/logger \
-I./common/color \
-I./common \
-DBUILDSTAMP='Mon Mar 30 2015 18:39:03' \
-DXAPI_PROVIDE_ERRCODE \
-O0 \
-ggdb3 \
-DDEBUG \
-DXAPI_RUNTIME_SANITY_CHECKS \
-DSANITY \
-o \
listwise/listwise/listwise.eapi.debug \
listwise/listwise/error/LISTWISE.errtab.eapi.debug.o \
common/wstdlib/wstdlib.eapi.debug.o \
common/strutil/strutil.eapi.debug.o \
common/logger/logger.eapi.debug.o \
listwise/listwise/logs/logs.eapi.debug.o \
common/parseint/parseint.eapi.debug.o \
listwise/listwise/args/args.eapi.debug.o \
listwise/listwise/main.eapi.debug.o \
./libxlinux/libxlinux.debug.so \
./libpstring/libpstring.debug.so \
./listwise/liblistwise/liblistwise.debug.so \
./libxapi/libxapi.debug.so


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[261]='listwise/listwise/listwise.xapi.debug'
fml_8_52()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc \
-DFABVERSIONS=v0.6.0.0 \
-DFABVERSIONN=600 \
-Ilistwise/listwise/man \
-Ilistwise/listwise/args \
-Ilistwise/listwise/logs \
-Ilistwise/listwise/error \
-Ilistwise/listwise \
-I./libxapi \
-I./libxlinux \
-I./listwise/liblistwise \
-I./libpstring \
-m64 \
-Wall \
-Werror \
-D_GNU_SOURCE \
-fplan9-extensions \
-fno-builtin-log \
-I./common/canon \
-I./common/strutil \
-I./common/map \
-I./common/parseint \
-I./common/coll \
-I./common/memblk \
-I./common/narrate \
-I./common/wstdlib \
-I./common/unitstring \
-I./common/yyutil \
-I./common/wstring \
-I./common/cksum \
-I./common/logger \
-I./common/color \
-I./common \
-DBUILDSTAMP='Mon Mar 30 2015 18:39:03' \
-DXAPI_PROVIDE_BACKTRACE \
-O0 \
-ggdb3 \
-DDEBUG \
-DXAPI_RUNTIME_SANITY_CHECKS \
-DSANITY \
-o \
listwise/listwise/listwise.xapi.debug \
listwise/listwise/error/LISTWISE.errtab.xapi.debug.o \
common/wstdlib/wstdlib.xapi.debug.o \
common/strutil/strutil.xapi.debug.o \
common/logger/logger.xapi.debug.o \
listwise/listwise/logs/logs.xapi.debug.o \
common/parseint/parseint.xapi.debug.o \
listwise/listwise/args/args.xapi.debug.o \
listwise/listwise/main.xapi.debug.o \
./libxlinux/libxlinux-xapi.debug.so \
./libpstring/libpstring-xapi.debug.so \
./listwise/liblistwise/liblistwise-xapi.debug.so \
./libxapi/libxapi-xapi.debug.so


  X=$?
  echo 0 1>&99
  exit $X
}


# formulas and names for stage 9
NAMES[262]='fab/fablw/op/fg/fg.xapi.debug.pic.o'
fml_9_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -fPIC -c fab/fablw/op/fg/fg.c -o fab/fablw/op/fg/fg.xapi.debug.pic.o


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[263]='fab/fablw/op/fi/fi.xapi.debug.pic.o'
fml_9_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -fPIC -c fab/fablw/op/fi/fi.c -o fab/fablw/op/fi/fi.xapi.debug.pic.o


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[264]='listwise/liblistwise/op/-X/-X.xapi.debug.so'
fml_9_2()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/-X/-X.xapi.debug.so listwise/liblistwise/op/-X/-X.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[265]='listwise/liblistwise/op/C/C.xapi.debug.so'
fml_9_3()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/C/C.xapi.debug.so listwise/liblistwise/op/C/C.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[266]='listwise/liblistwise/op/b/b.xapi.debug.so'
fml_9_4()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/b/b.xapi.debug.so listwise/liblistwise/op/b/b.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[267]='listwise/liblistwise/op/bn/bn.xapi.debug.so'
fml_9_5()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/bn/bn.xapi.debug.so listwise/liblistwise/op/bn/bn.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[268]='listwise/liblistwise/op/c/c.xapi.debug.so'
fml_9_6()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/c/c.xapi.debug.so listwise/liblistwise/op/c/c.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[269]='listwise/liblistwise/op/can/can.xapi.debug.so'
fml_9_7()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/can/can.xapi.debug.so common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o common/canon/canon.xapi.debug.pic.o listwise/liblistwise/op/can/can.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[270]='listwise/liblistwise/op/cons/cons.xapi.debug.so'
fml_9_8()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cons/cons.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o listwise/liblistwise/op/cons/cons.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[271]='listwise/liblistwise/op/cp/cp.xapi.debug.so'
fml_9_9()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/cp/cp.xapi.debug.so listwise/liblistwise/op/cp/cp.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[272]='listwise/liblistwise/op/d/d.xapi.debug.so'
fml_9_10()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/d/d.xapi.debug.so listwise/liblistwise/op/d/d.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[273]='listwise/liblistwise/op/datef/datef.xapi.debug.so'
fml_9_11()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/datef/datef.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o listwise/liblistwise/op/datef/datef.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[274]='listwise/liblistwise/op/dj/dj.xapi.debug.so'
fml_9_12()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dj/dj.xapi.debug.so listwise/liblistwise/op/dj/dj.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[275]='listwise/liblistwise/op/dn/dn.xapi.debug.so'
fml_9_13()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/dn/dn.xapi.debug.so listwise/liblistwise/op/dn/dn.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[276]='listwise/liblistwise/op/f/f.xapi.debug.so'
fml_9_14()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/f/f.xapi.debug.so listwise/liblistwise/op/f/f.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[277]='listwise/liblistwise/op/fn/fn.xapi.debug.so'
fml_9_15()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/fn/fn.xapi.debug.so listwise/liblistwise/op/fn/fn.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[278]='listwise/liblistwise/op/g/g.xapi.debug.so'
fml_9_16()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/g/g.xapi.debug.so listwise/liblistwise/op/g/g.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[279]='listwise/liblistwise/op/hl/hl.xapi.debug.so'
fml_9_17()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/hl/hl.xapi.debug.so common/color/color.xapi.debug.pic.o listwise/liblistwise/op/hl/hl.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[280]='listwise/liblistwise/op/j/j.xapi.debug.so'
fml_9_18()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/j/j.xapi.debug.so listwise/liblistwise/op/j/j.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[281]='listwise/liblistwise/op/l/l.xapi.debug.so'
fml_9_19()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/l/l.xapi.debug.so common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o listwise/liblistwise/op/l/l.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[282]='listwise/liblistwise/op/ln/ln.xapi.debug.so'
fml_9_20()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/ln/ln.xapi.debug.so listwise/liblistwise/op/ln/ln.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[283]='listwise/liblistwise/op/ls/ls.xapi.debug.so'
fml_9_21()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/ls/ls.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o listwise/liblistwise/op/ls/ls.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[284]='listwise/liblistwise/op/lx/lx.xapi.debug.so'
fml_9_22()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/lx/lx.xapi.debug.so listwise/liblistwise/op/lx/lx.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[285]='listwise/liblistwise/op/o/o.xapi.debug.so'
fml_9_23()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/o/o.xapi.debug.so listwise/liblistwise/op/o/o.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[286]='listwise/liblistwise/op/p/p.xapi.debug.so'
fml_9_24()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/p/p.xapi.debug.so listwise/liblistwise/op/p/p.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[287]='listwise/liblistwise/op/pop/pop.xapi.debug.so'
fml_9_25()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/pop/pop.xapi.debug.so listwise/liblistwise/op/pop/pop.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[288]='listwise/liblistwise/op/r/r.xapi.debug.so'
fml_9_26()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/r/r.xapi.debug.so listwise/liblistwise/op/r/r.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[289]='listwise/liblistwise/op/rb/rb.xapi.debug.so'
fml_9_27()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rb/rb.xapi.debug.so common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o common/canon/canon.xapi.debug.pic.o listwise/liblistwise/op/rb/rb.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[290]='listwise/liblistwise/op/re/re.xapi.debug.so'
fml_9_28()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/re/re.xapi.debug.so listwise/liblistwise/op/re/re.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[291]='listwise/liblistwise/op/rp/rp.xapi.debug.so'
fml_9_29()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rp/rp.xapi.debug.so listwise/liblistwise/op/rp/rp.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[292]='listwise/liblistwise/op/rx/rx.xapi.debug.so'
fml_9_30()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/rx/rx.xapi.debug.so listwise/liblistwise/op/rx/rx.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[293]='listwise/liblistwise/op/s/s.xapi.debug.so'
fml_9_31()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/s/s.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o listwise/liblistwise/op/s/s.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[294]='listwise/liblistwise/op/shift/shift.xapi.debug.so'
fml_9_32()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/shift/shift.xapi.debug.so listwise/liblistwise/op/shift/shift.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[295]='listwise/liblistwise/op/sort/sort.xapi.debug.so'
fml_9_33()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sort/sort.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o listwise/liblistwise/op/sort/sort.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[296]='listwise/liblistwise/op/stat/stat.xapi.debug.so'
fml_9_34()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/stat/stat.xapi.debug.so listwise/liblistwise/op/stat/stat.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[297]='listwise/liblistwise/op/sx/sx.xapi.debug.so'
fml_9_35()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/sx/sx.xapi.debug.so listwise/liblistwise/op/sx/sx.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[298]='listwise/liblistwise/op/t/t.xapi.debug.so'
fml_9_36()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/t/t.xapi.debug.so listwise/liblistwise/op/t/t.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[299]='listwise/liblistwise/op/u/u.xapi.debug.so'
fml_9_37()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/u/u.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o listwise/liblistwise/op/u/u.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[300]='listwise/liblistwise/op/up/up.xapi.debug.so'
fml_9_38()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/up/up.xapi.debug.so listwise/liblistwise/op/up/up.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[301]='listwise/liblistwise/op/uu/uu.xapi.debug.so'
fml_9_39()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/uu/uu.xapi.debug.so common/parseint/parseint.xapi.debug.pic.o common/wstdlib/wstdlib.xapi.debug.pic.o common/strutil/strutil.xapi.debug.pic.o listwise/liblistwise/op/uu/uu.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[302]='listwise/liblistwise/op/v/v.xapi.debug.so'
fml_9_40()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/v/v.xapi.debug.so listwise/liblistwise/op/v/v.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[303]='listwise/liblistwise/op/vf/vf.xapi.debug.so'
fml_9_41()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vf/vf.xapi.debug.so listwise/liblistwise/op/vf/vf.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[304]='listwise/liblistwise/op/vp/vp.xapi.debug.so'
fml_9_42()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/vp/vp.xapi.debug.so listwise/liblistwise/op/vp/vp.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[305]='listwise/liblistwise/op/w/w.xapi.debug.so'
fml_9_43()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/w/w.xapi.debug.so listwise/liblistwise/op/w/w.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[306]='listwise/liblistwise/op/wvf/wvf.xapi.debug.so'
fml_9_44()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvf/wvf.xapi.debug.so listwise/liblistwise/op/wvf/wvf.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[307]='listwise/liblistwise/op/wvp/wvp.xapi.debug.so'
fml_9_45()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/wvp/wvp.xapi.debug.so listwise/liblistwise/op/wvp/wvp.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[308]='listwise/liblistwise/op/x/x.xapi.debug.so'
fml_9_46()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/x/x.xapi.debug.so listwise/liblistwise/op/x/x.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[309]='listwise/liblistwise/op/xch/xch.xapi.debug.so'
fml_9_47()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/xch/xch.xapi.debug.so listwise/liblistwise/op/xch/xch.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}

NAMES[310]='listwise/liblistwise/op/y/y.xapi.debug.so'
fml_9_48()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/y/y.xapi.debug.so listwise/liblistwise/op/y/y.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[311]='listwise/liblistwise/op/z/z.xapi.debug.so'
fml_9_49()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -fPIC -I./listwise/liblistwise/lwx -I./listwise/liblistwise/object -I./listwise/liblistwise/listwise -I./listwise/liblistwise/operators -I./listwise/liblistwise/operator -I./listwise/liblistwise/exec -I./listwise/liblistwise/describe -I./listwise/liblistwise/selection -I./listwise/liblistwise/tune -I./listwise/liblistwise/lstack -I./listwise/liblistwise/internal -I./listwise/liblistwise/sanity -I./listwise/liblistwise/iterate -I./listwise/liblistwise/logging -I./listwise/liblistwise/op -I./listwise/liblistwise/transform -I./listwise/liblistwise/window -I./listwise/liblistwise/fs -I./listwise/liblistwise/re -I./listwise/liblistwise/error -I./listwise/liblistwise/test -I./listwise/liblistwise -I./libxapi -I./libxlinux -I./libpstring -I./xunit/libxunit -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -DLWOPDIR=/usr/lib/listwise -DLWOPEXT=so -o listwise/liblistwise/op/z/z.xapi.debug.so listwise/liblistwise/op/z/z.xapi.debug.pic.o ./listwise/liblistwise/liblistwise-xapi.debug.so -shared


  X=$?
  echo 1 1>&99
  exit $X
}


# formulas and names for stage 10
NAMES[312]='fab/fablw/op/fg/fg.xapi.debug.so'
fml_10_0()
{
  exec 1>/dev/null
  exec 2>&100

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fablw/op/fg/fg.xapi.debug.so	fab/fablw/op/fg/fg.xapi.debug.pic.o -shared


  X=$?
  echo 0 1>&99
  exit $X
}

NAMES[313]='fab/fablw/op/fi/fi.xapi.debug.so'
fml_10_1()
{
  exec 1>/dev/null
  exec 2>&101

  
	gcc -m64 -Wall -Werror -D_GNU_SOURCE -fplan9-extensions -fno-builtin-log -I./common/canon -I./common/strutil -I./common/map -I./common/parseint -I./common/coll -I./common/memblk -I./common/narrate -I./common/wstdlib -I./common/unitstring -I./common/yyutil -I./common/wstring -I./common/cksum -I./common/logger -I./common/color -I./common -DBUILDSTAMP='Mon Mar 30 2015 18:39:03' -DXAPI_PROVIDE_BACKTRACE -O0 -ggdb3 -DDEBUG -DXAPI_RUNTIME_SANITY_CHECKS -DSANITY -I./fab/fabd/list -I./fab/fabd/selector -I./fab/fabd/traverse -I./fab/fabd/ff -I./fab/fabd/ffproc -I./fab/fabd/tmp -I./fab/fabd/strstack -I./fab/fabd/ts -I./fab/fabd/dsc -I./fab/fabd/gn -I./fab/fabd/dep -I./fab/fabd/enclose -I./fab/fabd/bp -I./fab/fabd/hashblock -I./fab/fabd/lwutil -I./fab/fabd/bs -I./fab/fabd/var -I./fab/fabd/fml -I./fab/fabd/ffn -I./fab/common/identity -I./fab/common/dirutil -I./fab/common/args -I./fab/common/path -I./fab/common/params -I./fab/common/logs -I./fab/common/error -I./fab/fabd -I./fab/common -DFABVERSIONS=v0.6.0.0 -DFABVERSIONN=600 -I./libxapi -I./libxlinux -I./libpstring -I./listwise/liblistwise -DFABLWOPDIR=/usr/lib/fab/listwise -DFABINVOKEDIR=/usr/lib/fab/fablib -DFABTMPDIR=/var/tmp/fab -DFABIPCDIR=/var/run/fab -o fab/fablw/op/fi/fi.xapi.debug.so	fab/fablw/op/fi/fi.xapi.debug.pic.o -shared


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
  ((SKP+=2))
else
  # launch stage 1.0
  exec 100>$tmp ; rm -f $tmp ; fml_1_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_1 & PIDS[1]=$!

  # harvest stage 1.0
  C=0
  while [[ $C != 2 ]]; do
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
  ((SKP+=2))
else
  # launch stage 1.1
  exec 100>$tmp ; rm -f $tmp ; fml_1_2 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_3 & PIDS[1]=$!

  # harvest stage 1.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((3+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+2)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.2
  exec 100>$tmp ; rm -f $tmp ; fml_1_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_5 & PIDS[1]=$!

  # harvest stage 1.2
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((5+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.3
  exec 100>$tmp ; rm -f $tmp ; fml_1_6 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_7 & PIDS[1]=$!

  # harvest stage 1.3
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((7+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+6)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.4
  exec 100>$tmp ; rm -f $tmp ; fml_1_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_9 & PIDS[1]=$!

  # harvest stage 1.4
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((9+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.5
  exec 100>$tmp ; rm -f $tmp ; fml_1_10 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_11 & PIDS[1]=$!

  # harvest stage 1.5
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((11+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+10)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.6
  exec 100>$tmp ; rm -f $tmp ; fml_1_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_13 & PIDS[1]=$!

  # harvest stage 1.6
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((13+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.7
  exec 100>$tmp ; rm -f $tmp ; fml_1_14 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_15 & PIDS[1]=$!

  # harvest stage 1.7
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((15+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+14)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.8
  exec 100>$tmp ; rm -f $tmp ; fml_1_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_17 & PIDS[1]=$!

  # harvest stage 1.8
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((17+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.9
  exec 100>$tmp ; rm -f $tmp ; fml_1_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_19 & PIDS[1]=$!

  # harvest stage 1.9
  C=0
  while [[ $C != 2 ]]; do
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
  ((SKP+=2))
else
  # launch stage 1.10
  exec 100>$tmp ; rm -f $tmp ; fml_1_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_21 & PIDS[1]=$!

  # harvest stage 1.10
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((21+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.11
  exec 100>$tmp ; rm -f $tmp ; fml_1_22 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_23 & PIDS[1]=$!

  # harvest stage 1.11
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((23+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+22)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.12
  exec 100>$tmp ; rm -f $tmp ; fml_1_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_25 & PIDS[1]=$!

  # harvest stage 1.12
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((25+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.13
  exec 100>$tmp ; rm -f $tmp ; fml_1_26 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_27 & PIDS[1]=$!

  # harvest stage 1.13
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((27+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+26)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.14
  exec 100>$tmp ; rm -f $tmp ; fml_1_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_29 & PIDS[1]=$!

  # harvest stage 1.14
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((29+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.15
  exec 100>$tmp ; rm -f $tmp ; fml_1_30 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_31 & PIDS[1]=$!

  # harvest stage 1.15
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((31+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+30)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.16
  exec 100>$tmp ; rm -f $tmp ; fml_1_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_33 & PIDS[1]=$!

  # harvest stage 1.16
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((33+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.17
  exec 100>$tmp ; rm -f $tmp ; fml_1_34 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_35 & PIDS[1]=$!

  # harvest stage 1.17
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((35+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+34)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.18
  exec 100>$tmp ; rm -f $tmp ; fml_1_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_37 & PIDS[1]=$!

  # harvest stage 1.18
  C=0
  while [[ $C != 2 ]]; do
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
  ((SKP+=2))
else
  # launch stage 1.19
  exec 100>$tmp ; rm -f $tmp ; fml_1_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_39 & PIDS[1]=$!

  # harvest stage 1.19
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((39+$idx))
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
  ((SKP+=2))
else
  # launch stage 1.20
  exec 100>$tmp ; rm -f $tmp ; fml_1_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_41 & PIDS[1]=$!

  # harvest stage 1.20
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((41+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.21
  exec 100>$tmp ; rm -f $tmp ; fml_1_42 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_43 & PIDS[1]=$!

  # harvest stage 1.21
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((43+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+42)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.22
  exec 100>$tmp ; rm -f $tmp ; fml_1_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_45 & PIDS[1]=$!

  # harvest stage 1.22
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((45+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.23
  exec 100>$tmp ; rm -f $tmp ; fml_1_46 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_47 & PIDS[1]=$!

  # harvest stage 1.23
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((47+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+46)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.24
  exec 100>$tmp ; rm -f $tmp ; fml_1_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_49 & PIDS[1]=$!

  # harvest stage 1.24
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((49+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+48)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 1.25
  exec 100>$tmp ; rm -f $tmp ; fml_1_50 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_1_51 & PIDS[1]=$!

  # harvest stage 1.25
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((51+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+50)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 1.26
  exec 100>$tmp ; rm -f $tmp ; fml_1_52 & PIDS[0]=$!

  # harvest stage 1.26
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((53+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 1 $((idx+52)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.0
  exec 100>$tmp ; rm -f $tmp ; fml_2_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_1 & PIDS[1]=$!

  # harvest stage 2.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((54+$idx))
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
  exec 100>$tmp ; rm -f $tmp ; fml_2_2 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_3 & PIDS[1]=$!

  # harvest stage 2.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((56+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+2)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.2
  exec 100>$tmp ; rm -f $tmp ; fml_2_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_5 & PIDS[1]=$!

  # harvest stage 2.2
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((58+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.3
  exec 100>$tmp ; rm -f $tmp ; fml_2_6 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_7 & PIDS[1]=$!

  # harvest stage 2.3
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((60+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+6)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.4
  exec 100>$tmp ; rm -f $tmp ; fml_2_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_9 & PIDS[1]=$!

  # harvest stage 2.4
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((62+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.5
  exec 100>$tmp ; rm -f $tmp ; fml_2_10 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_11 & PIDS[1]=$!

  # harvest stage 2.5
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((64+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+10)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.6
  exec 100>$tmp ; rm -f $tmp ; fml_2_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_13 & PIDS[1]=$!

  # harvest stage 2.6
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((66+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.7
  exec 100>$tmp ; rm -f $tmp ; fml_2_14 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_15 & PIDS[1]=$!

  # harvest stage 2.7
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((68+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+14)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.8
  exec 100>$tmp ; rm -f $tmp ; fml_2_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_17 & PIDS[1]=$!

  # harvest stage 2.8
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((70+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.9
  exec 100>$tmp ; rm -f $tmp ; fml_2_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_19 & PIDS[1]=$!

  # harvest stage 2.9
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((72+$idx))
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
  ((SKP+=2))
else
  # launch stage 2.10
  exec 100>$tmp ; rm -f $tmp ; fml_2_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_21 & PIDS[1]=$!

  # harvest stage 2.10
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((74+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.11
  exec 100>$tmp ; rm -f $tmp ; fml_2_22 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_23 & PIDS[1]=$!

  # harvest stage 2.11
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((76+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+22)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.12
  exec 100>$tmp ; rm -f $tmp ; fml_2_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_25 & PIDS[1]=$!

  # harvest stage 2.12
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((78+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.13
  exec 100>$tmp ; rm -f $tmp ; fml_2_26 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_27 & PIDS[1]=$!

  # harvest stage 2.13
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((80+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+26)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.14
  exec 100>$tmp ; rm -f $tmp ; fml_2_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_29 & PIDS[1]=$!

  # harvest stage 2.14
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((82+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.15
  exec 100>$tmp ; rm -f $tmp ; fml_2_30 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_31 & PIDS[1]=$!

  # harvest stage 2.15
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((84+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+30)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.16
  exec 100>$tmp ; rm -f $tmp ; fml_2_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_33 & PIDS[1]=$!

  # harvest stage 2.16
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((86+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.17
  exec 100>$tmp ; rm -f $tmp ; fml_2_34 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_35 & PIDS[1]=$!

  # harvest stage 2.17
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((88+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+34)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.18
  exec 100>$tmp ; rm -f $tmp ; fml_2_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_37 & PIDS[1]=$!

  # harvest stage 2.18
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((90+$idx))
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
  ((SKP+=2))
else
  # launch stage 2.19
  exec 100>$tmp ; rm -f $tmp ; fml_2_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_39 & PIDS[1]=$!

  # harvest stage 2.19
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((92+$idx))
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
  ((SKP+=2))
else
  # launch stage 2.20
  exec 100>$tmp ; rm -f $tmp ; fml_2_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_41 & PIDS[1]=$!

  # harvest stage 2.20
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((94+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.21
  exec 100>$tmp ; rm -f $tmp ; fml_2_42 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_43 & PIDS[1]=$!

  # harvest stage 2.21
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((96+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+42)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.22
  exec 100>$tmp ; rm -f $tmp ; fml_2_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_45 & PIDS[1]=$!

  # harvest stage 2.22
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((98+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.23
  exec 100>$tmp ; rm -f $tmp ; fml_2_46 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_47 & PIDS[1]=$!

  # harvest stage 2.23
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((100+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+46)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.24
  exec 100>$tmp ; rm -f $tmp ; fml_2_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_49 & PIDS[1]=$!

  # harvest stage 2.24
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((102+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+48)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.25
  exec 100>$tmp ; rm -f $tmp ; fml_2_50 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_51 & PIDS[1]=$!

  # harvest stage 2.25
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((104+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+50)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.26
  exec 100>$tmp ; rm -f $tmp ; fml_2_52 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_53 & PIDS[1]=$!

  # harvest stage 2.26
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((106+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+52)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.27
  exec 100>$tmp ; rm -f $tmp ; fml_2_54 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_55 & PIDS[1]=$!

  # harvest stage 2.27
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((108+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+54)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.28
  exec 100>$tmp ; rm -f $tmp ; fml_2_56 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_57 & PIDS[1]=$!

  # harvest stage 2.28
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((110+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+56)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.29
  exec 100>$tmp ; rm -f $tmp ; fml_2_58 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_59 & PIDS[1]=$!

  # harvest stage 2.29
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((112+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+58)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.30
  exec 100>$tmp ; rm -f $tmp ; fml_2_60 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_61 & PIDS[1]=$!

  # harvest stage 2.30
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((114+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+60)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 2.31
  exec 100>$tmp ; rm -f $tmp ; fml_2_62 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_2_63 & PIDS[1]=$!

  # harvest stage 2.31
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((116+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+62)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 2.32
  exec 100>$tmp ; rm -f $tmp ; fml_2_64 & PIDS[0]=$!

  # harvest stage 2.32
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((118+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 2 $((idx+64)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.0
  exec 100>$tmp ; rm -f $tmp ; fml_3_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_1 & PIDS[1]=$!

  # harvest stage 3.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((119+$idx))
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
  ((SKP+=2))
else
  # launch stage 3.1
  exec 100>$tmp ; rm -f $tmp ; fml_3_2 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_3 & PIDS[1]=$!

  # harvest stage 3.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((121+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+2)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.2
  exec 100>$tmp ; rm -f $tmp ; fml_3_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_5 & PIDS[1]=$!

  # harvest stage 3.2
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((123+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.3
  exec 100>$tmp ; rm -f $tmp ; fml_3_6 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_7 & PIDS[1]=$!

  # harvest stage 3.3
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((125+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+6)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.4
  exec 100>$tmp ; rm -f $tmp ; fml_3_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_9 & PIDS[1]=$!

  # harvest stage 3.4
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((127+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.5
  exec 100>$tmp ; rm -f $tmp ; fml_3_10 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_11 & PIDS[1]=$!

  # harvest stage 3.5
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((129+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+10)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.6
  exec 100>$tmp ; rm -f $tmp ; fml_3_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_13 & PIDS[1]=$!

  # harvest stage 3.6
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((131+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.7
  exec 100>$tmp ; rm -f $tmp ; fml_3_14 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_15 & PIDS[1]=$!

  # harvest stage 3.7
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((133+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+14)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.8
  exec 100>$tmp ; rm -f $tmp ; fml_3_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_17 & PIDS[1]=$!

  # harvest stage 3.8
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((135+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.9
  exec 100>$tmp ; rm -f $tmp ; fml_3_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_19 & PIDS[1]=$!

  # harvest stage 3.9
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((137+$idx))
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
  ((SKP+=2))
else
  # launch stage 3.10
  exec 100>$tmp ; rm -f $tmp ; fml_3_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_21 & PIDS[1]=$!

  # harvest stage 3.10
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((139+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.11
  exec 100>$tmp ; rm -f $tmp ; fml_3_22 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_23 & PIDS[1]=$!

  # harvest stage 3.11
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((141+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+22)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.12
  exec 100>$tmp ; rm -f $tmp ; fml_3_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_25 & PIDS[1]=$!

  # harvest stage 3.12
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((143+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.13
  exec 100>$tmp ; rm -f $tmp ; fml_3_26 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_27 & PIDS[1]=$!

  # harvest stage 3.13
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((145+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+26)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.14
  exec 100>$tmp ; rm -f $tmp ; fml_3_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_29 & PIDS[1]=$!

  # harvest stage 3.14
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((147+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.15
  exec 100>$tmp ; rm -f $tmp ; fml_3_30 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_31 & PIDS[1]=$!

  # harvest stage 3.15
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((149+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+30)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.16
  exec 100>$tmp ; rm -f $tmp ; fml_3_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_33 & PIDS[1]=$!

  # harvest stage 3.16
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((151+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.17
  exec 100>$tmp ; rm -f $tmp ; fml_3_34 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_35 & PIDS[1]=$!

  # harvest stage 3.17
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((153+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+34)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.18
  exec 100>$tmp ; rm -f $tmp ; fml_3_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_37 & PIDS[1]=$!

  # harvest stage 3.18
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((155+$idx))
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
  ((SKP+=2))
else
  # launch stage 3.19
  exec 100>$tmp ; rm -f $tmp ; fml_3_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_39 & PIDS[1]=$!

  # harvest stage 3.19
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((157+$idx))
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
  ((SKP+=2))
else
  # launch stage 3.20
  exec 100>$tmp ; rm -f $tmp ; fml_3_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_41 & PIDS[1]=$!

  # harvest stage 3.20
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((159+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.21
  exec 100>$tmp ; rm -f $tmp ; fml_3_42 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_43 & PIDS[1]=$!

  # harvest stage 3.21
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((161+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+42)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.22
  exec 100>$tmp ; rm -f $tmp ; fml_3_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_45 & PIDS[1]=$!

  # harvest stage 3.22
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((163+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.23
  exec 100>$tmp ; rm -f $tmp ; fml_3_46 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_47 & PIDS[1]=$!

  # harvest stage 3.23
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((165+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+46)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.24
  exec 100>$tmp ; rm -f $tmp ; fml_3_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_49 & PIDS[1]=$!

  # harvest stage 3.24
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((167+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+48)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.25
  exec 100>$tmp ; rm -f $tmp ; fml_3_50 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_51 & PIDS[1]=$!

  # harvest stage 3.25
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((169+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+50)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.26
  exec 100>$tmp ; rm -f $tmp ; fml_3_52 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_53 & PIDS[1]=$!

  # harvest stage 3.26
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((171+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+52)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.27
  exec 100>$tmp ; rm -f $tmp ; fml_3_54 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_55 & PIDS[1]=$!

  # harvest stage 3.27
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((173+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+54)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.28
  exec 100>$tmp ; rm -f $tmp ; fml_3_56 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_57 & PIDS[1]=$!

  # harvest stage 3.28
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((175+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+56)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.29
  exec 100>$tmp ; rm -f $tmp ; fml_3_58 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_59 & PIDS[1]=$!

  # harvest stage 3.29
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((177+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+58)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.30
  exec 100>$tmp ; rm -f $tmp ; fml_3_60 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_61 & PIDS[1]=$!

  # harvest stage 3.30
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((179+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+60)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.31
  exec 100>$tmp ; rm -f $tmp ; fml_3_62 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_63 & PIDS[1]=$!

  # harvest stage 3.31
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((181+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+62)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.32
  exec 100>$tmp ; rm -f $tmp ; fml_3_64 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_65 & PIDS[1]=$!

  # harvest stage 3.32
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((183+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+64)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.33
  exec 100>$tmp ; rm -f $tmp ; fml_3_66 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_67 & PIDS[1]=$!

  # harvest stage 3.33
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((185+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+66)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.34
  exec 100>$tmp ; rm -f $tmp ; fml_3_68 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_69 & PIDS[1]=$!

  # harvest stage 3.34
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((187+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+68)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.35
  exec 100>$tmp ; rm -f $tmp ; fml_3_70 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_71 & PIDS[1]=$!

  # harvest stage 3.35
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((189+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+70)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.36
  exec 100>$tmp ; rm -f $tmp ; fml_3_72 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_73 & PIDS[1]=$!

  # harvest stage 3.36
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((191+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+72)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.37
  exec 100>$tmp ; rm -f $tmp ; fml_3_74 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_75 & PIDS[1]=$!

  # harvest stage 3.37
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((193+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+74)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.38
  exec 100>$tmp ; rm -f $tmp ; fml_3_76 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_77 & PIDS[1]=$!

  # harvest stage 3.38
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((195+$idx))
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
  ((SKP+=2))
else
  # launch stage 3.39
  exec 100>$tmp ; rm -f $tmp ; fml_3_78 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_79 & PIDS[1]=$!

  # harvest stage 3.39
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((197+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+78)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 3.40
  exec 100>$tmp ; rm -f $tmp ; fml_3_80 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_3_81 & PIDS[1]=$!

  # harvest stage 3.40
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((199+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 3 $((idx+80)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 4.0
  exec 100>$tmp ; rm -f $tmp ; fml_4_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_4_1 & PIDS[1]=$!

  # harvest stage 4.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((201+$idx))
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
  ((SKP+=2))
else
  # launch stage 5.0
  exec 100>$tmp ; rm -f $tmp ; fml_5_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_5_1 & PIDS[1]=$!

  # harvest stage 5.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((203+$idx))
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
  ((SKP+=2))
else
  # launch stage 6.0
  exec 100>$tmp ; rm -f $tmp ; fml_6_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_6_1 & PIDS[1]=$!

  # harvest stage 6.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((205+$idx))
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
  ((SKP+=2))
else
  # launch stage 7.0
  exec 100>$tmp ; rm -f $tmp ; fml_7_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_7_1 & PIDS[1]=$!

  # harvest stage 7.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((207+$idx))
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
  ((SKP+=2))
else
  # launch stage 8.0
  exec 100>$tmp ; rm -f $tmp ; fml_8_0 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_1 & PIDS[1]=$!

  # harvest stage 8.0
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((209+$idx))
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
  ((SKP+=2))
else
  # launch stage 8.1
  exec 100>$tmp ; rm -f $tmp ; fml_8_2 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_3 & PIDS[1]=$!

  # harvest stage 8.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((211+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+2)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.2
  exec 100>$tmp ; rm -f $tmp ; fml_8_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_5 & PIDS[1]=$!

  # harvest stage 8.2
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((213+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.3
  exec 100>$tmp ; rm -f $tmp ; fml_8_6 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_7 & PIDS[1]=$!

  # harvest stage 8.3
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((215+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+6)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.4
  exec 100>$tmp ; rm -f $tmp ; fml_8_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_9 & PIDS[1]=$!

  # harvest stage 8.4
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((217+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.5
  exec 100>$tmp ; rm -f $tmp ; fml_8_10 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_11 & PIDS[1]=$!

  # harvest stage 8.5
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((219+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+10)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.6
  exec 100>$tmp ; rm -f $tmp ; fml_8_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_13 & PIDS[1]=$!

  # harvest stage 8.6
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((221+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.7
  exec 100>$tmp ; rm -f $tmp ; fml_8_14 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_15 & PIDS[1]=$!

  # harvest stage 8.7
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((223+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+14)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.8
  exec 100>$tmp ; rm -f $tmp ; fml_8_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_17 & PIDS[1]=$!

  # harvest stage 8.8
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((225+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.9
  exec 100>$tmp ; rm -f $tmp ; fml_8_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_19 & PIDS[1]=$!

  # harvest stage 8.9
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((227+$idx))
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
  ((SKP+=2))
else
  # launch stage 8.10
  exec 100>$tmp ; rm -f $tmp ; fml_8_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_21 & PIDS[1]=$!

  # harvest stage 8.10
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((229+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.11
  exec 100>$tmp ; rm -f $tmp ; fml_8_22 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_23 & PIDS[1]=$!

  # harvest stage 8.11
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
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+22)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.12
  exec 100>$tmp ; rm -f $tmp ; fml_8_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_25 & PIDS[1]=$!

  # harvest stage 8.12
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((233+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.13
  exec 100>$tmp ; rm -f $tmp ; fml_8_26 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_27 & PIDS[1]=$!

  # harvest stage 8.13
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
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+26)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.14
  exec 100>$tmp ; rm -f $tmp ; fml_8_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_29 & PIDS[1]=$!

  # harvest stage 8.14
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((237+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.15
  exec 100>$tmp ; rm -f $tmp ; fml_8_30 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_31 & PIDS[1]=$!

  # harvest stage 8.15
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((239+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+30)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.16
  exec 100>$tmp ; rm -f $tmp ; fml_8_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_33 & PIDS[1]=$!

  # harvest stage 8.16
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((241+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.17
  exec 100>$tmp ; rm -f $tmp ; fml_8_34 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_35 & PIDS[1]=$!

  # harvest stage 8.17
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((243+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+34)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.18
  exec 100>$tmp ; rm -f $tmp ; fml_8_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_37 & PIDS[1]=$!

  # harvest stage 8.18
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
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+36)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.19
  exec 100>$tmp ; rm -f $tmp ; fml_8_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_39 & PIDS[1]=$!

  # harvest stage 8.19
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((247+$idx))
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
  # launch stage 8.20
  exec 100>$tmp ; rm -f $tmp ; fml_8_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_41 & PIDS[1]=$!

  # harvest stage 8.20
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((249+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.21
  exec 100>$tmp ; rm -f $tmp ; fml_8_42 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_43 & PIDS[1]=$!

  # harvest stage 8.21
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((251+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+42)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.22
  exec 100>$tmp ; rm -f $tmp ; fml_8_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_45 & PIDS[1]=$!

  # harvest stage 8.22
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((253+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.23
  exec 100>$tmp ; rm -f $tmp ; fml_8_46 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_47 & PIDS[1]=$!

  # harvest stage 8.23
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((255+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+46)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.24
  exec 100>$tmp ; rm -f $tmp ; fml_8_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_49 & PIDS[1]=$!

  # harvest stage 8.24
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((257+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+48)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 8.25
  exec 100>$tmp ; rm -f $tmp ; fml_8_50 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_8_51 & PIDS[1]=$!

  # harvest stage 8.25
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((259+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+50)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=1))
else
  # launch stage 8.26
  exec 100>$tmp ; rm -f $tmp ; fml_8_52 & PIDS[0]=$!

  # harvest stage 8.26
  C=0
  while [[ $C != 1 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((261+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 8 $((idx+52)) $X "$N"
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
    I=$((262+$idx))
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
  # launch stage 9.1
  exec 100>$tmp ; rm -f $tmp ; fml_9_2 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_3 & PIDS[1]=$!

  # harvest stage 9.1
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((264+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+2)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.2
  exec 100>$tmp ; rm -f $tmp ; fml_9_4 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_5 & PIDS[1]=$!

  # harvest stage 9.2
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((266+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+4)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.3
  exec 100>$tmp ; rm -f $tmp ; fml_9_6 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_7 & PIDS[1]=$!

  # harvest stage 9.3
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((268+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+6)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.4
  exec 100>$tmp ; rm -f $tmp ; fml_9_8 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_9 & PIDS[1]=$!

  # harvest stage 9.4
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((270+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+8)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.5
  exec 100>$tmp ; rm -f $tmp ; fml_9_10 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_11 & PIDS[1]=$!

  # harvest stage 9.5
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((272+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+10)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.6
  exec 100>$tmp ; rm -f $tmp ; fml_9_12 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_13 & PIDS[1]=$!

  # harvest stage 9.6
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((274+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+12)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.7
  exec 100>$tmp ; rm -f $tmp ; fml_9_14 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_15 & PIDS[1]=$!

  # harvest stage 9.7
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((276+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+14)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.8
  exec 100>$tmp ; rm -f $tmp ; fml_9_16 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_17 & PIDS[1]=$!

  # harvest stage 9.8
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((278+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+16)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.9
  exec 100>$tmp ; rm -f $tmp ; fml_9_18 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_19 & PIDS[1]=$!

  # harvest stage 9.9
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((280+$idx))
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
  ((SKP+=2))
else
  # launch stage 9.10
  exec 100>$tmp ; rm -f $tmp ; fml_9_20 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_21 & PIDS[1]=$!

  # harvest stage 9.10
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((282+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+20)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.11
  exec 100>$tmp ; rm -f $tmp ; fml_9_22 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_23 & PIDS[1]=$!

  # harvest stage 9.11
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((284+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+22)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.12
  exec 100>$tmp ; rm -f $tmp ; fml_9_24 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_25 & PIDS[1]=$!

  # harvest stage 9.12
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((286+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+24)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.13
  exec 100>$tmp ; rm -f $tmp ; fml_9_26 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_27 & PIDS[1]=$!

  # harvest stage 9.13
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((288+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+26)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.14
  exec 100>$tmp ; rm -f $tmp ; fml_9_28 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_29 & PIDS[1]=$!

  # harvest stage 9.14
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((290+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+28)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.15
  exec 100>$tmp ; rm -f $tmp ; fml_9_30 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_31 & PIDS[1]=$!

  # harvest stage 9.15
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((292+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+30)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.16
  exec 100>$tmp ; rm -f $tmp ; fml_9_32 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_33 & PIDS[1]=$!

  # harvest stage 9.16
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((294+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+32)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.17
  exec 100>$tmp ; rm -f $tmp ; fml_9_34 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_35 & PIDS[1]=$!

  # harvest stage 9.17
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((296+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+34)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.18
  exec 100>$tmp ; rm -f $tmp ; fml_9_36 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_37 & PIDS[1]=$!

  # harvest stage 9.18
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((298+$idx))
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
  ((SKP+=2))
else
  # launch stage 9.19
  exec 100>$tmp ; rm -f $tmp ; fml_9_38 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_39 & PIDS[1]=$!

  # harvest stage 9.19
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((300+$idx))
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
  # launch stage 9.20
  exec 100>$tmp ; rm -f $tmp ; fml_9_40 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_41 & PIDS[1]=$!

  # harvest stage 9.20
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((302+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+40)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.21
  exec 100>$tmp ; rm -f $tmp ; fml_9_42 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_43 & PIDS[1]=$!

  # harvest stage 9.21
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((304+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+42)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.22
  exec 100>$tmp ; rm -f $tmp ; fml_9_44 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_45 & PIDS[1]=$!

  # harvest stage 9.22
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((306+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+44)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.23
  exec 100>$tmp ; rm -f $tmp ; fml_9_46 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_47 & PIDS[1]=$!

  # harvest stage 9.23
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((308+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+46)) $X "$N"
    cat /proc/$$/fd/$((100+idx))
    ((C++))
  done
fi

# early termination 
if [[ $DIE -ne 0 ]]; then
  ((SKP+=2))
else
  # launch stage 9.24
  exec 100>$tmp ; rm -f $tmp ; fml_9_48 & PIDS[0]=$!
  exec 101>$tmp ; rm -f $tmp ; fml_9_49 & PIDS[1]=$!

  # harvest stage 9.24
  C=0
  while [[ $C != 2 ]]; do
    read -u 99 idx
    wait ${PIDS[$idx]}
    EXITS[$idx]=$?
    P=${PIDS[$idx]}
    X=${EXITS[$idx]}
    I=$((310+$idx))
    N=${NAMES[$I]}
    [[ $X -eq 0 ]] && ((WIN++))
    [[ $X -ne 0 ]] && ((DIE++))
    printf '[%3d,%3d] X=%d %s\n' 9 $((idx+48)) $X "$N"
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
    I=$((312+$idx))
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
