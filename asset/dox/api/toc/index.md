# mulle-thread Library Documentation for AI
<!-- Keywords: thread, mutex, recursive-mutex, condition-variable, tss, atomic, portable -->
## 1. Introduction & Purpose

- mulle-thread is a small C wrapper library offering cross-platform thread,
  mutex, recursive mutex, condition variable, thread-local-storage (TSS), and
  a compact atomic (pointer/function-pointer/integer) operation API.
- It solves the portability gap between C11 `<threads.h>`/`<stdatomic.h>`,
  POSIX pthreads, and Windows native threads by exposing one consistent,
  simplified API. On Windows it does considerably more work internally.
- Key features: thread create/join/detach/exit, mutex lock/unlock/trylock,
  a recursive mutex, condition variables, TSS with destructor callbacks,
  blocking and non-blocking one-time initialization (including a recursive
  variant), and C11/mintomic atomics.
- Relationship: `mulle-thread` is a component of the `mulle-core` library and
  is consumed through the umbrella header `<mulle-core/mulle-core.h>`. It
  depends on `mulle-c11` and optionally uses the bundled `mintomic` backend
  when C11 atomics are unavailable.

## 2. Key Concepts & Design Philosophy

- **Thin, explicit wrappers.** The public API is `mulle-thread.h` (threads,
  mutex, cond, once, recursive mutex) and `mulle-atomic.h` (atomics). The
  platform details live in one of three automatically-selected backends:
  `mulle-thread-c11.h`, `mulle-thread-pthreads.h`, or
  `mulle-thread-windows.h`.
- **Automatic backend selection.** `mulle-thread.h` defines
  `MULLE_THREAD_USE_PTHREADS` for contexts that lie about `<threads.h>`
  (DragonFly BSD, Cosmopolitan, and ThreadSanitizer builds). Otherwise it uses
  the C11 backend when `<threads.h>` is present, the Windows backend on
  `_WIN32`, and pthreads everywhere else. Atomically, `mulle-atomic.h` uses
  `mulle-atomic-c11.h` when `<stdatomic.h>` is available, else the mintomic
  backend `mulle-atomic-mintomic.h`.
- **Simplified/`0`-on-success convention.** Backend wrappers return `0` for
  success and `-1` (or a meaningful errno such as `EBUSY`/`ETIMEDOUT`) for
  failure; `errno` is not set. This is deliberately not the raw pthreads
  convention.
- **Consistent signatures across backends.** Parameter order is normalized
  (for example `mulle_thread_create` takes the output thread handle last,
  unlike pthreads).
- **Macros for scoped usage.** `mulle_thread_mutex_do`,
  `mulle_thread_recursive_mutex_do`, and the `mulle_thread_once_do` family
  provide block-scoped locking and one-time init. They use `__LINE__` for
  unique loop variables, so nested/different uses do not collide, but they
  have strict rules (see pitfall about `return`).
- **Recursive once by thread identity.** A once slot tracks the owning thread
  id so the same thread may re-enter the init block instead of deadlocking.

## 3. Core API & Data Structures

### 3.1. `mulle-thread.h` — Main Public Header

Include this single header; it selects the backend and declares the portable
types and the out-of-line once/recursive-mutex functions.

```c
#define MULLE__THREAD_VERSION  ((4UL << 20) | (11 << 8) | 0)
```

#### Compile-time configuration defines

```c
#define MULLE_THREAD_MUTEX_NEEDS_DONE   1
#define MULLE_THREAD_COND_NEEDS_DONE    1 // for the future
```

Indicates mutexes (and, prospectively, condition variables) must be released
with an explicit `_done` call.

#### Typedefs (platform-dependent; declared by the selected backend)

- `mulle_thread_t` — thread handle.
- `mulle_thread_tss_t` — TSS key handle.
- `mulle_thread_mutex_t` — mutex; native type on C11/pthreads, a struct with a
  `CRITICAL_SECTION` and an atomic `owner` on Windows.
- `mulle_thread_cond_t` — condition variable.
- `mulle_thread_id_t` — thread identifier (`uintptr_t`).
- `mulle_thread_rval_t` — thread return value type (`int` on C11, `void *` on
  pthreads, `unsigned int` on Windows).
