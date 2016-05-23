//
//  main.c
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
#include "mulle_atomic.h"
#include "mulle_thread.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRESS     0
#define FOREVER      1
#define LOOPS        10000000
#define MAX_THREADS  4


#if DEBUG_PRINT
extern void   mulle_aba_print( void);
#else
# define mulle_aba_print()
#endif

static mulle_thread_tss_t   timestamp_thread_key;
char  *mulle_aba_thread_name( void);


static mulle_atomic_pointer_t    central;

#pragma mark -
#pragma mark run test

static void    run_atomic_add_test( void)
{
   unsigned long   i;
   
   for( i = 0; i < LOOPS; i++)
   {
      _mulle_atomic_pointer_increment( &central);
   }
}


void  multi_threaded_test_each_thread()
{
#if PROGRESS
   fprintf( stdout,  "."); fflush( stdout);
#endif
   run_atomic_add_test();
}


static void   _wait_around( mulle_atomic_pointer_t *n_threads)
{
   // wait for all threads to materialize
   _mulle_atomic_pointer_decrement( n_threads);
   while( _mulle_atomic_read_pointer( n_threads) != 0)
      sched_yield();
}


struct thread_info
{
   char                  name[ 64];
   mulle_atomic_pointer_t    *n_threads;
};


static mulle_thread_rval_t   run_test( struct thread_info *info)
{
   mulle_thread_tss_set( timestamp_thread_key, strdup( info->name));
   
   _wait_around( info->n_threads);
   multi_threaded_test_each_thread();
   
   return( 0);
}


static void   finish_test( void)
{
}


void  multi_threaded_test( intptr_t n)
{
   int                  i;
   mulle_thread_t       *threads;
   struct thread_info   *info;
   mulle_atomic_pointer_t   n_threads;
   
#if MULLE_ABA_TRACE
   fprintf( stderr, "////////////////////////////////\n");
   fprintf( stderr, "multi_threaded_test( %ld) starts\n", n);
#endif
   threads = alloca( n * sizeof( mulle_thread_t));
   assert( threads);
   
   n_threads._nonatomic = (void *) n;
   info = alloca( sizeof(struct thread_info) * n);
   
   central._nonatomic = (void *) 0;
   
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
   
   assert( central._nonatomic == (void *) (n * LOOPS));
   
   finish_test();
   
#if MULLE_ABA_TRACE
   fprintf( stderr, "%s: multi_threaded_test( %ld) ends\n", mulle_aba_thread_name(), n);
#endif
}


char  *mulle_aba_thread_name( void)
{
   return( mulle_thread_tss_get( timestamp_thread_key));
}


#ifdef __APPLE__
#include <sys/resource.h>

__attribute__((constructor))
static void  __enable_core_dumps(void)
{
   struct rlimit   limit;
   
   limit.rlim_cur = RLIM_INFINITY;
   limit.rlim_max = RLIM_INFINITY;
   setrlimit(RLIMIT_CORE, &limit);
}
#endif

int   _main(int argc, const char * argv[])
{
   unsigned int   j;
   int            rval;
   
   srand( (unsigned int) time( NULL));
   
   rval = mulle_thread_tss_create( &timestamp_thread_key, free);
   assert( ! rval);
   
   rval = mulle_thread_tss_set( timestamp_thread_key, strdup( "main"));
   assert( ! rval);
   
#if MULLE_ABA_TRACE
   fprintf( stderr, "%s\n", mulle_aba_thread_name());
#endif
   
   //
   // if there are leaks anywhere, it will assert in
   // _mulle_aba_storage_done which is called by reset_memory
   // eventually
   //
   
   for( ;;)
   {
#if MULLE_ABA_TRACE || PROGRESS
# if MULLE_ABA_TRACE
      fprintf( stderr, "iteration %d\n", i);
# else
      fprintf( stdout, "iteration %d\n", i);
# endif
#endif
      for( j = 1; j <= MAX_THREADS; j += j)
      {
         multi_threaded_test( j);
      }
   }
   
   return( 0);
}


#ifndef NO_MAIN
int   main(int argc, const char * argv[])
{
   return( _main( argc, argv));
}
#endif
