This release of fab is built by the previous release of fab. First, build and
install the previous release.

Modules compatible with fab-v0.5 for building this version of fab are in the
build directory. Your build invocation should pass that directory to fab using
the -J switch.

```fab -J fab/build```

During the build, fab generates some intermediate files which are not committed
to the repository.  In general a build will fail if these files have not yet
been generated.

```
# generate intermediate files
set -e
fab -J ~/fab/build +bx $(for x in **/*.l ; do echo ${x%.l}.lex.h ; done)
fab -J ~/fab/build +bx $(for x in **/*.l ; do echo ${x%.l}.states.h ; done)
fab -J ~/fab/build +bx $(for x in **/*.y ; do echo ${x%.y}.tab.h ; done)
fab -J ~/fab/build +bx $(for x in **/*.y ; do echo ${x%.y}.tokens.h ; done)
fab -J ~/fab/build +bx $(for x in **/*.errtab ; do echo $x.h ; done)
```
