# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libyyutil.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libyyutil

CFLAGS += -I$(SRC_PATH)/libyyutil/grammar
CFLAGS += -I$(SRC_PATH)/libyyutil/parser
CFLAGS += -I$(SRC_PATH)/libyyutil/probes
CFLAGS += -I$(SRC_PATH)/libyyutil/scanner

libyyutil.a: grammar/grammar.o
libyyutil.a: parser/parser.o
libyyutil.a: probes/probes.o
libyyutil.a: scanner/scanner.o
