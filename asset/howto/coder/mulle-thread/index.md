<!-- Keywords: atomic cas mutex recursive-mutex cond tss once-init thread -->

# mulle-thread — coder umbrella

Use this topic for any code that needs cross-platform C threading, mutexes,
condition variables, thread-local storage, once-init patterns, or basic pointer
atomics. The library is a thin wrapper over C11 threads, POSIX pthreads, or
Windows threads — selected automatically.

## Understand first

```bash
mulle-sde api apropos mulle-thread
mulle-sde api cat mulle-thread
```

## Local references

| Source | Path |
|--------|------|
| Umbrella header | `src/mulle-thread.h` |
| Atomic header | `src/mulle-atomic.h` |
| C11 atomic backend | `src/mulle-atomic-c11.h` |
| C11 thread backend | `src/mulle-thread-c11.h` |
| pthreads backend | `src/mulle-thread-pthreads.h` |
| Recursive mutex/once impl | `src/mulle-thread.c` |
| TOC / deep API docs | `asset/dox/TOC.md` |
| Atomic CAS/weakCAS test | `test/atomic/cas.c` |
| Atomic add test | `test/atomic/add.c` |
| Atomic bench | `test/atomic-bench/bench.c` |
| Mutex do test | `test/mutex/mutex-do.c` |
| Mutex stress | `test/mutex/mutex-stress.c` |
| Recursive mutex test | `test/recursive-mutex/recursive-mutex.c` |
| Recursive mutex stress | `test/recursive-mutex/recursive-mutex-stress.c` |
| Cond basic test | `test/cond/cond-basic.c` |
| Cond stress | `test/cond/cond-stress.c` |
| Thread test | `test/threads/threads.c` |
| Once recursive test | `test/threads/once-do-recursive.c` |
| TSS with destructor | `test/tss/tss.c` |

## Dominant API families

| Family | Header | Key types / macros |
|--------|--------|-------------------|
| Pointer atomics | `mulle-atomic.h` | `mulle_atomic_pointer_t`; CAS, weakCAS, add, set, increment, decrement, read, write, memory barrier |
| Function-pointer atomics | `mulle-atomic.h` | `mulle_atomic_functionpointer_t`; CAS, set, read, write |
| Threads | backends | `mulle_thread_t`, `mulle_thread_id_t`, `mulle_thread_create`(p_thread last!) |
| Mutex | backends | `mulle_thread_mutex_t`, `mulle_thread_mutex_do()` |
| Recursive mutex | `mulle-thread.h` | `mulle_thread_recursive_mutex_t`, `mulle_thread_recursive_mutex_do()` |
| Condition variable | backends | `mulle_thread_cond_t`, `mulle_thread_cond_wait/signal/broadcast/timedwait` |
| TSS | backends | `mulle_thread_tss_t`, `mulle_thread_tss_create`(key of 0 is valid) |
| Once-init | `mulle-thread.h` | `mulle_thread_once_t`, `mulle_thread_once_recursive_t`, `mulle_thread_once_do[_recursive/_noblock]()` |

## Primary workflow

1. `#include <mulle-thread/mulle-thread.h>` — single include selects the right backend.
2. For mutex/cond/TSS: call `_init` before use, `_done` after. All return 0 on success.
3. Prefer the `_do` macros (`mulle_thread_mutex_do`, `mulle_thread_recursive_mutex_do`) for scoped locking.
4. Prefer `mulle_thread_once_do(name)` for module-level lazy init; use the recursive variant when re-entrancy is possible.
5. Prefer `mulle_thread_tss_create` with a destructor callback for per-thread cleanup.

## Verify / report

```bash
mulle-sde craft
```
