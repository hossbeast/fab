MODULE CONVENTIONS

 setup/teardown
  int foo_setup();
  int foo_teardown();

CLASS CONVENTIONS

 free/reset
  int foo_alloc(foo **)         # allocates foo
  int foo_mk(foo **, ...)       # allocates and sets up foo
  void foo_free(foo *)          # frees foo
  void foo_xfree(foo **)
  void foo_reset(foo *)         # idempotent resource release

 dispose/recycle
  int foo_dispose(foo *)        # like free
  int foo_recycle(foo *)        # like reset

 initialize/destroy
  int foo_initialize(foo *)     # does not allocate foo
  void foo_destroy(foo *)       # does not free foo
