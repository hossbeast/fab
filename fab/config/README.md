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

# shit you can configure

#filesystem - invalidation methods

filesystem.home.path

filesystem {
  "/" {
    invalidation stat
  }

  "/mnt/remote" {
    path /
    invalidation notify
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
