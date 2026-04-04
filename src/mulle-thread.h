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
#define MULLE__THREAD_VERSION  ((4UL << 20) | (8 << 8) | 0)

#include "include.h"

#include <stddef.h>
#include <stdlib.h>


#include "mulle-atomic.h"

// not sure how dragonflybsd manages to impress, that it really has
// <threads.h>, but then doesn't. fine...
#ifdef __DragonFly__
# define MULLE_THREAD_USE_PTHREADS 1
#endif


// clang lies about __STDC_NO_THREADS__, gcc does not necessary
// windows thinks it has threads.h but its apparently something else ?

#ifndef _WIN32
# ifndef HAVE_C11_THREADS
#  if defined( __has_include) || defined( __clang__) || (defined(__GNUC__) && (__GNUC__ >= 5))
#   if __has_include(<threads.h>)
#     define HAVE_C11_THREADS 1
#   endif
#  else
#   if __STDC_VERSION__ >= 201112L && ! defined( __STDC_NO_THREADS__)
#    define HAVE_C11_THREADS  1
#   endif
#  endif
# endif
#endif


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
// currently we say that a mutex must be cleaned up with a "done" call
// platforms that do not need this, can set this to 0 and then you can
// get rid of some -dealloc overhead
//
#define MULLE_THREAD_MUTEX_NEEDS_DONE   1
#define MULLE_THREAD_COND_NEEDS_DONE    1 // for the future

//
// True posix conformance is much more complicated, we dumb this down
// for windows and for everyone else. So we have the same behavior
// on all platforms.
// And also because this is much easier.
//
typedef mulle_atomic_pointer_t   mulle_thread_once_t;

#define MULLE_THREAD_ONCE_DATA   0
#define MULLE_THREAD_ONCE_BUSY   1848
#define MULLE_THREAD_ONCE_DONE   1

// old name
#define MULLE_THREAD_ONCE_INIT   MULLE_THREAD_ONCE_DATA


// recursive once: tracks thread ID to allow same thread to proceed
typedef struct
{
   mulle_atomic_pointer_t   _state;
   mulle_atomic_pointer_t   _thread_id;
} mulle_thread_once_recursive_t;

#define MULLE_THREAD_ONCE_RECURSIVE_INIT   ((mulle_thread_once_recursive_t) { MULLE_THREAD_ONCE_INIT, 0 })


// the old, not so useful interface
MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE__THREAD_GLOBAL
void   mulle_thread_once( mulle_thread_once_t  *once,
                          void (*init)( void));

// the new, more useful interface
MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE__THREAD_GLOBAL
void   mulle_thread_once_call( mulle_thread_once_t  *once,
                               void (*init)( void *),
                               void *userinfo);

// recursive variant: same thread can call again without deadlock
MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE__THREAD_GLOBAL
void   mulle_thread_once_call_recursive( mulle_thread_once_recursive_t  *once,
                                         void (*init)( void *),
                                         void *userinfo);

// convenience wrapper
MULLE_C_NO_INSTRUMENT_FUNCTION
static inline void   mulle_thread_once_recursive( mulle_thread_once_recursive_t  *once,
                                                   void (*init)( void))
{
   mulle_thread_once_call_recursive( once, (void (*)(void *)) init, NULL);
}

// non-blocking variants (old behavior)
MULLE_C_NO_INSTRUMENT_FUNCTION
static inline void   mulle_thread_once_noblock( mulle_thread_once_t  *once,
                                                 void (*init)( void))
{
   if( _mulle_atomic_pointer_compare_and_swap( once,
                                               (void *) MULLE_THREAD_ONCE_BUSY,
                                               (void *) MULLE_THREAD_ONCE_INIT))
   {
      (*init)();
      mulle_atomic_memory_barrier();
   }
}

MULLE_C_NO_INSTRUMENT_FUNCTION
static inline void   mulle_thread_once_call_noblock( mulle_thread_once_t  *once,
                                                      void (*init)( void *),
                                                      void *userinfo)
{
   if( _mulle_atomic_pointer_compare_and_swap( once,
                                               (void *) MULLE_THREAD_ONCE_BUSY,
                                               (void *) MULLE_THREAD_ONCE_INIT))
   {
      (*init)( userinfo);
      mulle_atomic_memory_barrier();
   }
}

