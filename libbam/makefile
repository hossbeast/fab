# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libbam.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libbam

CFLAGS += -I$(SRC_PATH)/libbam
CFLAGS += -I$(SRC_PATH)/libbam/include/bam
CFLAGS += -I$(SRC_PATH)/libbam/sigutil
CFLAGS += -I$(SRC_PATH)/libbam/envs_tell
CFLAGS += -I$(SRC_PATH)/libbam/config_tell
CFLAGS += -I$(SRC_PATH)/libbam/client
CFLAGS += -I$(SRC_PATH)/libbam/events
CFLAGS += -I$(SRC_PATH)/libbam/build_slot
CFLAGS += -I$(SRC_PATH)/libbam/include
CFLAGS += -I$(SRC_PATH)/libbam/load
CFLAGS += -I$(SRC_PATH)/libbam/vars_tell
CFLAGS += -I$(SRC_PATH)/libbam/list
CFLAGS += -I$(SRC_PATH)/libbam/ipc
CFLAGS += -I$(SRC_PATH)/libbam/lockfile
CFLAGS += -I$(SRC_PATH)/libbam/probes

CFLAGS += -I$(BUILD_PATH)/bam

bam/build_slot.desc.c bam/build_slot.desc.h : build_slot/build_slot.h
bam/config_tell.desc.c bam/config_tell.desc.h : config_tell/config_tell.h
bam/envs_tell.desc.c bam/envs_tell.desc.h : envs_tell/envs_tell.h
bam/list.desc.c bam/list.desc.h : list/list.h
bam/vars_tell.desc.c bam/vars_tell.desc.h : vars_tell/vars_tell.h

libbam.a: bam/build_slot.desc.o
libbam.a: bam/config_tell.desc.o
libbam.a: bam/envs_tell.desc.o
libbam.a: bam/list.desc.o
libbam.a: bam/vars_tell.desc.o
libbam.a: build_slot/build_slot.o
libbam.a: client/client.o
libbam.a: events/events.o
libbam.a: ipc/ipc.o
libbam.a: load/load.o
libbam.a: lockfile/lockfile.o
libbam.a: probes/probes.o
libbam.a: sigutil/sigutil.o
