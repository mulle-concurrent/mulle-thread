# Build Information


## What you get
 
* `libmulle_thread.a`, the mulle-thread static library along with a bunch of
headers. You can often get by, not linking with libmulle_thread.a, which is
fairly empty. That isexcept on windows, where you absolutely need it.


## Prerequisites

#### mulle-c11

[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/) is a header 
that abstracts a small set of non-standardized compiler features. 


#### mulle-configuration

[mulle-configuration](//www.mulle-kybernetik.com/software/git/mulle-c11/) are
configuration files for building with Xcode or cmake.


#### mintomic

If your compiler does not support C11 atomics, then `mulle-thread` needs 
[mintomic](https://mintomic.github.io/) as a prerequisite. 


> ##### Use mulle-bootstrap to fetch mintomic and mulle-c11
>
> ###### OSX, Linux: Use brew to install mulle-bootstrap, in case you don't have it
>
> Install the [homebrew](http://brew.sh/) package manager, then
>
> ```
> brew tap mulle-kybernetik/software
> brew install mulle-bootstrap
> ```
>
> ###### Other Unixes: Use git to install mulle-bootstrap, in case you don't have it
>
> ```
> git clone http://www.mulle-kybernetik.com/repositories/mulle-bootstrap
> cd mulle-bootstrap
> ./install.sh
> ```

In the `mulle-thread` project directory execute 

```console
mulle-bootstrap
```


## Building

### With cmake

You need **cmake 3.0** or better. It's easiest to get this also via homebrew
with `brew install cmake`:

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



