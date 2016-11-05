# file format goals

* concise
* allows comments
* typed values
* able to merge samples

# config file locations

in order, later files take precedence

/etc/fabconfig
$HOME/.fabconfig
$PROJECT/.fabconfig

# configuration keys

 mode :- exec | generate | buildscript
 buildscript.path "/foo/bar"
 buildscript.runtime-vars [ foo bar ]
 targets [ "./foo" ]
 concurrency : unbounded | heuristic | <integer>
 license : std | fab
 backtrace : pithy | full

## logging filters

 logging.console.filters [ "+IPC" ]

## filesystem - invalidation methods

filesystem {
  "/mnt/remote" {
    invalidation stat | content | notify | always | never
  }
}

#whether to include the command when a command fails

includeCommand : true

#amount to copy from stderr when a command fails, bytes, lines

onfailure {
  lines 5
}

onfailure.lines 5

onfailure {
  bytes 7
}

onfailure.bytes 7

#colorization

color {
  diff red
  status blue
  
}

color.diff red
color.status blue

#aliases - fab <cmdline options>

alias.fabj "fab -j k +BPFOO"

alias {
  fabj "foo bar"
  fabk "bar foo"
}