- `mulle_thread_callback_t` — `void (*)(void *)` TSS destructor type.
- `mulle_thread_function_t` — thread entry type, includes `MULLE_THREAD_CALL`.

```c
typedef mulle_thread_rval_t  (MULLE_THREAD_CALL mulle_thread_function_t)( void *);
typedef void                  mulle_thread_callback_t( void *);
```

`MULLE_THREAD_CALL` expands to `__stdcall` only on 32-bit x86 Windows (where
`_beginthreadex` requires it) and to nothing elsewhere. Declare your thread
entry functions with it for portability.

#### Once-init types and constants

```c
typedef mulle_atomic_pointer_t   mulle_thread_once_t;

// Raw scalar values a once slot holds during its lifecycle. These are used as
// plain integers cast to (void *) in the atomic compare-and-swaps below.
#define MULLE_THREAD_ONCE_DATA   0
#define MULLE_THREAD_ONCE_BUSY   1848
#define MULLE_THREAD_ONCE_DONE   1

// Static initializer for a mulle_thread_once_t. This must be backend-aware:
// with C11 atomics mulle_atomic_pointer_t is a scalar (`= 0`), with the
// mintomic backend it is a struct (`= { 0 }`).
#define MULLE_THREAD_ONCE_INIT   MULLE_ATOMIC_POINTER_INIT
```

Do not confuse the runtime state value `MULLE_THREAD_ONCE_DATA` (which happens
to be `0`) with the initializer macro `MULLE_THREAD_ONCE_INIT`; the latter
expands to `MULLE_ATOMIC_POINTER_INIT` so it works on both atomic backends.

#### `mulle_thread_once_recursive_t`

```c
typedef struct
{
   mulle_atomic_pointer_t   _state;
   mulle_atomic_pointer_t   _thread_id;
} mulle_thread_once_recursive_t;

#define MULLE_THREAD_ONCE_RECURSIVE_INIT   { MULLE_THREAD_ONCE_INIT, MULLE_ATOMIC_POINTER_INIT }
```

- **Purpose:** one-time initialization that lets the same thread call the init
  path again without deadlocking (re-entrant lazy init).
- **Key fields:** `_state` (the DATA/BUSY/DONE slot) and `_thread_id` (the
  thread currently running/owning init). Fields are private; do not touch.

#### Once-init Functions (declared here, defined in `mulle-thread.c`)

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

- `mulle_thread_once`: blocking init with no user data. Other callers
  spin-yield until the slot reaches `DONE`.
- `mulle_thread_once_call`: blocking init passing `userinfo` to `init`.
- `mulle_thread_once_call_recursive`: same-thread re-entry returns immediately;
  other threads wait. `init` must not be re-run by the owning thread.

#### Once-init Inline Convenience Functions (defined in `mulle-thread.h`)

```c
static inline void   mulle_thread_once_recursive( mulle_thread_once_recursive_t  *once,
                                                   void (*init)( void));

static inline void   mulle_thread_once_noblock( mulle_thread_once_t  *once,
                                                 void (*init)( void));

static inline void   mulle_thread_once_call_noblock( mulle_thread_once_t  *once,
                                                      void (*init)( void *),
                                                      void *userinfo);
```

- `mulle_thread_once_recursive`: wrapper for
  `mulle_thread_once_call_recursive` with `NULL` userinfo.
- `mulle_thread_once_noblock` / `mulle_thread_once_call_noblock`: do **not**
  wait. Only the thread that wins the CAS runs `init`; if init is BUSY or DONE
  the call is skipped. The winner still publishes `MULLE_THREAD_ONCE_DONE`
  afterwards, so mixing this with a blocking once on the same slot cannot hang.

#### Once-init Convenience Macros

```c
#define mulle_thread_once_do( name)
#define mulle_thread_once_do_recursive( name)
#define mulle_thread_once_do_noblock( name)
```

- `mulle_thread_once_do(name)` — declares `static mulle_thread_once_t name =
  MULLE_THREAD_ONCE_INIT;` and runs the following block exactly once. Other
  threads spin-yield until done. `break` terminates the block cleanly; do not
  `return` from inside a body that must complete the protocol.
- `mulle_thread_once_do_recursive(name)` — same, using
  `mulle_thread_once_recursive_t`; the owning thread may re-enter and proceed
  without blocking.
