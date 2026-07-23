<!-- Keywords: atomic incremental-cas scoped-lock producer-consumer tss-destructor module-init nonatomic-init functionpointer -->

# Patterns

## 1. Atomic pointer CAS with incremental update

Read, compute a new value, then CAS in a loop until success. This is the
dominant atomic pattern from the tests.

```c
mulle_atomic_pointer_t   central;

void   *expect;
void   *value;

do
{
   MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   expect = _mulle_atomic_pointer_read( &central);
   value  = (void *) ((intptr_t) expect + 1);
   MULLE_THREAD_UNPLEASANT_RACE_YIELD();
}
while( ! _mulle_atomic_pointer_weakcas( &central, value, expect));
```

`MULLE_THREAD_UNPLEASANT_RACE_YIELD()` is a test-mode macro that randomly
yields or nanosleeps. In production builds it is a no-op. Omit it from
non-test code.

**Atomic arithmetic shortcuts** (`src/mulle-atomic-c11.h:553-582`):

```c
void   *prev;

prev = _mulle_atomic_pointer_increment( &central);   // returns PREVIOUS value
prev = _mulle_atomic_pointer_decrement( &central);   // returns PREVIOUS value

void   *result;
result = _mulle_atomic_pointer_add( &central, 5);    // returns RESULT (prev+5), not previous
```

The return-value semantics differ — `add` returns the result while
`increment`/`decrement` return the prior value.

**Spin-loop set** (`src/mulle-atomic.h:72-89`):

```c
void  *old;
old = _mulle_atomic_pointer_set( &central, new_value);
```

This atomically writes `new_value` by CAS-looping, returning the old value.

**CAS naming convention:**

| Function | Returns |
|----------|---------|
| `__mulle_atomic_pointer_cas(addr, value, expect)` | actual value at address |
| `_mulle_atomic_pointer_cas(addr, value, expect)` | 1 on success, 0 on failure |

The double-underscore variant returns the actual value; the single-underscore
variant returns a boolean result. Both use strong CAS with `assert(value != expect)`.

## 2. Scoped mutex with `mulle_thread_mutex_do`

```c
mulle_thread_mutex_t   lock;

mulle_thread_mutex_init( &lock);

mulle_thread_mutex_do( lock)
{
   /* critical section */
}
/* lock is unlocked here */

mulle_thread_mutex_done( &lock);
```

`break` and `continue` exit the critical section and unlock. **Do not `return`
from inside** — the mutex stays locked.

The macro takes the mutex by value (not pointer), consistent with the `_do`
convention.

## 3. Recursive mutex (same-thread re-entrant lock)

```c
mulle_thread_recursive_mutex_t   lock;

mulle_thread_recursive_mutex_init( &lock);

mulle_thread_recursive_mutex_do( lock)
{
   /* re-entrant: same thread may lock again from within */
   mulle_thread_recursive_mutex_do( lock)
   {
      /* still safe */
   }
}

mulle_thread_recursive_mutex_done( &lock);
```

The same `break`/`continue`/`no-return` rules apply.

Low-level API (`src/mulle-thread.c:160-208`):
- `mulle_thread_recursive_mutex_lock` — if same thread owns lock, just increments depth; otherwise acquires the underlying mutex.
- `mulle_thread_recursive_mutex_unlock` — decrements depth; releases underlying mutex only when depth reaches 0.
- `mulle_thread_recursive_mutex_trylock` — returns 0 on success.
- Internal fields `_thread_id` and `_depth` are private; do not access.

## 4. Condition variable — producer/consumer

The wait must always be inside a `while` loop checking the predicate
(`test/cond/cond-stress.c:39-40`):

```c
mulle_thread_mutex_t   mutex;
mulle_thread_cond_t    cond;

mulle_thread_mutex_init( &mutex);
mulle_thread_cond_init( &cond);

/* Consumer */
mulle_thread_mutex_lock( &mutex);
while( queue == 0 && ! done)
   mulle_thread_cond_wait( &cond, &mutex);
/* consume item */
mulle_thread_mutex_unlock( &mutex);

/* Producer */
mulle_thread_mutex_lock( &mutex);
queue++;
mulle_thread_cond_signal( &cond);
mulle_thread_mutex_unlock( &mutex);

mulle_thread_cond_done( &cond);
mulle_thread_mutex_done( &mutex);
```

