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

# example configuration

````
# paths to index
extern (
  /usr/lib/fab/builtin-modules
  /home/todd/my/modules
)

# default to stat-based invalidation
filesystem {
  "/" {
    invalidate notify
  }

  # home is network mounted
  /home {
    invalidate stat
  }
}

# heuristic based on detected cpus
build {
  concurrency 0
}

# defaults for formula execution output
formula {
  capture-stdout STREAM_PART_LEADING
  stdout-buffer-size 4096
  capture-stderr STREAM_PART_LEADING
  stderr-buffer-size 4096

  success {
    show-command false
    show-environment false
    show-status false
    show-stdout false
    show-stdout-limit-lines 0
    show-stdout-limit-bytes 0
    show-stderr false
    show-stderr-limit-lines 0
    show-stderr-limit-bytes 0
  }

  error {
    show-command true
    show-environment true
    show-status true
    show-stdout true
    show-stdout-limit-lines 0
    show-stdout-limit-bytes 0
    show-stderr true
    show-stderr-limit-lines 0
    show-stderr-limit-bytes 0
  }
}
````