- `mulle_thread_once_do_noblock(name)` — non-blocking; the block runs only if
  this thread wins, otherwise it is skipped.

#### Mutex Convenience Macro

```c
#define mulle_thread_mutex_do( mutex)   \
   _mulle_thread_mutex_do( mutex, __LINE__)
```

Takes the address of `mutex` (pass the variable itself, or `*ptr`), locks,
runs the block, then unlocks. `break`/`continue` exit and unlock. `return`
from inside leaves the mutex locked; and a `mulle_thread_mutex_do` must not be
nested inside another `mulle_thread_mutex_do`.

#### `mulle_thread_recursive_mutex_t`

```c
typedef struct
{
   mulle_thread_mutex_t    _mutex;
   mulle_atomic_pointer_t  _thread_id;  // owning thread id, or NULL
   mulle_atomic_pointer_t  _depth;      // recursion depth (1-based when locked)
} mulle_thread_recursive_mutex_t;
```

**Lifecycle Functions (defined in `mulle-thread.c`):**

```c
int   mulle_thread_recursive_mutex_init( mulle_thread_recursive_mutex_t *p);
int   mulle_thread_recursive_mutex_done( mulle_thread_recursive_mutex_t *p);
```

**Core Operations (defined in `mulle-thread.c`):**

```c
void  mulle_thread_recursive_mutex_lock( mulle_thread_recursive_mutex_t *p);
void  mulle_thread_recursive_mutex_unlock( mulle_thread_recursive_mutex_t *p);
int   mulle_thread_recursive_mutex_trylock( mulle_thread_recursive_mutex_t *p); // 0=acquired
```

- The owning thread may lock repeatedly; each lock increments depth.
  `unlock` decrements and only releases the underlying mutex at depth 1.
- `trylock` returns `0` when acquired (including same-thread re-entry),
  `-1` when busy/error.
- Fields are private.

**Convenience Macro:**

```c
#define mulle_thread_recursive_mutex_do( mutex)   \
   _mulle_thread_recursive_mutex_do( mutex, __LINE__)
```

Same block semantics as `mulle_thread_mutex_do`, but re-entrant.

#### Test Helper Macro

```c
#define MULLE_THREAD_UNPLEASANT_RACE_YIELD()
```

When `MULLE_TEST` is defined (and `NO_MULLE_THREAD_UNPLEASANT_RACE_YIELD` is
not), randomly yields (and on non-Windows occasionally nanosleeps) to expose
races during testing. Otherwise a no-op.

### 3.2. Backend Headers — Thread, Mutex, Cond, TSS API

`mulle-thread-c11.h`, `mulle-thread-pthreads.h`, and `mulle-thread-windows.h`
implement the same API. Return semantics: `0` success, `-1` generic failure,
and meaningful errno values (`EBUSY`, `ETIMEDOUT`) where applicable.

#### Backend type definitions

C11:

```c
typedef int   mulle_thread_rval_t;
#define mulle_thread_return()  return( 0)
typedef mtx_t       mulle_thread_mutex_t;
typedef cnd_t       mulle_thread_cond_t;
typedef tss_t       mulle_thread_tss_t;
typedef thrd_t      mulle_thread_t;
typedef int         mulle_thread_native_rval_t;
typedef uintptr_t   mulle_thread_id_t;
typedef mulle_thread_rval_t  (MULLE_THREAD_CALL mulle_thread_function_t)( void *);
typedef void                  mulle_thread_callback_t( void *);
```

pthreads:

```c
typedef void   *mulle_thread_rval_t;
#define mulle_thread_return()  return( NULL)
typedef pthread_mutex_t   mulle_thread_mutex_t;
typedef pthread_cond_t    mulle_thread_cond_t;
typedef pthread_key_t     mulle_thread_tss_t;
typedef pthread_t         mulle_thread_t;
typedef void              *mulle_thread_native_rval_t;
typedef uintptr_t         mulle_thread_id_t;
```

Windows:

```c
typedef unsigned int   mulle_thread_rval_t;
#define mulle_thread_return()  do { mulle_thread_windows_destroy_tss_if_needed(); return( 0); } while(0)
typedef DWORD    mulle_thread_tss_t;
typedef HANDLE   mulle_thread_t;
typedef DWORD    mulle_thread_native_rval_t;
typedef uintptr_t   mulle_thread_id_t;

typedef struct
{
   CRITICAL_SECTION         section;
   mulle_atomic_pointer_t   owner;
} mulle_thread_mutex_t;

typedef CONDITION_VARIABLE   mulle_thread_cond_t;
```

