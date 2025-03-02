# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

SHELL=/bin/bash -x

all: bamd

include ../config.mak
include ../makefile.common

VPATH=$(SRC_PATH)/bamd

LDFLAGS += -pie

#LDFLAGS += -lbacktrace
#bamd.exe: $(BUILD_PATH)/asan/asan.o

CFLAGS += -I$(SRC_PATH)/bamd
CFLAGS += -I$(SRC_PATH)/bamd/adaptor
CFLAGS += -I$(SRC_PATH)/bamd/args
CFLAGS += -I$(SRC_PATH)/bamd/block
CFLAGS += -I$(SRC_PATH)/bamd/bpe
CFLAGS += -I$(SRC_PATH)/bamd/build
CFLAGS += -I$(SRC_PATH)/bamd/buildplan
CFLAGS += -I$(SRC_PATH)/bamd/build_record
CFLAGS += -I$(SRC_PATH)/bamd/build_slot
CFLAGS += -I$(SRC_PATH)/bamd/channel
CFLAGS += -I$(SRC_PATH)/bamd/channel_server_thread
CFLAGS += -I$(SRC_PATH)/bamd/channel_thread
CFLAGS += -I$(SRC_PATH)/bamd/client
CFLAGS += -I$(SRC_PATH)/bamd/command
CFLAGS += -I$(SRC_PATH)/bamd/config
CFLAGS += -I$(SRC_PATH)/bamd/configset
CFLAGS += -I$(SRC_PATH)/bamd/configset_parser
CFLAGS += -I$(SRC_PATH)/bamd/mak_parser
CFLAGS += -I$(SRC_PATH)/bamd/config_system
CFLAGS += -I$(SRC_PATH)/bamd/dependency
CFLAGS += -I$(SRC_PATH)/bamd/describe
CFLAGS += -I$(SRC_PATH)/bamd/detail
CFLAGS += -I$(SRC_PATH)/bamd/dir_rma_edge
CFLAGS += -I$(SRC_PATH)/bamd/dir_fsname_edge
CFLAGS += -I$(SRC_PATH)/bamd/disintegrate
CFLAGS += -I$(SRC_PATH)/bamd/edge
CFLAGS += -I$(SRC_PATH)/bamd/edges
CFLAGS += -I$(SRC_PATH)/bamd/events
CFLAGS += -I$(SRC_PATH)/bamd/exec
CFLAGS += -I$(SRC_PATH)/bamd/exec_builder
CFLAGS += -I$(SRC_PATH)/bamd/exec_render
CFLAGS += -I$(SRC_PATH)/bamd/exec_value
CFLAGS += -I$(SRC_PATH)/bamd/filesystem
CFLAGS += -I$(SRC_PATH)/bamd/formula
CFLAGS += -I$(SRC_PATH)/bamd/formula_parser
CFLAGS += -I$(SRC_PATH)/bamd/formula_record
CFLAGS += -I$(SRC_PATH)/bamd/fsedge
CFLAGS += -I$(SRC_PATH)/bamd/fsent
CFLAGS += -I$(SRC_PATH)/bamd/fsent_name
CFLAGS += -I$(SRC_PATH)/bamd/fsent_dir
CFLAGS += -I$(SRC_PATH)/bamd/fsname
CFLAGS += -I$(SRC_PATH)/bamd/fstree
CFLAGS += -I$(SRC_PATH)/bamd/goals
CFLAGS += -I$(SRC_PATH)/bamd/graph
CFLAGS += -I$(SRC_PATH)/bamd/import
CFLAGS += -I$(SRC_PATH)/bamd/identity
CFLAGS += -I$(SRC_PATH)/bamd/inotify_mask
CFLAGS += -I$(SRC_PATH)/bamd/lookup
CFLAGS += -I$(SRC_PATH)/bamd/meta_edge
CFLAGS += -I$(SRC_PATH)/bamd/module
CFLAGS += -I$(SRC_PATH)/bamd/module_edge
CFLAGS += -I$(SRC_PATH)/bamd/module_system
CFLAGS += -I$(SRC_PATH)/bamd/monitor_thread
CFLAGS += -I$(SRC_PATH)/bamd/namespace
CFLAGS += -I$(SRC_PATH)/bamd/notify
CFLAGS += -I$(SRC_PATH)/bamd/notify_thread
CFLAGS += -I$(SRC_PATH)/bamd/ns_edge
CFLAGS += -I$(SRC_PATH)/bamd/params
CFLAGS += -I$(SRC_PATH)/bamd/pattern
CFLAGS += -I$(SRC_PATH)/bamd/pattern/generate
CFLAGS += -I$(SRC_PATH)/bamd/pattern/match
CFLAGS += -I$(SRC_PATH)/bamd/pattern_parser
CFLAGS += -I$(SRC_PATH)/bamd/pattern/resolve
CFLAGS += -I$(SRC_PATH)/bamd/pattern/search
CFLAGS += -I$(SRC_PATH)/bamd/plugin
CFLAGS += -I$(SRC_PATH)/bamd/plugin_process
CFLAGS += -I$(SRC_PATH)/bamd/plugin_system
CFLAGS += -I$(SRC_PATH)/bamd/plugin_thread
CFLAGS += -I$(SRC_PATH)/bamd/rcu
CFLAGS += -I$(SRC_PATH)/bamd/rcu_list
CFLAGS += -I$(SRC_PATH)/bamd/reconcile
CFLAGS += -I$(SRC_PATH)/bamd/ref_edge
CFLAGS += -I$(SRC_PATH)/bamd/request
CFLAGS += -I$(SRC_PATH)/bamd/request_handler
CFLAGS += -I$(SRC_PATH)/bamd/request_parser
CFLAGS += -I$(SRC_PATH)/bamd/rma
CFLAGS += -I$(SRC_PATH)/bamd/rule
CFLAGS += -I$(SRC_PATH)/bamd/ruleset
CFLAGS += -I$(SRC_PATH)/bamd/ruleset_parser
CFLAGS += -I$(SRC_PATH)/bamd/rule_system
CFLAGS += -I$(SRC_PATH)/bamd/run_thread
CFLAGS += -I$(SRC_PATH)/bamd/selection
CFLAGS += -I$(SRC_PATH)/bamd/selector
CFLAGS += -I$(SRC_PATH)/bamd/selector_parser
CFLAGS += -I$(SRC_PATH)/bamd/shadow
CFLAGS += -I$(SRC_PATH)/bamd/special_edge
CFLAGS += -I$(SRC_PATH)/bamd/stats
CFLAGS += -I$(SRC_PATH)/bamd/subsys
CFLAGS += -I$(SRC_PATH)/bamd/sweeper_thread
CFLAGS += -I$(SRC_PATH)/bamd/system
CFLAGS += -I$(SRC_PATH)/bamd/sysvar
CFLAGS += -I$(SRC_PATH)/bamd/symlink
CFLAGS += -I$(SRC_PATH)/bamd/timers
CFLAGS += -I$(SRC_PATH)/bamd/timeout_thread
CFLAGS += -I$(SRC_PATH)/bamd/tree_list
CFLAGS += -I$(SRC_PATH)/bamd/varset
CFLAGS += -I$(SRC_PATH)/bamd/vertex
CFLAGS += -I$(SRC_PATH)/bamd/vertices
CFLAGS += -I$(SRC_PATH)/bamd/walker

