# example selector

````
env {
select TARGET

select MODULE

traverse {
  min-distance 0
  max-distance 1
  direction TRAVERSE_DOWN
  relation RELATION_TYPE_MODULE
  from MODULES
}
````

# adhoc query examples

````
# query the immediate dependencies of a particular node
{
  property RELPATH
  traverse {
    min-distance 1
    max-distance 1
    direction TRAVERSE_DOWN
    relation RELATION_TYPE_STRONG
    from {
      path foo/bar/main.o
    }
  }
}

# source files in the module
{
  property RELPATH

  # primary files in the transitive dependencies of the artifacts
  traverse {
    relation RELATION_TYPE_STRONG
    visit NODE_TYPE_PRIMARY
    direction TRAVERSE_DOWN

    # artifacts defined by the module
    traverse {
      relation RELATION_TYPE_ARTIFACTS
      direction TRAVERSE_DOWN
      min-distance 1
      max-distance 1

      # the module
      from MODULE
    }
  }
}

# source files in the module and its requires
{
  property RELPATH

  # primary files in the transitive dependencies of the artifacts
  traverse {
    relation RELATION_TYPE_STRONG
    visit NODE_TYPE_PRIMARY
    direction TRAVERSE_DOWN

    # artifacts defined by the modules
    traverse {
      relation RELATION_TYPE_ARTIFACTS
      direction TRAVERSE_DOWN
      min-distance 1
      max-distance 1

      # this module and its immediate requires
      traverse {
        relation RELATION_TYPE_REQUIRES
        direction TRAVERSE_DOWN
        min-distance 0
        max-distance 1

        # the module
        from MODULE
      }
    }
  }
}

````
