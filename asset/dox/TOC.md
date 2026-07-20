# mulle-thread Library Documentation for AI
<!-- Keywords: thread, mutex, recursive-mutex, tss, atomic, condition-variable, portability -->
## 1. Introduction & Purpose

- mulle-thread is a small C wrapper API offering cross-platform thread, mutex, condition variable, thread-local-storage (TSS), and basic atomic operations.
- Solves portability differences between C11 threads, POSIX pthreads, and Windows threads by providing a consistent, minimal API surface.
- Key features: thread create/join/detach, mutex lock/unlock/trylock, recursive mutex, condition variables, TSS create/get/set, once-init utilities (including recursive once), and a compact atomic pointer/function-pointer helper API.
- Relationship: component of mulle-core-style ecosystem; depends on mulle-c11 and optionally mintomic for atomics; falls back to pthreads or Windows native implementations.

## 2. Key Concepts & Design Philosophy

- Thin, explicit wrappers: public API lives in mulle-thread.h and mulle-atomic.h. Platform specifics are implemented in *_pthreads.h, *_c11.h, or *_windows.h (all selected automatically by mulle-thread.h).
- Provide a stable, simplified interface even when underlying platform APIs differ.
- Convenience macros (`mulle_thread_mutex_do`, `mulle_thread_recursive_mutex_do`, `mulle_thread_once_do`, etc.) encourage idiomatic usage but require care (see pitfalls).
- Emphasis on portability and predictability rather than advanced features; small surface area for easier analysis and verification.

## 3. Core API & Data Structures

### 3.1. mulle-thread.h — Main Public Header

This is the single include header. It automatically selects the correct platform backend (C11, pthreads, or Windows), defines once-init primitives, the recursive mutex type, and convenience macros.

#### Typedefs (platform-dependent; defined by backend headers)
- `mulle_thread_t` — thread handle
- `mulle_thread_tss_t` — TSS key handle
- `mulle_thread_mutex_t` — mutex handle
- `mulle_thread_cond_t` — condition variable handle
- `mulle_thread_id_t` — thread identifier (uintptr_t)
- `mulle_thread_rval_t` — thread return value type

#### Once-init types and constants

```c
typedef mulle_atomic_pointer_t   mulle_thread_once_t;
```

```c
#define MULLE_THREAD_ONCE_DATA   0
#define MULLE_THREAD_ONCE_BUSY   1848
#define MULLE_THREAD_ONCE_DONE   1
#define MULLE_THREAD_ONCE_INIT   MULLE_THREAD_ONCE_DATA
```

#### `mulle_thread_once_recursive_t`
- **Purpose:** Recursive once-init state — allows the same thread to call the init function again without deadlock (useful for re-entrant lazy initialization).
- **Key Fields:** `_state` (mulle_atomic_pointer_t), `_thread_id` (mulle_atomic_pointer_t)
- **Initializer:** `MULLE_THREAD_ONCE_RECURSIVE_INIT`

```c
typedef struct
{
   mulle_atomic_pointer_t   _state;
   mulle_atomic_pointer_t   _thread_id;
} mulle_thread_once_recursive_t;

#define MULLE_THREAD_ONCE_RECURSIVE_INIT   ((mulle_thread_once_recursive_t) { MULLE_THREAD_ONCE_INIT, 0 })
```

#### Once-init Functions (declare only; defined in mulle-thread.c)

```c
void   mulle_thread_once( mulle_thread_once_t  *once,
                          void (*init)( void));

void   mulle_thread_once_call( mulle_thread_once_t  *once,
                               void (*init)( void *),
                               void *userinfo);

void   mulle_thread_once_call_recursive( mulle_thread_once_recursive_t  *once,
                                         void (*init)( void *),
                                         void *userinfo);
```

#### Once-init Inline Convenience Functions (in mulle-thread.h)

```c
static inline void   mulle_thread_once_recursive( mulle_thread_once_recursive_t  *once,
                                                   void (*init)( void));

static inline void   mulle_thread_once_noblock( mulle_thread_once_t  *once,
                                                 void (*init)( void));

static inline void   mulle_thread_once_call_noblock( mulle_thread_once_t  *once,
                                                      void (*init)( void *),
                                                      void *userinfo);
```

