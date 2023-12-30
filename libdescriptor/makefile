# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libdescriptor.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libdescriptor

CFLAGS += -I$(SRC_PATH)/libdescriptor/marshal
CFLAGS += -I$(SRC_PATH)/libdescriptor/unmarshal

libdescriptor.a: unmarshal/unmarshal.o
libdescriptor.a: marshal/marshal.o
