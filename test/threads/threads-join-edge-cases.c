#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif


static mulle_thread_rval_t   quick_thread( void *arg)
{
   (void) arg;
   return( (mulle_thread_rval_t) 42);
}


static mulle_thread_rval_t   slow_thread( void *arg)
{
   (void) arg;
   sleep_ms( 2000);  // 2 seconds
   return( (mulle_thread_rval_t) 99);
}


int   main( void)
{
   mulle_thread_t       thread;
   mulle_thread_rval_t  rval;
   int                  result;

   printf( "=== Thread Join Edge Cases Test ===\n\n");

   // Test 1: Normal join
   printf( "Test 1: Normal join...\n");
   if( mulle_thread_create( quick_thread, NULL, &thread))
   {
      printf( "FAIL: Could not create thread\n");
      return( 1);
   }
   rval = mulle_thread_join( thread);
   if( rval != (mulle_thread_rval_t) 42)
   {
      printf( "FAIL: Expected 42, got %d\n", (int) rval);
      return( 1);
   }
   printf( "PASS: Normal join returned correct value\n\n");

   // Test 2: Join on slow thread (tests actual waiting)
   printf( "Test 2: Join on slow thread...\n");
   if( mulle_thread_create( slow_thread, NULL, &thread))
   {
      printf( "FAIL: Could not create thread\n");
      return( 1);
   }
   printf( "Waiting for slow thread...\n");
   rval = mulle_thread_join( thread);
   if( rval != (mulle_thread_rval_t) 99)
   {
      printf( "FAIL: Expected 99, got %d\n", (int) rval);
      return( 1);
   }
   printf( "PASS: Slow thread join returned correct value\n\n");

   // Test 3: Detach (no join)
   printf( "Test 3: Detach thread...\n");
   if( mulle_thread_create( quick_thread, NULL, &thread))
   {
      printf( "FAIL: Could not create thread\n");
      return( 1);
   }
   result = mulle_thread_detach( thread);
   if( result != 0)
   {
      printf( "FAIL: Detach failed\n");
      return( 1);
   }
   printf( "PASS: Thread detached successfully\n");
   sleep_ms( 100);  // Give detached thread time to finish
   printf( "\n");

   // Test 4: Multiple threads join
   printf( "Test 4: Multiple threads join...\n");
   mulle_thread_t threads[5];
   for( int i = 0; i < 5; i++)
   {
      if( mulle_thread_create( quick_thread, NULL, &threads[i]))
      {
         printf( "FAIL: Could not create thread %d\n", i);
         return( 1);
      }
   }
   for( int i = 0; i < 5; i++)
   {
      rval = mulle_thread_join( threads[i]);
      if( rval != (mulle_thread_rval_t) 42)
      {
         printf( "FAIL: Thread %d returned wrong value\n", i);
         return( 1);
      }
   }
   printf( "PASS: All threads joined successfully\n\n");

   printf( "=== All tests passed ===\n");
   return( 0);
}
