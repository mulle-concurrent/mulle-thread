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

#ifndef mulle_atomic_c11_h
#define mulle_atomic_c11_h

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdatomic.h>

//
// this is due to the mintomic heritage
//
typedef struct { _Atomic( void *) _nonatomic; }   mulle_atomic_ptr_t;


# pragma mark -
# pragma mark primitive code
static inline void   *__mulle_atomic_compare_and_swap_pointer( mulle_atomic_ptr_t *adress,
                                                               void *value,
                                                               void *expect)
{
   _Bool   result; // that hated bool type
   
   result = atomic_compare_exchange_weak_explicit( &adress->_nonatomic, &expect, value, memory_order_relaxed, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
      
      decor = "";
      if( param != expect)
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n", pthread_name(), decor, adress, expect, value, param);
   }
#endif
   return( expect);
}


static inline int   _mulle_atomic_compare_and_swap_pointer( mulle_atomic_ptr_t *adress,
                                                            void *value,
                                                            void *expect)
{
   void  *result;
   
   result = __mulle_atomic_compare_and_swap_pointer( adress, value, expect);
   return( result == expect);
}


static inline void   *_mulle_atomic_increment_pointer( mulle_atomic_ptr_t *adress)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) &adress->_nonatomic, 1, memory_order_relaxed));
}


static inline void  *_mulle_atomic_decrement_pointer( mulle_atomic_ptr_t *adress)
{
   return( (void *) atomic_fetch_add_explicit( (atomic_intptr_t *) &adress->_nonatomic, -1, memory_order_relaxed));
}


// returns the result, not the previous value like increment/decrement
static inline void  *_mulle_atomic_add_pointer( mulle_atomic_ptr_t *adress, intptr_t diff)
{
   return( (void *) ((intptr_t) atomic_fetch_add_explicit( (atomic_intptr_t *) &adress->_nonatomic, diff, memory_order_relaxed) + diff));
}


static inline void  *_mulle_atomic_read_pointer( mulle_atomic_ptr_t *adress)
{
   void   *result;
   
   result = atomic_load_explicit( &adress->_nonatomic, memory_order_relaxed);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      
      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), adress, result);
   }
#endif
   return( result);
}

          
static inline void  _mulle_atomic_write_pointer( mulle_atomic_ptr_t *adress, void *value)
{
   atomic_store_explicit( &adress->_nonatomic, value, memory_order_relaxed);
}


static inline void   mulle_atomic_memory_barrier( void)
{
   atomic_signal_fence( memory_order_seq_cst);
}

#endif /* mulle_atomic_c11_h */
