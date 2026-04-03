#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <time.h>

#define NUM_THREADS  2
#define NUM_ONCE_CALLS  10

static int                     values[ NUM_ONCE_CALLS];
static mulle_atomic_pointer_t  init_count[ NUM_ONCE_CALLS];

#define INIT_VALUE( i) \
   mulle_thread_once_do_noblock( once_##i)               \
   {                                                     \
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();              \
      _mulle_atomic_pointer_increment( &init_count[ i]); \
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();              \
      values[ i] = i + 1;                                \
   }

static mulle_thread_rval_t   once_thread( void *arg)
{
   int   i;
   int   val;
   
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      switch( i)
      {
      case 0: { INIT_VALUE( 0); val = values[ 0]; } break;
      case 1: { INIT_VALUE( 1); val = values[ 1]; } break;
      case 2: { INIT_VALUE( 2); val = values[ 2]; } break;
      case 3: { INIT_VALUE( 3); val = values[ 3]; } break;
      case 4: { INIT_VALUE( 4); val = values[ 4]; } break;
      case 5: { INIT_VALUE( 5); val = values[ 5]; } break;
      case 6: { INIT_VALUE( 6); val = values[ 6]; } break;
      case 7: { INIT_VALUE( 7); val = values[ 7]; } break;
      case 8: { INIT_VALUE( 8); val = values[ 8]; } break;
      case 9: { INIT_VALUE( 9); val = values[ 9]; } break;
      }
      
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
   int              failures;
   int              multiple_inits;
   int              i;
   
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
