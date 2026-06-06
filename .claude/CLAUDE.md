# OVERVIEW

bam is a build system. it consists of several components,

 * bamd - build server. spun up on demand by bam. serves one specific build.
 * bam - bam cli. this is how users use bam.
 * bacon - command line tool for interacting with bacon text and files. bacon stands for bam
           configuration object notation. it is used extensively in this project.

there are also several supporting libraries. these are internal to the project, not primary
artifacts from the project.

# BUILDING

there are 4 build variants,

 * final - distributed to users. optimized, and without extra debug symbols
 * debug - like final, but with debug symbols and visiblity improvements
 * trace - experimental, and with extra instrumentation for profiling. not used much
 * devel - debug symbols and visibility, plus runtime checks like asan.

there are two build systems. the first one uses make and is for building bam for distributions. it
runs directly in tree.

the second build system uses bam itself, and is used when doing development on bam. it builds in a
separate directory tree, which is at /home/todd/bam-build, which mirrors the repo directory
structure, except that each component adds top-level directories for each build variant.

so if you want to build all variants of libmoria, you cd to /home/todd/bam-build/libmoria and run
bam. if you want to build only the devel variant, you cd to /home/todd/bam-build/libmoria/devel and
run bam.

you should use the bam build.

# UNIT TESTS

unit tests are built with the devel variant. they are run automatically when the devel variant is
built. you can also build an xunit binary for just one test, named test-*.xunit. so if you want to
build test_generate_pattern_parser in bamd, you cd to /home/todd/bam-build/bamd/devel and run bam
build pattern_parser/test/test_generate_pattern_generate.xunit. you can then run that xunit binary
to execute the tests.

# INTEG TESTS

integ tests use the binaries built from the devel variant. they are in the top-level integ
directory. you can simply execute the files named test-* in that directory.
