#!/usr/bin/env bash

set -e

#
# installs binaries/libraries built by fab-v0.7
#

# require root
if [[ $UID != 0 || $EUID != 0 ]]; then
  echo "run as root!" 1>2
  exit 1
fi

v=final
bindir=/usr/local/bin
libdir=/usr/lib
rundir=/var/run
sharedir=/usr/share
etcdir=/etc

# filesystem
mkdir -p       $rundir/fab/
chmod 777 -R   $rundir/fab/
mkdir -p       $sharedir/fab/modules/
mkdir -p       $etcdir/bam/

# executables
set -x
cp fab/fab/fab.$v.xapi       $bindir/fab
cp fab/fabd/fabd.$v.xapi     $bindir/fabd
cp fab/fabc/fabc.$v.xapi     $bindir/fabc
cp value/value/value.$v.xapi $bindir/value

# libraries
cp libxapi/libxapi.$v.pic.xapi.so                  $libdir/libxapi.so
cp libxlinux/libxlinux.$v.pic.xapi.so              $libdir/libxlinux.so
cp libnarrator/libnarrator.$v.pic.xapi.so          $libdir/libnarrator.so
cp liblogger/liblogger.$v.pic.xapi.so              $libdir/liblogger.so
cp liblorien/liblorien.$v.pic.xapi.so              $libdir/liblorien.so
cp libmoria/libmoria.$v.pic.xapi.so                $libdir/libmoria.so
cp libvalyria/libvalyria.$v.pic.xapi.so            $libdir/libvalyria.so
cp libyyutil/libyyutil.$v.pic.xapi.so              $libdir/libyyutil.so
cp value/libvalue/libvalue.$v.pic.xapi.so          $libdir/libvalue.so
cp fab/libfab/libfab.$v.pic.xapi.so                $libdir/libfab.so
cp libcommon/libcommon.$v.pic.xapi.so              $libdir/libcommon.so
cp libdescriptor/libdescriptor.$v.pic.xapi.so      $libdir/libdescriptor.so
set +x

# builtin modules
for x in $(find builtin-modules -type f | grep -v +devel); do
  frag=${x##builtin-modules/}
  mkdir -p $(dirname $sharedir/fab/modules/$frag/)
  set -x
  cp $x $sharedir/fab/modules/$frag
  set +x
done

# default config files
set -x
cp etc/fabconfig          $etcdir/fabconfig
cp etc/bam/client_config  $etcdir/bam/client_config
set +x
