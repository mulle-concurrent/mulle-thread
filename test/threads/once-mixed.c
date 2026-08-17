#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdint.h>

#define NUM_THREADS       4
#define NUM_ONCE_CALLS    500

static int                     values[ NUM_ONCE_CALLS];
static mulle_atomic_pointer_t  init_count[ NUM_ONCE_CALLS];
static mulle_thread_once_t     once_controls[ NUM_ONCE_CALLS];

// macro form, exercised on its own flag
static int   macro_value;


static void   init_value( void *arg)
{
   int   index;

   index = (int)(intptr_t) arg;
   _mulle_atomic_pointer_increment( &init_count[ index]);
   values[ index] = index + 1;
}


static void   init_plain( void)
{
   macro_value++;
}


static mulle_thread_rval_t MULLE_THREAD_CALL   mixed_thread( void *arg)
{
   int   i;
   int   val;
   int   parity;

   parity = (int)(intptr_t) arg;
   for( i = 0; i < NUM_ONCE_CALLS; i++)
   {
      // Some threads use the non-blocking variant, some the blocking one,
      // on the SAME flags. The non-blocking variant now publishes DONE after
      // init, so a blocking call on a flag a noblock call initialized
      // terminates; before that fix, the blocking call would spin forever.
      if( (i & 1) == parity)
         mulle_thread_once_call_noblock( &once_controls[ i], init_value, (void *)(intptr_t) i);
      else
         mulle_thread_once_call( &once_controls[ i], init_value, (void *)(intptr_t) i);

      val = values[ i];
      if( val != 0 && val != i + 1)
      {
         printf( "FAILED: thread got wrong value %d for values[%d] (expected %d)\n", val, i, i + 1);
         mulle_thread_return();
      }
   }

   // macro variant, mixed with the blocking function on the same flag
   mulle_thread_once_do_noblock( s_macro)
   {
      macro_value++;
   }
   mulle_thread_once( &s_macro, init_plain);
   if( macro_value != 1)
   {
      printf( "FAILED: macro once initialized %d times (expected 1)\n", macro_value);
      mulle_thread_return();
   }

   mulle_thread_return();
}


int main( void)
{
   mulle_thread_t   threads[ NUM_THREADS];
   int              i;
   int              failures;

   for( i = 0; i < NUM_THREADS; i++)
      mulle_thread_create( mixed_thread, (void *)(intptr_t) (i & 1), &threads[ i]);

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
      if( _mulle_atomic_pointer_read( &init_count[ i]) > (void *) 1)
      {
         printf( "FAILED: values[%d] initialized %d times\n",
                 i, (int)(intptr_t) _mulle_atomic_pointer_read( &init_count[ i]));
         failures++;
      }
   }

   if( failures == 0)
      printf( "test passed: %d once flags, no hangs, single init each\n", NUM_ONCE_CALLS);
   else
      printf( "test FAILED: %d failures\n", failures);

   return( failures == 0 ? 0 : 1);
}
