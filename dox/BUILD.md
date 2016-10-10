# Build Information


## What you get

* `libmulle_thread.a`, the mulle-thread static library along with a bunch of
headers. You can often get by not linking with libmulle_thread.a, except on
windows, where you absolutely do need it.


## Prerequisites

### Windows

You need to install some more prerequisites first.

* Install [Visual Studio 2015 Community Edition](//beta.visualstudio.com/downloads/)
or better (free). Make sure that you install Windows C++ support. Also add git support.
* [Git for Windows](//git-scm.com/download/win) is included in VS 2015, make sure it's there
* [Python 2 for Windows](//www.python.org/downloads/windows/). Make sure that python is installed in **PATH**, which is not the default.
* [CMake for Windows](//cmake.org/download/). CMake should also add itself to **PATH**.

Reboot, so that Windows picks up the **PATH** changes (Voodoo).

Now the tricky part is to get the "Git bash" shell running with the proper VS
environment.  Assuming you kept default settings the "Git bash" is
`C:\Program Files\Git\git-bash.exe`. Open the "Developer Command Prompt for VS 2015"
from the start menu and execute the git-bash from there. A second window with
the bash should open.

Check that you have the proper environment for VS compilation with `env`.

#### mulle-build

[mulle-build](//www.mulle-kybernetik.com/software/git/mulle-build) is required
to assemble the dependencies together and build the library.

#### mulle-c11

[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/) is a header
that abstracts a small set of non-standardized compiler features.


#### mulle-configuration

[mulle-configuration](//www.mulle-kybernetik.com/software/git/mulle-c11/) are
configuration files for building with Xcode or cmake.


#### mintomic

If your compiler does not support C11 atomics, then `mulle-thread` needs
[mintomic](https://mintomic.github.io/) as a prerequisite.


### Use mulle-build to fetch prerequisites and build the library

Fetch dependencies and build mulle-thread in debug mode:

```console
( cd mulle-thread ; mulle-build --debug )
```

Run some tests:

```console
( cd mulle-thread ; mulle-test )
```

Build library in release mode and install into `tmp` :

```console
(
   cd mulle-thread ;
   mulle-clean ;
   mulle-install --prefix /tmp
```


#### OSX: Use brew to install mulle-build

Install the [homebrew](http://brew.sh/) package manager, then

```
brew tap mulle-kybernetik/software
brew install mulle-build
```

#### Other Unices / Windows: Use git to install mulle-build

```
git clone http://www.mulle-kybernetik.com/repositories/mulle-bootstrap
( cd mulle-bootstrap ; ./install.sh )
git clone http://www.mulle-kybernetik.com/repositories/mulle-build
( cd mulle-build ; ./install.sh )
```


### Compile Flags

For development use no flags.

For production use NDEBUG

* DEBUG : turns on some compile time facilities to aid debugging `mulle-thread` itself.
* MULLE_ATOMIC_TRACE : turns on a very detailed amount of tracing for some atomic operations.



