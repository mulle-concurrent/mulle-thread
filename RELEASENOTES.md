0.2
===
Added mulle_thread_tss_delete.
Add MULLE_THREAD_VERSION.
User can select mintomic or pthreads, with MULLE_THREAD_USE_MINTOMIC viz
MULLE_THREAD_USE_PTHREADS

0.1
===

Adorned mulle_thread_self() with __attribute__((const, returns_nonnull)) so that
the caller can cache it.


 