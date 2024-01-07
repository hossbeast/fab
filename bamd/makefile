# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all: bamd

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/bamd

LDFLAGS += -pie

CFLAGS += -I$(SRC_PATH)/bamd
CFLAGS += -I$(SRC_PATH)/bamd/anchor
CFLAGS += -I$(SRC_PATH)/bamd/args
CFLAGS += -I$(SRC_PATH)/bamd/block
CFLAGS += -I$(SRC_PATH)/bamd/build
CFLAGS += -I$(SRC_PATH)/bamd/build_slot
CFLAGS += -I$(SRC_PATH)/bamd/buildplan
CFLAGS += -I$(SRC_PATH)/bamd/buildplan_entity
CFLAGS += -I$(SRC_PATH)/bamd/channel
CFLAGS += -I$(SRC_PATH)/bamd/config
CFLAGS += -I$(SRC_PATH)/bamd/config_parser
CFLAGS += -I$(SRC_PATH)/bamd/config_system
CFLAGS += -I$(SRC_PATH)/bamd/configuration
CFLAGS += -I$(SRC_PATH)/bamd/dependency
CFLAGS += -I$(SRC_PATH)/bamd/dir_rma
CFLAGS += -I$(SRC_PATH)/bamd/edge
CFLAGS += -I$(SRC_PATH)/bamd/entlist
CFLAGS += -I$(SRC_PATH)/bamd/events
CFLAGS += -I$(SRC_PATH)/bamd/exec
CFLAGS += -I$(SRC_PATH)/bamd/exec_builder
CFLAGS += -I$(SRC_PATH)/bamd/exec_render
CFLAGS += -I$(SRC_PATH)/bamd/filesystem
CFLAGS += -I$(SRC_PATH)/bamd/formula
CFLAGS += -I$(SRC_PATH)/bamd/formula_parser
CFLAGS += -I$(SRC_PATH)/bamd/formula_value
CFLAGS += -I$(SRC_PATH)/bamd/fsedge
CFLAGS += -I$(SRC_PATH)/bamd/fsent
CFLAGS += -I$(SRC_PATH)/bamd/fsent_index
CFLAGS += -I$(SRC_PATH)/bamd/fsname
CFLAGS += -I$(SRC_PATH)/bamd/goals
CFLAGS += -I$(SRC_PATH)/bamd/graph
CFLAGS += -I$(SRC_PATH)/bamd/handler
CFLAGS += -I$(SRC_PATH)/bamd/handler_thread
CFLAGS += -I$(SRC_PATH)/bamd/import
CFLAGS += -I$(SRC_PATH)/bamd/index_edge
CFLAGS += -I$(SRC_PATH)/bamd/index_vertex
CFLAGS += -I$(SRC_PATH)/bamd/inotify_mask
CFLAGS += -I$(SRC_PATH)/bamd/meta_edge
CFLAGS += -I$(SRC_PATH)/bamd/metadata
CFLAGS += -I$(SRC_PATH)/bamd/module
CFLAGS += -I$(SRC_PATH)/bamd/module_edge
CFLAGS += -I$(SRC_PATH)/bamd/module_system
CFLAGS += -I$(SRC_PATH)/bamd/monitor_thread
CFLAGS += -I$(SRC_PATH)/bamd/notify_thread
CFLAGS += -I$(SRC_PATH)/bamd/timeout_thread
CFLAGS += -I$(SRC_PATH)/bamd/params
CFLAGS += -I$(SRC_PATH)/bamd/path_cache
CFLAGS += -I$(SRC_PATH)/bamd/pattern
CFLAGS += -I$(SRC_PATH)/bamd/pattern/generate
CFLAGS += -I$(SRC_PATH)/bamd/pattern/lookup
CFLAGS += -I$(SRC_PATH)/bamd/pattern/match
CFLAGS += -I$(SRC_PATH)/bamd/pattern/render
CFLAGS += -I$(SRC_PATH)/bamd/pattern/search
CFLAGS += -I$(SRC_PATH)/bamd/pattern_parser
CFLAGS += -I$(SRC_PATH)/bamd/rcu
CFLAGS += -I$(SRC_PATH)/bamd/rcu_list
CFLAGS += -I$(SRC_PATH)/bamd/reconcile
CFLAGS += -I$(SRC_PATH)/bamd/request
CFLAGS += -I$(SRC_PATH)/bamd/request_parser
CFLAGS += -I$(SRC_PATH)/bamd/rule
CFLAGS += -I$(SRC_PATH)/bamd/rule_module_association
CFLAGS += -I$(SRC_PATH)/bamd/rule_system
CFLAGS += -I$(SRC_PATH)/bamd/rules
CFLAGS += -I$(SRC_PATH)/bamd/rules_parser
CFLAGS += -I$(SRC_PATH)/bamd/run_thread
CFLAGS += -I$(SRC_PATH)/bamd/selection
CFLAGS += -I$(SRC_PATH)/bamd/selector
CFLAGS += -I$(SRC_PATH)/bamd/selector_parser
CFLAGS += -I$(SRC_PATH)/bamd/server_thread
CFLAGS += -I$(SRC_PATH)/bamd/shadow
CFLAGS += -I$(SRC_PATH)/bamd/special_edge
CFLAGS += -I$(SRC_PATH)/bamd/stats
CFLAGS += -I$(SRC_PATH)/bamd/sweeper_thread
CFLAGS += -I$(SRC_PATH)/bamd/system_state
CFLAGS += -I$(SRC_PATH)/bamd/sysvar
CFLAGS += -I$(SRC_PATH)/bamd/var
CFLAGS += -I$(SRC_PATH)/bamd/vertex
CFLAGS += -I$(SRC_PATH)/bamd/walker

