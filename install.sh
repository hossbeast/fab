#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>
#

#
# installs binaries/libraries built by fab-v0.8
#

# requires root
if [[ $UID != 0 || $EUID != 0 ]]; then
  echo "run as root!" 1>&2
  exit 1
fi

variant=final
bindir=/usr/bin
libdir=/usr/lib
rundir=/var/run
sharedir=/usr/share
etcdir=/etc

killall fab fab.devel fabd fabd.devel
if [[ $? == 0 ]]; then
  sleep 1
fi

set -e
set -x

# filesystem
mkdir -p       $rundir/fab/
chmod 777 -R   $rundir/fab/
mkdir -p       $sharedir/fab/modules/
mkdir -p       $etcdir/bam/

# executables
cp fab/fab/fab.$variant       $bindir/fab
cp fab/fabd/fabd.$variant     $bindir/fabd
cp bacon/bacon.$variant       $bindir/bacon

# builtin modules
rm -rf $sharedir/fab/modules/
cp -dR builtin-modules $sharedir/fab/modules/

# default config files
cp etc/config.bam         $etcdir/config.bam
cp etc/bam/client_config  $etcdir/bam/client_config