#### Once-init Convenience Macros

- `mulle_thread_once_do(name)` — Block-scoped once: declares a static `mulle_thread_once_t`, blocks contending threads, runs the block body exactly once. `break` exits; `return` must not be used.
- `mulle_thread_once_do_recursive(name)` — Recursive variant: same thread can re-enter and immediately succeed without blocking. Uses `mulle_thread_once_recursive_t`.
- `mulle_thread_once_do_noblock(name)` — Non-blocking variant: only runs the block if the once was not already started; otherwise skips. Old behavior.

#### Mutex Convenience Macro

```c
#define mulle_thread_mutex_do( mutex)   \
   _mulle_thread_mutex_do( mutex, __LINE__)
```

Locks `mutex` (by address-of), executes block body, then unlocks. Uses `__LINE__` for unique loop variable names, preventing nesting conflicts. `break` and `continue` exit the block and unlock; `return` from inside the block does NOT unlock — the mutex will remain locked.

#### Recursive Mutex

```c
typedef struct
{
   mulle_thread_mutex_t    _mutex;
   mulle_atomic_pointer_t  _thread_id;
   mulle_atomic_pointer_t  _depth;
} mulle_thread_recursive_mutex_t;
```

- **Purpose:** A mutex that allows the owning thread to lock it multiple times without deadlock. Internally wraps a standard mutex and tracks owner thread ID and recursion depth with atomics.

**Lifecycle Functions (defined in mulle-thread.c):**

```c
int   mulle_thread_recursive_mutex_init( mulle_thread_recursive_mutex_t *p);
int   mulle_thread_recursive_mutex_done( mulle_thread_recursive_mutex_t *p);
```

**Core Operations (defined in mulle-thread.c):**

```c
void  mulle_thread_recursive_mutex_lock( mulle_thread_recursive_mutex_t *p);
void  mulle_thread_recursive_mutex_unlock( mulle_thread_recursive_mutex_t *p);
int   mulle_thread_recursive_mutex_trylock( mulle_thread_recursive_mutex_t *p); // returns 0 on success
```

**Convenience Macro:**

```c
#define mulle_thread_recursive_mutex_do( mutex)   \
   _mulle_thread_recursive_mutex_do( mutex, __LINE__)
```

Same semantics as `mulle_thread_mutex_do` but for recursive mutexes. Re-entrant by the same thread.

### 3.2. Backend Headers — Thread, Mutex, Cond, TSS API

All three backends (pthreads, c11, windows) expose the same API. Return semantics: 0 for success, non-zero for error.

#### Thread

```c
int   mulle_thread_create( mulle_thread_function_t *f, void *arg, mulle_thread_t *p_thread);
mulle_thread_rval_t mulle_thread_join( mulle_thread_t thread);
int   mulle_thread_detach( mulle_thread_t thread);
void  mulle_thread_exit( int rval);
void  mulle_thread_yield( void);
mulle_thread_t      mulle_thread_self( void);
mulle_thread_id_t   mulle_thread_id( void);
mulle_thread_id_t   mulle_thread_get_id( mulle_thread_t thread);
```

Note: `mulle_thread_create` returns the created thread via `p_thread` (last parameter) — different from pthreads where the thread pointer is the second argument.

#### Mutex

```c
int   mulle_thread_mutex_init( mulle_thread_mutex_t *lock);
int   mulle_thread_mutex_lock( mulle_thread_mutex_t *lock);
int   mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock);
int   mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock);
int   mulle_thread_mutex_done( mulle_thread_mutex_t *lock);
```

#### Condition Variable

```c
int   mulle_thread_cond_init( mulle_thread_cond_t *cond);
int   mulle_thread_cond_done( mulle_thread_cond_t *cond);
int   mulle_thread_cond_wait( mulle_thread_cond_t *cond, mulle_thread_mutex_t *mutex);
int   mulle_thread_cond_signal( mulle_thread_cond_t *cond);
int   mulle_thread_cond_broadcast( mulle_thread_cond_t *cond);
int   mulle_thread_cond_timedwait( mulle_thread_cond_t *cond, mulle_thread_mutex_t *mutex, struct timespec *abstime);
```

