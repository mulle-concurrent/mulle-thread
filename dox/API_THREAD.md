# Thread API

The thread API is like pthreads, except where noted. Please consult the pthread
man pages for more extensive help.

* Main differences to be aware of: the parameters of `mulle_thread_create` are
  in a different order than in `pthread_create`.
* The return value of a thread differs between platforms (`mulle_thread_rval_t`
  is `void *` on pthreads, an integer on C11/Windows). For portable code, return
  only small nonnegative integer values representable by all supported backends;
  pointer values, large values, and negative values are not portable. Treat the
  result as opaque and use `mulle_thread_return()` to return from a thread
  function portably.
* Status-returning functions return status directly; they do not set the
  process/thread-local `errno` variable. `0` means success, `-1` means a generic
  failure, and selected standard constants such as `EBUSY` and `ETIMEDOUT` are
  returned directly where they carry useful meaning.
* On Windows, thread local storage destructors only run when a thread exits
  through `mulle_thread_exit` / `mulle_thread_return()` — do not just `return`
  from the thread function there.

## Types

Type                  | Description
----------------------|------------------------------------------------
`mulle_thread_t`      | thread type (a typedef of `pthread_t`, `thrd_t` or a Windows `HANDLE`)
`mulle_thread_rval_t` | the return value of a thread function (`void *` on pthreads, `int`/`unsigned int` elsewhere — treat it as opaque)
`mulle_thread_id_t`   | a platform-independent numeric thread id, for debugging output and for comparing threads
`mulle_thread_function_t` | thread entry point: `mulle_thread_rval_t (*)( void *)` (use the `MULLE_THREAD_CALL` macro when declaring one — see below)
`mulle_thread_callback_t` | thread local storage destructor: `void (*)( void *)`

> **Declaring thread functions portably:** on 32-bit x86 Windows, thread entry
> points must use the `__stdcall` calling convention (that is what
> `_beginthreadex` expects). Declare yours with the `MULLE_THREAD_CALL` macro,
> which expands to `__stdcall` there and to nothing everywhere else:
>
> ``` c
> static mulle_thread_rval_t MULLE_THREAD_CALL  my_thread( void *arg);
> ```


#### mulle_thread_self

``` c
mulle_thread_t   mulle_thread_self( void)
```

Returns the current thread, just like `pthread_self`. **On Windows this returns
the `GetCurrentThread()` pseudo-handle, which you must not compare with other
handles** — use `mulle_thread_id()` / `mulle_thread_get_id()` for comparisons.


#### mulle_thread_id

``` c
mulle_thread_id_t   mulle_thread_id( void)
```

Returns a numeric id for the current thread. Use this (and
`mulle_thread_get_id`) for debugging output and for comparing threads instead
of comparing `mulle_thread_t` values directly — `==` does not work on Windows.


#### mulle_thread_get_id

``` c
mulle_thread_id_t   mulle_thread_get_id( mulle_thread_t thread)
```

Returns the numeric id of a thread handle. Two threads are equal if and only if
their ids are equal.


#### mulle_thread_equal

``` c
int   mulle_thread_equal( mulle_thread_t thread1,
                          mulle_thread_t thread2)
```

Returns 1 if both handles refer to the same thread, 0 otherwise. Implemented as
`mulle_thread_get_id( a) == mulle_thread_get_id( b)` so it works on all three
backends — including Windows, where `mulle_thread_self()` returns a pseudo
handle that must never be compared with `==`.


#### mulle_thread_create

``` c
int   mulle_thread_create( mulle_thread_function_t *f,
                           void *arg,
                           mulle_thread_t *thread)
```

> Note: Parameters are differently ordered than in pthread.

**f** is the thread function with **arg** as its argument. The created thread
is returned in **thread**. Obviously **arg** should not be a pointer to stack
memory. The return value of the thread function can be retrieved with
`mulle_thread_join`.

A return value of 0 indicates success, -1 an error.


#### mulle_thread_join

``` c
mulle_thread_rval_t   mulle_thread_join( mulle_thread_t thread)
```

Join with a terminated thread (wait for a thread to exit) and return the value
the thread function returned. Treat that value as opaque (see `mulle_thread_rval_t`).
Returns `(mulle_thread_rval_t) -1` on join failure. This legacy API uses the
thread result itself for the return value, so portable thread functions must not
return `-1`.

See: [pthread_join](//man7.org/linux/man-pages/man3/pthread_join.3.html)


#### mulle_thread_detach

``` c
int   mulle_thread_detach( mulle_thread_t thread)
```

Detach a thread, frees caller from having to join it later.

See: [pthread_detach](//man7.org/linux/man-pages/man3/pthread_detach.3.html)


#### mulle_thread_exit

``` c
void   mulle_thread_exit( int rval)
```

Terminate the calling thread. Your thread **must** call this (or use the
`mulle_thread_return()` macro) to exit, so that thread local storage
destructors are run. On Windows this is the only way the emulated destructors
run at all.

See: [pthread_exit](//man7.org/linux/man-pages/man3/pthread_exit.3.html)


#### mulle_thread_yield

``` c
void   mulle_thread_yield( void)
```

Initiate a context switch.

See: [sched_yield](//man7.org/linux/man-pages/man2/sched_yield.2.html)
