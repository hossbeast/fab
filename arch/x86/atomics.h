/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* returns the value previously held in *p */
#define atomic_fetch_add(p, v) ({               \
  __atomic_fetch_add(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_add(p, v) ({                     \
  __atomic_fetch_add(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_fetch_sub(p, v) ({               \
  __atomic_fetch_sub(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_sub(p, v) ({                     \
  __atomic_fetch_sub(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_inc(p) ({                        \
  __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);   \
})

#define atomic_fetch_inc(p) ({                  \
  __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);   \
})

#define atomic_fetch_dec(p) ({                  \
  __atomic_fetch_sub(p, 1, __ATOMIC_SEQ_CST);   \
})

/* returns the new value of *p */
#define atomic_add_fetch(p, v) ({               \
  __atomic_add_fetch(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_inc_fetch(p) ({                  \
  __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);   \
})

#define atomic_sub_fetch(p, v) ({               \
  __atomic_sub_fetch(p, v, __ATOMIC_SEQ_CST);   \
})

#define atomic_dec_fetch(p) ({                  \
  __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);   \
})

#define atomic_load(p) ({                       \
  __atomic_load_n(p, __ATOMIC_SEQ_CST);         \
})

#define atomic_store(p, v) ({                   \
  __atomic_store_n(p, v, __ATOMIC_SEQ_CST);     \
})

#define atomic_exchange(p, v) ({                \
  __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST);  \
})

#define atomic_cas(p, oldp, v) ({                                                  \
  __atomic_compare_exchange_n(p, oldp, v, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);  \
})
