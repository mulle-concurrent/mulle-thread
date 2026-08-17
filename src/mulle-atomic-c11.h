//
//  mulle-atomic-c11.h
//  mulle-thread
//
//  Copyright (c) 2018 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
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

// new
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_nonatomic( mulle_atomic_functionpointer_t *p)
{
   return( *(mulle_functionpointer_t *) p);
}


// old
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_nonatomic_read( mulle_atomic_functionpointer_t *p)
{
   return( *(mulle_functionpointer_t *) p);
}


// new
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write_nonatomic( mulle_atomic_functionpointer_t *p,
                                                  mulle_functionpointer_t value)
{
   *(mulle_functionpointer_t *) p = value;
}


// old
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_nonatomic_write( mulle_atomic_functionpointer_t *p,
   	                                            mulle_functionpointer_t value)
{
   *(mulle_functionpointer_t *) p = value;
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read( mulle_atomic_functionpointer_t *address)
{
   mulle_functionpointer_t   result;

   result = atomic_load_explicit( address, memory_order_seq_cst);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), address, result);
   }
#endif
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_acquire( mulle_atomic_functionpointer_t *address)
{
   mulle_functionpointer_t   result;

   result = atomic_load_explicit( address, memory_order_acquire);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read_acquire %p -> %p\n", pthread_name(), address, result);
   }
#endif
   return( result);
}


//
// Use for disconnected values only: diagnostic counters, statistics,
// flags that never gate access to other memory. Do NOT use when
// the value read or written controls access to a shared data structure
// (e.g. array lengths, queue indices, publication flags).
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_relaxed( mulle_atomic_functionpointer_t *address)
{
   mulle_functionpointer_t   result;

   result = atomic_load_explicit( address, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read_relaxed %p -> %p\n", pthread_name(), address, result);
   }
#endif
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write( mulle_atomic_functionpointer_t *address,
                                        mulle_functionpointer_t value)
{
   atomic_store_explicit( address, value, memory_order_seq_cst);
}


//
// Use for disconnected values only: diagnostic counters, statistics,
// flags that never gate access to other memory. Do NOT use when
// the value read or written controls access to a shared data structure
// (e.g. array lengths, queue indices, publication flags).
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write_relaxed( mulle_atomic_functionpointer_t *address,
                                                mulle_functionpointer_t value)
{
   atomic_store_explicit( address, value, memory_order_relaxed);
}


# pragma mark -
# pragma mark primitive code

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
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
                                                      memory_order_seq_cst,
                                                      memory_order_seq_cst);
      decor = "";
      if( ! result)
      {
         decor  = "FAILED to";
      }
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
         pthread_name(), decor, address, expect, value, actual);
   }
#else
   atomic_compare_exchange_weak_explicit( address,
                                          &actual,
                                          value,
                                          memory_order_seq_cst,
                                          memory_order_seq_cst);
#endif
   // https://stackoverflow.com/questions/20179315/why-does-stdatomic-compare-exchange-update-the-expected-value
   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_weakcas_relaxed( mulle_atomic_functionpointer_t *address,
                                                   mulle_functionpointer_t value,
                                                   mulle_functionpointer_t expect)
{
   mulle_functionpointer_t   actual;

   actual = expect;
   atomic_compare_exchange_weak_explicit( address,
                                          &actual,
                                          value,
                                          memory_order_relaxed,
                                          memory_order_relaxed);
   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
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
                                                            memory_order_seq_cst,
                                                            memory_order_seq_cst);
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
                                                   memory_order_seq_cst,
                                                   memory_order_seq_cst);
#endif
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_weakcas_relaxed( mulle_atomic_functionpointer_t *address,
                                                  mulle_functionpointer_t value,
                                                  mulle_functionpointer_t expect)
{
   mulle_functionpointer_t    actual;

   assert( value != expect);
   actual = expect;
   return( (int) atomic_compare_exchange_weak_explicit( address,
                                                        &actual,
                                                        value,
                                                        memory_order_relaxed,
                                                        memory_order_relaxed));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
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
                                                        memory_order_seq_cst,
                                                        memory_order_seq_cst);
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
                                            memory_order_seq_cst,
                                            memory_order_seq_cst);
