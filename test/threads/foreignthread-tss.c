#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

static mulle_thread_tss_t   test_key;

static void   tss_destructor( void *value)
{
   printf( "tss_destructor called\n");
   free( value);
}

static mulle_thread_rval_t   thread_function( void *arg)
{
   void   *data;

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
   mulle_thread_join( thread);
   
   printf( "main: thread joined\n");

   printf( "main: freeing TSS key\n");
   mulle_thread_tss_free( test_key);

   printf( "main: exiting\n");
   return( 0);
}
