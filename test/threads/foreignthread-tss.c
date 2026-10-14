#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

static mulle_thread_tss_t    test_key;
static mulle_thread_mutex_t  test_mutex;
static mulle_thread_cond_t   test_cond;
static int                   test_destructed;
static int                   test_go;

static void   tss_destructor( void *value)
{
   printf( "tss_destructor called\n");
   free( value);

   mulle_thread_mutex_do( test_mutex)
   {
      test_destructed = 1;
      mulle_thread_cond_signal( &test_cond);
   }
}

static mulle_thread_rval_t MULLE_THREAD_CALL   thread_function( void *arg)
{
   void   *data;

   // Wait until main has announced the join, so the interleaving of the
   // two threads' output does not depend on the scheduler.
   mulle_thread_mutex_do( test_mutex)
   {
      while( ! test_go)
         mulle_thread_cond_wait( &test_cond, &test_mutex);
   }

   printf( "thread: allocating TSS data\n");
   data = malloc( 64);
   
   printf( "thread: setting TSS value\n");
   if( mulle_thread_tss_set( test_key, data))
   {
      printf( "thread: TSS set failed\n");
      free( data);
      return( (mulle_thread_rval_t) 1);
   }

   printf( "thread: getting TSS value\n");
   if( mulle_thread_tss_get( test_key) != data)
   {
      printf( "thread: TSS get mismatch\n");
      return( (mulle_thread_rval_t) 2);
   }

   printf( "thread: exiting normally\n");
   mulle_thread_return();
}

int   main( void)
{
   mulle_thread_t   thread;

   mulle_thread_mutex_init( &test_mutex);
   mulle_thread_cond_init( &test_cond);

   printf( "main: creating TSS key with destructor\n");
   if( mulle_thread_tss_create( tss_destructor, &test_key))
   {
      printf( "main: TSS create failed\n");
      return( 1);
   }

   printf( "main: creating thread\n");
   if( mulle_thread_create( thread_function, NULL, &thread))
   {
      printf( "main: thread create failed\n");
      return( 1);
   }

   printf( "main: joining thread\n");
   mulle_thread_mutex_do( test_mutex)
   {
      test_go = 1;
      mulle_thread_cond_signal( &test_cond);
   }
   mulle_thread_join( thread);

   // The TSS destructor runs during thread teardown. The scheduler does not
   // order it against the return of join (valgrind!), so explicitly wait for
   // the destructor to keep the output deterministic.
   mulle_thread_mutex_do( test_mutex)
   {
      while( ! test_destructed)
         mulle_thread_cond_wait( &test_cond, &test_mutex);
   }

   printf( "main: thread joined\n");

   printf( "main: freeing TSS key\n");
   mulle_thread_tss_free( test_key);

   mulle_thread_cond_done( &test_cond);
   mulle_thread_mutex_done( &test_mutex);

   printf( "main: exiting\n");
   return( 0);
}
