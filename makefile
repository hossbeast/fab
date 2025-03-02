# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all:

include config.mak
include makefile.common

all:
	$(MAKE) -C libxlinux
	$(MAKE) -C libcommon
	$(MAKE) -C libnarrator
	$(MAKE) -C libdescriptor
	$(MAKE) -C libvalyria
	$(MAKE) -C libyyutil
	$(MAKE) -C liblorien
	$(MAKE) -C libbacon
	$(MAKE) -C libmoria
	$(MAKE) -C bacon
	$(MAKE) -C libbam
	$(MAKE) -C bam
	$(MAKE) -C bamd

#	$(MAKE) -C asan
