//
//  mulle-thread.c
//  mulle-thread
//
//  Copyright (c) 2026 Mulle kybernetiK. All rights reserved.
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
#include "mulle-thread.h"


// the old, not so useful interface
MULLE_C_NO_INSTRUMENT_FUNCTION
void   mulle_thread_once( mulle_thread_once_t  *once,
                          void (*init)( void))
{
   void   *actual;

   actual = __mulle_atomic_pointer_compare_and_swap( once,
                                                    (void *) MULLE_THREAD_ONCE_BUSY,
                                                    (void *) MULLE_THREAD_ONCE_INIT);
   if( actual == (void *) MULLE_THREAD_ONCE_DONE)
      return;
   if( actual != (void *) MULLE_THREAD_ONCE_INIT)
   {
      do
         mulle_thread_yield();
      while( _mulle_atomic_pointer_read( once) != (void *) MULLE_THREAD_ONCE_DONE);
      mulle_atomic_memory_barrier();
      return;
   }
   (*init)();
   mulle_atomic_memory_barrier();
   _mulle_atomic_pointer_compare_and_swap( once,
                                           (void *) MULLE_THREAD_ONCE_DONE,
                                           (void *) MULLE_THREAD_ONCE_BUSY);
}


// the new, more useful interface
MULLE_C_NO_INSTRUMENT_FUNCTION
void   mulle_thread_once_call( mulle_thread_once_t  *once,
                               void (*init)( void *),
                               void *userinfo)
{
   void   *actual;

   actual = __mulle_atomic_pointer_compare_and_swap( once,
                                                    (void *) MULLE_THREAD_ONCE_BUSY,
                                                    (void *) MULLE_THREAD_ONCE_INIT);
   if( actual == (void *) MULLE_THREAD_ONCE_DONE)
      return;
   if( actual != (void *) MULLE_THREAD_ONCE_INIT)
   {
      do
         mulle_thread_yield();
      while( _mulle_atomic_pointer_read( once) != (void *) MULLE_THREAD_ONCE_DONE);
      // fear of cached neighbor memory area
      mulle_atomic_memory_barrier();
      return;
   }
   (*init)( userinfo);
   // fear of delayed write before swap
   mulle_atomic_memory_barrier();
   _mulle_atomic_pointer_compare_and_swap( once,
                                           (void *) MULLE_THREAD_ONCE_DONE,
                                           (void *) MULLE_THREAD_ONCE_BUSY);
}


// recursive variant: same thread can call again without deadlock
MULLE_C_NO_INSTRUMENT_FUNCTION
void   mulle_thread_once_call_recursive( mulle_thread_once_recursive_t  *once,
                                         void (*init)( void *),
                                         void *userinfo)
{
   void               *actual;
   mulle_thread_id_t  this_thread_id;
   mulle_thread_id_t  thread_id;

   this_thread_id = mulle_thread_id();
   thread_id      = (mulle_thread_id_t) _mulle_atomic_pointer_read( &once->_thread_id);

   // If same thread owns it, return immediately (recursive call)
   if( thread_id == this_thread_id)
      return;

   // Try to acquire
   actual = __mulle_atomic_pointer_compare_and_swap( &once->_state,
                                                    (void *) MULLE_THREAD_ONCE_BUSY,
                                                    (void *) MULLE_THREAD_ONCE_INIT);
   if( actual == (void *) MULLE_THREAD_ONCE_DONE)
      return;

   // Different thread is busy, wait
   if( actual != (void *) MULLE_THREAD_ONCE_INIT)
   {
      do
         mulle_thread_yield();
      while( _mulle_atomic_pointer_read( &once->_state) != (void *) MULLE_THREAD_ONCE_DONE);
      mulle_atomic_memory_barrier();
      return;
   }

   // We acquired it, store our thread ID
   _mulle_atomic_pointer_write( &once->_thread_id, (void *) this_thread_id);

   (*init)( userinfo);

   // Clear thread ID and mark done
   _mulle_atomic_pointer_write( &once->_thread_id, NULL);
   mulle_atomic_memory_barrier();
   _mulle_atomic_pointer_compare_and_swap( &once->_state,
                                           (void *) MULLE_THREAD_ONCE_DONE,
                                           (void *) MULLE_THREAD_ONCE_BUSY);
}


// we include mintomic, if we need it
//
// if we are building with mulle-core this file is in "src/mulle-thread" and
// "mintomic_gcc.c" is in "mintomic/mintomic_gcc.c" -> "../../mintomic"
//
// if we are building alone, this file is in "src" and "mintomic_gcc.c" is
// in "mintomic/src/mintomic/mintomic_gcc.c" "../mintomic/src/mintomic"
//
#if __STDC_VERSION__ < 201112L || defined( __STDC_NO_ATOMICS__)
// variable defined by Executable.cmake
# ifdef MULLE__CORE_BUILD
#  include "../../mintomic/mintomic_gcc.c"
# else
#  include "../mintomic/src/mintomic/mintomic_gcc.c"
# endif
#endif
