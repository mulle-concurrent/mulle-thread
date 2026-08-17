//
//  mulle-atomic-mintomic.h
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
#ifndef mulle_atomic_mintomic_h__
#define mulle_atomic_mintomic_h__

#include "include.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <mintomic/mintomic.h>

//
// just some stand-in code for later
// the mulle code just provides a subset of mintomic and exposes its own
// type, in case mintomic goes away
//

typedef mint_atomicPtr_t   mulle_atomic_pointer_t;

// wrong, but it just for a couple more years...
typedef mint_atomicPtr_t   mulle_atomic_functionpointer_t;
typedef void   (*mulle_functionpointer_t)();


#pragma mark - value pointers

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   _mulle_atomic_pointer_read_nonatomic( mulle_atomic_pointer_t *address)
{
   return( address->_nonatomic);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   _mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *address)
{
   return( address->_nonatomic);
}

// old
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *address, void *value)
{
   address->_nonatomic = value;
}

// new
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_pointer_write_nonatomic( mulle_atomic_pointer_t *address, void *value)
{
   address->_nonatomic = value;
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_read( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = mint_load_ptr_relaxed( address);
   mint_thread_fence_seq_cst();
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
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_read_acquire( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = mint_load_ptr_relaxed( address);
   mint_thread_fence_acquire();
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

   result = mint_load_ptr_relaxed( address);
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
   _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address,
                              void *value)
{
   mint_thread_fence_seq_cst();
   mint_store_ptr_relaxed( address, value);
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
   _mulle_atomic_pointer_write_relaxed( mulle_atomic_pointer_t *address,
                                        void *value)
{
   mint_store_ptr_relaxed( address, value);
}



MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                               void *value,
                               void *expect)
{
   void    *result;

   result = mint_compare_exchange_strong_ptr_relaxed( address, expect, value);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;

      decor = "";
      if( result != expect )
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
              pthread_name(), decor, address, expect, value, result);
   }
#endif
   return( result);
}


