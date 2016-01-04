# Build Information


## What you get
 
* `libmulle_thread.a`, the mulle-thread` static library along with a bunch of
headers. You can often get by NOT linking with libmulle_thread.a, which is
fairly empty.


## Prerequisites

### mintomic

mulle-aba` needs [mintomic](https://mintomic.github.io/) as a prerequisite. Install it in the top directory besides <tt>src</tt> and <tt>dox</tt> as **mulle_mintomic**-
The rename is done for packaging reasons, because we install and link only a subset of the functionality.

If you know that your compiler does C11 atomics, you don't need mintomic.

### Use mulle-bootstrap to fetch mintomic

In the `mulle-thread` project directory execute 

```console
mulle-bootstrap
```

### OSX, Linux: Use brew to install mulle-bootstrap, in case you don't have it

Install the [homebrew](http://brew.sh/) package manager

```
brew tap mulle-kybernetik/software
brew install mulle-bootstrap
```

### Other Unixes: Use git to install mulle-bootstrap, in case you don't have it

```
git clone http://www.mulle-kybernetik.com/repositories/mulle-bootstrap
cd mulle-bootstrap
./install.sh
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



