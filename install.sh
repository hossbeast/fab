#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>
#

#
# install bam and related executables
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

killall bam bamd
if [[ $? == 0 ]]; then
  sleep 1
fi

set -e
set -x

# filesystem
mkdir -p       $rundir/bam/
chmod 777 -R   $rundir/bam/
mkdir -p       $sharedir/bam/modules/
mkdir -p       $etcdir/bam/

# executables
cp ~/bam-build/bam/$variant/bam       $bindir/bam
cp ~/bam-build/bamd/$variant/bamd     $bindir/bamd
cp ~/bam-build/bacon/$variant/bacon   $bindir/bacon

# builtin modules
rm -rf $sharedir/bam/modules/
cp -dR builtin-modules $sharedir/bam/modules/
chmod 0755 $sharedir/bam/modules/**/*.bam
chmod 0644 $sharedir/bam/modules/**/rules.bam

# default config files
cp etc/config.bam         $etcdir/config.bam
cp etc/bam/client_config  $etcdir/bam/client_config
