# libfab

library for interacting with fabd

# messages

client messages

```
FABIPC_MSG_RESPONSE
FABIPC_MSG_RESULT
FABIPC_MSG_EVENTSUB
FABIPC_MSG_EVENTS
FABIPC_MSG_REQUEST
 list - descriptor_fab_list_item
 metadata - descriptor_fab_metadata
 stats-read - descriptor_fab_fsent_stats
  descriptor_fab_module_file_stats
  descriptor_fab_global_stats
  descriptor_fab_var_stats
  descriptor_fab_config_stats
  descriptor_fab_module_stats
  descriptor_fab_formula_stats
 describe
  descriptor_fab_describe_item;
```

# events

```
FABIPC_EVENT_FORMULA_EXEC_WAITED - descriptor_fab_build_slot_results
FABIPC_EVENT_FORMULA_EXEC_FORKED - descriptor_fab_build_slot_info
FABIPC_EVENT_FORMULA_EXEC_STDOUT - text
FABIPC_EVENT_FORMULA_EXEC_STDERR - text
FABIPC_EVENT_FORMULA_EXEC_AUXOUT - text
FABIPC_EVENT_NODE_STALE - project root relative path
FABIPC_EVENT_NODE_FRESH - project root relative path
FABIPC_EVENT_NODE_CREATE - project root relative path
FABIPC_EVENT_NODE_DELETE - project root relative path
FABIPC_EVENT_GLOBAL_INVALIDATE
FABIPC_EVENT_GOALS - none - msgid is client pid
FABIPC_EVENT_SYSTEM_STATE - text
```
