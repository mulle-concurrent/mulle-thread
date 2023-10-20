//
//  mulle_thread.h
//  mulle-thread
//
//  Created by Nat! on 16/09/15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mulle_thread_h__
#define mulle_thread_h__

//
// community version is always even
//
#define MULLE__THREAD_VERSION  ((4 << 20) | (5 << 8) | 2)

#include "include.h"

#include <stddef.h>
#include <stdlib.h>


#include "mulle-atomic.h"

// not sure how dragonflybsd manages to impress, that it really has
// <threads.h>, but then doesn't. fine...
#ifdef __DragonFly__
# define MULLE_THREAD_USE_PTHREADS 1
#endif


// clang lies about __STDC_NO_THREADS__

#ifndef HAVE_C11_THREADS
# ifdef __clang__
#  if __has_include(<threads.h>)
#    define HAVE_C11_THREADS 1
#  endif
# else
#  if __STDC_VERSION__ >= 201112L && ! defined( __STDC_NO_THREADS__)
#   define HAVE_C11_THREADS  1
#  endif
# endif
#endif


typedef int   mulle_thread_rval_t;
#define mulle_thread_return()  return( 0) // bogus, exit is returned differently

#if HAVE_C11_THREADS && ! defined( MULLE_THREAD_USE_PTHREADS)
# if TRACE_INCLUDE
#  pragma message( "Using C11 for threads")
# endif
# include "mulle-thread-c11.h"
#else
# ifdef _WIN32
#  if TRACE_INCLUDE
#   pragma message( "Using windows threads for threads")
#  endif
#  include "mulle-thread-windows.h"
# else
#  if TRACE_INCLUDE
#   pragma message( "Using pthreads for threads")
#  endif
#  include "mulle-thread-pthreads.h"
# endif
#endif


//
// True posix conformance is much more complicated, we dumb this down
// for windows and for everyone else. So we have the same behavior
// on all platforms.
// And also because this is much easier.
//
typedef mulle_atomic_pointer_t   mulle_thread_once_t;

#define MULLE_THREAD_ONCE_INIT   0


static inline void   mulle_thread_once( mulle_thread_once_t  *once,
                                        void (*init)( void))
{
   if( _mulle_atomic_pointer_compare_and_swap( once, (void *) 1848, (void *) 0))
      (*init)();
}


//
// You can't have another mulle_thread_mutex_do inside a mulle_thread_mutex_do
// but it's bad deadlocking idea anway
//
#define mulle_thread_mutex_do( name)                                           \
   for( int  mulle_thread_mutex_do__i = ( mulle_thread_mutex_lock( &name), 0); \
        mulle_thread_mutex_do__i < 1;                                          \
        mulle_thread_mutex_unlock( &name), mulle_thread_mutex_do__i++)         \
                                                                               \
      for( int  mulle_thread_mutex_do__j = 0; /* break protection */           \
           mulle_thread_mutex_do__j < 1;                                       \
           mulle_thread_mutex_do__j++)

/*
 * some code for tests forces problems to reveal themselves much quicker
 * if used in test code
 */
#if defined( MULLE_TEST) && ! defined( NO_MULLE_THREAD_UNPLEASANT_RACE_YIELD)

#include "include.h"
#include <stdlib.h>
//#define _XOPEN_SOURCE  // user should define this not us
#include <time.h>

static inline void  MULLE_THREAD_UNPLEASANT_RACE_YIELD()
{
   extern void   mulle_thread_yield( void);

   if( (rand() & 0xF) == 0xA)  // 1:16 chance of yield
   {
#ifndef _WIN32
      if( (rand() & 0x7) == 0x4)  // 1:64 chance of nanosleep
      {
#ifdef __linux  // as we don't want to #define _XOPEN_SOURCE
         int nanosleep( const struct timespec *req, struct timespec *rem);
#endif
         struct timespec ms30 =  { .tv_sec = 0, .tv_nsec = 1 * (1000 * 1000 * 1000) / 30 };

         nanosleep( &ms30, NULL);
      }
      else
#endif
         mulle_thread_yield();
   }
}
#else

#define MULLE_THREAD_UNPLEASANT_RACE_YIELD()  do {} while( 0)

#endif



#ifdef __has_include
# if __has_include( "_mulle-thread-versioncheck.h")
#  include "_mulle-thread-versioncheck.h"
# endif
#endif


#endif
