# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: bacon

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/bacon

LDFLAGS += -pie

CFLAGS += -I$(SRC_PATH)/bacon/args
CFLAGS += -I$(BUILD_PATH)/bacon

bacon.exe: args/args.o
bacon.exe: main.o
bacon.exe: git-state.o

bacon.exe: $(BUILD_PATH)/libbacon/libbacon.a
bacon.exe: $(BUILD_PATH)/libxlinux/libxlinux.a
bacon.exe: $(BUILD_PATH)/libvalyria/libvalyria.a
bacon.exe: $(BUILD_PATH)/libnarrator/libnarrator.a
bacon.exe: $(BUILD_PATH)/libyyutil/libyyutil.a
bacon.exe: $(BUILD_PATH)/libcommon/libcommon.a

bacon: bacon.exe
	ln -vfs $^ $@
