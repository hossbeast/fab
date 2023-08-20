/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* returns the value previously held in *p */
#define atomic_fetch_inc(p) ({                  \
  __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);   \
})

/* returns the value previously held in *p */
#define atomic_fetch_dec(p) ({                  \
  __atomic_fetch_sub(p, 1, __ATOMIC_SEQ_CST);   \
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

#define atomic_cas(p, oldp, v) ({                                                      \
  __atomic_compare_exchange_n(p, oldp, v, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);  \
})
