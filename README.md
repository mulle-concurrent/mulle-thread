# mulle-thread

## Intro

These are wrapper headers for subset functionalities of pthreads and mintomic. 
When expanding to different platforms, only those functions need to be ported.

Since C11 has `<stdatomic>` and `<threads>`, eventually this project will be 
obsolete.


## mintomic

This was the original source for atomic operations, unfortunately it's not 
really active anymore, so f.e. ARM64 doesn't work. It will be eventually 
completely replaced by C11.

For now, mulle_atomic will use mintomic if <stdatomic.h> is not available.

## C11

C11 support in clang is currently lacking, as there is no <threads.h>. The
fallback is pthreads. 


