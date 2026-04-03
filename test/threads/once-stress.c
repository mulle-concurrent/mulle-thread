#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

#define NUM_THREADS  2
#define NUM_ONCE_CALLS  50000

static int                     values[ NUM_ONCE_CALLS];
static mulle_thread_once_t     once_controls[ NUM_ONCE_CALLS];

static void   init_value( void *arg)
{
   int   index;
   
   index = (int)(intptr_t) arg;
   MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   values[ index] = index + 1;
   MULLE_THREAD_UNPLEASANT_RACE_YIELD();
}

static mulle_thread_rval_t   once_thread( void *arg)
{
   int   i;
   int   val;
   
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      mulle_thread_once_call( &once_controls[ i], init_value, (void *)(intptr_t) i);
      val = values[ i];
      if( val == 0)
      {
         printf( "FAILED: thread got stale value 0 for values[%d]\n", i);
         mulle_thread_return();
      }
   }
   
   mulle_thread_return();
}

int main( void)
{
   mulle_thread_t   threads[ NUM_THREADS];
   int              i;
   int              failures;
   
   for( i = 0; i < NUM_THREADS; i++)
      mulle_thread_create( once_thread, NULL, &threads[ i]);
   
   for( i = 0; i < NUM_THREADS; i++)
      mulle_thread_join( threads[ i]);
   
   failures = 0;
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      if( values[ i] == 0)
      {
         printf( "FAILED: values[%d] is 0\n", i);
         failures++;
      }
   }
   
   if( failures == 0)
      printf( "test passed: all %d values initialized exactly once\n", NUM_ONCE_CALLS);
   else
      printf( "test FAILED: %d values were not initialized\n", failures);
   
   return( failures == 0 ? 0 : 1);
}