- `mulle_thread_cond_broadcast` wakes all waiters.
- `mulle_thread_cond_timedwait` takes `abstime` (absolute time, `struct timespec *`), returns `ETIMEDOUT` on timeout.

## 5. TSS with destructor callback

```c
static mulle_thread_tss_t   key;

static void   free_storage( void *p)
{
   free( p);
}

mulle_thread_tss_create( free_storage, &key);  // 0 on success; key of 0 is VALID

void   *data = malloc( 256);
mulle_thread_tss_set( key, data);              // 0 on success

void   *ptr = mulle_thread_tss_get( key);      // returns per-thread value
```

- The destructor runs on thread exit (C11/pthreads), but likely NOT on the main thread in C11 builds.
- A key value of 0 is valid and distinct from "unset". This differs from some platform conventions.
- Destructor only fires for threads that actually called `mulle_thread_tss_set`.

## 6. Once-init — module-level lazy init

**Blocking variant** (contending threads spin-yield until init completes):

```c
/* Declares a static mulle_thread_once_t in scope, runs body exactly once */
mulle_thread_once_do( s_init)
{
   /* expensive init */
}
```

**Recursive variant** (same thread may re-enter from within init):

```c
mulle_thread_once_do_recursive( s_init)
{
   /* re-entrant init ok */
}
```

**Non-blocking variant** (skips init if another thread is already initializing):

```c
mulle_thread_once_do_noblock( s_init)
{
   /* only runs if no other thread is already initializing */
}
```

**Function-call API** (no macro):

```c
static mulle_thread_once_t   once = MULLE_THREAD_ONCE_INIT;

mulle_thread_once_call( &once, my_init, userinfo);                    // blocking, with userinfo
mulle_thread_once_call_recursive( &once_recursive, my_init, userinfo); // recursive
mulle_thread_once_call_noblock( &once, my_init, userinfo);             // non-blocking inline
```

## 7. Thread create/join

```c
mulle_thread_t   thread;

static mulle_thread_rval_t
worker( void *arg)
{
   /* ... */
   mulle_thread_return();  // or return( (mulle_thread_rval_t) value);
}

mulle_thread_create( worker, arg, &thread);  // thread handle is LAST param
mulle_thread_rval_t rval = mulle_thread_join( thread);
mulle_thread_detach( thread);               // fire-and-forget alternative to join
```

`mulle_thread_create` returns 0 on success. The thread handle `p_thread` is the
**last** parameter — unlike pthreads where it is the second argument.

Other thread utilities:
- `mulle_thread_exit( int rval)` — terminate the calling thread.
- `mulle_thread_yield()` — yield the CPU.
- `mulle_thread_self()` — return the calling thread's handle.
- `mulle_thread_id()` — return the calling thread's ID as `uintptr_t`.
- `mulle_thread_get_id( thread)` — return a given thread's ID.

## 8. Memory barrier

```c
mulle_atomic_memory_barrier();
```

Implemented as `atomic_signal_fence(memory_order_seq_cst)` on C11. Use after
stores that must be visible to other threads before subsequent operations.

## 9. Nonatomic init before threaded use

Use `_mulle_atomic_pointer_nonatomic_write` to initialize an atomic variable
before any threads are spawned, and `_mulle_atomic_pointer_nonatomic_read` to
check the final value after all threads have joined (`test/atomic/cas.c:140,162`):

```c
mulle_atomic_pointer_t   central;

_mulle_atomic_pointer_nonatomic_write( &central, (void *) 0);
// ... spawn threads, do work, join ...
assert( _mulle_atomic_pointer_nonatomic_read( &central) == (void *) expected);
```

These are **not** safe for concurrent access — they bypass atomics entirely.
Use only in single-threaded setup/teardown.

## 10. Function pointer atomic CAS

The same CAS/weakCAS/set/read/write API exists for function pointers
(`src/mulle-atomic.h:92-108`):

```c
mulle_atomic_functionpointer_t   fp_slot;

void                       (*old)( void);
mulle_functionpointer_t   value = my_handler;

old = __mulle_atomic_functionpointer_cas( &fp_slot, value, expect);
_mulle_atomic_functionpointer_set( &fp_slot, value);
```

Types and naming mirror the pointer API exactly — substitute
`mulle_atomic_functionpointer_t` for `mulle_atomic_pointer_t` and
`_mulle_atomic_functionpointer_*` for `_mulle_atomic_pointer_*`.
