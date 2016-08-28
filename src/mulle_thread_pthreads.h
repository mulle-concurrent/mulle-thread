//
//  mulle_thread_pthreads.h
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

#ifndef mulle_thread_pthreads_h__
#define mulle_thread_pthreads_h__

#include <mulle_c11/mulle_c11.h>
#include <pthread.h>
#include <errno.h>


typedef pthread_mutex_t   mulle_thread_mutex_t;
typedef pthread_key_t     mulle_thread_tss_t;
typedef pthread_t         mulle_thread_t;
typedef void *            mulle_thread_rval_t;


#pragma mark -
#pragma Threads

MULLE_C_CONST_RETURN
static inline mulle_thread_t  mulle_thread_self( void)
{
   return( pthread_self());
}


// parameters different to pthreads!
static inline int   mulle_thread_create( mulle_thread_rval_t (*f)(void *),
                                         void *arg,
                                         mulle_thread_t *thread)
{
   return( pthread_create( thread, NULL, f, arg));
}


// parameters different to pthreads!
static inline int   mulle_thread_join( mulle_thread_t thread)
{
   return( pthread_join( thread, NULL));
}



static inline int   mulle_thread_detach( mulle_thread_t thread)
{
   return( pthread_detach( thread));
}


static inline void   mulle_thread_exit( void)
{
   pthread_exit( NULL);
}


static inline void   mulle_thread_yield( void)
{
   extern int   sched_yield( void);  // POSIX... don't want the header though
   
   sched_yield();
}


#pragma mark -
#pragma mark Lock


// parameters different to pthreads!
static inline int  mulle_thread_mutex_init( mulle_thread_mutex_t *lock)
{
   return( pthread_mutex_init( lock, NULL));
}


static inline int  mulle_thread_mutex_lock( mulle_thread_mutex_t *lock)
{
   return( pthread_mutex_lock( lock));
}


static inline int  mulle_thread_mutex_trylock( mulle_thread_mutex_t *lock)
{
   if( ! pthread_mutex_trylock( lock))
      return( 0);
   if( errno == EBUSY)
      return( 1);
   return( -1);
}


static inline int  mulle_thread_mutex_unlock( mulle_thread_mutex_t *lock)
{
   return( pthread_mutex_unlock( lock));
}


static inline int  mulle_thread_mutex_done( mulle_thread_mutex_t *lock)
{
   return( pthread_mutex_destroy( lock));
}


#pragma mark -
#pragma mark Thread Local Storage

// different parameters, rval always last
static inline int   mulle_thread_tss_create( void (*f)( void *), mulle_thread_tss_t *key)
{
   return( pthread_key_create( key, f));
}


static inline void   mulle_thread_tss_free( mulle_thread_tss_t key)
{
   pthread_key_delete( key);
}


static inline void   *mulle_thread_tss_get( mulle_thread_tss_t key)
{
   return( pthread_getspecific( key));
}


static inline int  mulle_thread_tss_set( mulle_thread_tss_t key,
                                         void *userdata)
{
   return( pthread_setspecific( key, userdata));
}

#endif