On Windows `MULLE_THREAD_CALL` is `__stdcall` on 32-bit x86 and empty
otherwise.

#### Thread

```c
static inline mulle_thread_t        mulle_thread_self( void);
static inline mulle_thread_id_t     mulle_thread_id( void);
static inline mulle_thread_id_t     mulle_thread_get_id( mulle_thread_t thread);
static inline int                   mulle_thread_equal( mulle_thread_t thread1,
                                                       mulle_thread_t thread2);
static inline int                   mulle_thread_create( mulle_thread_function_t *f,
                                                         void *arg,
                                                         mulle_thread_t *p_thread);
static inline mulle_thread_rval_t   mulle_thread_join( mulle_thread_t thread);
static inline int                   mulle_thread_detach( mulle_thread_t thread);
static inline void                  mulle_thread_exit( int rval);
static inline void                  mulle_thread_yield( void);
```

- `mulle_thread_create` returns the handle through the last argument
  `p_thread` — different from pthreads. Returns `0`/`-1`.
- `mulle_thread_join` returns the thread function's value. Use
  `mulle_thread_return()` inside the thread function rather than a bare
  `return` (on Windows it also flushes TSS destructors).
- `mulle_thread_self` / `mulle_thread_get_id` / `mulle_thread_equal` let you
  identify and compare threads portably (`pthread_t`/`HANDLE` are not
  guaranteed comparable).
- Thread return values are not fully portable: small nonnegative integers are
  safe across backends; pointers, large values, and negatives are not.

#### Mutex

```c
static inline int   mulle_thread_mutex_init( mulle_thread_mutex_t *lock);
static inline int   mulle_thread_mutex_lock( mulle_thread_mutex_t *lock);
static inline int   mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock);
static inline int   mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock);
static inline int   mulle_thread_mutex_done( mulle_thread_mutex_t *lock);
```

- `0` success, `-1` error; `trylock` returns `EBUSY` when already held, `0`
  when acquired.
- The mutex is non-recursive on every backend. On Windows the recursive
  `CRITICAL_SECTION` is tamed with an atomic owner pointer so that re-locking
  from the owner self-deadlocks (asserting in debug builds) like pthreads.
- Windows `mulle_thread_mutex_t` is a struct, not an opaque handle; never copy
  or compare it.

#### Condition Variable

```c
static inline int   mulle_thread_cond_init( mulle_thread_cond_t *cond);
static inline int   mulle_thread_cond_done( mulle_thread_cond_t *cond);
static inline int   mulle_thread_cond_wait( mulle_thread_cond_t *cond,
                                            mulle_thread_mutex_t *mutex);
static inline int   mulle_thread_cond_signal( mulle_thread_cond_t *cond);
static inline int   mulle_thread_cond_broadcast( mulle_thread_cond_t *cond);
static inline int   mulle_thread_cond_timedwait( mulle_thread_cond_t *cond,
                                                 mulle_thread_mutex_t *mutex,
                                                 const struct timespec *abstime);
```

- `cond_timedwait` takes an **absolute** `struct timespec` (same as pthreads)
  and returns `ETIMEDOUT` on timeout.
- Always wait inside a `while` loop that re-checks the predicate, because
  spurious wakeups are possible.
- On Windows `cond_wait` and `cond_timedwait` are out-of-line
  (`MULLE__THREAD_GLOBAL`), not inline.

#### Thread-Local Storage (TSS)

```c
int    mulle_thread_tss_create( mulle_thread_callback_t *f,
                                mulle_thread_tss_t *key);
void   mulle_thread_tss_free( mulle_thread_tss_t key);
void  *mulle_thread_tss_get( mulle_thread_tss_t key);
int    mulle_thread_tss_set( mulle_thread_tss_t key, void *value);
```

- `f` is an optional destructor invoked when a thread that has a value dies.
- A returned key of `0` is valid.
- Signature variance: the C11 backend takes the callback **by value**
  (`mulle_thread_callback_t f`), while pthreads/Windows take a pointer
  (`mulle_thread_callback_t *f`). Pass the function name in both cases.

