# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libvalyria.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libvalyria

CFLAGS += -I$(SRC_PATH)/libvalyria/faults
CFLAGS += -I$(SRC_PATH)/libvalyria/array
CFLAGS += -I$(SRC_PATH)/libvalyria/hashtable
CFLAGS += -I$(SRC_PATH)/libvalyria/list
CFLAGS += -I$(SRC_PATH)/libvalyria/llist
CFLAGS += -I$(SRC_PATH)/libvalyria/map
CFLAGS += -I$(SRC_PATH)/libvalyria/pstring
CFLAGS += -I$(SRC_PATH)/libvalyria/rbtree

libvalyria.a: array/array.o
libvalyria.a: hashtable/hashtable.o
libvalyria.a: list/list.o
libvalyria.a: llist/llist.o
libvalyria.a: map/map.o
libvalyria.a: pstring/pstring.o
libvalyria.a: rbtree/rbtree.o
