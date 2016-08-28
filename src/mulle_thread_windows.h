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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <errno.h>
#include <mulle_c11/mulle_c11.h>

typedef CRITICAL_SECTION  mulle_thread_mutex_t;
typedef DWORD             mulle_thread_tss_t;
typedef HANDLE            mulle_thread_t;
typedef int               mulle_thread_rval_t;


#pragma mark -
#pragma Threads

MULLE_C_VALUE_RETURN
static inline mulle_thread_t  mulle_thread_self(void)
{
	return( GetCurrentThread());
}


// parameters different to pthreads!
static inline int   mulle_thread_create( mulle_thread_rval_t (*f)(void *),
	void *arg,
	mulle_thread_t *thread)
{
	*thread = (HANDLE) _beginthreadex( NULL, 0, (_beginthreadex_proc_type) f, arg, 0, NULL);
	return( *thread ? 0 : -1);
}


static inline void   mulle_thread_exit( void)
{
   extern void  mulle_thread_windows_destroy_tss( void);

   mulle_thread_windows_destroy_tss();
   _endthreadex( 0);
}


// parameters different to pthreads!
static inline int   mulle_thread_join(mulle_thread_t thread)
{
   // TODO: fix error codes 
	return( WaitForSingleObject( thread, INFINITE));
}


// http://stackoverflow.com/questions/12744324/how-to-detach-a-thread-on-windows-c
static inline int   mulle_thread_detach(mulle_thread_t thread)
{
	return( CloseHandle( thread) ? 0 : -1);
}


static inline void   mulle_thread_yield(void)
{
	SwitchToThread();
}


#pragma mark -
#pragma mark Lock

static inline int  mulle_thread_mutex_init(mulle_thread_mutex_t *lock)
{
	InitializeCriticalSection( lock);
   return( 0);
}


static inline int  mulle_thread_mutex_lock(mulle_thread_mutex_t *lock)
{
	EnterCriticalSection( lock);
   return( 0);
}


static inline int  mulle_thread_mutex_trylock(mulle_thread_mutex_t *lock)
{
	if( TryEnterCriticalSection( lock))
		return(0);
   return(1);
}


static inline int  mulle_thread_mutex_unlock(mulle_thread_mutex_t *lock)
{
	LeaveCriticalSection( lock);
   return( 0);
}


static inline int  mulle_thread_mutex_done(mulle_thread_mutex_t *lock)
{
	DeleteCriticalSection( lock);
   return( 0);
}


#pragma mark -
#pragma mark Thread Local Storage

//
// Don't call mulle_thread_tss_init after having called mulle_thread_tss_done
// (in the same thread, except if you are single-threaded)
//
extern void   mulle_thread_tss_init( void);
extern void   mulle_thread_tss_done( void);


static inline int   mulle_thread_tss_create( void(*f)(void *), mulle_thread_tss_t *key)
{
   extern int   mulle_thread_windows_add_tss( mulle_thread_tss_t key, void(*f)(void *));

	*key = TlsAlloc();
   if( *key == TLS_OUT_OF_INDEXES)
   { 
      *key = 0;
      return(-1);
   }
   return( mulle_thread_windows_add_tss( *key, f));
}


static inline void   mulle_thread_tss_free(mulle_thread_tss_t key)
{
   extern void  mulle_thread_windows_remove_tss( mulle_thread_tss_t key);

   mulle_thread_windows_remove_tss( key);
   TlsFree( key);
}


static inline void   *mulle_thread_tss_get(mulle_thread_tss_t key)
{
	return( TlsGetValue(key));
}


static inline int  mulle_thread_tss_set(mulle_thread_tss_t key,
	void *userdata)
{
	return( TlsSetValue(key, userdata) ? 0 : -1);
}

#endif
