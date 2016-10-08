# Mutex API

The mutex API is almost identical to the pthreads mutex API, but a bit simplified. A mutex is local to the process and it's threads. 


## Types

* `mulle_thread_mutex_t` : mutex type (when using **pthreads** a typedef of `pthread_mutex_t`)


## Functions

#### mulle_thread_mutex_init

```
int  mulle_thread_mutex_init( mulle_thread_mutex_t *lock)
```

Initialize a mutex. You must call this before using it. You can not
statically initialize a `mulle_thread_mutex_t`.

Returns 0 on success.


#### mulle_thread_mutex_lock

```
int  mulle_thread_mutex_lock( mulle_thread_mutex_t *lock)
```

Lock the mutex. A thread will deadlock if it tries to lock the mutex again.

Returns 0 on success. A negative value indicates that you set up the mutex wrong.

See: [pthread_mutex_lock](https://linux.die.net/man/3/pthread_mutex_lock)


#### mulle_thread_mutex_trylock

```
int  mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock)
```

Returns 0 on success. Returns 1 if busy. Returns -1 on failure.

See: [pthread_mutex_trylock](https://linux.die.net/man/3/pthread_mutex_trylock)


#### mulle_thread_mutex_unlock

```
int  mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock)
```
Unlock the mutex. Unlocking a non-locked mutex is wrong. Unlocking a mutex, that was locked by a different thread, is wrong. 

Returns 0 on success.

See: [pthread_mutex_unlock](https://linux.die.net/man/3/pthread_mutex_unlock)


#### mulle_thread_mutex_done

```
int  mulle_thread_mutex_done( mulle_thread_mutex_t *lock)
```

Destroy the mutex. You should not destroy a locked mutex. 

Returns 0 on success.

See: [pthread_mutex_done](https://linux.die.net/man/3/pthread_mutex_done)

