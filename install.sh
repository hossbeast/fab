#!/usr/bin/env bash

set -e
set -x

#
# installs binaries/libraries built by fab-v0.6
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

# executables
cp fab/fab/fab.$v.xapi       $bindir/fab
cp fab/fabw/fabw.$v.xapi     $bindir/fabw
cp fab/fabd/fabd.$v.xapi     $bindir/fabd
cp fab/fabc/fabc.$v.xapi     $bindir/fabc
cp value/value/value.$v.xapi $bindir/value

# libraries
cp libxapi/libxapi.$v.pic.xapi.so            $libdir/libxapi.so
cp libxlinux/libxlinux.$v.pic.xapi.so        $libdir/libxlinux.so
cp libnarrator/libnarrator.$v.pic.xapi.so    $libdir/libnarrator.so
cp liblogger/liblogger.$v.pic.xapi.so        $libdir/liblogger.so
cp liblorien/liblorien.$v.pic.xapi.so        $libdir/liblorien.so
cp libmoria/libmoria.$v.pic.xapi.so          $libdir/libmoria.so
cp libvalyria/libvalyria.$v.pic.xapi.so      $libdir/libvalyria.so
cp libyyutil/libyyutil.$v.pic.xapi.so        $libdir/libyyutil.so
cp value/libvalue/libvalue.$v.pic.xapi.so    $libdir/libvalue.so
cp fab/libfab/libfab.$v.pic.xapi.so          $libdir/libfab.so

# filesystem
mkdir -p       $rundir/fab
chmod ugoa+rwx $rundir/fab