Windows-only TSS subsystem management (normally automatic):

```c
MULLE__THREAD_GLOBAL
void   mulle_thread_tss_init( void);
MULLE__THREAD_GLOBAL
void   mulle_thread_tss_done( void);
MULLE__THREAD_GLOBAL
void   mulle_thread_windows_destroy_tss_if_needed( void);
```

- `mulle_thread_tss_init` / `mulle_thread_tss_done` are reference-counted
  init/cleanup of the global TSS destructor table. Do not call `_init` after
  `_done` within the same thread (unless single-threaded).
- `mulle_thread_windows_destroy_tss_if_needed` runs TSS destructors for the
  current thread; it is called automatically by `mulle_thread_return()` and
  `mulle_thread_exit`.

### 3.3. `mulle-atomic.h` — Atomic Helpers

Small atomic helpers for pointer, function-pointer, and pointer-sized integer
operations. It selects `mulle-atomic-c11.h` or `mulle-atomic-mintomic.h`.

#### Types (C11 backend; see `mulle-atomic-c11.h`)

```c
typedef void   (*mulle_functionpointer_t)( void);
typedef _Atomic( void *)                    mulle_atomic_pointer_t;
typedef _Atomic( mulle_functionpointer_t)   mulle_atomic_functionpointer_t;
typedef _Atomic( intptr_t)                  mulle_atomic_intptr_t;

#define MULLE_ATOMIC_POINTER_INIT   0
#define MULLE_ATOMIC_INTPTR_INIT    0
```

With the mintomic backend the types are all `mint_atomicPtr_t` (a struct) and
the initializers are `{ 0 }`. `mulle_atomic_intptr_t` is designed for counters
and statistics: unlike `mulle_atomic_pointer_t` it is not pointer-shaped, so a
forgotten `&` is a compile error instead of a silent load-and-pass.

#### Read / Write (inline)

```c
void  *_mulle_atomic_pointer_read( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_read_acquire( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_read_relaxed( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_read_nonatomic( mulle_atomic_pointer_t *p);
void  *_mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *p);   // old alias

void   _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address, void *value);
void   _mulle_atomic_pointer_write_relaxed( mulle_atomic_pointer_t *address, void *value);
void   _mulle_atomic_pointer_write_nonatomic( mulle_atomic_pointer_t *p, void *value);
void   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *p, void *value); // old alias

mulle_functionpointer_t  _mulle_atomic_functionpointer_read( mulle_atomic_functionpointer_t *address);
mulle_functionpointer_t  _mulle_atomic_functionpointer_read_acquire( mulle_atomic_functionpointer_t *address);
mulle_functionpointer_t  _mulle_atomic_functionpointer_read_relaxed( mulle_atomic_functionpointer_t *address);
mulle_functionpointer_t  _mulle_atomic_functionpointer_read_nonatomic( mulle_atomic_functionpointer_t *p);
mulle_functionpointer_t  _mulle_atomic_functionpointer_nonatomic_read( mulle_atomic_functionpointer_t *p);

void   _mulle_atomic_functionpointer_write( mulle_atomic_functionpointer_t *address,
                                            mulle_functionpointer_t value);
void   _mulle_atomic_functionpointer_write_relaxed( mulle_atomic_functionpointer_t *address,
                                                    mulle_functionpointer_t value);
void   _mulle_atomic_functionpointer_write_nonatomic( mulle_atomic_functionpointer_t *p,
                                                      mulle_functionpointer_t value);
```

- `_read`/`_write` use sequential consistency.
- `_read_acquire` reads with acquire ordering: use it when the loaded value
  gates access to other shared memory published by a release store.
- `_relaxed` variants are for disconnected values only (diagnostic counters,
  statistics, flags that never gate access to other memory).
- `_nonatomic` variants are plain, unsynchronized accesses.

#### Compare-and-Swap (inline)

```c
// Returns the actual value at address after the attempt
void  *__mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address, void *value, void *expect);
void  *__mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address, void *value, void *expect);

// Returns 1 if the CAS succeeded, 0 otherwise
int    _mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address, void *value, void *expect);
int    _mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address, void *value, void *expect);

// Weak CAS (may fail spuriously)
void  *__mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address, void *value, void *expect);
void  *__mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address, void *value, void *expect);
int    _mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address, void *value, void *expect);
int    _mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address, void *value, void *expect);
// also: __mulle_atomic_pointer_cas_weak / _cas_weak_relaxed and the int-returning equivalents

// Function-pointer CAS / weak CAS (same naming scheme)
mulle_functionpointer_t  __mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
int                      _mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
mulle_functionpointer_t  __mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
int                      _mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address, mulle_functionpointer_t value, mulle_functionpointer_t expect);
```

