//
//  linkage.c
//  mulle-thread
//
//  Created by Nat! on 16/09/15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
//


// provide linkage to glom thread lib unto (if dylib)
// but you can live without it in .a
unsigned long   __mulle_thread_version__ = 0x0000;

// we include this, if we need it

#if __STDC_VERSION__ < 201112L || defined( __STDC_NO_ATOMICS__)
#include "mintomic/mintomic_gcc.c"
#endif
