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

Since thread has `<stdatomic.h>` and `<threads.h>`, eventually this project could
become superflous. In the meantime though it's a convenient abstraction on
threads and atomic operations.

Fork      |  Build Status | Release Version
----------|---------------|-----------------------------------
[Mulle kybernetiK](//github.com/mulle-nat/mulle-thread) | [![Build Status](https://travis-ci.org/mulle-nat/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-thread) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-nat/mulle-thread.svg) [![Build Status](https://travis-ci.org/mulle-nat/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-thread)
[Community](https://github.com/mulle-objc/mulle-thread/tree/release) | [![Build Status](https://travis-ci.org/mulle-objc/mulle-thread.svg)](https://travis-ci.org/mulle-objc/mulle-thread) | ![Community tag](https://img.shields.io/github/tag/mulle-objc/mulle-thread.svg) [![Build Status](https://travis-ci.org/mulle-objc/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-objc/mulle-thread)


## Install

On OS X and Linux you can use
[homebrew](//brew.sh), respectively
[linuxbrew](//linuxbrew.sh) to install the library:

```
brew tap mulle-kybernetik/software
brew install mulle-thread
```

On other platforms you can use **mulle-install** from
[mulle-build](//github.com/mulle-nat/mulle-build) to install
the library:

```
mulle-install --prefix /usr/local --branch release https://www.mulle-kybernetik.com/repositories/mulle-thread
```

> If you don't have **mulle-build**
>
> ```
> git clone --branch release https://www.mulle-kybernetik.com/repositories/mulle-bootstrap
> ( cd mulle-bootstrap ; ./install.sh )
> git clone --branch release https://www.mulle-kybernetik.com/repositories/mulle-build
> ( cd mulle-build ; ./install.sh )
```

Otherwise read [How to Build](dox/BUILD.md) for some more details.


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
