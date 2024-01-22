//
//  mulle_atomic_c11.h
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

#ifndef mulle_atomic_c11_h__
#define mulle_atomic_c11_h__

#include "include.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>


//
// this is due to the mintomic heritage
//
typedef void   (*mulle_functionpointer_t)( void);

typedef _Atomic( void *)                    mulle_atomic_pointer_t;
typedef _Atomic( mulle_functionpointer_t)   mulle_atomic_functionpointer_t;

# pragma mark -
# pragma mark function pointer set and get

MULLE_C_ALWAYS_INLINE static inline mulle_functionpointer_t
   _mulle_atomic_functionpointer_nonatomic_read( mulle_atomic_functionpointer_t *p)
{
   return( *(mulle_functionpointer_t *) p);
}


MULLE_C_ALWAYS_INLINE static inline void
   _mulle_atomic_functionpointer_nonatomic_write( mulle_atomic_functionpointer_t *p,
   	                                            mulle_functionpointer_t value)
{
   *(mulle_functionpointer_t *) p = value;
}


MULLE_C_ALWAYS_INLINE static inline mulle_functionpointer_t
   _mulle_atomic_functionpointer_read( mulle_atomic_functionpointer_t *address)
{
   mulle_functionpointer_t   result;

   result = atomic_load_explicit( address, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), address, result);
   }
#endif
   return( result);
}


MULLE_C_ALWAYS_INLINE static inline void
   _mulle_atomic_functionpointer_write( mulle_atomic_functionpointer_t *address,
                                        mulle_functionpointer_t value)
{
   atomic_store_explicit( address, value, memory_order_relaxed);
}


# pragma mark -
# pragma mark primitive code

MULLE_C_ALWAYS_INLINE static inline mulle_functionpointer_t
   __mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address,
                                           mulle_functionpointer_t value,
                                           mulle_functionpointer_t expect)
{
   mulle_functionpointer_t   actual;

   actual = expect;
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
     _Bool          result;   // that hated bool type

      result = atomic_compare_exchange_weak_explicit( address,
                                                      &actual,
                                                      value,
                                                      memory_order_relaxed,
                                                      memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   atomic_compare_exchange_weak_explicit( address,
                                          &actual,
                                          value,
                                          memory_order_relaxed,
                                          memory_order_relaxed);
#endif
   return( actual);
}


MULLE_C_ALWAYS_INLINE static inline int
   _mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address,
                                          mulle_functionpointer_t value,
                                          mulle_functionpointer_t expect)
{
   mulle_functionpointer_t    actual;
   int                        result;

   assert( value != expect);

   actual = expect;

#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;

      result = (int) atomic_compare_exchange_weak_explicit( address,
                                                            &actual,
                                                            value,
                                                            memory_order_relaxed,
                                                            memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   result = atomic_compare_exchange_weak_explicit( address,
                                                   &actual,
                                                   value,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed);
#endif
   return( result);
}


MULLE_C_ALWAYS_INLINE static inline mulle_functionpointer_t
   __mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address,
                                       mulle_functionpointer_t value,
                                       mulle_functionpointer_t expect)
{
   mulle_functionpointer_t   actual;

   actual = expect;
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
     _Bool          result;   // that hated bool type

      result = atomic_compare_exchange_strong_explicit( address,
                                                        &actual,
                                                        value,
                                                        memory_order_relaxed,
                                                        memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   atomic_compare_exchange_strong_explicit( address,
                                            &actual,
                                            value,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
#endif
   return( actual);
}


MULLE_C_ALWAYS_INLINE static inline int
   _mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address,
                                      mulle_functionpointer_t value,
                                      mulle_functionpointer_t expect)
{
   mulle_functionpointer_t    actual;
   int                        result;

   assert( value != expect);

   actual = expect;

#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;

      result = (int) atomic_compare_exchange_strong_explicit( address,
                                                              &actual,
                                                              value,
                                                              memory_order_relaxed,
                                                              memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   result = atomic_compare_exchange_strong_explicit( address,
                                                     &actual,
                                                     value,
                                                     memory_order_relaxed,
                                                     memory_order_relaxed);
#endif
   return( result);
}


# pragma mark -
# pragma mark pointer set and get

MULLE_C_ALWAYS_INLINE static inline void *
   _mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *p)
{
   return( *(void **) p);
}


MULLE_C_ALWAYS_INLINE static inline void
   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *p, void *value)
{
   *(void **) p = value;
}


