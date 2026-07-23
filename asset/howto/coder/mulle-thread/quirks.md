<!-- Keywords: do-not-return-unsafe do-not-access-internals return-semantics tss-zero-key main-thread-destructor cas-assert-eq noblock-is-if -->

# Quirks

## Macros: no `return` from `_do` blocks

`mulle_thread_mutex_do`, `mulle_thread_recursive_mutex_do`, and the
`mulle_thread_once_do*` macros use hidden for-loops. A `return` from inside
the block skips the cleanup path:

- Mutex/recursive-mutex `_do`: `return` leaves the mutex **locked**.
- Once `_do` variants: `return` leaves the init state **BUSY** so other threads will wait forever.

Use `break` to exit the blocked region cleanly (mutex unlocks, once state transitions).

## `mulle_thread_create` parameter order

```c
mulle_thread_create( func, arg, &thread);   // thread handle is LAST
```

This is the **reverse** of pthreads (`pthread_create( &thread, attr, func, arg)`).
The thread handle goes last.

## Atomic return-value semantics differ by function

| Function | Returns |
|----------|---------|
| `_mulle_atomic_pointer_increment` | **previous** value |
| `_mulle_atomic_pointer_decrement` | **previous** value |
| `_mulle_atomic_pointer_add(addr, diff)` | **result** (previous + diff) |

This is a common source of off-by-one bugs in ref-count patterns.

## Atomic `add()` vs `increment()`/`decrement()`

`_mulle_atomic_pointer_add` is the odd one out — it returns the result, not the previous value:

```c
void *prev = _mulle_atomic_pointer_decrement( &counter);   // old value
void *result = _mulle_atomic_pointer_add( &counter, 5);    // new value (old + 5)
```

## `_mulle_atomic_pointer_set` is a spin-loop CAS, not a simple store

```c
void   *old;
old = _mulle_atomic_pointer_set( &addr, value);
```

This spins in a weakCAS loop. For a simple relaxed write use
`_mulle_atomic_pointer_write` instead. Prefer `_set` only when you need the
old value returned atomically.

## TSS key 0 is valid

A TSS key returned as 0 from `mulle_thread_tss_create` is a **valid** key.
Do not use 0 as a "no key" sentinel. Use a separate flag or a key of
`(mulle_thread_tss_t) -1` as an invalid sentinel if needed.

## TSS destructor may not run on main thread

In C11 backends, the TSS destructor callback is likely **not** called when
the main thread exits. The pthreads backend may also skip it. Do not rely on
TSS destructor for main-thread cleanup.

## `mulle_thread_once` state machine is not resettable

Once the once state reaches `MULLE_THREAD_ONCE_DONE`, it stays done. Thread
cancellation or exceptions **inside** the init function will not transition
back to INIT for a retry. The once is permanently BUSY until the init callback
returns, then permanently DONE.

## Recursive mutex internal fields are private

`mulle_thread_recursive_mutex_t._mutex`, `._thread_id`, and `._depth` must
not be accessed directly. Use only the public `_lock`/`_unlock`/`_trylock` API.

## No nested `mulle_thread_mutex_do` with the same mutex

You cannot nest two `mulle_thread_mutex_do(m)` blocks on the **same** mutex
— the lock is not recursive. Use `mulle_thread_recursive_mutex_t` if
re-entrant locking is needed.

## Non-blocking once variants skip init silently

`mulle_thread_once_do_noblock` and `mulle_thread_once_call_noblock` **skip**
the initialization block entirely if another thread is already initializing.
They do not wait. Callers that enter the once_do body must not assume
initialization was actually performed by prior callers.

## `mulle_thread_once_do_noblock` does not guarantee execution

If a thread sees the once as BUSY or DONE, the body is skipped without error.
Only the first thread that sees INIT and successfully CAS-es to BUSY runs the
body.

## Memory barrier scope

`mulle_atomic_memory_barrier()` is `atomic_signal_fence(memory_order_seq_cst)`,
not a full hardware `atomic_thread_fence`. It is a compiler barrier, not a
CPU memory fence.

## `_cas`/`_weakcas` assert value != expect

Both strong and weak CAS functions assert `value != expect`
(`src/mulle-atomic-c11.h:519,424`). Passing equal pointers triggers an abort in
debug builds. Use `_set` or `_write` instead when the old value is not needed.

## `mulle_thread_once_do_noblock` is an `if`, not a `for` loop

Unlike the blocking `mulle_thread_once_do` which is a two-level `for` loop, the
`_noblock` variant expands to a single `if` statement. `break` has no effect — if
you need early exit, wrap the block body in a do-while(0) or restructure the
caller.

## `mulle_thread_tss_create` param order: destructor first, key last

```c
mulle_thread_tss_create( destructor_callback, &key);   // 0 on success
```

The destructor is the **first** argument, the output key is the **last**. This
differs from the C11 `tss_create` which puts the key first and destructor second.

## Nonatomic read/write are not safe for concurrent use

`_mulle_atomic_pointer_nonatomic_read` / `_mulle_atomic_pointer_nonatomic_write`
bypass atomic semantics entirely. They are for single-threaded init and final
verification only. Never use them when another thread may be accessing the same
variable.

## Do not mix platform-native APIs with wrappers

Using raw pthreads or C11-threads APIs alongside `mulle_thread_*` wrappers on
the same objects can produce undefined or platform-divergent behavior. Always
use the mulle-thread wrapper consistently.
