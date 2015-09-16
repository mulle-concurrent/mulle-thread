# Build Information


## What you get
 
* `libmulle_thread.a`, the mulle-thread` static library along with a bunch of
headers. You can often get by NOT linking with libmulle_thread.a, which is
fairly empty.


## Prerequisites

### mintomic

mulle-aba` needs [mintomic](https://mintomic.github.io/) as a prerequisite. Install it in the top directory besides <tt>src</tt> and <tt>dox</tt> as **mulle_mintomic**-
The rename is done for packaging reasons, because we install and link only a subset of the functionality.

```
git clone -d mulle_mintomic https://github.com/mintomic/mintomic
```

## Building

### With cmake

```
mkdir build
cd build
cmake ..
make
```


### With Xcode

```
xcodebuild -alltargets  
```


### Compile Flags

For development use no flags.

For production use NDEBUG

* DEBUG : turns on some compile time facilities to aid debugging `mulle-thread` itself. 
* MULLE_ATOMIC_TRACE : turns on a very detailed amount of tracing for some atomic operations.