// mintomic CAS is always seq_cst on supported platforms; the relaxed
// variant is identical but named for API consistency
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address,
                                       void *value,
                                       void *expect)
{
   return( mint_compare_exchange_strong_ptr_relaxed( address, expect, value));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas_weak( mulle_atomic_pointer_t *address,
                                   void *value,
                                   void *expect)
{
   return( __mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_cas_weak_relaxed( mulle_atomic_pointer_t *address,
                                            void *value,
                                            void *expect)
{
   return( __mulle_atomic_pointer_cas_relaxed( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                   void *value,
                                   void *expect)
{
   return( __mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   __mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address,
                                           void *value,
                                           void *expect)
{
   return( __mulle_atomic_pointer_cas_relaxed( address, value, expect));
}



MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                              void *value,
                              void *expect)
{
   void  *result;

   result = __mulle_atomic_pointer_cas( address, value, expect);
   return( result == expect);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_relaxed( mulle_atomic_pointer_t *address,
                                      void *value,
                                      void *expect)
{
   void  *result;

   result = __mulle_atomic_pointer_cas_relaxed( address, value, expect);
   return( result == expect);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_weak( mulle_atomic_pointer_t *address,
                                   void *value,
                                   void *expect)
{
   return( _mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_cas_weak_relaxed( mulle_atomic_pointer_t *address,
                                           void *value,
                                           void *expect)
{
   return( _mulle_atomic_pointer_cas_relaxed( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_weakcas( mulle_atomic_pointer_t *address,
                                  void *value,
                                  void *expect)
{
   return( _mulle_atomic_pointer_cas( address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_pointer_weakcas_relaxed( mulle_atomic_pointer_t *address,
                                          void *value,
                                          void *expect)
{
   return( _mulle_atomic_pointer_cas_relaxed( address, value, expect));
}



#pragma mark - function pointers

// new
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_nonatomic( mulle_atomic_functionpointer_t *address)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) _mulle_atomic_pointer_nonatomic_read( (mulle_atomic_pointer_t *) address));
}

// old
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_nonatomic_read( mulle_atomic_functionpointer_t *address)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) _mulle_atomic_pointer_nonatomic_read( (mulle_atomic_pointer_t *) address));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write_nonatomic( mulle_atomic_functionpointer_t *address,
                                                  mulle_functionpointer_t value)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   _mulle_atomic_pointer_nonatomic_write( (mulle_atomic_pointer_t *) address, (void *) value);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_nonatomic_write( mulle_atomic_functionpointer_t *address,
                                                  mulle_functionpointer_t value)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   _mulle_atomic_pointer_nonatomic_write( (mulle_atomic_pointer_t *) address, (void *) value);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read( mulle_atomic_functionpointer_t *address)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) _mulle_atomic_pointer_read( (mulle_atomic_pointer_t *) address));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_acquire( mulle_atomic_functionpointer_t *address)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) _mulle_atomic_pointer_read_acquire( (mulle_atomic_pointer_t *) address));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   _mulle_atomic_functionpointer_read_relaxed( mulle_atomic_functionpointer_t *address)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) _mulle_atomic_pointer_read_relaxed( (mulle_atomic_pointer_t *) address));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write( mulle_atomic_functionpointer_t *address,
                                        mulle_functionpointer_t value)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   _mulle_atomic_pointer_write( (mulle_atomic_pointer_t *) address, (void *) value);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   _mulle_atomic_functionpointer_write_relaxed( mulle_atomic_functionpointer_t *address,
                                                mulle_functionpointer_t value)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   _mulle_atomic_pointer_write_relaxed( (mulle_atomic_pointer_t *) address, (void *) value);
}



MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address,
                                       mulle_functionpointer_t value,
                                       mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_cas( (mulle_atomic_pointer_t *) address,
                                                                 (void *) value,
                                                                 (void *) expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_cas_relaxed( mulle_atomic_functionpointer_t *address,
                                               mulle_functionpointer_t value,
                                               mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_cas_relaxed( (mulle_atomic_pointer_t *) address,
                                                                         (void *) value,
                                                                         (void *) expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_cas( mulle_atomic_functionpointer_t *address,
                                      mulle_functionpointer_t value,
                                      mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_cas( (mulle_atomic_pointer_t *) address,
                                      (void *) value,
                                      (void *) expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_cas_relaxed( mulle_atomic_functionpointer_t *address,
                                              mulle_functionpointer_t value,
                                              mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_cas_relaxed( (mulle_atomic_pointer_t *) address,
                                              (void *) value,
                                              (void *) expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_cas_weak( mulle_atomic_functionpointer_t *address,
                                           mulle_functionpointer_t value,
                                           mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_weakcas( (mulle_atomic_pointer_t *) address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_cas_weak_relaxed( mulle_atomic_functionpointer_t *address,
                                                   mulle_functionpointer_t value,
                                                   mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_weakcas_relaxed( (mulle_atomic_pointer_t *) address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address,
                                          mulle_functionpointer_t value,
                                          mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_weakcas( (mulle_atomic_pointer_t *) address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
int
   _mulle_atomic_functionpointer_weakcas_relaxed( mulle_atomic_functionpointer_t *address,
                                                  mulle_functionpointer_t value,
                                                  mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( _mulle_atomic_pointer_weakcas_relaxed( (mulle_atomic_pointer_t *) address, value, expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
	__mulle_atomic_functionpointer_cas_weak( mulle_atomic_functionpointer_t *address,
                                            mulle_functionpointer_t value,
                                            mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_weakcas( (mulle_atomic_pointer_t *) address,
   																						value,
   																						expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
	__mulle_atomic_functionpointer_cas_weak_relaxed( mulle_atomic_functionpointer_t *address,
                                                    mulle_functionpointer_t value,
                                                    mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_weakcas_relaxed( (mulle_atomic_pointer_t *) address,
   																						value,
   																						expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_weakcas( mulle_atomic_functionpointer_t *address,
                                           mulle_functionpointer_t value,
                                           mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_weakcas( (mulle_atomic_pointer_t *) address,
                                                                     value,
                                                                     expect));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
mulle_functionpointer_t
   __mulle_atomic_functionpointer_weakcas_relaxed( mulle_atomic_functionpointer_t *address,
                                                   mulle_functionpointer_t value,
                                                   mulle_functionpointer_t expect)
{
   MULLE_C_ASSERT( sizeof( void *) == sizeof( mulle_functionpointer_t));

   return( (mulle_functionpointer_t) __mulle_atomic_pointer_weakcas_relaxed( (mulle_atomic_pointer_t *) address,
                                                                             value,
                                                                             expect));
}


#pragma mark - atomic arithmetic

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   _mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = mint_fetch_add_ptr_relaxed( address, 1);
   mint_thread_fence_seq_cst();
   return( result);
}


//
// Use for disconnected counters only: statistics, diagnostics, values
// that never gate access to other memory.
//
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void   *
   _mulle_atomic_pointer_increment_relaxed( mulle_atomic_pointer_t *address)
{
   return( mint_fetch_add_ptr_relaxed( address, 1));
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address)
{
   void   *result;

   result = mint_fetch_add_ptr_relaxed( address, -1);
   mint_thread_fence_seq_cst();
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_decrement_relaxed( mulle_atomic_pointer_t *address)
{
   return( mint_fetch_add_ptr_relaxed( address, -1));
}


// returns the result, not the previous value like increment/decrement
MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, intptr_t diff)
{
   void   *result;

   result = (void *) ((intptr_t) mint_fetch_add_ptr_relaxed( address, diff) + diff);
   mint_thread_fence_seq_cst();
   return( result);
}


MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void  *
   _mulle_atomic_pointer_add_relaxed( mulle_atomic_pointer_t *address, intptr_t diff)
{
   return( (void *) ((intptr_t) mint_fetch_add_ptr_relaxed( address, diff) + diff));
}


#pragma mark - barrier

MULLE_C_STATIC_ALWAYS_INLINE
MULLE_C_NO_INSTRUMENT_FUNCTION
void
   mulle_atomic_memory_barrier( void)
{
   mint_thread_fence_seq_cst();
}

#endif
