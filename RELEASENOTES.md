### 4.1.7

* move mintomic from mulle-nat to mulle-concurrent

### 4.1.6

* fix more mintomic stuff

### 4.1.5

* fix functionpointer case code for mintomic

### 4.1.4

* fix missing mintomic private headers...

### 4.1.3

* fix missing mintomic headers

### 4.1.2

* fix misspelled variables in mintomic code

### 4.1.1

* add missing cmake files

## 4.1.0

* added support for `mulle_thread_once`


### 4.0.20

* remove obsolete file

### 4.0.19

* adapt to mulle-c11 3.0.0

### 4.0.18

* fix misspelling

### 4.0.17

* modernize mulle-sde and fix for mingw

### 4.0.16

* add mintomic headers back in for trusty and related

### 4.0.15

* modernized mulle-sde

### 4.0.14

* handle sources manually again, publish mintomic headers again

### 4.0.13

* fix travis.yml

### 4.0.12

* make -isystem absolute

### 4.0.11

* local buildinfos are a bad idea IMO, do it in cmake

### 4.0.10

* proper isystem directory now...

### 4.0.9

* fix build problems on old linux with .mulle-make

### 4.0.8

* fix ppa sourceline

### 4.0.7

* try to add ppa to travis for cmake 3

### 4.0.6

* fix key for debian

### 4.0.5

* try different YAML style for && escapement

### 4.0.4

* fix yaml

### 4.0.3

* fix travis

### 4.0.2

* upgraded mulle-sde extensions

### 4.0.1

* fix travis

# 4.0.0

* use uniform - instead of `_` in header files
* migrated to mulle-sde
* no longer available through homebrew

### 3.3.17

* support new mulle-tests

### 3.3.15

* fixed scion wrapper command

### 3.3.13

* refinements for mulle-configuration 3.1

### 3.3.11

* Modernize CMakeLists.txt


### 3.3.9

* Various small improvements

### 3.3.7

* make it a cmake "C" projezt

### 3.3.5

* modernized project


3.3.4
===

* community release


3.3.1
===

* use mulle-bootstrap alpha, clean-up some garbage

3.2.15
===

* merge community release

3.2.14
===

* improved README, improved distribution

3.2.13
===

* merge in community release


3.2.12
===

* fix homebrew package

3.2.10
===

* first community release version, fix for community repositories

3.2.9
===

* modernized tests dir structure
* modernized release.sh


3.2.8
===

* improved .travis.yml


3.2.7
===

* fix README.md

3.2.6
===

* fix test for windows


3.2.5
===

* improve documentation, don't duplicate mulle-build install documentation


3.2.4
===

* fix prettied up windows code

3.2.3
===

* pretty up windows code

3.2.2
===

* improve documentation

3.2.1
===

* improve documentation
* remove some outdated remnants

3.2
===

* Add bounce documentation
* Remove magic bounceinfo code from pthreads_create introduced around 3.0. This
should be done by the caller.
* Make windows trylock behavior consistent with pthreads


3.1.1
===

* fix wrong version number
* migrate homebrew install to mulle-build 0.5
* move atomic tests to tests folder and integrate into mulle-tests


3.1
===

* simplify use of mulle_thread_create, so that mulle_thread_exit will always
be called
* fix bug in pthreads when using trylock
* pthread_join now returns the rval from the thread (whoever needs it).

3.0
===

* add thread code for windows
* mulle_thread_cancel doesn't exist anymore.
* mulle_thread_exit is a new function
* improved ocumentation


2.1
===

* add function pointer atomicity. Why is a function pointer different to a void
pointer ? Imagine a machine with 64 KB ram. Data pointers could be 16 bit. Now
imagine it having a ROM of 4 MB, function pointers could be 32 bit. Basically
though the function pointers are just there to reduce warnings at the moment.


2.0
===
* renamed _destroy and _delete functions to be orthogonal with other mulle
  functions (sorry) to _free and _done.


1.1
===

* `returns_nonnull` does not mean `returns_nonzero` for some compilers,
making them throw an error. Moronic...

1.0
===

* Moved return value parameter to the back of `mulle_thread_tss_create`.
* Added mulle_thread_detach.
* Added mulle_thread_yield (will be sched_yield for pthreads)
* Don't assert value != expect in __...CAS.

0.2
===

* Added mulle_thread_tss_delete.
* Add MULLE_THREAD_VERSION.
* User can select mintomic or pthreads, with MULLE_THREAD_USE_MINTOMIC viz
MULLE_THREAD_USE_PTHREADS

0.1
===

* Adorned mulle_thread_self() with __attribute__((const, returns_nonnull)) so that
the caller can cache it.
