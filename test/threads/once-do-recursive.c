#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static int call_count = 0;

static void   recursive_function( int depth)
{
   printf( "Entering recursive_function at depth %d\n", depth);
   
   mulle_thread_once_do_recursive( my_once)
   {
      call_count++;
      printf( "  Once block executed (call_count=%d)\n", call_count);
      
      if( depth < 3)
      {
         printf( "  Calling recursively from depth %d\n", depth);
         recursive_function( depth + 1);
         printf( "  Returned to depth %d\n", depth);
      }
   }
   
   printf( "Exiting recursive_function at depth %d\n", depth);
}


int   main( void)
{
   printf( "=== Recursive Once Do Test ===\n\n");
   
   printf( "First call:\n");
   recursive_function( 1);
   
   printf( "\nAfter first call: call_count=%d\n", call_count);
   
   if( call_count != 1)
   {
      printf( "FAIL: Once block was executed %d times, expected 1\n", call_count);
      return( 1);
   }
   
   printf( "\nSecond call (should skip once block):\n");
   recursive_function( 99);
   
   printf( "\nAfter second call: call_count=%d\n", call_count);
   
   if( call_count != 1)
   {
      printf( "FAIL: Once block was executed again\n");
      return( 1);
   }
   
   printf( "\n=== Test passed ===\n");
   return( 0);
}