CFLAGS += -I$(BUILD_PATH)/bamd
CFLAGS += -I$(BUILD_PATH)/bamd/selector_parser
CFLAGS += -I$(BUILD_PATH)/bamd/formula_parser
CFLAGS += -I$(BUILD_PATH)/bamd/configset_parser
CFLAGS += -I$(BUILD_PATH)/bamd/mak_parser
CFLAGS += -I$(BUILD_PATH)/bamd/request_parser
CFLAGS += -I$(BUILD_PATH)/bamd/ruleset_parser
CFLAGS += -I$(BUILD_PATH)/bamd/pattern_parser
CFLAGS += -I$(BUILD_PATH)/bamd/rcu
CFLAGS += -I$(BUILD_PATH)/bamd/fsent
CFLAGS += -I$(BUILD_PATH)/bamd/fsent_dir
CFLAGS += -I$(BUILD_PATH)/libbam

build/build_stats.desc.c build/build_stats.desc.h : build/build.h
buildplan/buildplan_stats.desc.c buildplan/buildplan_stats.desc.h : buildplan/buildplan.h
bpe/stats_bpe.desc.c bpe/stats_bpe.desc.h : bpe/bpe.h
configset/stats_configset.desc.c configset/stats_configset.desc.h : configset/configset.h
config_system/config_system_stats.desc.c config_system/config_system_stats.desc.h : config_system/config_system.h
filesystem/filesystem_stats.desc.c filesystem/filesystem_stats.desc.h : filesystem/filesystem.h
rcu/rcu_stats.desc.c rcu/rcu_stats.desc.h : rcu/rcu.h
formula/stats_formula.desc.c formula/stats_formula.desc.h : formula/formula.h
fstree/stats_fstree.desc.c fstree/stats_fstree.desc.h : fstree/fstree.h
client/stats_client.desc.c client/stats_client.desc.h : client/client.h
fsname/fsname_stats.desc.c fsname/fsname_stats.desc.h : fsname/fsname.h
namespace/namespace_stats.desc.c namespace/namespace_stats.desc.h : namespace/namespace.h
module/stats_module.desc.c module/stats_module.desc.h : module/module.h
notify/notify_stats.desc.c notify/notify_stats.desc.h : notify/notify.h
plugin/stats_plugin.desc.c plugin/stats_plugin.desc.h : plugin/plugin.h
rule/stats_rule.desc.c rule/stats_rule.desc.h : rule/rule.h
rma/stats_rma.desc.c rma/stats_rma.desc.h : rma/rma.h
ruleset/stats_ruleset.desc.c ruleset/stats_ruleset.desc.h : ruleset/ruleset.h
fsent_dir/stats_fsent_dir.desc.c fsent_dir/stats_fsent_dir.desc.h : fsent_dir/fsent_dir.h
fsent/stats_fsent_symlink.desc.c fsent/stats_fsent_symlink.desc.h : fsent/fsent_symlink.h
system/system_stats.desc.c system/system_stats.desc.h : system/system.h
varset/stats_varset.desc.c varset/stats_varset.desc.h : varset/varset.h
vertex/stats_vertex.desc.c vertex/stats_vertex.desc.h : vertex/vertex.h

