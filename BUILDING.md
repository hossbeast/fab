# building

At every point in time, bam should be buildable with each of the following methods

## the previous release

The tool you use to build bam while working on bam. Uses build config files with their
default/public names and extensions. The builtin modules used would come from their installed
location on the system.

```
staging global config @ /etc/fabconfig
staging user config @ /home/todd/.fab/config
no project config @ /home/todd/fab/fab/fabd/.fab/config
effective config
extern : {
  /home/todd/fab-v0.6/fs/fab/builtin-modules
}
```

## latest master

While validating changes to bam, you can build bam itself with the new version. To keep the build
config files separate from those used with the previous release, these use names/extensions with the
devel tag. The builtin modules are the ones in the repo.

```
staging system config @ /etc/fabconfig+devel
staging user config @ /home/todd/.fab/config+devel
staging project config @ /home/todd/fab/xunit/xunit/.fab/config+devel
effective config
extern : {
  /home/todd/fab
}
special : {
  module : "module.bam+devel"
  model : "model.bam+devel"
  var : "var.bam+devel"
  formula-suffix : ".bam+devel"
}
```

## make

It's expected that package maintainers will run configure/make to build bam for distribution. This
uses the included makefiles.
