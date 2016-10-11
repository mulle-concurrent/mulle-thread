# Mutex API

The mutex API is like the [pthreads](//en.wikipedia.org/wiki/POSIX_Threads) thread local storage API, except where noted.

On Windows the  destructor facility of pthreads is emulated to provide a saner experience.

Think of thread local storage as a hashtable/dictionary that belongs to every
thread, where the keys are globally defined (with `mulle_thread_tss_create`).


## Types

Type                 | Description
---------------------|------------------------------------------------
`mulle_thread_tss_t` | thread local storage key type (when using **pthreads** a typedef of `pthread_key_t`)


## Functions


#### mulle_thread_tss_create

```
int   mulle_thread_tss_create( void (*f)( void *),
                               mulle_thread_tss_t *key)
```

Create a thread local storage key, with a destructor function. This key is
common to all threads. TSS storage keys are very limited. Do not create too
many for your application. Less than 100 I would expect to work evertime. The destructor function will be called, whenever a thread terminates properly with `mulle_thread_exit`.

Returns 0 on success.

See: [pthread_key_create](//linux.die.net/man/3/pthread_key_create)


#### mulle_thread_tss_free

```
void   mulle_thread_tss_free( mulle_thread_tss_t key)
```

Free a thread local storage key. Be sure that you do not free a key, when it
is still in use by other threads. In general applications don't need to call this
function.

See: [pthread_key_delete](//linux.die.net/man/3/pthread_key_delete)


#### mulle_thread_tss_get

```
void   *mulle_thread_tss_get( mulle_thread_tss_t key)
```

Get value from thread local storage with key.


See: [pthread_getspecific](//linux.die.net/man/3/pthread_getspecific)


#### mulle_thread_tss_set

```
int  mulle_thread_tss_set( mulle_thread_tss_t key,
                           void *value)
```

Set value in thread local storage with key. Obviously **value** should not
be a pointer to memory writable by other threads or stack memory. If you want
to write an integer use `mulle_thread_tss_set( key, (void *) 1848)`.

See: [pthread_setspecific](//linux.die.net/man/3/pthread_setspecific)
