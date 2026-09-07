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
