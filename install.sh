#!/usr/bin/env bash

set -e
set -x

#
# installs binaries/libraries built by fab-v0.5
#

# require root
if [[ $UID != 0 || $EUID != 0 ]]; then
  echo "run as root!" 1>2
  exit 1
fi

v=debug
bindir=/usr/local/bin
libdir=/usr/lib
rundir=/var/run

# executables
cp fab/fab/fab.$v       $bindir/fab
cp fab/fabw/fabw.$v     $bindir/fabw
cp fab/fabd/fabd.$v     $bindir/fabd
cp value/value/value.$v $bindir/value

# libraries
cp libxapi/libxapi-xapi.$v.so            $libdir/libxapi-xapi.so
cp libxlinux/libxlinux-xapi.$v.so        $libdir/libxlinux-xapi.so
cp libnarrator/libnarrator-xapi.$v.so    $libdir/libnarrator-xapi.so
cp liblogger/liblogger-xapi.$v.so        $libdir/liblogger-xapi.so
cp liblorien/liblorien-xapi.$v.so        $libdir/liblorien-xapi.so
cp libmoria/libmoria-xapi.$v.so          $libdir/libmoria-xapi.so
cp libvalyria/libvalyria-xapi.$v.so      $libdir/libvalyria-xapi.so
cp libyyutil/libyyutil-xapi.$v.so        $libdir/libyyutil-xapi.so
cp value/libvalue/libvalue-xapi.$v.so    $libdir/libvalue-xapi.so
cp fab/libfab/libfab-xapi.$v.so          $libdir/libfab-xapi.so

# filesystem
mkdir -p       $rundir/fab
chmod ugoa+rwx $rundir/fab
