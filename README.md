# mulle-thread

## Intro

**mulle-thread** is basically a set of wrapper headers for a simplified subset 
of  pthreads functions and for a limited range of atomic operations. 
mulle-threads main advantages are **simplicity**, **portability**, **sanity**.

* atomic operations CAS, ++, -- on `sizeof( void *)` pointers.
* mutex (lock)
* thread local storage, with proper destruction on windows
* basic thread operations like create, join, yield

Since C11 has `<stdatomic.h>` and `<threads.h>`, eventually this project could
become superflous. In the meantime though it's a convenient abstraction on 
threads and atomic operations.

## API

* [Atomic Operations](API_ATOMIC.md)
* [Threads](API_THREAD.md)
* [Mutex](API_MUTEX.md)
* [Thread Local Storage](API_TSS.md)


### Platforms

* OS X
* iOS
* Linux
* FreeBSD
* Windows

### Compilers

* clang
* gcc
* MSVC

> C11 support in clang is currently lacking, as there is no `<threads.h>`. 
> The fallback is **pthreads**.


### Architectures

If **not** using proper C11

* x86
* x86_64
* arm


## mintomic

This was the original source for atomic operations, unfortunately it's not 
really active anymore, so for example ARM64 doesn't work.

For now, `mulle_atomic` will use mintomic only, if `<stdatomic.h>` is not 
available.
