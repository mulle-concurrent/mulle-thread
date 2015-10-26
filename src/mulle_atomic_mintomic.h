//
//  mulle_atomic_mintomic.h
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

#ifndef mulle_atomic_mintomic_h__
#define mulle_atomic_mintomic_h__

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include <mintomic/mintomic.h>

//
// just some stand-in code for later
// the mulle code just provides a subset of mintomic and exposes its own
// type, in case mintomic goes away
//

typedef mint_atomicPtr_t   mulle_atomic_ptr_t;

# pragma mark -
# pragma mark primitive code
static inline void   *__mulle_atomic_compare_and_swap_pointer( mulle_atomic_ptr_t *adress,
                                                              void *value,
                                                              void *expect)
{
   void    *result;
   
   result = mint_compare_exchange_strong_ptr_relaxed( adress, expect, value);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      char          *decor;
      
      decor = "";
      if( result != expect )
         decor = "FAILED to";
      fprintf( stderr, "%s: %sswap %p %p -> %p (%p)\n",
          pthread_name(), decor, adress, expect, value, result);
   }
#endif
   return( result);
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
   return( mint_fetch_add_ptr_relaxed( adress, 1));
}


static inline void  *_mulle_atomic_decrement_pointer( mulle_atomic_ptr_t *adress)
{
   return( mint_fetch_add_ptr_relaxed( adress, -1));
}


// returns the result, not the previous value like increment/decrement
static inline void  *_mulle_atomic_add_pointer( mulle_atomic_ptr_t *adress,
                                                intptr_t diff)
{
   return( (void *) ((intptr_t) mint_fetch_add_ptr_relaxed( adress, diff) + diff));
}


static inline void  *_mulle_atomic_read_pointer( mulle_atomic_ptr_t *adress)
{
   void   *result;
   
   result = mint_load_ptr_relaxed( adress);
#if MULLE_ATOMIC_TRACE
   {
      extern char   *pthread_name( void);
      
      fprintf( stderr, "%s: read %p -> %p\n", pthread_name(), adress, result);
   }
#endif
   return( result);
}

static inline void  _mulle_atomic_write_pointer( mulle_atomic_ptr_t *adress,
                                                 void *value)
{
   mint_store_ptr_relaxed( adress, value);
}


static inline void   mulle_atomic_memory_barrier( void)
{
   mint_thread_fence_seq_cst();
}

#endif