#### Thread-Local Storage (TSS)

```c
int   mulle_thread_tss_create( mulle_thread_callback_t *f, mulle_thread_tss_t *key);
void  mulle_thread_tss_free( mulle_thread_tss_t key);
void  *mulle_thread_tss_get( mulle_thread_tss_t key);
int   mulle_thread_tss_set( mulle_thread_tss_t key, void *value);
```

- `mulle_thread_callback_t` is `void (*)(void *)`. Pass a destructor callback to `tss_create` if per-thread cleanup is needed.
- Returned TSS key of 0 is valid (unlike some platforms).

### 3.3. mulle-atomic.h — Atomic Helpers

Purpose: small set of atomic helpers for pointer and function-pointer operations, using C11 `stdatomic.h` or mintomic as available.

#### Core Types

```c
typedef void   (*mulle_functionpointer_t)( void);
typedef _Atomic( void *)                mulle_atomic_pointer_t;         // C11
typedef _Atomic( mulle_functionpointer_t) mulle_atomic_functionpointer_t; // C11
```

#### Atomic Read/Write (inline)

```c
void  *_mulle_atomic_pointer_read( mulle_atomic_pointer_t *address);
void  _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address, void *value);

void  *_mulle_atomic_pointer_read_nonatomic( mulle_atomic_pointer_t *p);
void  _mulle_atomic_pointer_write_nonatomic( mulle_atomic_pointer_t *p, void *value);
// (also: _mulle_atomic_pointer_nonatomic_read, _mulle_atomic_pointer_nonatomic_write — older names)

mulle_functionpointer_t  _mulle_atomic_functionpointer_read( mulle_atomic_functionpointer_t *address);
void                     _mulle_atomic_functionpointer_write( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value);
```

#### Atomic CAS (inline)

```c
// Returns the actual value at address after the attempt
void  *__mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address, void *value, void *expect);

// Returns 1 if CAS succeeded, 0 otherwise
int   _mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address, void *value, void *expect);

// Deprecated aliases: _mulle_atomic_pointer_compare_and_swap, __mulle_atomic_pointer_compare_and_swap

// Function-pointer CAS variants
mulle_functionpointer_t  __mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
int                      _mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
```

#### Atomic Set (spin-loop CAS; inline)

```c
void  *_mulle_atomic_pointer_set( mulle_atomic_pointer_t *address, void *value);

mulle_functionpointer_t  _mulle_atomic_functionpointer_set( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value);
```

#### Atomic Arithmetic (inline)

```c
// Returns the previous value (before increment/decrement)
void  *_mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address);

// Returns the result (previous + diff), NOT the previous value
void  *_mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, intptr_t diff);
```

#### Memory Barrier (inline)

```c
void  mulle_atomic_memory_barrier( void);
```

## 4. Performance Characteristics

- Atomics: O(1) per operation (hardware CAS / atomic primitives). Intended for pointer/function-pointer CAS, set, increment/decrement operations.
- Mutex operations: O(1) for lock/unlock, but blocking cost depends on scheduler and number of contending threads.
- Recursive Mutex: O(1) per lock/unlock; additional cost of atomic pointer read for owner-check on every lock. Re-entrant lock/unlock is a simple atomic increment/decrement (cheap).
- Condition variables: waiting is O(1) (per wait), signaling is O(1) but waking cost depends on contention.
- Thread creation/join: relatively expensive (OS cost). Prefer worker pools for high-rate tasks.
- Once-init: first caller pays init cost + CAS; other threads spin-yield wait (or skip for non-blocking variants). Recursive once adds a thread-ID read for re-entrancy check.
- Thread-safety: API is thread-safe where appropriate (atomics, mutex, cond); data structures and object lifecycles are the caller's responsibility. The library provides primitives, not higher-level synchronized containers.

## 5. AI Usage Recommendations & Patterns

