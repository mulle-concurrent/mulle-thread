# mulle-thread

🔠 Cross-platform thread/mutex/tss/atomic operations in C

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

### Warning 

`mulle_thread_once` doesn't guarantee, that exceptions or thread cancellation 
within the init function will clear the once flag for a second run. In fact 
it will not.


Build Status | Release Version
-------------|-----------------------------------
[![Build Status](https://travis-ci.org/mulle-concurrent/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-concurrent/mulle-thread) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-concurrent/mulle-thread.svg) [![Build Status](https://travis-ci.org/mulle-concurrent/mulle-thread.svg?branch=release)](https://travis-ci.org/mulle-concurrent/mulle-thread)



## Add 

Use [mulle-sde](//github.com/mulle-sde) to add mulle-thread to your project:

```
mulle-sde dependency add --c --github mulle-concurrent mulle-thread
```

## Install

### mulle-sde

Use [mulle-sde](//github.com/mulle-sde) to build and install mulle-thread and all dependencies:

```
mulle-sde install --prefix /usr/local \
   https://github.com/mulle-concurrent/mulle-thread/archive/latest.tar.gz
```

### Manual Installation


Install the requirements:

Requirements                                         | Description
-----------------------------------------------------|-----------------------
[mulle-c11](//github.com/mulle-c/mulle-c11)          | Compiler glue header
[mintomic](//github.com/mulle-concurrent/mintomic)   | **Don't build this!** Unpack it into the mulle-thread folder and rename the directory to `mintomic`


Install into `/usr/local`:

```
mkdir build 2> /dev/null
(
   cd build ;
   cmake -DCMAKE_INSTALL_PREFIX=/usr/local \
         -DCMAKE_PREFIX_PATH=/usr/local \
         -DCMAKE_BUILD_TYPE=Release .. ;
   make install
)
```

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