bamd.exe: adaptor/adaptor.o
bamd.exe: args/args.o
bamd.exe: block/block.o
bamd.exe: bpe/bpe.o
bamd.exe: build/build.o
bamd.exe: build/events.o
bamd.exe: buildplan/buildplan.o
bamd.exe: build/probes.o
bamd.exe: build_slot/build_slot.o
bamd.exe: build_record/build_record.o
bamd.exe: channel/channel.o
bamd.exe: channel/probes.o
bamd.exe: channel_server_thread/channel_server_thread.o
bamd.exe: channel_thread/channel_thread.o
bamd.exe: client/client.o
bamd.exe: command/command.o
bamd.exe: config/config.o
bamd.exe: configset/configset.o
bamd.exe: configset_parser/configset_parser.o
bamd.exe: mak_parser/mak_parser.o
bamd.exe: config_system/config_system.o
bamd.exe: dependency/dependency.o
bamd.exe: describe/describe.o
bamd.exe: detail/detail.o
bamd.exe: dir_rma_edge/dir_rma_edge.o
bamd.exe: dir_fsname_edge/dir_fsname_edge.o
bamd.exe: disintegrate/disintegrate.o
bamd.exe: disintegrate/probes.o
bamd.exe: edge/edge.o
bamd.exe: edges/edges.o
bamd.exe: events/events.o
bamd.exe: exec_builder/exec_builder.o
bamd.exe: exec/exec.o
bamd.exe: exec_render/exec_render.o
bamd.exe: exec_value/exec_value.o
bamd.exe: filesystem/filesystem.o
bamd.exe: formula/formula.o
bamd.exe: formula_parser/formula_parser.o
bamd.exe: formula_record/formula_record.o
bamd.exe: fsedge/fsedge.o
bamd.exe: fsent/fsent.o
bamd.exe: fsent_name/fsent_name.o
bamd.exe: fsent_dir/fsent_dir.o
bamd.exe: fsent/fsent_symlink.o
bamd.exe: fsname/fsname.o
bamd.exe: fstree/fstree.o
bamd.exe: goals/goals.o
bamd.exe: graph/graph.o
bamd.exe: import/import.o
bamd.exe: identity/identity.o
bamd.exe: inotify_mask/inotify_mask.o
bamd.exe: lookup/lookup.o
bamd.exe: main.o
bamd.exe: meta_edge/meta_edge.o
bamd.exe: module_edge/module_edge.o
bamd.exe: module/module.o
bamd.exe: module/probes.o
bamd.exe: module_system/module_system.o
bamd.exe: monitor_thread/monitor_thread.o
bamd.exe: namespace/namespace.o
bamd.exe: notify/notify.o
bamd.exe: notify_thread/notify_thread.o
bamd.exe: notify_thread/probes.o
bamd.exe: ns_edge/ns_edge.o
bamd.exe: params/params.o
bamd.exe: pattern/alternation.o
bamd.exe: pattern/byte.o
bamd.exe: pattern/class.o
bamd.exe: pattern/generate/generate.o
bamd.exe: pattern/group.o
bamd.exe: pattern/match/match.o
bamd.exe: pattern_parser/pattern_parser.o
bamd.exe: pattern/pattern.o
bamd.exe: pattern/qualifier.o
bamd.exe: pattern/range.o
bamd.exe: pattern/resolve/resolve.o
bamd.exe: pattern/search/search.o
bamd.exe: pattern/section.o
bamd.exe: pattern/segment.o
bamd.exe: pattern/segments.o
bamd.exe: pattern/star.o
bamd.exe: pattern/variable.o
bamd.exe: pattern/word.o
bamd.exe: plugin/plugin.o
bamd.exe: plugin/probes.o
bamd.exe: plugin_process/plugin_process.o
bamd.exe: plugin_system/plugin_system.o
bamd.exe: plugin_thread/plugin_thread.o
bamd.exe: rcu/rcu.o
bamd.exe: reconcile/reconcile.o
bamd.exe: ref_edge/ref_edge.o
bamd.exe: request_handler/request_handler.o
bamd.exe: request_parser/request_parser.o
bamd.exe: request/request.o
bamd.exe: rma/rma.o
bamd.exe: rma/events.o
bamd.exe: rule/probes.o
bamd.exe: rule/rule.o
bamd.exe: ruleset_parser/ruleset_parser.o
bamd.exe: ruleset/ruleset.o
bamd.exe: rule_system/rule_system.o
bamd.exe: run_thread/run_thread.o
bamd.exe: selection/selection.o
bamd.exe: selector_parser/selector_parser.o
bamd.exe: selector/selector.o
bamd.exe: shadow/shadow.o
bamd.exe: special_edge/special_edge.o
bamd.exe: stats/stats.o
bamd.exe: subsys/subsys.o
bamd.exe: sweeper_thread/probes.o
bamd.exe: sweeper_thread/sweeper_thread.o
bamd.exe: system/system.o
bamd.exe: sysvar/sysvar.o
bamd.exe: symlink/symlink.o
bamd.exe: timers/timers.o
bamd.exe: timeout_thread/timeout_thread.o
bamd.exe: tree_list/tree_list.o
bamd.exe: varset/varset.o
bamd.exe: vertex/probes.o
bamd.exe: vertex/vertex.o
bamd.exe: vertices/vertices.o
bamd.exe: walker/walker.o

