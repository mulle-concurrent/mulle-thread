# mulle-thread

Release on [github](//github.com/mulle-nat/mulle-thread): [![Build Status](https://travis-ci.org/mulle-nat/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-thread)


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


## Install

On OS X and Linux you can use [homebrew](//brew.sh), respectively [linuxbrew](//linuxbrew.sh) to install the library:

```
brew tap mulle-kybernetik/software
brew install mulle-thread
```

On other platforms you can use [mulle-install](//www.mulle-kybernetik.com/software/git/mulle-install) to install the library:

```
mulle-install --prefix /usr/local --branch release https://www.mulle-kybernetik.com/repositories/mulle-thread
```


Otherwise read:

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