- `__mulle_*` return the actual (possibly updated) value — useful for CAS
  loops; `_mulle_*` return a boolean success flag.
- The parameter order is `(address, value, expect)`, i.e. the **new** value
  precedes the expected one. All CAS calls assert `value != expect`.

#### Atomic Set (spin-loop CAS; defined in `mulle-atomic.h`)

```c
void  *_mulle_atomic_pointer_set( mulle_atomic_pointer_t *address,
                                  void  *value);

mulle_functionpointer_t
   _mulle_atomic_functionpointer_set( mulle_atomic_functionpointer_t *address,
                                      mulle_functionpointer_t value);
```

Spins in a CAS loop until the store succeeds; returns the previous value.

#### Atomic Arithmetic (inline)

```c
// Return the value before increment/decrement
void  *_mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_increment_relaxed( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address);
void  *_mulle_atomic_pointer_decrement_relaxed( mulle_atomic_pointer_t *address);

// Return the result (previous + diff), NOT the previous value
void  *_mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, intptr_t diff);
void  *_mulle_atomic_pointer_add_relaxed( mulle_atomic_pointer_t *address, intptr_t diff);
```

The arithmetic helpers type-pun the pointer atomic as `atomic_intptr_t`; only
store plain pointer-sized integers in an atomic used with these.

#### Atomic Integer Operations (defined in `mulle-atomic.h`)

For counters, statistics, and other plain integers. All are relaxed and all
return the **previous** value. Do not use them for reference counts or
anything that gates access to other memory.

```c
intptr_t  _mulle_atomic_intptr_read_relaxed( mulle_atomic_intptr_t *address);
intptr_t  _mulle_atomic_intptr_read_nonatomic( mulle_atomic_intptr_t *address);

void      _mulle_atomic_intptr_write_relaxed( mulle_atomic_intptr_t *address, intptr_t value);
void      _mulle_atomic_intptr_write_nonatomic( mulle_atomic_intptr_t *address, intptr_t value);

intptr_t  _mulle_atomic_intptr_increment_relaxed( mulle_atomic_intptr_t *address);
intptr_t  _mulle_atomic_intptr_decrement_relaxed( mulle_atomic_intptr_t *address);
intptr_t  _mulle_atomic_intptr_add_relaxed( mulle_atomic_intptr_t *address, intptr_t diff);
```

`_mulle_atomic_intptr_add_relaxed` returns the previous value (unlike
`_mulle_atomic_pointer_add`).

#### Memory Barrier (inline)

```c
void   mulle_atomic_memory_barrier( void);
```

A **thread** fence: `atomic_thread_fence( memory_order_seq_cst)` on C11,
`mint_thread_fence_seq_cst()` on mintomic. It is required by the once publish
protocol on weakly ordered CPUs (ARM/ARM64/PPC). Under ThreadSanitizer the
gcc `-Wtsan` warning for a standalone fence is suppressed, but the fence is
still emitted. Detection of a TSan build (`MULLE_THREAD_TSAN`) also forces the
pthreads thread backend, because TSan cannot intercept `thrd_create`.

#### Deprecated / old API (defined in `mulle-atomic.h`)

```c
int   _mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address,
                                              void *value, void *expect);
void *__mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address,
                                               void *value, void *expect);

mulle_functionpointer_t
   __mulle_atomic_functionpointer_compare_and_swap( mulle_atomic_functionpointer_t *address,
                                                    mulle_functionpointer_t value,
                                                    mulle_functionpointer_t expect);
int
   _mulle_atomic_functionpointer_compare_and_swap( mulle_atomic_functionpointer_t *address,
                                                   mulle_functionpointer_t value,
                                                   mulle_functionpointer_t expect);
```

Prefer the `_cas` / `_weakcas` names in new code.

### 3.4. Out-of-line Implementations