# git state
bamd.exe: git-state.o

# descriptors
bamd.exe: build/build_stats.desc.o
bamd.exe: buildplan/buildplan_stats.desc.o
bamd.exe: bpe/stats_bpe.desc.o
bamd.exe: configset/stats_configset.desc.o
bamd.exe: config_system/config_system_stats.desc.o
bamd.exe: filesystem/filesystem_stats.desc.o
bamd.exe: rcu/rcu_stats.desc.o
bamd.exe: formula/stats_formula.desc.o
bamd.exe: fstree/stats_fstree.desc.o
bamd.exe: client/stats_client.desc.o
bamd.exe: fsname/fsname_stats.desc.o
bamd.exe: namespace/namespace_stats.desc.o
bamd.exe: module/stats_module.desc.o
bamd.exe: notify/notify_stats.desc.o
bamd.exe: plugin/stats_plugin.desc.o
bamd.exe: rule/stats_rule.desc.o
bamd.exe: rma/stats_rma.desc.o
bamd.exe: ruleset/stats_ruleset.desc.o
bamd.exe: fsent_dir/stats_fsent_dir.desc.o
bamd.exe: fsent/stats_fsent_symlink.desc.o
bamd.exe: system/system_stats.desc.o
bamd.exe: varset/stats_varset.desc.o
bamd.exe: vertex/stats_vertex.desc.o

