#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PRODUCERS  2
#define NUM_CONSUMERS  2
#define ITEMS_PER_PRODUCER  100

static mulle_thread_mutex_t   mutex;
static mulle_thread_cond_t    cond;
static int                    queue = 0;
static int                    done = 0;

static mulle_thread_rval_t   producer_thread( void *arg)
{
   int   i;
   
   for( i = 0; i < ITEMS_PER_PRODUCER; i++)
   {
      mulle_thread_mutex_lock( &mutex);
      queue++;
      mulle_thread_cond_signal( &cond);
      mulle_thread_mutex_unlock( &mutex);
      
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   }
   
   mulle_thread_return();
}

static mulle_thread_rval_t   consumer_thread( void *arg)
{
   int   consumed = 0;
   
   for(;;)
   {
      mulle_thread_mutex_lock( &mutex);
      
      while( queue == 0 && ! done)
         mulle_thread_cond_wait( &cond, &mutex);
      
      if( queue > 0)
      {
         queue--;
         consumed++;
      }
      else if( done)
      {
         mulle_thread_mutex_unlock( &mutex);
         break;
      }
      
      mulle_thread_mutex_unlock( &mutex);
      
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();
   }
   
   mulle_thread_return();
}

int main( void)
{
   mulle_thread_t   producers[ NUM_PRODUCERS];
   mulle_thread_t   consumers[ NUM_CONSUMERS];
   int              i;
   
   mulle_thread_mutex_init( &mutex);
   mulle_thread_cond_init( &cond);
   
   for( i = 0; i < NUM_CONSUMERS; i++)
      mulle_thread_create( consumer_thread, (void *)(intptr_t)i, &consumers[ i]);
   
   for( i = 0; i < NUM_PRODUCERS; i++)
      mulle_thread_create( producer_thread, (void *)(intptr_t)i, &producers[ i]);
   
   for( i = 0; i < NUM_PRODUCERS; i++)
      mulle_thread_join( producers[ i]);
   
   mulle_thread_mutex_lock( &mutex);
   done = 1;
   mulle_thread_cond_broadcast( &cond);
   mulle_thread_mutex_unlock( &mutex);
   
   for( i = 0; i < NUM_CONSUMERS; i++)
      mulle_thread_join( consumers[ i]);
   
   mulle_thread_cond_done( &cond);
   mulle_thread_mutex_done( &mutex);
   
   if( queue == 0)
      printf( "test passed: all items consumed\n");
   else
      printf( "test FAILED: %d items remaining\n", queue);
   
   return( queue == 0 ? 0 : 1);
}
