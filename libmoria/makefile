# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libmoria.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libmoria

CFLAGS += -I$(SRC_PATH)/libmoria/attr
CFLAGS += -I$(SRC_PATH)/libmoria/connect
CFLAGS += -I$(SRC_PATH)/libmoria/dispatch
CFLAGS += -I$(SRC_PATH)/libmoria/edge
CFLAGS += -I$(SRC_PATH)/libmoria/foreach
CFLAGS += -I$(SRC_PATH)/libmoria/graph
CFLAGS += -I$(SRC_PATH)/libmoria/load
CFLAGS += -I$(SRC_PATH)/libmoria/lookup
CFLAGS += -I$(SRC_PATH)/libmoria/operations
CFLAGS += -I$(SRC_PATH)/libmoria/parser
CFLAGS += -I$(SRC_PATH)/libmoria/probes
CFLAGS += -I$(SRC_PATH)/libmoria/traverse
CFLAGS += -I$(SRC_PATH)/libmoria/vertex
CFLAGS += -I$(SRC_PATH)/libmoria/membership
CFLAGS += -I$(BUILD_PATH)/libmoria/parser

libmoria.a: connect/connect.o
libmoria.a: dispatch/dispatch.o
libmoria.a: edge/edge.o
libmoria.a: foreach/foreach.o
libmoria.a: graph/graph.o
libmoria.a: load/load.o
libmoria.a: lookup/lookup.o
libmoria.a: operations/operations.o
libmoria.a: parser/parser.o
libmoria.a: probes/probes.o
libmoria.a: traverse/traverse.o
libmoria.a: vertex/vertex.o
libmoria.a: membership/membership.o
