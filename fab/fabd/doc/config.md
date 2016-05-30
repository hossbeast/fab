# configuration

## set / push / pop / shift / unshift

* logger ring size
* logger ring filters
* buildscript variables
* buildscript output path
* backtrace mode
* concurrency setting

$logger.ring.size
@logger.ring.filters
$mode
$buildscript.vars
$buildscript.path
$concurrency

* lines to output when build fails
* bytes to output when build fails

## set

* per-filesystem invalidation settings

filesystem.0
  path : ""
  invalidation : "stat|content|notify|always|never"
