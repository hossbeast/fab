faba (fab agent)
===

The fab agent is a long-running process that carries out tasks for which that
require the users credentials, such as reading the fabfiles, and executing
build tasks which may create / write to files in user directories

When the user re-executes fab with different credentials, the faba process
is recycled

 - executing dependency discovery formulas
 - executing buildplan formulas
 - reading fabfiles
 - reading config files
 - monitoring primary files with inotify
