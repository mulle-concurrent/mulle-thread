2.1
===

* add function pointer atomicity. Why is a function pointer different to a void 
pointer ? Imagine a machine with 64 KB ram. Data pointers could be 16 bit. Now
imagine it having a ROM of 4 MB, function pointers could be 32 bit.

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


 