//
//  mulle_thread_atomic.h
//  mulle-thread
//
//  Created by Nat! on 16.03.15.
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

#ifndef mulle_atomic_h__
#define mulle_atomic_h__

#include "include.h"


#ifndef HAVE_C11_STDATOMIC
# ifdef __clang__
#  if __has_include(<stdatomic.h>)
#    define HAVE_C11_STDATOMIC 1
#  endif
# else
#  if __STDC_VERSION__ >= 201112L && ! defined( __STDC_NO_ATOMICS__)
#   define HAVE_C11_STDATOMIC  1
#  endif
# endif
#endif

//
// http://stackoverflow.com/questions/11805636/how-do-i-check-by-using-stdc-version-if-is-std-c1x-in-use
// http://en.cppreference.com/w/c/atomic
//
#if HAVE_C11_STDATOMIC && ! defined( MULLE_THREAD_USE_MINTOMIC)
# if TRACE_INCLUDE
#  pragma message( "Using C11 for atomics")
# endif
# include "mulle-atomic-c11.h"
#else
# if TRACE_INCLUDE
#  pragma message( "Using mintomic for atomics")
# endif
# include "mulle-atomic-mintomic.h"
#endif


#pragma mark - additional API

MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE
void *
   _mulle_atomic_pointer_set( mulle_atomic_pointer_t *address,
                              void  *value)
{
   void   *expect;   // random contents don't matter
   void   *previous;

	expect = value; // contents don't matter, let compiler not get a heart attack
   for(;;)
   {
      previous = __mulle_atomic_pointer_weakcas( address, value, expect);
      if( previous == expect)
         return( previous);
      expect = previous;
   }
}


MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE mulle_functionpointer_t
   _mulle_atomic_functionpointer_set( mulle_atomic_functionpointer_t *address,
                                      mulle_functionpointer_t value)
{
   mulle_functionpointer_t   expect;
   mulle_functionpointer_t   previous;

	expect = value; // contents don't matter, let compiler not get a heart attack
   for(;;)
   {
      previous = __mulle_atomic_functionpointer_weakcas( address, value, expect);
      if( previous == expect)
         return( previous);
      expect = previous;
   }
}


#pragma mark - old API

MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE
int
   _mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address,
                                           void *value,
                                           void *expect)
{
   return( _mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE
void   *
   __mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address,
                                            void *value,
                                            void *expect)
{
   return( __mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE
mulle_functionpointer_t
   __mulle_atomic_functionpointer_compare_and_swap( mulle_atomic_functionpointer_t *address,
                                                    mulle_functionpointer_t value,
                                                    mulle_functionpointer_t expect)
{
   return( __mulle_atomic_functionpointer_cas( address, value, expect));
}


MULLE_C_NO_INSTRUMENT_FUNCTION
MULLE_C_STATIC_ALWAYS_INLINE
int
   _mulle_atomic_functionpointer_compare_and_swap( mulle_atomic_functionpointer_t *address,
                                                   mulle_functionpointer_t value,
                                                   mulle_functionpointer_t expect)
{
   return( _mulle_atomic_functionpointer_cas( address, value, expect));
}


#endif
