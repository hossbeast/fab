# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: liblorien.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/liblorien

CFLAGS += -I$(SRC_PATH)/liblorien/load
CFLAGS += -I$(SRC_PATH)/liblorien/mkdirp
CFLAGS += -I$(SRC_PATH)/liblorien/nftwat
CFLAGS += -I$(SRC_PATH)/liblorien/path_normalize
CFLAGS += -I$(SRC_PATH)/liblorien/rmdirp
CFLAGS += -I$(SRC_PATH)/liblorien/symlinkp

liblorien.a: load/load.o
liblorien.a: mkdirp/mkdirp.o
liblorien.a: nftwat/nftwat.o
liblorien.a: path_normalize/path_normalize.o
liblorien.a: rmdirp/rmdirp.o
liblorien.a: symlinkp/symlinkp.o
