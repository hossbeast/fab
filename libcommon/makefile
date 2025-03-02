# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libcommon.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libcommon

CFLAGS += -I$(SRC_PATH)/libcommon/assure
CFLAGS += -I$(SRC_PATH)/libcommon/attrs
CFLAGS += -I$(SRC_PATH)/libcommon/bech32
CFLAGS += -I$(SRC_PATH)/libcommon/color
CFLAGS += -I$(SRC_PATH)/libcommon/fmt
CFLAGS += -I$(SRC_PATH)/libcommon/grow
CFLAGS += -I$(SRC_PATH)/libcommon/hash
CFLAGS += -I$(SRC_PATH)/libcommon/parseint
CFLAGS += -I$(SRC_PATH)/libcommon/permute
CFLAGS += -I$(SRC_PATH)/libcommon/snarf
CFLAGS += -I$(SRC_PATH)/libcommon/sha1
CFLAGS += -I$(SRC_PATH)/libcommon/stresc

libcommon.a: assure/assure.o
libcommon.a: bech32/bech32.o
libcommon.a: attrs/attrs.o
libcommon.a: color/color.o
libcommon.a: fmt/fmt.o
libcommon.a: grow/grow.o
libcommon.a: hash/hash.o
libcommon.a: parseint/parseint.o
libcommon.a: permute/permute.o
libcommon.a: snarf/snarf.o
libcommon.a: sha1/sha1.o
libcommon.a: stresc/stresc.o