// convenient interface
#define mulle_thread_once_do( name)                                                                 \
   static mulle_thread_once_t   name = MULLE_THREAD_ONCE_INIT;                                      \
   for( void *actual = __mulle_atomic_pointer_compare_and_swap( &name,                              \
                                                                 (void *) MULLE_THREAD_ONCE_BUSY,   \
                                                                 (void *) MULLE_THREAD_ONCE_INIT);  \
        actual != (void *) MULLE_THREAD_ONCE_DONE;                                                  \
        actual = ( mulle_atomic_memory_barrier(),                                                   \
                   _mulle_atomic_pointer_compare_and_swap( &name,                                   \
                                                          (void *) MULLE_THREAD_ONCE_DONE,          \
                                                          (void *) MULLE_THREAD_ONCE_BUSY),         \
                  (void *) MULLE_THREAD_ONCE_DONE))                                                 \
      MULLE_C_CONFINED_LOOP                                                                         \
      for( int  mulle_thread_once_do__j = 0; /* break protection */                                 \
           mulle_thread_once_do__j < 1;                                                             \
           mulle_thread_once_do__j++)                                                               \
         if( actual != MULLE_THREAD_ONCE_INIT)                                                      \
         {                                                                                          \
            do                                                                                      \
               mulle_thread_yield();                                                                \
            while( _mulle_atomic_pointer_read( &name) != (void *) MULLE_THREAD_ONCE_DONE);          \
            mulle_atomic_memory_barrier();                                                          \
            break;                                                                                  \
         }                                                                                          \
         else

// convenient recursive interface
#define mulle_thread_once_do_recursive( name)                                                       \
   static mulle_thread_once_recursive_t   name = MULLE_THREAD_ONCE_RECURSIVE_INIT;                  \
   mulle_thread_id_t  mulle_thread_once_do_recursive__tid = mulle_thread_id();                      \
   for( void *actual = ((mulle_thread_id_t) _mulle_atomic_pointer_read( &name._thread_id) ==        \
                        mulle_thread_once_do_recursive__tid)                                        \
                       ? (void *) MULLE_THREAD_ONCE_DONE                                            \
                       : __mulle_atomic_pointer_compare_and_swap( &name._state,                     \
                                                                 (void *) MULLE_THREAD_ONCE_BUSY,   \
                                                                 (void *) MULLE_THREAD_ONCE_INIT);  \
        actual != (void *) MULLE_THREAD_ONCE_DONE;                                                  \
        actual = ( _mulle_atomic_pointer_write( &name._thread_id, NULL),                            \
                   mulle_atomic_memory_barrier(),                                                   \
                   _mulle_atomic_pointer_compare_and_swap( &name._state,                            \
                                                          (void *) MULLE_THREAD_ONCE_DONE,          \
                                                          (void *) MULLE_THREAD_ONCE_BUSY),         \
                  (void *) MULLE_THREAD_ONCE_DONE))                                                 \
      MULLE_C_CONFINED_LOOP                                                                         \
      for( int  mulle_thread_once_do_recursive__j = 0; /* break protection */                       \
           mulle_thread_once_do_recursive__j < 1;                                                   \
           mulle_thread_once_do_recursive__j++)                                                     \
         if( actual != MULLE_THREAD_ONCE_INIT)                                                      \
         {                                                                                          \
            do                                                                                      \
               mulle_thread_yield();                                                                \
            while( _mulle_atomic_pointer_read( &name._state) != (void *) MULLE_THREAD_ONCE_DONE);   \
            mulle_atomic_memory_barrier();                                                          \
            break;                                                                                  \
         }                                                                                          \
         else                                                                                       \
            if( (_mulle_atomic_pointer_write( &name._thread_id,                                     \
                                             (void *) mulle_thread_once_do_recursive__tid), 1))

// convenient non-blocking interface (old behavior)
#define mulle_thread_once_do_noblock( name)                                             \
   static mulle_thread_once_t   name = MULLE_THREAD_ONCE_INIT;                          \
   if( _mulle_atomic_pointer_compare_and_swap( &name,                                   \
                                               (void *) MULLE_THREAD_ONCE_BUSY,         \
                                               (void *) MULLE_THREAD_ONCE_INIT))

//
// You can't have another mulle_thread_mutex_do inside a mulle_thread_mutex_do.
// It's a bad deadlocking idea anyway. Using a name and not a pointer is
// consistent with mulle-buffer.
//
#define mulle_thread_mutex_do( mutex)                                           \
   for( int  mulle_thread_mutex_do__i = ( mulle_thread_mutex_lock( &mutex), 0); \
        mulle_thread_mutex_do__i < 1;                                           \
        mulle_thread_mutex_unlock( &mutex), mulle_thread_mutex_do__i++)         \
                                                                                \
      MULLE_C_CONFINED_LOOP                                                     \
      for( int  mulle_thread_mutex_do__j = 0; /* break protection */            \
           mulle_thread_mutex_do__j < 1;                                        \
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
