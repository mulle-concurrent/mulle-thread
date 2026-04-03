#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_WAITERS  4

static mulle_thread_mutex_t   mutex;
static mulle_thread_cond_t    cond;
static int                    ready = 0;
static int                    woken = 0;

static mulle_thread_rval_t   waiter_thread( void *arg)
{
   mulle_thread_mutex_lock( &mutex);
   
   while( ! ready)
      mulle_thread_cond_wait( &cond, &mutex);
   
   woken++;
   
   mulle_thread_mutex_unlock( &mutex);
   
   mulle_thread_return();
}

int main( void)
{
   mulle_thread_t   threads[ NUM_WAITERS];
   int              i;
   
   mulle_thread_mutex_init( &mutex);
   mulle_thread_cond_init( &cond);
   
   for( i = 0; i < NUM_WAITERS; i++)
      mulle_thread_create( waiter_thread, (void *)(intptr_t)i, &threads[ i]);
   
   // Give waiters time to start waiting
   mulle_thread_yield();
   
   mulle_thread_mutex_lock( &mutex);
   ready = 1;
   mulle_thread_cond_broadcast( &cond);
   mulle_thread_mutex_unlock( &mutex);
   
   for( i = 0; i < NUM_WAITERS; i++)
      mulle_thread_join( threads[ i]);
   
   mulle_thread_cond_done( &cond);
   mulle_thread_mutex_done( &mutex);
   
   if( woken == NUM_WAITERS)
      printf( "test passed: all %d threads woken\n", NUM_WAITERS);
   else
      printf( "test FAILED: only %d of %d threads woken\n", woken, NUM_WAITERS);
   
   return( woken == NUM_WAITERS ? 0 : 1);
}