- **Best Practices:**
  - Use the public lifecycle functions (init/done) for mutexes, condvars, and recursive mutexes; do not manipulate platform internals.
  - Use `mulle_thread_mutex_do(mutex)` or `mulle_thread_recursive_mutex_do(mutex)` for scoped locking. `break` and `continue` will exit the block and unlock. Do NOT use `return` inside these blocks — the mutex will remain locked.
  - Use `mulle_thread_recursive_mutex_t` when a single thread may need to lock the same mutex multiple times (e.g., recursive function calls, nested callbacks).
  - Prefer `mulle_thread_once_call` over `mulle_thread_once` when user data is needed. Use `mulle_thread_once_call_recursive` when re-entrant lazy init is possible.
  - Use `mulle_thread_once_do(name)` for module-level lazy init. Use `mulle_thread_once_do_recursive(name)` when the init path may re-enter the same once.
  - Use TSS (`mulle_thread_tss_create`/`get`/`set`) for per-thread state to avoid global locks.
  - Provide a destructor callback when creating TSS if per-thread cleanup is needed.
- **Common Pitfalls:**
  - `mulle_thread_mutex_do` and `mulle_thread_recursive_mutex_do` will NOT unlock on function return from inside the block. Avoid `return` inside these macro blocks.
  - When using non-blocking once variants (`mulle_thread_once_do_noblock`, `mulle_thread_once_call_noblock`), initialization may be skipped if another thread is already initializing.
  - Mixing different backends' raw types (e.g., using pthread APIs directly) can break portability — always use the wrapper API.
  - `mulle_thread_once` does not guarantee that exceptions or thread cancellation within the init function will clear the once flag for a second run.
  - The recursive mutex's internal fields (`_mutex`, `_thread_id`, `_depth`) are private; do not access them directly.
- **Idiomatic Usage:**
  - Use `mulle_thread_once_do(name)` for module-level lazy init.
  - Use TSS to avoid global locks when each thread needs its own state.
  - Use `mulle_thread_recursive_mutex_t` for re-entrant locking patterns.

## 6. Integration Examples

### Example 1: Creating and Joining a Thread

```c
#include <mulle-thread/mulle-thread.h>

static mulle_thread_rval_t
worker( void *arg)
{
   (void) arg;
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

### Example 2: Mutex Scoped Block

```c
#include <mulle-thread/mulle-thread.h>

static void
increment_shared( mulle_thread_mutex_t *m, int *shared)
{
   mulle_thread_mutex_do( *m)
   {
      *shared = *shared + 1;
   }
}
```

### Example 3: Recursive Mutex

```c
#include <mulle-thread/mulle-thread.h>

static mulle_thread_recursive_mutex_t  s_lock;

static void
do_stuff( unsigned int depth)
{
   mulle_thread_recursive_mutex_do( s_lock)
   {
      if( depth)
         do_stuff( depth - 1);
   }
}

int
main( void)
{
   mulle_thread_recursive_mutex_init( &s_lock);
   do_stuff( 3);
   mulle_thread_recursive_mutex_done( &s_lock);
   return( 0);
}
```

### Example 4: Recursive Once-Init (Re-entrant Lazy Init)

```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static void
ensure_ready( void *userinfo)
{
   printf( "init with %p\n", userinfo);
}

static void
callerA( void)
{
   mulle_thread_once_do_recursive( s_once)
   {
      ensure_ready( (void *) 0x1);
      // can re-enter same init from callback without deadlock
   }
}

int
main( void)
{
   callerA();
   return( 0);
}
```

### Example 5: TSS Key with Destructor

```c
#include <mulle-thread/mulle-thread.h>
#include <stdlib.h>

static void
free_thread_storage( void *p)
{
   free( p);
}

static void
setup_tls( void)
{
   mulle_thread_tss_t   key;

   if( ! mulle_thread_tss_create( free_thread_storage, &key))
   {
      void   *storage;

      storage = malloc( 128);
      mulle_thread_tss_set( key, storage);
   }
}
```

## 7. Dependencies

- Direct mulle-sde / library dependencies:
  - `mulle-c11` — Cross-platform C compiler glue
  - `mintomic` — Optional; used when C11 atomics are unavailable
  - POSIX pthreads (on Unix backends) or Windows API (on Windows backends)