`mulle-thread.c` defines: `mulle_thread_once`, `mulle_thread_once_call`,
`mulle_thread_once_call_recursive`, and the five
`mulle_thread_recursive_mutex_*` functions. It also includes
`mintomic_gcc.c` when C11 atomics are unavailable.

`mulle-thread-windows.c` defines (Windows only): `mulle_thread_join`,
`mulle_thread_exit`, `mulle_thread_tss_init`/`_done`,
`mulle_thread_tss_create`/`_free`, `mulle_thread_cond_wait`/`_timedwait`, and
`mulle_thread_windows_destroy_tss_if_needed`.

## 4. Performance Characteristics

- Atomics: O(1) per operation (hardware CAS/atomic primitives). `_relaxed`
  variants avoid fences for disconnected counters; `_read`/`_write` and the
  CAS primitives are sequentially consistent.
- Mutex lock/unlock: O(1) plus scheduling cost under contention.
- Recursive mutex: O(1); each lock/unlock performs an extra atomic read of the
  owner id. Same-thread re-entry is a cheap atomic increment/decrement.
- Condition variables: O(1) per wait/signal; wakeup cost depends on the number
  of waiters and the OS scheduler.
- Thread create/join: dominated by OS cost; use worker pools where threads are
  created frequently.
- Once-init: the winner pays the init cost plus a CAS; other threads
  spin-yield. Non-blocking variants skip instead of waiting. Recursive once
  adds a thread-id read on each call.
- Thread-safety: the primitives are thread-safe. Higher-level object
  lifecycles and shared data remain the caller's responsibility.

## 5. AI Usage Recommendations & Patterns

- **Best practices**
  - Always use the lifecycle functions: `_init`/`_done` for mutexes and
    condition variables, `mulle_thread_recursive_mutex_init`/`_done` for the
    recursive mutex, `mulle_thread_tss_create`/`_free` for TSS keys.
  - Use `mulle_thread_mutex_do(mutex)` and
    `mulle_thread_recursive_mutex_do(mutex)` for scoped locking.
  - Use `mulle_thread_once_call` (not the old `mulle_thread_once`) when you
    need to pass data; use `mulle_thread_once_call_recursive` or
    `mulle_thread_once_do_recursive` when the init path may re-enter.
  - Use TSS plus a destructor callback for per-thread state and cleanup.
  - Prefer `_read_acquire`/release-aware operations when a loaded value gates
    access to other shared memory; reserve `_relaxed` for counters/statistics.
  - Call `mulle_thread_return()` (or on Windows rely on it to flush TSS)
    instead of a bare `return` from a thread function.
- **Common pitfalls**
  - `return` inside a `mulle_thread_mutex_do` /
    `mulle_thread_recursive_mutex_do` block does **not** unlock; use `break`.
  - Never nest `mulle_thread_mutex_do` on the same (non-recursive) mutex.
  - Non-blocking once variants (`_do_noblock`, `_call_noblock`) may skip
    initialization entirely if another thread won.
  - `mulle_thread_once` does not clear the once flag if init throws or the
    thread is cancelled; a later re-run is not guaranteed.
  - Do not access the `_`-prefixed fields of `mulle_thread_once_recursive_t`
    or `mulle_thread_recursive_mutex_t`.
  - On Windows `mulle_thread_mutex_t` is a struct; do not copy or compare it.
  - Do not mix raw backend calls (pthreads/Win32/`<threads.h>`) with the
    wrapper API; especially `mulle_thread_create`'s argument order differs.
- **Idiomatic usage**
  - Module-level lazy init: `mulle_thread_once_do(name)`.
  - Re-entrant lazy init: `mulle_thread_once_do_recursive(name)`.
  - Per-thread scratch/session state: TSS with a destructor.
  - Re-entrant locking of a shared structure: `mulle_thread_recursive_mutex_t`.

## 6. Integration Examples

### Example 1: Creating, Running and Joining a Thread

```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static mulle_thread_rval_t MULLE_THREAD_CALL
thread_main( void *arg)
{
   if( arg != (void *) 0x1847)
      printf( "mistaken arg\n");

   printf( "thread done\n");
   return( (mulle_thread_rval_t) 1848);
}

int   main( void)
{
   mulle_thread_t         thread;
   mulle_thread_rval_t    rval;

   if( mulle_thread_create( thread_main, (void *) 0x1847, &thread))
      return( 1);

   rval = mulle_thread_join( thread);
   if( rval == (mulle_thread_rval_t) -1)
      return( 1);

   return( 0);
}
```

