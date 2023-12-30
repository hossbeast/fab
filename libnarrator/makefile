# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libnarrator.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libnarrator

CFLAGS += -I$(SRC_PATH)/libnarrator/base
CFLAGS += -I$(SRC_PATH)/libnarrator/fd
CFLAGS += -I$(SRC_PATH)/libnarrator/fixed
CFLAGS += -I$(SRC_PATH)/libnarrator/growing
CFLAGS += -I$(SRC_PATH)/libnarrator/load
CFLAGS += -I$(SRC_PATH)/libnarrator/multi
CFLAGS += -I$(SRC_PATH)/libnarrator/nullity
CFLAGS += -I$(SRC_PATH)/libnarrator/record
CFLAGS += -I$(SRC_PATH)/libnarrator/rolling
CFLAGS += -I$(SRC_PATH)/libnarrator/units
CFLAGS += -I$(SRC_PATH)/libnarrator/vtable

libnarrator.a: base/base.o
libnarrator.a: fd/fd.o
libnarrator.a: fixed/fixed.o
libnarrator.a: growing/growing.o
libnarrator.a: load/load.o
libnarrator.a: multi/multi.o
libnarrator.a: nullity/nullity.o
libnarrator.a: record/record.o
libnarrator.a: rolling/rolling.o
libnarrator.a: units/units.o