MULLE_C_ALWAYS_INLINE static inline void  *
   _mulle_atomic_pointer_read( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = atomic_load_explicit( address, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), address, result);
   }
#endif

   return( result);
}


MULLE_C_ALWAYS_INLINE static inline void
   _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address, void *value)
{
   atomic_store_explicit( address, value, memory_order_relaxed);
}


# pragma mark -
# pragma mark primitive code

MULLE_C_ALWAYS_INLINE static inline void   *
   __mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                   void *value,
                                   void *expect)
{
   void    *actual;

   actual = expect;
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
      int           result;

      result = (int) atomic_compare_exchange_weak_explicit( address,
                                                            &actual,
                                                            value,
                                                            memory_order_relaxed,
                                                            memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   atomic_compare_exchange_weak_explicit( address,
                                          &actual,
                                          value,
                                          memory_order_relaxed,
                                          memory_order_relaxed);
#endif

   return( actual);
}


MULLE_C_ALWAYS_INLINE static inline int
   _mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                  void *value,
                                  void *expect)
{
   void    *actual;
   int     result;

   assert( value != expect);

   actual = expect;

#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;

      result = (int) atomic_compare_exchange_weak_explicit( address,
                                                            &actual,
                                                            value,
                                                            memory_order_relaxed,
                                                            memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   result = atomic_compare_exchange_weak_explicit( address,
                                                   &actual,
                                                   value,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed);
#endif

   return( result);
}


// this returns actual
MULLE_C_ALWAYS_INLINE static inline void  *
   __mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                               void *value,
                               void *expect)
{
   void    *actual;

   actual = expect;
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
      int           result;

      result = (int) atomic_compare_exchange_strong_explicit( address,
                                                              &actual,
                                                              value,
                                                              memory_order_relaxed,
                                                              memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   atomic_compare_exchange_strong_explicit( address,
                                            &actual,
                                            value,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
#endif

   return( actual);
}


// this returns a flag if the operation was successful
MULLE_C_ALWAYS_INLINE static inline int
   _mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                              void *value,
                              void *expect)
{
   void    *actual;
   int     result;

   assert( value != expect);

   actual = expect;

#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;

      result = (int) atomic_compare_exchange_strong_explicit( address,
                                                              &actual,
                                                              value,
                                                              memory_order_relaxed,
                                                              memory_order_relaxed);
      decor = "";
      if( ! result)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   result = atomic_compare_exchange_strong_explicit( address,
                                                     &actual,
                                                     value,
                                                     memory_order_relaxed,
                                                     memory_order_relaxed);
#endif

   return( result);
}


MULLE_C_ALWAYS_INLINE static inline void *
   _mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               1,
                                               memory_order_relaxed));
}


MULLE_C_ALWAYS_INLINE static inline void  *
   _mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               -1,
                                               memory_order_relaxed));
}


// returns the result, not the previous value like increment/decrement
MULLE_C_ALWAYS_INLINE static inline void  *
   _mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, intptr_t diff)
{
   return( (void *) ((intptr_t) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                                            diff,
                                                            memory_order_relaxed) + diff));
}


MULLE_C_ALWAYS_INLINE static inline void
   mulle_atomic_memory_barrier( void)
{
   atomic_signal_fence( memory_order_seq_cst);
}


#endif /* mulle_atomic_c11_h */
