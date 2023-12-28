# libbam

library for interacting with bamd

# messages

client messages

```
BAMIPC_MSG_RESPONSE
BAMIPC_MSG_RESULT
BAMIPC_MSG_EVENTSUB
BAMIPC_MSG_EVENTS
BAMIPC_MSG_REQUEST
 list - descriptor_bam_list_item
 metadata - descriptor_bam_metadata
 stats-read - descriptor_bam_fsent_stats
  descriptor_bam_module_file_stats
  descriptor_bam_global_stats
  descriptor_bam_var_stats
  descriptor_bam_config_stats
  descriptor_bam_module_stats
  descriptor_bam_formula_stats
 describe
  descriptor_bam_describe_item;
```

# events

```
BAMIPC_EVENT_FORMULA_EXEC_WAITED - descriptor_bam_build_slot_results
BAMIPC_EVENT_FORMULA_EXEC_FORKED - descriptor_bam_build_slot_info
BAMIPC_EVENT_FORMULA_EXEC_STDOUT - text
BAMIPC_EVENT_FORMULA_EXEC_STDERR - text
BAMIPC_EVENT_FORMULA_EXEC_AUXOUT - text
BAMIPC_EVENT_NODE_STALE - project root relative path
BAMIPC_EVENT_NODE_FRESH - project root relative path
BAMIPC_EVENT_NODE_CREATE - project root relative path
BAMIPC_EVENT_NODE_DELETE - project root relative path
BAMIPC_EVENT_GLOBAL_INVALIDATE
BAMIPC_EVENT_GOALS - none - msgid is client pid
BAMIPC_EVENT_SYSTEM_STATE - text
```
