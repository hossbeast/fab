# summary

No state maintained across requests

See fab/fabd/request/request.internal.h

# example requests

````
(
  { stage-config { build { concurrency 0 } } }
  reconfigure

  { select { traverse { min-distance 1 } }
  { select { path foo/bar } }
  { select all }
  { select none }

  # operate on the selection
  list          # list nodes in the selection
  describe      # describe nodes
  invalidate    # invalidate nodes

  target        # append to the targets list
  target-exact  # append to the exact-targets list

  plan       # plan the build
  build      # execute the plan
  script     # write the plan to a script
  autobuild  # plan/build on inotify
)
````