#endif
   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_cas_relaxed( mulle_atomic_functionpointer_t *address,
                                               mulle_functionpointer_t value,
                                               mulle_functionpointer_t expect)
{
   mulle_functionpointer_t   actual;

   actual = expect;
   atomic_compare_exchange_strong_explicit( address,
                                            &actual,
                                            value,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
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
                                                              memory_order_seq_cst,
                                                              memory_order_seq_cst);
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
                                                     memory_order_seq_cst,
                                                     memory_order_seq_cst);
#endif
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_cas_relaxed( mulle_atomic_functionpointer_t *address,
                                              mulle_functionpointer_t value,
                                              mulle_functionpointer_t expect)
{
   mulle_functionpointer_t    actual;

   assert( value != expect);
   actual = expect;
   return( (int) atomic_compare_exchange_strong_explicit( address,
                                                          &actual,
                                                          value,
                                                          memory_order_relaxed,
                                                          memory_order_relaxed));
}


# pragma mark -
# pragma mark pointer set and get

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void *
   _mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *p)
{
   return( *(void **) p);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void *
   _mulle_atomic_pointer_read_nonatomic( mulle_atomic_pointer_t *p)
{
   return( *(void **) p);
}



MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_write_nonatomic( mulle_atomic_pointer_t *p, void *value)
{
   *(void **) p = value;
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *p, void *value)
{
   *(void **) p = value;
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_read( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = atomic_load_explicit( address, memory_order_seq_cst);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), address, result);
   }
#endif

   return( result);
}


//
// Use when the loaded value gates access to other shared memory that was
// published by the writing thread with a release-store. Synchronizes-with
// the release, so all writes before the release are visible after this load.
// Same cost as a plain load on x86; one `ldar` on ARM64.
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_read_acquire( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = atomic_load_explicit( address, memory_order_acquire);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read_acquire %p -> %p\n", pthread_name(), address, result);
   }
#endif

   return( result);
}


//
// Use for disconnected values only: diagnostic counters, statistics,
// flags that never gate access to other memory. Do NOT use when
// the value read or written controls access to a shared data structure
// (e.g. array lengths, queue indices, publication flags).
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_read_relaxed( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = atomic_load_explicit( address, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);

      fprintf( stderr, "%s: read_relaxed %p -> %p\n", pthread_name(), address, result);
   }
#endif

   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address, void *value)
{
   atomic_store_explicit( address, value, memory_order_seq_cst);
}


//
// Use for disconnected values only: diagnostic counters, statistics,
// flags that never gate access to other memory. Do NOT use when
// the value read or written controls access to a shared data structure
// (e.g. array lengths, queue indices, publication flags).
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_write_relaxed( mulle_atomic_pointer_t *address, void *value)
{
   atomic_store_explicit( address, value, memory_order_relaxed);
}


# pragma mark -
# pragma mark primitive code

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas_weak( mulle_atomic_pointer_t *address,
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
                                                            memory_order_seq_cst,
                                                            memory_order_seq_cst);
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
                                          memory_order_seq_cst,
                                          memory_order_seq_cst);
