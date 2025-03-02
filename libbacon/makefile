# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libbacon.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libbacon

CFLAGS += -I$(SRC_PATH)/libbacon/builder
CFLAGS += -I$(SRC_PATH)/libbacon/clone
CFLAGS += -I$(SRC_PATH)/libbacon/cmp
CFLAGS += -I$(SRC_PATH)/libbacon/coerce
CFLAGS += -I$(SRC_PATH)/libbacon/eval
CFLAGS += -I$(SRC_PATH)/libbacon/expr
CFLAGS += -I$(SRC_PATH)/libbacon/load
CFLAGS += -I$(SRC_PATH)/libbacon/location
CFLAGS += -I$(SRC_PATH)/libbacon/make
CFLAGS += -I$(SRC_PATH)/libbacon/merge
CFLAGS += -I$(SRC_PATH)/libbacon/parser
CFLAGS += -I$(SRC_PATH)/libbacon/registry
CFLAGS += -I$(SRC_PATH)/libbacon/say
CFLAGS += -I$(SRC_PATH)/libbacon/set
CFLAGS += -I$(SRC_PATH)/libbacon/value
CFLAGS += -I$(SRC_PATH)/libbacon/writer
CFLAGS += -I$(BUILD_PATH)/libbacon/parser

libbacon.a: bacon_narrator/bacon_narrator.o
libbacon.a: builder/builder.o
libbacon.a: clone/clone.o
libbacon.a: cmp/cmp.o
libbacon.a: coerce/coerce.o
libbacon.a: eval/eval.o
libbacon.a: expr/expr.o
libbacon.a: load/load.o
libbacon.a: location/location.o
libbacon.a: make/make.o
libbacon.a: merge/merge.o
libbacon.a: parser/expr.tab.o
libbacon.a: parser/expr.tokens.o
libbacon.a: parser/parser.o
libbacon.a: parser/value.lex.o
libbacon.a: parser/value.states.o
libbacon.a: parser/value.tab.o
libbacon.a: parser/value.tokens.o
libbacon.a: parser/value_list.tab.o
libbacon.a: parser/value_list.tokens.o
libbacon.a: parser/value_set.tab.o
libbacon.a: parser/value_set.tokens.o
libbacon.a: registry/registry.o
libbacon.a: say/say.o
libbacon.a: set/set.o
libbacon.a: value/value.o
libbacon.a: writer/writer.o
