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


