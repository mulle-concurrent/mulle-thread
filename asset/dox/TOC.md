# mulle-thread Library Documentation for AI
<!-- Keywords: thread, mutex, tss, atomic, portability -->
## 1. Introduction & Purpose

- mulle-thread is a small C wrapper API offering cross-platform thread, mutex, condition variable, thread-local-storage (TSS) and basic atomic operations.
- Solves portability differences between C11 threads, POSIX pthreads and Windows threads by providing a consistent, minimal API surface.
- Key features: thread creation/join/detach, mutex lock/unlock/trylock, condition variables, TSS create/get/set, once-init utilities, and a compact atomic pointer/function-pointer helper API.
- Relationship: component of mulle-core-style ecosystem; depends on mulle-c11 and optionally mintomic for atomics; falls back to pthreads or Windows native implementations.

## 2. Key Concepts & Design Philosophy

- Thin, explicit wrappers: public API lives in mulle-thread.h and mulle-atomic.h. Platform specifics are implemented in *_pthreads.h, *_c11.h or *_windows.h.
- Provide a stable, simplified interface even when underlying platform APIs differ.
- Convenience macros (mulle_thread_mutex_do, mulle_thread_once_do) encourage idiomatic usage but require care (see pitfalls).
- Emphasis on portability and predictability rather than advanced features; small surface area for easier analysis and verification.

## 3. Core API & Data Structures

### 3.1. [mulle-atomic.h]

- Purpose: small set of atomic helpers for pointer and function-pointer operations, selecting C11 stdatomic or mintomic implementations.
- Notable API:
  - _mulle_atomic_pointer_set(address, value) — atomically set pointer, returns previous.
  - _mulle_atomic_pointer_compare_and_swap(address, value, expect) — CAS returning int success.
  - __mulle_atomic_pointer_compare_and_swap(...) — CAS returning previous pointer.
  - _mulle_atomic_functionpointer_set(...) and functionpointer CAS variants.
  - mulle_atomic_memory_barrier() (implementation dependent) — memory barrier.
- Lifecycle: none (stateless utilities). Use these for lock-free pointer updates and once-init primitives.

### 3.2. [mulle-thread.h]

- Purpose: public umbrella header selecting platform backends and providing cross-cutting macros and once-helpers.
- Key typedefs (platform-dependent; documented in backend headers):
  - mulle_thread_t, mulle_thread_tss_t, mulle_thread_mutex_t, mulle_thread_cond_t, mulle_thread_id_t, mulle_thread_rval_t.
- Once-init primitives:
  - mulle_thread_once_t, mulle_thread_once_recursive_t — state and recursive-once helper.
  - mulle_thread_once(once, init), mulle_thread_once_call(once, init, userinfo), mulle_thread_once_call_recursive(...).
  - Macros: mulle_thread_once_do(name), mulle_thread_once_do_recursive(name), mulle_thread_once_do_noblock(name).
- Convenience macros:
  - mulle_thread_mutex_do(mutex) — locks, runs block, unlocks. Note: return from inside will not unlock on return.

### 3.3. [mulle-thread-pthreads.h / mulle-thread-c11.h / mulle-thread-windows.h]

- Purpose: platform-specific implementations. Public API is uniform across these headers.
- Thread API:
  - int   mulle_thread_create( mulle_thread_function_t *f, void *arg, mulle_thread_t *p_thread)
  - mulle_thread_rval_t mulle_thread_join( mulle_thread_t thread)
  - int   mulle_thread_detach( mulle_thread_t thread)
  - void  mulle_thread_exit(int rval)
  - void  mulle_thread_yield(void)
  - mulle_thread_id_t mulle_thread_id(void), mulle_thread_self(void), mulle_thread_get_id(thread)
- Mutex API:
  - int mulle_thread_mutex_init(mulle_thread_mutex_t *lock)
  - int mulle_thread_mutex_lock(mulle_thread_mutex_t *lock)
  - int mulle_thread_mutex_trylock(mulle_thread_mutex_t *lock)
  - int mulle_thread_mutex_unlock(mulle_thread_mutex_t *lock)
  - int mulle_thread_mutex_done(mulle_thread_mutex_t *lock)
