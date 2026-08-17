<!-- Keywords: do-not-return-unsafe do-not-access-internals return-semantics tss-zero-key main-thread-destructor cas-assert-eq noblock-is-if tsan-needs-pthreads sanitizer -->

# Quirks

## ThreadSanitizer needs `-DMULLE_THREAD_USE_PTHREADS`

`mulle_thread_create` uses C11 `thrd_create` whenever `<threads.h>` is
available. ThreadSanitizer intercepts `pthread_create`, but not `thrd_create`,
which glibc implements through a libc-internal call that bypasses interposition.
The thread then starts with no tsan thread state and segfaults as soon as it
executes instrumented code:

```
ThreadSanitizer:DEADLYSIGNAL
ERROR: ThreadSanitizer: SEGV on unknown address 0x000000000018
```

The backtrace shows `__tsan_func_entry` at the first line of the thread
function, which looks like a bug in the code under test. It is not. Build
sanitizer targets with:

``` sh
cc -fsanitize=thread -DMULLE_THREAD_USE_PTHREADS ...
```

The macro is checked in `src/mulle-thread.h` before the backend header is
included, so it has to be a compiler flag, not a `#define` in a `.c` file.
Reproduced with both gcc libtsan and clang. A thread function with nothing
left to instrument after optimization may pass, so do not conclude from a
trivial smoke test that tsan works.

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

The winning thread publishes `MULLE_THREAD_ONCE_DONE` after init, so a later
blocking `mulle_thread_once`/`mulle_thread_once_do` on the same flag returns
immediately instead of spinning forever.

## `mulle_thread_once_do_noblock` does not guarantee execution

If a thread sees the once as BUSY or DONE, the body is skipped without error.
Only the first thread that sees INIT and successfully CAS-es to BUSY runs the
body.

## Memory barrier scope

`mulle_atomic_memory_barrier()` is `atomic_thread_fence(memory_order_seq_cst)`
on the C11 backend and `mint_thread_fence_seq_cst()` on mintomic — a full
hardware fence, not just a compiler barrier. This is required by the
`mulle_thread_once*` publish protocol: init writes must be visible to other
threads before they can observe `MULLE_THREAD_ONCE_DONE`.

## `_cas`/`_weakcas` assert value != expect

Both strong and weak CAS functions assert `value != expect`
(`src/mulle-atomic-c11.h:519,424`). Passing equal pointers triggers an abort in
debug builds. Use `_set` or `_write` instead when the old value is not needed.

## `mulle_thread_once_do_noblock` is a `for` loop, like `mulle_thread_once_do`

Since the DONE-publishing fix, the `_noblock` variant expands to the same
two-level `for` loop shape as the blocking `mulle_thread_once_do` (the outer
loop publishes `MULLE_THREAD_ONCE_DONE`). `break` inside the body exits the
block cleanly and still publishes DONE; `continue` does the same (it jumps to
the hidden inner-loop increment, then the outer loop publishes DONE and the
once exits). The difference to the blocking variant is only that a thread
which did not win the CAS does not wait — it skips the body entirely.

Note that `break`/`continue` no longer target an *enclosing* user loop — they
are captured by the macro's own loops, exactly as in `mulle_thread_once_do`.
That is a behavior change vs. the pre-fix `if` form (where they fell through to
the surrounding loop).

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
