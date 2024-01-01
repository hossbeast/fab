# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: bam

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/bam

LDFLAGS += -pie

CFLAGS += -I$(SRC_PATH)/bam
CFLAGS += -I$(SRC_PATH)/bam/args
CFLAGS += -I$(SRC_PATH)/bam/command
CFLAGS += -I$(SRC_PATH)/bam/command/adhoc
CFLAGS += -I$(SRC_PATH)/bam/command/autobuild
CFLAGS += -I$(SRC_PATH)/bam/command/build
CFLAGS += -I$(SRC_PATH)/bam/command/buildplan
CFLAGS += -I$(SRC_PATH)/bam/command/config
CFLAGS += -I$(SRC_PATH)/bam/command/describe
CFLAGS += -I$(SRC_PATH)/bam/command/envs
CFLAGS += -I$(SRC_PATH)/bam/command/events
CFLAGS += -I$(SRC_PATH)/bam/command/filesystem
CFLAGS += -I$(SRC_PATH)/bam/command/fstree
CFLAGS += -I$(SRC_PATH)/bam/command/kill
CFLAGS += -I$(SRC_PATH)/bam/command/metadata
CFLAGS += -I$(SRC_PATH)/bam/command/reconcile
CFLAGS += -I$(SRC_PATH)/bam/command/rules
CFLAGS += -I$(SRC_PATH)/bam/command/stats
CFLAGS += -I$(SRC_PATH)/bam/command/touch
CFLAGS += -I$(SRC_PATH)/bam/command/trace
CFLAGS += -I$(SRC_PATH)/bam/command/tree
CFLAGS += -I$(SRC_PATH)/bam/command/vars
CFLAGS += -I$(SRC_PATH)/bam/params
CFLAGS += -I$(BUILD_PATH)/bam
CFLAGS += -I$(BUILD_PATH)/bam/args
CFLAGS += -I$(BUILD_PATH)/libbam

bam.exe: args/args.lex.o
bam.exe: args/args.o
bam.exe: args/args.states.o
bam.exe: args/args.tab.o
bam.exe: args/args.tokens.o
bam.exe: command/adhoc/adhoc.o
bam.exe: command/autobuild/autobuild.o
bam.exe: command/build/build.o
bam.exe: command/buildplan/buildplan.o
bam.exe: command/command.o
bam.exe: command/config/config.o
bam.exe: command/describe/describe.o
bam.exe: command/envs/envs.o
bam.exe: command/events/events.o
bam.exe: command/filesystem/filesystem.o
bam.exe: command/fstree/fstree.o
bam.exe: command/kill/kill.o
bam.exe: command/metadata/metadata.o
bam.exe: command/reconcile/reconcile.o
bam.exe: command/rules/rules.o
bam.exe: command/stats/stats.o
bam.exe: command/touch/touch.o
bam.exe: command/trace/trace.o
bam.exe: command/tree/tree.o
bam.exe: command/vars/vars.o
bam.exe: git-state.o
bam.exe: main.o
bam.exe: params/params.o

bam.exe: $(BUILD_PATH)/libbacon/libbacon.a
bam.exe: $(BUILD_PATH)/libbam/libbam.a
bam.exe: $(BUILD_PATH)/libcommon/libcommon.a
bam.exe: $(BUILD_PATH)/libdescriptor/libdescriptor.a
bam.exe: $(BUILD_PATH)/liblorien/liblorien.a
bam.exe: $(BUILD_PATH)/libmoria/libmoria.a
bam.exe: $(BUILD_PATH)/libnarrator/libnarrator.a
bam.exe: $(BUILD_PATH)/libvalyria/libvalyria.a
bam.exe: $(BUILD_PATH)/libxlinux/libxlinux.a
bam.exe: $(BUILD_PATH)/libyyutil/libyyutil.a

bam: bam.exe
	ln -vfs $^ $@
