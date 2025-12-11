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
#include <mulle-thread/mulle-thread.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRESS        0
#define MAX_ITERATIONS  100
#define LOOPS           10000
#define MAX_THREADS     4


typedef double mulle_timeinterval_t;
typedef double mulle_relativetime_t;

#ifdef _WIN32

#include <windows.h>

// https://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows

mulle_timeinterval_t   mulle_timeinterval_now( void)
{
   __int64   wintime;

   GetSystemTimeAsFileTime((FILETIME*)&wintime);

#if defined( __clang__) || defined( __GNUC__)
   wintime -=116444736000000000LL;    //1jan1601 to 1jan1970
#else   
   wintime -=116444736000000000i64;  //1jan1601 to 1jan1970
#endif   
   return( (mulle_timeinterval_t) wintime / 10000000.0);
}

#else

mulle_timeinterval_t   mulle_timeinterval_now( void)
{
   struct timespec   now;

   clock_gettime( CLOCK_REALTIME, &now);
   return( now.tv_sec + now.tv_nsec / (1000.0*1000*1000));
}

#endif



static mulle_thread_tss_t   name_thread_key;
char  *mulle_aba_thread_name( void);


static mulle_atomic_pointer_t    central;

#pragma mark - run test

static void    run_atomic_read_cas( void)
{
   unsigned long   i;
   void            *value;
   void            *expect;

   for( i = 0; i < LOOPS; i++)
   {
      do
      {
         expect = _mulle_atomic_pointer_read( &central);
         value  = (void *) ((intptr_t) expect + 1);
      }
      while( ! _mulle_atomic_pointer_weakcas( &central, value, expect));
   }
}


static void    run_atomic_better_read_cas( void)
{
   unsigned long   i;
   void            *value;
   void            *expect;
   void            *actual;

   for( i = 0; i < LOOPS; i++)
   {
      expect = _mulle_atomic_pointer_read( &central);
      for(;;)
      {
         value  = (void *) ((intptr_t) expect + 1);
         actual = __mulle_atomic_pointer_cas( &central, value, expect);
         if( actual == expect)
            break;
         expect = actual;
      }
   }
}


static void    run_atomic_double_cas( void)
{
   unsigned long   i;
   void            *value;
   void            *expect;
   void            *prev;

   expect = _mulle_atomic_pointer_read( &central);
   for( i = 0; i < LOOPS; i++)
   {
      for(;;)
      {
         value = (void *) ((intptr_t) expect + 1);
         prev  = __mulle_atomic_pointer_weakcas( &central, value, expect);
         if( prev == expect)
            break;
         expect = prev;
      }
   }
}


static void   _wait_around( mulle_atomic_pointer_t *n_threads)
{
   _mulle_atomic_pointer_decrement( n_threads);
   while( _mulle_atomic_pointer_read( n_threads) != 0)
      mulle_thread_yield();
}


struct thread_info
{
   char                       name[ 64];
   void                       (*f)( void);
   mulle_atomic_pointer_t     *n_threads;
};


static mulle_thread_rval_t   run_test( struct thread_info *info)
{
#ifndef MULLE_TEST_VALGRIND
   mulle_thread_tss_set( name_thread_key, strdup( info->name));
#endif

   // all thread should start at same time...
   _wait_around( info->n_threads);
   (*info->f)();

   return( 0);
}


static void   finish_test( void)
{
}


void  multi_threaded_test( intptr_t n, void (*f)( void))
{
   int                      i;
   mulle_thread_t           *threads;
   struct thread_info       *info;
   mulle_atomic_pointer_t   n_threads;

   threads = alloca( sizeof( mulle_thread_t) * n);
   {
      assert( threads);

      _mulle_atomic_pointer_nonatomic_write( &n_threads, (void *) n);

      info = alloca( sizeof(struct thread_info) * n);
      {
         _mulle_atomic_pointer_nonatomic_write( &central, (void *) 0);

         for( i = 1; i < n; i++)
         {
            info[ i].n_threads = &n_threads;
            info[ i].f         = f;
            sprintf( info[ i].name, "thread #%d", i);

            if( mulle_thread_create( (void *) run_test, &info[ i], &threads[ i]))
               abort();
         }

         info[ 0].n_threads = &n_threads;
         info[ 0].f         = f;
         sprintf( info[ 0].name, "thread #%d", 0);
         run_test( &info[ 0]);

         for( i = 1; i < n; i++)
            if( mulle_thread_join( threads[ i]))
            {
               perror( "mulle_thread_join");
               abort();
            }

         finish_test();
      }
   }
}


char  *mulle_aba_thread_name( void)
{
   return( mulle_thread_tss_get( name_thread_key));
}



mulle_relativetime_t   test( void (*f)( void))
{
   unsigned long          i;
   unsigned int           j;
   int                    rval;
   mulle_timeinterval_t   start;
   mulle_timeinterval_t   end;

   start = mulle_timeinterval_now();

   for( i = 1; i <= MAX_ITERATIONS; i++)
   {
      for( j = 1; j <= MAX_THREADS; j += j)
      {
         multi_threaded_test( j, f);
      }
   }

   end = mulle_timeinterval_now();

   return( end - start);
}


int   main( int argc, const char * argv[])
{
   int                    rval;
   mulle_relativetime_t   elapsed_read_cas;
   mulle_relativetime_t   elapsed_double_cas;
   mulle_relativetime_t   elapsed_better_cas;

   rval = mulle_thread_tss_create( free, &name_thread_key);
   assert( ! rval);

#ifndef MULLE_TEST_VALGRIND
   rval = mulle_thread_tss_set( name_thread_key, strdup( "main"));
   assert( ! rval);
#endif

   test( run_atomic_better_read_cas);
   test( run_atomic_read_cas);
   test( run_atomic_double_cas);

   elapsed_better_cas = test( run_atomic_better_read_cas);
   fprintf( stderr, "better-cas %g\n", elapsed_better_cas);

   elapsed_read_cas   = test( run_atomic_read_cas);
   fprintf( stderr, "read-cas %g\n", elapsed_read_cas);

   elapsed_double_cas = test( run_atomic_double_cas);
   fprintf( stderr, "double-cas %g\n", elapsed_double_cas);

   printf( "better-cas is %s than read-cas, which is %s than double-cas\n",
            elapsed_better_cas < elapsed_read_cas ? "faster" : "slower",
            elapsed_read_cas < elapsed_double_cas ? "faster" : "slower");


   return( 0);
}
