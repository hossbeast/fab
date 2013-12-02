
int xopen(const char * path, int flags, int * const fd)
	__attribute__((nonnull));

int xopen_mode(const char * path, int flags, mode_t mode, int * const fd)
	__attribute__((nonnull));
