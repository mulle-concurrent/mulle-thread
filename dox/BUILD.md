# Build Information


## What you get

* `libmulle_thread.a`, the mulle-thread static library along with a bunch of
headers. You can often get by, not linking with libmulle_thread.a, which is
fairly empty. That isexcept on windows, where you absolutely need it.


## Prerequisites

### Windows

You need to install some more prerequisites first.

* Install [Visual Studio 2015 Community Edition](//beta.visualstudio.com/downloads/)
or better (free). Make sure that you install Windows C++ support. Also add git support.
* [Git for Windows](//git-scm.com/download/win) is included in VS 2015, make sure it's there
* [Python 2 for Windows](//www.python.org/downloads/windows/). **Make sure that python is installed in **PATH**, which is not the default**
* [CMake for Windows](//cmake.org/download/). CMake should also add itself to **PATH**.

Reboot, so that Windows picks up the **PATH** changes (Voodoo).

Now the tricky part is to get the "Git bash" shell running with the proper VS
environment.  Assuming you kept default settings the "Git bash" is
`C:\Program Files\Git\git-bash.exe`. Open the "Developer Command Prompt for VS 2015"
from the start menu and execute the git-bash from there. A second window with
the bash should open.


#### mulle-c11

[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/) is a header
that abstracts a small set of non-standardized compiler features.


#### mulle-configuration

[mulle-configuration](//www.mulle-kybernetik.com/software/git/mulle-c11/) are
configuration files for building with Xcode or cmake.


#### mintomic

If your compiler does not support C11 atomics, then `mulle-thread` needs
[mintomic](https://mintomic.github.io/) as a prerequisite.


### Use mulle-bootstrap to fetch prerequisites


#### OSX: Use brew to install mulle-bootstrap

Install the [homebrew](http://brew.sh/) package manager, then

```
brew tap mulle-kybernetik/software
brew install mulle-bootstrap
```

#### Other Unixes: Use git to install mulle-bootstrap

```
git clone http://www.mulle-kybernetik.com/repositories/mulle-bootstrap
cd mulle-bootstrap
./install.sh
```

Then in the `mulle-thread` project directory execute

```console
mulle-bootstrap
```


## Building

### All OS except Windows

You need **cmake 3.0** or better. It's easiest to get this also via homebrew
with `brew install cmake`:

```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```

### Windows

```
mkdir build
cd build
cmake -G "NMake Makefiles" ..
nmake
```


## Installing


#### On Unixes

```
mulle-bootstrap install
cd build
make install
```

#### On Windows

```
mulle-bootstrap install
cd build
nmake install
```


### Compile Flags

For development use no flags.

For production use NDEBUG

* DEBUG : turns on some compile time facilities to aid debugging `mulle-thread` itself.
* MULLE_ATOMIC_TRACE : turns on a very detailed amount of tracing for some atomic operations.