### Example 2: Mutex with the Scoped Macro

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

static mulle_thread_recursive_mutex_t   s_lock;

static void
do_stuff( unsigned int depth)
{
   mulle_thread_recursive_mutex_do( s_lock)
   {
      if( depth)
         do_stuff( depth - 1);
   }
}

int   main( void)
{
   if( mulle_thread_recursive_mutex_init( &s_lock))
      return( 1);

   do_stuff( 3);

   mulle_thread_recursive_mutex_done( &s_lock);
   return( 0);
}
```

### Example 4: One-time Initialization

```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static int   value;

static void
init_value( void)
{
   value = 42;
}

int   main( void)
{
   mulle_thread_once_do( value_once)
   {
      init_value();
   }

   printf( "value is %d\n", value);
   return( 0);
}
```

### Example 5: TSS with a Destructor

```c
#include <mulle-thread/mulle-thread.h>
#include <stdlib.h>

static mulle_thread_tss_t   s_key;

static void
free_thread_storage( void *p)
{
   free( p);
}

static mulle_thread_rval_t MULLE_THREAD_CALL
worker( void *arg)
{
   void   *storage;

   (void) arg;

   storage = malloc( 128);
   if( mulle_thread_tss_set( s_key, storage))
   {
      free( storage);
      return( (mulle_thread_rval_t) 1);
   }

   if( mulle_thread_tss_get( s_key) != storage)
      return( (mulle_thread_rval_t) 2);

   mulle_thread_return();
}

int   main( void)
{
   mulle_thread_t   thread;

   if( mulle_thread_tss_create( free_thread_storage, &s_key))
      return( 1);

   if( mulle_thread_create( worker, NULL, &thread))
      return( 1);

   mulle_thread_join( thread);
   mulle_thread_tss_free( s_key);

   return( 0);
}
```

### Example 6: Condition Variable Producer/Consumer

```c
#include <mulle-thread/mulle-thread.h>

static mulle_thread_mutex_t   s_mutex;
static mulle_thread_cond_t    s_cond;
static int                    s_data      = 0;
static int                    s_available = 0;

static void
produce( int value)
{
   mulle_thread_mutex_lock( &s_mutex);
   s_data      = value;
   s_available = 1;
   mulle_thread_cond_signal( &s_cond);
   mulle_thread_mutex_unlock( &s_mutex);
}

static int
consume( void)
{
   int   value;

   mulle_thread_mutex_lock( &s_mutex);
   while( ! s_available)
      mulle_thread_cond_wait( &s_cond, &s_mutex);
   value       = s_data;
   s_available = 0;
   mulle_thread_mutex_unlock( &s_mutex);

   return( value);
}
```

### Example 7: Atomic CAS and Relaxed Counter

```c
#include <mulle-thread/mulle-thread.h>

static mulle_atomic_pointer_t   s_slot;
static mulle_atomic_intptr_t    s_count = MULLE_ATOMIC_INTPTR_INIT;

static void
install( void *value)
{
   void   *actual;

   // __mulle_* returns the actual value (use in a CAS loop)
   actual = __mulle_atomic_pointer_cas( &s_slot, value, (void *) 0);
   if( actual == (void *) 0)
   {
      // we installed the value
   }

   // _mulle_* returns a success flag
   if( _mulle_atomic_pointer_cas( &s_slot, value, (void *) 0))
   {
      // success
   }

   // relaxed counter: previous value is returned, ignored here
   (void) _mulle_atomic_intptr_increment_relaxed( &s_count);
}
```

## 7. Dependencies

- `mulle-c11` — required; cross-platform C compiler glue and preprocessor
  conveniences (`MULLE_C_*` macros used throughout the headers).
- `mintomic` — optional atomic backend used when C11 `<stdatomic.h>` is not
  available; its `mintomic_gcc.c` is compiled into `mulle-thread` in that case.
- Platform thread facilities (selected by `mulle-thread.h`): C11
  `<threads.h>`, POSIX pthreads, or the Windows API. The sourcetree config
  also declares `stdthreads` (FreeBSD only) and `pthread` as platform
  dependencies.