- Condition variable API:
  - int mulle_thread_cond_init(mulle_thread_cond_t *cond)
  - int mulle_thread_cond_done(mulle_thread_cond_t *cond)
  - int mulle_thread_cond_wait(mulle_thread_cond_t *cond, mulle_thread_mutex_t *mutex)
  - int mulle_thread_cond_signal(mulle_thread_cond_t *cond)
  - int mulle_thread_cond_broadcast(mulle_thread_cond_t *cond)
  - int mulle_thread_cond_timedwait(...)
- Thread-local storage (TSS):
  - int mulle_thread_tss_create(mulle_thread_callback_t *f, mulle_thread_tss_t *key)
  - void mulle_thread_tss_free(mulle_thread_tss_t key)
  - void *mulle_thread_tss_get(mulle_thread_tss_t key)
  - int mulle_thread_tss_set(mulle_thread_tss_t key, void *value)

## 4. Performance Characteristics

- Atomics: O(1) per operation (hardware CAS / atomic primitives). Intended for pointer/function-pointer CAS and set operations.
- Mutex operations: O(1) for lock/unlock, but blocking cost depends on scheduler and number of contending threads.
- Condition variables: waiting is O(1) (per wait), signaling is O(1) but waking cost depends on contention.
- Thread creation/join: relatively expensive (OS cost). Prefer worker pools for high-rate tasks.
- Thread-safety: API is thread-safe where appropriate (atomics, mutex, cond); data structures and object lifecycles are the caller's responsibility. The library provides primitives, not higher-level synchronized containers.

## 5. AI Usage Recommendations & Patterns

- Best Practices:
  - Use the public lifecycle functions (init/done) for mutexes and condvars; do not manipulate platform internals.
  - Use mulle_thread_mutex_do(mutex) for scoped locking; prefer careful use to avoid returning inside that block (it will not unlock on return).
  - Prefer using the `_call` once variants when user data is needed.
  - Use TSS for per-thread destructors where supported; provide a destructor callback when creating TSS if per-thread cleanup is needed.
- Common Pitfalls:
  - Do not assume mulle_thread_mutex_do will unlock on function return from inside the block. Avoid return inside the macro block.
  - When using non-blocking once variants, initialization may be skipped if not carefully handled.
  - Mixing different backends' raw types (e.g., using pthread APIs directly) can break portability — always use the wrapper API.
- Idiomatic Usage:
  - Use mulle_thread_once_do(name) for module-level lazy init.
  - Use TSS to avoid global locks when each thread needs its own state.

## 6. Integration Examples

### Example 1: Creating and Joining a Thread

```c
#include "mulle-thread.h"

void *
worker( void *arg)
{
   (void) arg;
   // do work
   mulle_thread_return();
}

int
main( void)
{
   mulle_thread_t   thread;
   int              rval;

   rval = mulle_thread_create( worker, NULL, &thread);
   if( rval)
      return( 1);

   (void) mulle_thread_join( thread);
   return( 0);
}
```

### Example 2: Mutex scoped block using macro

```c
#include "mulle-thread.h"

void
increment_shared( mulle_thread_mutex_t *m, int *shared)
{
   mulle_thread_mutex_do( *m)
   {
      *shared = *shared + 1;
   }
}
```

### Example 3: Using a TSS key with destructor

```c
#include "mulle-thread.h"

static void
free_thread_storage( void *p)
{
   free( p);
}

void
setup_tls( void)
{
   mulle_thread_tss_t   key;

   if( ! mulle_thread_tss_create( free_thread_storage, &key))
   {
      void  *storage;

      storage = malloc( 128);
      mulle_thread_tss_set( key, storage);
   }
}
```

## 7. Dependencies

- Direct mulle-sde / library dependencies:
  - mulle-c11 (recommended)
  - mintomic (optional; used when C11 atomics unavailable)
  - POSIX pthreads (on Unix backends) or Windows API (on Win backends)



<!-- End of TOC -->