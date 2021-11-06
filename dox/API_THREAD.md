# Thread API

The thread API is like pthreads, except where noted. Please consult the pthread
man pages for more extensive help.

* Main differences to be aware of: There is no such thing as a return value from a thread.
* You must execute `mulle_thread_exit` at the end of your thread.


## Types

Type                  | Description
----------------------|------------------------------------------------
`mulle_thread_t`      | thread type, with **pthreads** a typedef of `pthread_t`
`mulle_thread_rval_t` | the return value of a thread. Assume this being **void**!


#### mulle_thread_self

```
mulle_thread_t   mulle_thread_self( void)
```

Returns the current thread, just like `pthread_self`


#### mulle_thread_create

```
mulle_thread_create( mulle_thread_rval_t (*f)(void *),
                     void *arg,
                     mulle_thread_t *thread)
```

> Note: Parameters are differently ordered than in pthread

**f** is the thread function with **arg** as its argument.
The created thread is returned in **thread**. Obviously **arg** should not
be a pointer to stack memory. `mulle_thread_rval_t` can be `void` or `int`
depending on platform. The return value will be ignored though. Use
`mulle_thread_return()` in your code to avoid warnings.

A return value of 0 indicates success.


#### mulle_thread_join

```
int   mulle_thread_join( mulle_thread_t thread)
```

Join with a terminated thread (wait for a thread to exit)


See: [pthread_join](//man7.org/linux/man-pages/man3/pthread_join.3.html)


#### mulle_thread_detach

```
int   mulle_thread_detach( mulle_thread_t thread)
```

Detach a thread, frees caller from having to join it later.

See: [pthread_detach](//man7.org/linux/man-pages/man3/pthread_detach.3.html)


#### mulle_thread_exit

```
void   mulle_thread_exit( void)
```

Terminate calling thread. Your thread **must** call this to exit,
so that thread local storage destructors are run.

See: [pthread_exit](//man7.org/linux/man-pages/man3/pthread_exit.3.html)



#### mulle_thread_yield

```
void   mulle_thread_yield( void)
```

Initiate a context switch.

See: [sched_yield](//man7.org/linux/man-pages/man2/sched_yield.2.html)

