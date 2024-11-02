//
//  mulle_thread_c11.h
//  mulle-thread
//
//  Created by Nat! on 09/10/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
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

#ifndef mulle_thread_c11_h__
#define mulle_thread_c11_h__

#include "include.h"

#include <assert.h>
#include <errno.h>
#include <threads.h>


//typedef once_flag   mulle_thread_once_t;
typedef mtx_t       mulle_thread_mutex_t;
typedef tss_t       mulle_thread_tss_t;
typedef thrd_t      mulle_thread_t;
typedef int         mulle_thread_native_rval_t;


typedef mulle_thread_rval_t   mulle_thread_function_t( void *);
typedef void                  mulle_thread_callback_t( void *);

//#define MULLE_THREAD_ONCE_DATA   ONCE_FLAG_INIT


#pragma mark -
#pragma Threads

MULLE_C_CONST_RETURN
static inline mulle_thread_t  mulle_thread_self( void)
{
   return( thrd_current());
}


// p_thread is a return value
static inline int   mulle_thread_create( mulle_thread_function_t *f,
                                         void *arg,
                                         mulle_thread_t *p_thread)
{
   return( thrd_create( p_thread, (thrd_start_t) f, arg) == thrd_success ? 0 : -1);
}


static inline mulle_thread_rval_t   mulle_thread_join( mulle_thread_t thread)
{
   mulle_thread_rval_t   storage;
   int                   rval;

   rval = thrd_join( thread, &storage);
   if( rval == thrd_success)
      return( storage);
   return( (mulle_thread_rval_t) -1);
}


static inline int   mulle_thread_detach( mulle_thread_t thread)
{
   return( thrd_detach( thread) == thrd_success ? 0 : -1);
}


static inline void   mulle_thread_exit( mulle_thread_rval_t rval)
{
   thrd_exit( rval);
}


static inline void   mulle_thread_yield( void)
{
   thrd_yield();
}


//static inline void   mulle_thread_once( mulle_thread_once_t  *once,
//                                        void (*init)( void))
//{
//   call_once( once, init);
//}
//

#pragma mark - Lock

// moronic FreeBSD <threads.h>
// parameters different to mtx!
static inline int   mulle_thread_mutex_init( mulle_thread_mutex_t *lock)
{
   return( mtx_init( lock, mtx_plain) == thrd_success ? 0 : -1);
}


static inline int   mulle_thread_mutex_lock( mulle_thread_mutex_t *lock)
{
   return( mtx_lock( lock) == thrd_success ? 0 : -1);
}


static inline int   mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock)
{
   switch( mtx_trylock( lock))
   {
   case thrd_success : return( 0) ;
   case thrd_busy    : return( EBUSY);
   default           : return( EINVAL);
   }
}


static inline int   mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock)
{
   return( mtx_unlock( lock) == thrd_success ? 0 : -1);
}


static inline int   mulle_thread_mutex_done( mulle_thread_mutex_t *lock)
{
   mtx_destroy( lock);
   return( 0);
}


#pragma mark - Thread Local Storage

// different parameters, rval always last
// a returned zero key is valid!
// destruktor is likely NOT to be called on main thread
//
static inline int   mulle_thread_tss_create( mulle_thread_callback_t f,
                                             mulle_thread_tss_t *key)
{
   assert( key);
   return( tss_create( key, (tss_dtor_t) f) == thrd_success ? 0 : -1);
}


static inline void   mulle_thread_tss_free( mulle_thread_tss_t key)
{
   tss_delete( key);
}


static inline void   *mulle_thread_tss_get( mulle_thread_tss_t key)
{
   // if you SIGSEGV here, it probably means: your stack has overflown
   return( tss_get( key));
}


static inline int  mulle_thread_tss_set( mulle_thread_tss_t key,
                                         void *value)
{
   return( tss_set( key, value) == thrd_success ? 0 : -1);
}

#endif