#endif

   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas_weak_relaxed( mulle_atomic_pointer_t *address,
                                            void *value,
                                            void *expect)
{
   void    *actual;

   actual = expect;
   atomic_compare_exchange_weak_explicit( address,
                                          &actual,
                                          value,
                                          memory_order_relaxed,
                                          memory_order_relaxed);
   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                   void *value,
                                   void *expect)
{
   return( __mulle_atomic_pointer_cas_weak( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address,
                                           void *value,
                                           void *expect)
{
   return( __mulle_atomic_pointer_cas_weak_relaxed( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_weak( mulle_atomic_pointer_t *address,
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
                                                            memory_order_seq_cst,
                                                            memory_order_seq_cst);
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
                                                   memory_order_seq_cst,
                                                   memory_order_seq_cst);
#endif

   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_weak_relaxed( mulle_atomic_pointer_t *address,
                                           void *value,
                                           void *expect)
{
   void    *actual;

   assert( value != expect);
   actual = expect;
   return( (int) atomic_compare_exchange_weak_explicit( address,
                                                        &actual,
                                                        value,
                                                        memory_order_relaxed,
                                                        memory_order_relaxed));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                  void *value,
                                  void *expect)
{
   return( _mulle_atomic_pointer_cas_weak( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address,
                                          void *value,
                                          void *expect)
{
   return( _mulle_atomic_pointer_cas_weak_relaxed( address, value, expect));
}



// this returns actual
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
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
                                                              memory_order_seq_cst,
                                                              memory_order_seq_cst);
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
                                            memory_order_seq_cst,
                                            memory_order_seq_cst);
#endif

   return( actual);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   __mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address,
                                       void *value,
                                       void *expect)
{
   void    *actual;

   actual = expect;
   atomic_compare_exchange_strong_explicit( address,
                                            &actual,
                                            value,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
   return( actual);
}


// this returns a flag if the operation was successful
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
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
                                                              memory_order_seq_cst,
                                                              memory_order_seq_cst);
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
                                                     memory_order_seq_cst,
                                                     memory_order_seq_cst);
#endif

   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address,
                                      void *value,
                                      void *expect)
{
   void    *actual;

   assert( value != expect);
   actual = expect;
   return( (int) atomic_compare_exchange_strong_explicit( address,
                                                          &actual,
                                                          value,
                                                          memory_order_relaxed,
                                                          memory_order_relaxed));
}


//
// The arithmetic helpers below access a _Atomic( void *) as an
// _Atomic( intptr_t). This is a deliberate type-pun, inherited from the
// mintomic API: C11 has no atomic pointer arithmetic, so the pointer-sized
// atomic is reinterpreted as a pointer-sized integer atomic. Both types have
// identical size, alignment and lock-free status on all supported
// platforms, so this is only sound while the values stored in the atomic
// are plain pointer-sized integers (as mulle-thread's once states, TSS
// refcounts etc. guarantee). Do not store actual pointers in an atomic that
// is later passed to these functions.
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void *
   _mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               1,
                                               memory_order_seq_cst));
}


//
// Use for disconnected counters only: statistics, diagnostics, values
// that never gate access to other memory.
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void *
   _mulle_atomic_pointer_increment_relaxed( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               1,
                                               memory_order_relaxed));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               -1,
                                               memory_order_seq_cst));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_decrement_relaxed( mulle_atomic_pointer_t *address)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                               -1,
                                               memory_order_relaxed));
}


// returns the result, not the previous value like increment/decrement
// (see the comment on _mulle_atomic_pointer_increment about the cast)
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, intptr_t diff)
{
   return( (void *) ((intptr_t) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                                            diff,
                                                            memory_order_seq_cst) + diff));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_add_relaxed( mulle_atomic_pointer_t *address, intptr_t diff)
{
   return( (void *) ((intptr_t) atomic_fetch_add_explicit( (atomic_intptr_t *) address,
                                                            diff,
                                                            memory_order_relaxed) + diff));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   mulle_atomic_memory_barrier( void)
{
   //
   // MEMO: this must be a *thread* fence, not a signal fence.
   //
   // atomic_signal_fence() only orders operations between a thread and a
   // signal handler executing in the same thread. It is purely a compiler
   // barrier and provides no ordering between threads.
   //
   // mulle_thread_once()/mulle_thread_once_call() and the recursive variants
   // rely on this barrier to publish the data written by the init function
   // before other threads may observe the state as MULLE_THREAD_ONCE_DONE.
   // With a signal fence and memory_order_relaxed atomics, that guarantee
   // only holds on strongly ordered CPUs (x86/TSO); on ARM/ARM64/PPC another
   // thread could observe DONE and still read stale init data.
   //
   // atomic_thread_fence( memory_order_seq_cst) is the correct choice: it
   // keeps all compiler ordering of the signal fence and adds the hardware
   // fence on weakly ordered CPUs (a no-op on x86). It also matches the
   // mintomic backend (mint_thread_fence_seq_cst), so both atomic backends
   // behave identically.
   //
   atomic_thread_fence( memory_order_seq_cst);
}


#endif /* mulle_atomic_c11_h */
