# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: libbam.a

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/libbam

CFLAGS += -I$(SRC_PATH)/libbam/build_slot
CFLAGS += -I$(SRC_PATH)/libbam/client
CFLAGS += -I$(SRC_PATH)/libbam/config_describe
CFLAGS += -I$(SRC_PATH)/libbam/describe
CFLAGS += -I$(SRC_PATH)/libbam/events
CFLAGS += -I$(SRC_PATH)/libbam/fsent
CFLAGS += -I$(SRC_PATH)/libbam/graph
CFLAGS += -I$(SRC_PATH)/libbam/ipc
CFLAGS += -I$(SRC_PATH)/libbam/list
CFLAGS += -I$(SRC_PATH)/libbam/load
CFLAGS += -I$(SRC_PATH)/libbam/lockfile
CFLAGS += -I$(SRC_PATH)/libbam/metadata
CFLAGS += -I$(SRC_PATH)/libbam/probes
CFLAGS += -I$(SRC_PATH)/libbam/rule_info
CFLAGS += -I$(SRC_PATH)/libbam/rules_describe
CFLAGS += -I$(SRC_PATH)/libbam/sigutil
CFLAGS += -I$(SRC_PATH)/libbam/stats
CFLAGS += -I$(SRC_PATH)/libbam/vars_describe
CFLAGS += -I$(SRC_PATH)/libbam/vertex
CFLAGS += -I$(BUILD_PATH)/bam

libbam.a: bam/build_slot.desc.o
libbam.a: bam/config_describe.desc.o
libbam.a: bam/describe.desc.o
libbam.a: bam/list.desc.o
libbam.a: bam/metadata.desc.o
libbam.a: bam/rule_info.desc.o
libbam.a: bam/rules_describe.desc.o
libbam.a: bam/stats.desc.o
libbam.a: bam/vars_describe.desc.o
libbam.a: build_slot/build_slot.o
libbam.a: client/client.o
libbam.a: events/events.o
libbam.a: fsent/fsent.o
libbam.a: graph/graph.o
libbam.a: ipc/ipc.o
libbam.a: load/load.o
libbam.a: lockfile/lockfile.o
libbam.a: metadata/metadata.o
libbam.a: probes/probes.o
libbam.a: sigutil/sigutil.o
libbam.a: vertex/vertex.o
