//
//  mulle_thread_windows.h
//  mulle-thread
//
//  Created by Nat! on 25/08/16.
//  Copyright (c) 2016 Mulle kybernetiK. All rights reserved.
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

#ifndef mulle_thread_windows_h__
#define mulle_thread_windows_h__

#include "include.h"

// Must include mulle-c11-bool.h before windows.h to avoid BOOL conflicts
#ifndef MULLE_BOOL_DEFINED
# error "you need to include <mulle-c11/mulle-c11-bool.h> before including <windows.h>"
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <errno.h>
#include <assert.h>

typedef unsigned int   mulle_thread_rval_t;

MULLE__THREAD_GLOBAL
void   mulle_thread_windows_destroy_tss_if_needed( void);

#define mulle_thread_return()  do { mulle_thread_windows_destroy_tss_if_needed(); return( 0); } while(0)

typedef DWORD    mulle_thread_tss_t;
typedef HANDLE   mulle_thread_t;
typedef DWORD    mulle_thread_native_rval_t;

typedef uintptr_t   mulle_thread_id_t;

typedef mulle_thread_rval_t   mulle_thread_function_t( void *);
typedef void                  mulle_thread_callback_t( void *);


// too complicated and unsafe to extract and interpret section.OwningThread
typedef struct
{
   CRITICAL_SECTION         section;
   mulle_atomic_pointer_t   owner;
} mulle_thread_mutex_t;


typedef CONDITION_VARIABLE   mulle_thread_cond_t;


static inline uintptr_t   mulle_thread_dword_as_uintptr_t( DWORD value)
{
   LONG      signed32 = (LONG) value;   // reinterpret the bits as signed 32-bit
   intptr_t  extended = signed32;       // sign-extend to 64-bit
   return( (uintptr_t) extended);
}

#pragma mark - Threads

MULLE_C_CONST_RETURN
MULLE_C_NO_INSTRUMENT_FUNCTION
static inline mulle_thread_t  mulle_thread_self( void)
{
   // on windows this is always -2 a pseudo handle
   return( GetCurrentThread());
}

// use this for debugging output
MULLE_C_CONST_RETURN
MULLE_C_NO_INSTRUMENT_FUNCTION
static inline mulle_thread_id_t   mulle_thread_id( void)
{
   return( (mulle_thread_id_t) mulle_thread_dword_as_uintptr_t( GetCurrentThreadId()));
}


// use this for debugging output
MULLE_C_CONST_RETURN
static inline mulle_thread_id_t   mulle_thread_get_id( mulle_thread_t thread)
{
   return( (mulle_thread_id_t) mulle_thread_dword_as_uintptr_t( GetThreadId( thread)));
}


// parameters different to pthreads!
static inline int   mulle_thread_create( mulle_thread_function_t *f,
                                         void *arg,
                                         mulle_thread_t *p_thread)
{
   unsigned   threadId;   /* not strictly needed, but shows parameter use */
   HANDLE     handle;

   handle = (HANDLE) _beginthreadex( NULL,
                                     0,
                                     (_beginthreadex_proc_type) f,
                                     arg,
                                     0,
                                     &threadId);
   *p_thread = handle;
   if( handle)
   {
      assert( threadId != (unsigned) -1);
      assert( threadId != (unsigned) 0);
      return( 0);
   }
   return( -1);
}


MULLE__THREAD_GLOBAL
void   mulle_thread_exit( int rval);



MULLE__THREAD_GLOBAL
mulle_thread_rval_t   mulle_thread_join( mulle_thread_t thread);



// http://stackoverflow.com/questions/12744324/how-to-detach-a-thread-on-windows-c
static inline int   mulle_thread_detach( mulle_thread_t thread)
{
   return( CloseHandle( thread) ? 0 : -1);
}


static inline void   mulle_thread_yield(void)
{
   SwitchToThread();
}


#pragma mark - Lock

static inline int   mulle_thread_mutex_init( mulle_thread_mutex_t *lock)
{
   InitializeCriticalSection( &lock->section);
   _mulle_atomic_pointer_write_nonatomic( &lock->owner, 0);

   return( 0);
}


static inline int   mulle_thread_mutex_lock( mulle_thread_mutex_t *lock)
{
   mulle_thread_id_t   self_id;

   self_id = mulle_thread_id();
   EnterCriticalSection( &lock->section);
   _mulle_atomic_pointer_write( &lock->owner, (void *) self_id);

   return( 0);
}


static inline int   mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock)
{
   mulle_thread_id_t   owner_id;
   mulle_thread_id_t   self_id;

   //
   // in win it's ok to reenter a critical, but that's not OK in
   // pthreads. So if it's locked and we own, then... it's busy
   //
   owner_id = (mulle_thread_id_t) _mulle_atomic_pointer_read( &lock->owner);
   self_id  = mulle_thread_id();
   if( owner_id == self_id)
      return( EBUSY);

   if( TryEnterCriticalSection( &lock->section))
   {
      _mulle_atomic_pointer_write( &lock->owner, (void *) self_id);
      return( 0);
   }
   return( EBUSY);
}


static inline int   mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock)
{
   assert( _mulle_atomic_pointer_read_nonatomic( &lock->owner) == (void *) mulle_thread_id());

   _mulle_atomic_pointer_write( &lock->owner, (void *) 0);
   LeaveCriticalSection( &lock->section);
   return( 0);
}


static inline int   mulle_thread_mutex_done( mulle_thread_mutex_t *lock)
{
   DeleteCriticalSection( &lock->section);
   return( 0);
}


#pragma mark - Condition Variables

static inline int   mulle_thread_cond_init( mulle_thread_cond_t *cond)
{
   InitializeConditionVariable( cond);
   return( 0);
}


static inline int   mulle_thread_cond_done( mulle_thread_cond_t *cond)
{
   // Windows condition variables don't need cleanup
   (void) cond;
   return( 0);
}


static inline int   mulle_thread_cond_signal( mulle_thread_cond_t *cond)
{
   WakeConditionVariable( cond);
   return( 0);
}


static inline int   mulle_thread_cond_broadcast( mulle_thread_cond_t *cond)
{
   WakeAllConditionVariable( cond);
   return( 0);
}


MULLE__THREAD_GLOBAL
int   mulle_thread_cond_wait( mulle_thread_cond_t *cond,
                              mulle_thread_mutex_t *mutex);


MULLE__THREAD_GLOBAL
int   mulle_thread_cond_timedwait( mulle_thread_cond_t *cond,
                                   mulle_thread_mutex_t *mutex,
                                   struct timespec *abstime);

#pragma mark - Thread Local Storage

//
// Don't call mulle_thread_tss_init after having called mulle_thread_tss_done
// (in the same thread, except if you are single-threaded)
//
MULLE__THREAD_GLOBAL
void   mulle_thread_tss_init( void);

MULLE__THREAD_GLOBAL
void   mulle_thread_tss_done( void);

MULLE__THREAD_GLOBAL
int   mulle_thread_tss_create( mulle_thread_callback_t *f,
                               mulle_thread_tss_t *key);
MULLE__THREAD_GLOBAL
void   mulle_thread_tss_free( mulle_thread_tss_t key);


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *mulle_thread_tss_get( mulle_thread_tss_t key)
{
   // if you SIGSEGV here, it probably means: your stack is overflown
   return( TlsGetValue( key));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int  mulle_thread_tss_set( mulle_thread_tss_t key, void *value)
{
   return( TlsSetValue( key, value) ? 0 : -1);
}

//
// TODO: use CreateEventA to implement pthread_cond style conditions
//
#endif
