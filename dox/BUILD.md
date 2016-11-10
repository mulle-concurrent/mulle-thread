# How to build mulle-thread


## What you get

* `libmulle_thread.a`, the mulle-thread static library along with a bunch of
headers. You can often get by not linking with libmulle_thread.a, except on
windows, where you absolutely do need it.


## Prerequisites

#### mintomic

If your compiler does not support C11 atomics, then `mulle-thread` needs
[mintomic](//mintomic.github.io/) as a prerequisite. This is expected to
exist in the project directory root.

#### mulle-c11

[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/) is a header
that abstracts a small set of non-standardized compiler features.

#### mulle-configuration

[mulle-configuration](//www.mulle-kybernetik.com/software/git/mulle-configuration/) are
configuration files for building with Xcode or cmake. This is expected to
exist in the project directory root.

#### mulle-tests

[mulle-tests](//www.mulle-kybernetik.com/software/git/mulle-tests/) are
scripts to provide an environment for running the tests. This is expected to
exist in the project directory root if you want to run tests.

#### mulle-build

[mulle-build](//www.mulle-kybernetik.com/software/git/mulle-build) will
painlessly assemble the dependencies and build the library.


### Windows: Installing further prerequisites

Check the [mulle-build README.md](//www.mulle-kybernetik.com/software/git/mulle-build/README.md)
for instrutions how to get the "Git for Windows" bash going.


## Install mulle-thread using mulle-build

Grab the latest **mulle-thread** release and go into the project directory:

```
git clone https://github.com/mulle-objc/mulle-thread
cd mulle-thread
```

Then let **mulle-build** fetch the dependencies and build **mulle-thread** in
debug mode:

```
mulle-build --debug
```

Run some tests:

```
mulle-test
```

Build library in release mode and install into `tmp` :

```
mulle-clean ;
mulle-install --prefix /tmp
```


### Compile Flags

For production use NDEBUG
