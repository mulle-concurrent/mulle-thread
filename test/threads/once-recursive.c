#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static int call_count = 0;
static int recursive_depth = 0;

static mulle_thread_once_recursive_t   once = MULLE_THREAD_ONCE_RECURSIVE_INIT;


static void   recursive_init( void *userinfo)
{
   int   depth = (int) (intptr_t) userinfo;
   
   call_count++;
   recursive_depth = depth;
   
   printf( "Init called at depth %d (call_count=%d)\n", depth, call_count);
   
   // Recursive call - should return immediately without re-executing
   if( depth < 3)
   {
      printf( "  Making recursive call from depth %d\n", depth);
      mulle_thread_once_call_recursive( &once, recursive_init, (void *) (intptr_t) (depth + 1));
      printf( "  Returned from recursive call at depth %d\n", depth);
   }
}


int   main( void)
{
   printf( "=== Recursive Once Test ===\n\n");
   
   // First call - should execute and handle recursion
   printf( "First call:\n");
   mulle_thread_once_call_recursive( &once, recursive_init, (void *) (intptr_t) 1);
   
   printf( "\nAfter first call: call_count=%d, recursive_depth=%d\n", call_count, recursive_depth);
   
   if( call_count != 1)
   {
      printf( "FAIL: Init was called %d times, expected 1\n", call_count);
      return( 1);
   }
   
   if( recursive_depth != 1)
   {
      printf( "FAIL: Recursive depth was %d, expected 1\n", recursive_depth);
      return( 1);
   }
   
   // Second call - should not execute at all
   printf( "\nSecond call (should be no-op):\n");
   mulle_thread_once_call_recursive( &once, recursive_init, (void *) (intptr_t) 99);
   
   printf( "After second call: call_count=%d, recursive_depth=%d\n", call_count, recursive_depth);
   
   if( call_count != 1)
   {
      printf( "FAIL: Init was called again\n");
      return( 1);
   }
   
   printf( "\n=== Test passed ===\n");
   return( 0);
}
