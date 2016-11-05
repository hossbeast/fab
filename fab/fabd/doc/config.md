# configuration keys

logging.console.filters [ "+ERROR|WARN" ]
logging.logfile.filters [ "+ERROR|WARN" "+CONFIG%" ]

filesystems."/".invalidate "stat|content|notify|always|never"

build.targets.fabricate [ ]
build.targets.fabricate-exact [ ]
build.targets.fabricate-nofile [ ]
build.mode "script|plan|exec"

license "std|fab"
node.invalidate [ ]
concurrency "unbounded|umlimited|infinite|percpu" | <integer>

buildscript.path ""
buildscript.runtime-vars [ ]

* lines to output when build fails
* bytes to output when build fails

# client commands

* execute the build
* kill the daemon
* merge this config
* apply config
* invalidate these nodes

## maybe

* run discovery

## someday

* so many graph operations are possible
** shortest path between X and Y

* print summary about these nodes
* print dependencies of these nodes
