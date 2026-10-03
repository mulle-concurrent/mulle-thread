## 4.11.0


* document automatic backend selection (C11, pthreads, Windows) and when pthreads is forced (DragonFly, Cosmopolitan, ThreadSanitizer)
* clarify the 0-on-success/`-1`-on-failure convention and normalized argument order across backends
* document the scoped ``mulle_thread_mutex_do`` / ``recursive_mutex_do`` / ``once_do`` macros and their `return`/nesting pitfalls
* describe recursive-once ownership by thread id and the blocking vs non-blocking once variants
* expand atomic docs: acquire/relaxed variants, `(address, value, expect)` CAS order, and separate intptr counters
* note ``mulle_thread_return()`` and Windows TSS destructor flushing, plus Windows-only TSS lifecycle calls
* add runnable examples for threads, recursive mutex, once-init, TSS, condition variables, and atomics



feature: add ``mulle_atomic_intptr_t`` for integer counters

* new ``mulle_atomic_intptr_t`` atomic type for counters, statistics and other plain integers
* ``_mulle_atomic_intptr_read/write/increment/decrement/add`` relaxed and nonatomic accessors, all returning the previous value
* ``_mulle_atomic_intptr_add_relaxed`` deliberately differs from the pointer variant by returning the previous value
* a forgotten `&` on the integer type is now a compile error (`-Wint-conversion`) instead of a silent load-and-pass crash
* ThreadSanitizer builds now auto-select the pthreads backend, and the unavoidable standalone fence warning is silenced
* cosmopolitan selects the pthreads backend, whose ``<threads.h>`` lacks ``mtx_t`/`tss_t``
