# mulle-thread

## Intro

**mulle-thread** is a set of wrapper headers for a simplified subset
of thread functions and for a limited range of atomic operations that strongly
mimics the C11 standard, even if C11 is not available. 

mulle-threads main advantages are **simplicity**, **portability**, **sanity**.

* basic atomic operations CAS, ++, -- on void pointers.
* basic thread operations
* mutex functionality
* thread local storage, with proper destruction

Since C11 has `<stdatomic.h>` and `<threads.h>`, eventually this project could
become superflous. In the meantime though it's a convenient abstraction on
threads and atomic operations.

## Build

* [How to Build](dox/BUILD.md)

## API

* [Atomic Operations](dox/API_ATOMIC.md)
* [Threads](dox/API_THREAD.md)
* [Mutex](dox/API_MUTEX.md)
* [Thread Local Storage](dox/API_TSS.md)


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

> C11 support in clang as of v3.8 is lacking, as there is no `<threads.h>`.
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