CFLAGS += -I$(BUILD_PATH)/bamd
CFLAGS += -I$(BUILD_PATH)/bamd/selector_parser
CFLAGS += -I$(BUILD_PATH)/bamd/formula_parser
CFLAGS += -I$(BUILD_PATH)/bamd/config_parser
CFLAGS += -I$(BUILD_PATH)/bamd/request_parser
CFLAGS += -I$(BUILD_PATH)/bamd/rules_parser
CFLAGS += -I$(BUILD_PATH)/bamd/pattern_parser
CFLAGS += -I$(BUILD_PATH)/libbam

bamd.exe: anchor/anchor.o
bamd.exe: args/args.o
bamd.exe: block/block.o
bamd.exe: build/build.o
bamd.exe: build/probes.o
bamd.exe: build_slot/build_slot.o
bamd.exe: buildplan/buildplan.o
bamd.exe: buildplan_entity/buildplan_entity.o
bamd.exe: channel/channel.o
bamd.exe: channel/probes.o
bamd.exe: config/config.o
bamd.exe: config_parser/config.lex.o
bamd.exe: config_parser/config.states.o
bamd.exe: config_parser/config.tab.o
bamd.exe: config_parser/config.tokens.o
bamd.exe: config_parser/config_parser.o
bamd.exe: config_system/config_system.o
bamd.exe: configuration/configuration.o
bamd.exe: dependency/dependency.o
bamd.exe: dir_rma/dir_rma.o
bamd.exe: edge/edge.o
bamd.exe: entlist/entlist.o
bamd.exe: events/events.o
bamd.exe: exec_builder/exec_builder.o
bamd.exe: exec_render/exec_render.o
bamd.exe: filesystem/filesystem.o
bamd.exe: formula/formula.o
bamd.exe: formula_parser/formula.lex.o
bamd.exe: formula_parser/formula.states.o
bamd.exe: formula_parser/formula.tab.o
bamd.exe: formula_parser/formula.tokens.o
bamd.exe: formula_parser/formula_parser.o
bamd.exe: formula_value/formula_value.o
bamd.exe: fsedge/fsedge.o
bamd.exe: fsent/fsent.o
bamd.exe: fsent_index/fsent_index.o
bamd.exe: fsname/fsname.o
bamd.exe: git-state.o
bamd.exe: goals/goals.o
bamd.exe: graph/graph.o
bamd.exe: handler/handler.o
bamd.exe: handler_thread/handler_thread.o
bamd.exe: timeout_thread/timeout_thread.o
bamd.exe: import/import.o
bamd.exe: index_edge/index_edge.o
bamd.exe: index_vertex/index_vertex.o
bamd.exe: inotify_mask/inotify_mask.o
bamd.exe: main.o
bamd.exe: meta_edge/meta_edge.o
bamd.exe: metadata/metadata.o
bamd.exe: module/module.o
bamd.exe: module_edge/module_edge.o
bamd.exe: module_system/module_system.o
bamd.exe: monitor_thread/monitor_thread.o
bamd.exe: notify_thread/notify_thread.o
bamd.exe: notify_thread/probes.o
bamd.exe: params/params.o
bamd.exe: path_cache/path_cache.o
bamd.exe: pattern/alternation.o
bamd.exe: pattern/byte.o
bamd.exe: pattern/class.o
bamd.exe: pattern/generate/generate.o
bamd.exe: pattern/group.o
bamd.exe: pattern/lookup/lookup.o
bamd.exe: pattern/match/match.o
bamd.exe: pattern/pattern.o
bamd.exe: pattern/qualifier.o
bamd.exe: pattern/range.o
bamd.exe: pattern/render/render.o
bamd.exe: pattern/search/search.o
bamd.exe: pattern/section.o
bamd.exe: pattern/segment.o
bamd.exe: pattern/segments.o
bamd.exe: pattern/star.o
bamd.exe: pattern/variable.o
bamd.exe: pattern/word.o
bamd.exe: pattern_parser/generate_pattern.tab.o
bamd.exe: pattern_parser/generate_pattern.tokens.o
bamd.exe: pattern_parser/include_pattern.tab.o
bamd.exe: pattern_parser/include_pattern.tokens.o
bamd.exe: pattern_parser/lookup_pattern.tab.o
bamd.exe: pattern_parser/lookup_pattern.tokens.o
bamd.exe: pattern_parser/match_pattern.tab.o
bamd.exe: pattern_parser/match_pattern.tokens.o
bamd.exe: pattern_parser/pattern.lex.o
bamd.exe: pattern_parser/pattern.states.o
bamd.exe: pattern_parser/pattern_parser.o
bamd.exe: pattern_parser/reference_pattern.tab.o
bamd.exe: pattern_parser/reference_pattern.tokens.o
bamd.exe: pattern_parser/search_pattern.tab.o
bamd.exe: pattern_parser/search_pattern.tokens.o
bamd.exe: rcu/rcu.o
bamd.exe: reconcile/reconcile.o
bamd.exe: request/request.o
bamd.exe: request_parser/request.lex.o
bamd.exe: request_parser/request.states.o
bamd.exe: request_parser/request.tab.o
bamd.exe: request_parser/request.tokens.o
bamd.exe: request_parser/request_parser.o
bamd.exe: rule/probes.o
bamd.exe: rule/rule.o
bamd.exe: rule_module_association/rule_module_association.o
bamd.exe: rule_system/rule_system.o
bamd.exe: rules/rules.o
bamd.exe: rules_parser/rules.lex.o
bamd.exe: rules_parser/rules.states.o
bamd.exe: rules_parser/rules.tab.o
bamd.exe: rules_parser/rules.tokens.o
bamd.exe: rules_parser/rules_parser.o
bamd.exe: run_thread/run_thread.o
bamd.exe: selection/selection.o
bamd.exe: selector/selector.o
bamd.exe: selector_parser/selector.lex.o
bamd.exe: selector_parser/selector.states.o
bamd.exe: selector_parser/selector.tab.o
bamd.exe: selector_parser/selector.tokens.o
bamd.exe: selector_parser/selector_parser.o
bamd.exe: server_thread/server_thread.o
bamd.exe: shadow/shadow.o
bamd.exe: special_edge/special_edge.o
bamd.exe: stats/stats.o
bamd.exe: sweeper_thread/probes.o
bamd.exe: sweeper_thread/sweeper_thread.o
bamd.exe: system_state/system_state.o
bamd.exe: sysvar/sysvar.o
bamd.exe: var/var.o
bamd.exe: vertex/probes.o
bamd.exe: vertex/vertex.o
bamd.exe: walker/walker.o

bamd.exe: $(BUILD_PATH)/libbacon/libbacon.a
bamd.exe: $(BUILD_PATH)/libbam/libbam.a
bamd.exe: $(BUILD_PATH)/libcommon/libcommon.a
bamd.exe: $(BUILD_PATH)/libdescriptor/libdescriptor.a
bamd.exe: $(BUILD_PATH)/liblorien/liblorien.a
bamd.exe: $(BUILD_PATH)/libmoria/libmoria.a
bamd.exe: $(BUILD_PATH)/libnarrator/libnarrator.a
bamd.exe: $(BUILD_PATH)/libvalyria/libvalyria.a
bamd.exe: $(BUILD_PATH)/libxlinux/libxlinux.a
bamd.exe: $(BUILD_PATH)/libyyutil/libyyutil.a

bamd: bamd.exe
	ln -vfs $^ $@