# tab
bamd.exe: configset_parser/configset.tab.o
bamd.exe: mak_parser/mak.tab.o
bamd.exe: formula_parser/formula.tab.o
bamd.exe: pattern_parser/attachment_pattern.tab.o
bamd.exe: pattern_parser/generate_pattern.tab.o
bamd.exe: pattern_parser/include_pattern.tab.o
bamd.exe: pattern_parser/lookup_pattern.tab.o
bamd.exe: pattern_parser/match_pattern.tab.o
bamd.exe: pattern_parser/path_pattern.tab.o
bamd.exe: pattern_parser/search_pattern.tab.o
bamd.exe: request_parser/request.tab.o
bamd.exe: ruleset_parser/ruleset.tab.o
bamd.exe: selector_parser/selector.tab.o

# lex
bamd.exe: configset_parser/configset.lex.o
bamd.exe: mak_parser/mak.lex.o
bamd.exe: formula_parser/formula.lex.o
bamd.exe: pattern_parser/pattern.lex.o
bamd.exe: request_parser/request.lex.o
bamd.exe: request_parser/request.lex.o
bamd.exe: ruleset_parser/ruleset.lex.o
bamd.exe: selector_parser/selector.lex.o

# tokens
bamd.exe: configset_parser/configset.tokens.o
bamd.exe: mak_parser/mak.tokens.o
bamd.exe: formula_parser/formula.tokens.o
bamd.exe: pattern_parser/attachment_pattern.tokens.o
bamd.exe: pattern_parser/generate_pattern.tokens.o
bamd.exe: pattern_parser/include_pattern.tokens.o
bamd.exe: pattern_parser/lookup_pattern.tokens.o
bamd.exe: pattern_parser/match_pattern.tokens.o
bamd.exe: pattern_parser/path_pattern.tokens.o
bamd.exe: pattern_parser/search_pattern.tokens.o
bamd.exe: request_parser/request.tokens.o
bamd.exe: ruleset_parser/ruleset.tokens.o
bamd.exe: selector_parser/selector.tokens.o

# states
bamd.exe: configset_parser/configset.states.o
bamd.exe: mak_parser/mak.states.o
bamd.exe: formula_parser/formula.states.o
bamd.exe: pattern_parser/pattern.states.o
bamd.exe: request_parser/request.states.o
bamd.exe: ruleset_parser/ruleset.states.o
bamd.exe: selector_parser/selector.states.o

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
