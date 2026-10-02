//
//  intptr.c
//  test-atomic
//
//  Created by Nat! on 19.03.15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
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
#include <mulle-thread/mulle-thread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_ITERATIONS  100
#define LOOPS           1000
#define MAX_THREADS     4


static mulle_atomic_intptr_t    central;

#pragma mark - run test

static void    run_atomic_intptr_test( void)
{
   unsigned long   i;

   for( i = 0; i < LOOPS; i++)
   {
      _mulle_atomic_intptr_increment_relaxed( &central);
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   }
}


void  multi_threaded_test_each_thread()
{
   run_atomic_intptr_test();
}


static void   _wait_around( mulle_atomic_intptr_t *n_threads)
{
   // wait for all threads to materialize
   _mulle_atomic_intptr_decrement_relaxed( n_threads);
   while( _mulle_atomic_intptr_read_relaxed( n_threads) != 0)
      mulle_thread_yield();
}


struct thread_info
{
   char                     name[ 64];
   mulle_atomic_intptr_t   *n_threads;
};


static mulle_thread_rval_t MULLE_THREAD_CALL   run_test( struct thread_info *info)
{
   _wait_around( info->n_threads);
   multi_threaded_test_each_thread();

   return( 0);
}


void  multi_threaded_test( intptr_t n)
{
   int                     i;
   mulle_thread_t         *threads;
   struct thread_info     *info;
   mulle_atomic_intptr_t   n_threads;

   threads = alloca( n * sizeof( mulle_thread_t));
   assert( threads);

   _mulle_atomic_intptr_write_nonatomic( &n_threads, n);
   info = alloca( sizeof(struct thread_info) * n);

   _mulle_atomic_intptr_write_nonatomic( &central, 0);

   for( i = 1; i < n; i++)
   {
      info[ i].n_threads = &n_threads;
      sprintf( info[ i].name, "thread #%d", i);

      if( mulle_thread_create( (void *) run_test, &info[ i], &threads[ i]))
         abort();
   }

   info[ 0].n_threads = &n_threads;
   sprintf( info[ 0].name, "thread #%d", 0);
   run_test( &info[ 0]);

   for( i = 1; i < n; i++)
      if( mulle_thread_join( threads[ i]))
      {
         perror( "mulle_thread_join");
         abort();
      }

   assert( _mulle_atomic_intptr_read_nonatomic( &central) == n * LOOPS);
   assert( _mulle_atomic_intptr_read_relaxed( &n_threads) == 0);
}


int   main(int argc, const char * argv[])
{
   unsigned long  i;
   unsigned int   j;

   for( i = 1; i <= MAX_ITERATIONS; i++)
   {
      for( j = 1; j <= MAX_THREADS; j++)
      {
         multi_threaded_test( j);
      }
      fprintf( stdout, "%ld\n", i);
   }

   return( 0);
}
