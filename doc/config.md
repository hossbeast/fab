# configuration keys

logging
```
logging.console.exprs [ "+ERROR|WARN" ]
logging.logfile.exprs [ "+ERROR|WARN" "+CONFIG" ]
```

file invalidation
```
filesystems."/absolute-path".invalidate stat|content|notify|always|never
```

build
```
build.targets.fabricate [ ]
build.targets.fabricate-exact [ ]
build.targets.fabricate-nofile [ ]
build.mode "script|plan|exec"
node.invalidate [ ]
```

build execution
```
concurrency "unbounded|umlimited|infinite|percpu" | <integer>
```
