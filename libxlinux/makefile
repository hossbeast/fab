# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libxlinux.a

include ../config.mak
include ../makefile.common
VPATH=$(SRC_PATH)/libxlinux

CFLAGS += -I$(SRC_PATH)/libxlinux/fmt
CFLAGS += -I$(SRC_PATH)/libxlinux/include
CFLAGS += -I$(SRC_PATH)/libxlinux/xdirent
CFLAGS += -I$(SRC_PATH)/libxlinux/xdlfcn
CFLAGS += -I$(SRC_PATH)/libxlinux/xepoll
CFLAGS += -I$(SRC_PATH)/libxlinux/xfcntl
CFLAGS += -I$(SRC_PATH)/libxlinux/xfile
CFLAGS += -I$(SRC_PATH)/libxlinux/xfutex
CFLAGS += -I$(SRC_PATH)/libxlinux/xgrp
CFLAGS += -I$(SRC_PATH)/libxlinux/xinet
CFLAGS += -I$(SRC_PATH)/libxlinux/xinotify
CFLAGS += -I$(SRC_PATH)/libxlinux/xmman
CFLAGS += -I$(SRC_PATH)/libxlinux/xprctl
CFLAGS += -I$(SRC_PATH)/libxlinux/xpthread
CFLAGS += -I$(SRC_PATH)/libxlinux/xresource
CFLAGS += -I$(SRC_PATH)/libxlinux/xshm
CFLAGS += -I$(SRC_PATH)/libxlinux/xsignal
CFLAGS += -I$(SRC_PATH)/libxlinux/xsocket
CFLAGS += -I$(SRC_PATH)/libxlinux/xstat
CFLAGS += -I$(SRC_PATH)/libxlinux/xstdio
CFLAGS += -I$(SRC_PATH)/libxlinux/xstdlib
CFLAGS += -I$(SRC_PATH)/libxlinux/xstring
CFLAGS += -I$(SRC_PATH)/libxlinux/xtime
CFLAGS += -I$(SRC_PATH)/libxlinux/xunistd
CFLAGS += -I$(SRC_PATH)/libxlinux/xwait
CFLAGS += -I$(BUILD_PATH)/libxlinux

libxlinux.a: fmt/fmt.o
libxlinux.a: xdirent/xdirent.o
libxlinux.a: xdlfcn/xdlfcn.o
libxlinux.a: xepoll/xepoll.o
libxlinux.a: xfcntl/xfcntl.o
libxlinux.a: xfile/xfile.o
libxlinux.a: xfutex/xfutex.o
libxlinux.a: xgrp/xgrp.o
libxlinux.a: xinet/xinet.o
libxlinux.a: xinotify/xinotify.o
libxlinux.a: xmman/xmman.o
libxlinux.a: xprctl/xprctl.o
libxlinux.a: xpthread/xpthread.o
libxlinux.a: xresource/xresource.o
libxlinux.a: xshm/xshm.o
libxlinux.a: xsignal/xsignal.o
libxlinux.a: xsocket/xsocket.o
libxlinux.a: xstat/xstat.o
libxlinux.a: xstdio/xstdio.o
libxlinux.a: xstdlib/xstdlib.o
libxlinux.a: xstring/xstring.o
libxlinux.a: xtime/xtime.o
libxlinux.a: xunistd/xunistd.o
libxlinux.a: xwait/xwait.o
libxlinux.a: sigtable.o

sigtable.c sigtable.h :
	cc=`command -v cc` bm_target0=sigtable.c bm_target1=sigtable.h git=`command -v git` $(ROOT_PATH)/libxlinux/sigtable.bam
