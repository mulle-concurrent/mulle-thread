#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

#define NUM_THREADS  2
#define NUM_ONCE_CALLS  50000

static int                     values[ NUM_ONCE_CALLS];
static mulle_atomic_pointer_t  init_count[ NUM_ONCE_CALLS];
static mulle_thread_once_t     once_controls[ NUM_ONCE_CALLS];

static void   init_value( void *arg)
{
   int   index;
   
   index = (int)(intptr_t) arg;
   MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   _mulle_atomic_pointer_increment( &init_count[ index]);
   values[ index] = index + 1;
}

static mulle_thread_rval_t   once_thread( void *arg)
{
   int   i;
   int   val;
   
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      mulle_thread_once_call_noblock( &once_controls[ i], init_value, (void *)(intptr_t) i);
      val = values[ i];
      if( val != 0 && val != i + 1)
      {
         printf( "FAILED: thread got wrong value %d for values[%d] (expected %d)\n", val, i, i + 1);
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
   int              multiple_inits;
   
   for( i = 0; i < NUM_THREADS; i++)
      mulle_thread_create( once_thread, NULL, &threads[ i]);
   
   for( i = 0; i < NUM_THREADS; i++)
      mulle_thread_join( threads[ i]);
   
   failures = 0;
   multiple_inits = 0;
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      if( values[ i] == 0)
      {
         printf( "FAILED: values[%d] is 0\n", i);
         failures++;
      }
      if( _mulle_atomic_pointer_read( &init_count[ i]) > (void *) 1)
         multiple_inits++;
   }
   
   if( failures == 0)
   {
      printf( "test passed: all %d values initialized\n", NUM_ONCE_CALLS);
      printf( "note: %d values had multiple init calls (expected for noblock)\n", multiple_inits);
   }
   else
      printf( "test FAILED: %d values were not initialized\n", failures);
   
   return( failures == 0 ? 0 : 1);
}
