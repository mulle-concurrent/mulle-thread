# mulle-thread

**mulle-thread** is a set of **C** wrapper headers for a simplified
subset of thread functions and for a limited range of atomic operations that
strongly mimics the thread standard, even if thread is not available. On Windows it
has to do a bit more work.

mulle-threads main advantages are **simplicity**, **portability**, **sanity**.

* basic atomic operations CAS, ++, -- on void pointers.
* basic thread operations
* mutex functionality
* thread local storage, with proper destruction

Since thread has `<stdatomic.h>` and `<threads.h>`, eventually this project
could become superflous. In the meantime though it's a convenient abstraction
on threads and atomic operations.

Fork      |  Build Status | Release Version
----------|---------------|-----------------------------------
[Mulle kybernetiK](//github.com/mulle-c/mulle-thread) | [![Build Status](https://travis-ci.org/mulle-c/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-thread) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-c/mulle-thread.svg) [![Build Status](https://travis-ci.org/mulle-c/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-c/mulle-thread)


## Install

Install the prerequisites first:

| Prerequisites                                 |
|-----------------------------------------------|
| [mulle-c11](//github.com/mulle-c/mulle-c11) |

Then build and install

```
mkdir build 2> /dev/null
(
   cd build ;
   cmake .. ;
   make install
)
```

Or let [mulle-sde](//github.com/mulle-sde)  do it all for you.


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

> thread support in clang as of v3.8 is lacking, as there is no `<threads.h>`.
> The fallback is **pthreads**.


### Architectures

If `<stdatomic.h>` is not available for your architecture, then **mulle-thread**
is limited to:

* x86
* x86_64
* arm

That is because of [mintomic](//mintomic.github.io/) limitations. **mintomic**
is the original source for the provided atomic operations. Unfortunately it's not
really active anymore, so for example ARM64 doesn't work. **mulle-thread** will
use mintomic only, if `<stdatomic.h>` is not available.


## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
