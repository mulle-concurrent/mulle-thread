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


## 4.10.0








* atomic pointer/functionpointer operations now default to ``seq_cst`` ordering instead of `relaxed`, fixing lost-update and stale-read bugs on weakly-ordered CPUs (ARM, PPC)
* ``mulle_atomic_memory_barrier`` is now a real thread fence, fixing publication of once-init data on non-TSO architectures
* new explicit ``_relaxed`/`_acquire`` variants for load, store, CAS and arithmetic so opt-in relaxed semantics remain available
* non-blocking once variants `(`mulle_thread_once_noblock`,` ``mulle_thread_once_do_noblock`,` ``*_call_noblock`)` now publish `DONE` after init, so mixing them with blocking once on the same flag can no longer hang
* add ``mulle_thread_equal()`` to compare thread handles portably
* add ``MULLE_THREAD_CALL`` calling-convention macro for portable thread entry functions (32-bit x86 Windows ``__stdcall`)`
* Windows: joined threads no longer leak their kernel handle
* Windows: mutex re-lock from the owning thread now self-deadlocks like the pthreads backend instead of silently succeeding
* pthreads backend now returns `0`/`-1` consistently instead of leaking `errno` values



* added Documentation & Guides section to README with API and coder guide links
* unified license header format across all source files with consistent copyright attribution


## 4.9.0





*  added (code moved from MulleObjC)
