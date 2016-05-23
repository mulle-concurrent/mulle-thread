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

#include <assert.h>
#include <threads.h>


typedef mtx_t    mulle_thread_mutex_t;
typedef tss_t    mulle_thread_tss_t;
typedef thrd_t   mulle_thread_t;
typedef int      mulle_thread_rval_t;

#pragma mark -
#pragma Threads

__attribute__((const))
static inline mulle_thread_t  mulle_thread_self( void)
{
   return( thrd_current());
}


// parameters different to pthreads!
static inline int   mulle_thread_create( mulle_thread_rval_t (*f)(void *),
                                         void *arg,
                                         mulle_thread_t *thread)
{
   return( thrd_create( thread, f, arg));
}


// parameters different to pthreads!
static inline int   mulle_thread_join( mulle_thread_t thread)
{
   return( thrd_join( thread, NULL));
}


static inline int   mulle_thread_detach( mulle_thread_t thread)
{
   return( thrd_detach( thread));
}


static inline int   mulle_thread_cancel( void)
{
   thrd_exit( -1);
   return( 0);  // never returns anyway
}


static inline void   mulle_thread_yield( void)
{
   thrd_yield();
}



#pragma mark -
#pragma mark Lock


// parameters different to mtx!
static inline int  mulle_thread_mutex_init( mulle_thread_mutex_t *lock)
{
   assert( ! thrd_success);
   return( mtx_init( lock, mtx_plain));
}


static inline int  mulle_thread_mutex_lock( mulle_thread_mutex_t *lock)
{
   assert( ! thrd_success);
   return( mtx_lock( lock));
}


static inline int  mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock)
{
   assert( ! thrd_success);
   return( mtx_trylock( lock));  // negate 4 pthread
}



static inline int  mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock)
{
   assert( ! thrd_success);
   return( mtx_unlock( lock));
}


static inline int  mulle_thread_mutex_done( mulle_thread_mutex_t *lock)
{
   mtx_destroy( lock);
   return( 0);
}


#pragma mark -
#pragma mark Thread Local Storage


// different parameters, rval always last
static inline int   mulle_thread_tss_create( void (*f)( void *), mulle_thread_tss_t *key)
{
   return( tss_create( key, f));
}


static inline void   mulle_thread_tss_free( mulle_thread_tss_t key)
{
   tss_delete( key);
}


static inline void   *mulle_thread_tss_get( mulle_thread_tss_t key)
{
   return( tss_get( key));
}


static inline int  mulle_thread_tss_set( mulle_thread_tss_t key,
                                         void *userdata)
{
   return( tss_set( key, userdata));
}

#endif
