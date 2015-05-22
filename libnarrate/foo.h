typedef struct
{
  int (*sayf)(char * const restrict dst, const size_t sz, struct pstring ** restrict ps, size_t * restrict z, const char * const restrict fmt, ...);
  int (*sayw)(char * const restrict dst, const size_t sz, struct pstring ** restrict ps, size_t * restrict z, const char * const restrict buf, size_t bufl);
} narrator;

/// narrate_fixed
//
// SUMMARY
//  write to a fixed size buffer discarding overflow
//
narrator * narrate_fixed;

/// narrate_dynamic
//
// SUMMARY
//  write to a dynamically-resizing buffer
//
narrator * narrate_dynamic;
